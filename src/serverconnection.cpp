#include "serverconnection.h"
#include "mainwindow.hpp"
#include "varint.h"
#include "vectorstream.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <regex>
#include <future>
#include <thread>
#include <chrono>

#include <nlohmann/json.hpp>

#include <QMessageBox>
#include <QSettings>

using json = nlohmann::json;
using namespace asio::ip;
using namespace std::literals;

asio::io_context io_context;

ServerConnection::ServerConnection()
	: server_socket(io_context)
	, rcon_socket  (io_context)
	, query_socket (io_context, udp::endpoint(udp::v4(), 0)) 
{
}

ServerConnection::~ServerConnection()
{
	if (reconnect_thread.joinable())
		reconnect_thread.detach();
}

struct RCONPacket
{
	int length;
	int id;
	int type;
	uint8_t payload[4096];
};

void ServerConnection::Reconnect()
{	
	dontTry = true;

	std::packaged_task<void()> task([&]{
		switch (IPCheck())
		{
		case State::Failed:
			QMessageBox::critical(
				window,
				"ERROR",
				"IP Address is not valid.\n"
			);

			if (!window)
				return;

			window->UpdateMotd("");
			window->UpdateOnline(0);
			window->UpdateMax(0);
			window->UpdateType("");
			window->UpdateImage({});

		case State::NoAttempt:
			return;
		}

		switch (ServerConnect())
		{
		case State::Failed:
			QMessageBox::critical(
				window,
				"ERROR",
				"Failed to connect to server.\n"
			);

			if (!window)
				return;

			window->UpdateMotd("");
			window->UpdateOnline(0);
			window->UpdateMax(0);
			window->UpdateType("");
			window->UpdateImage({});

		case State::NoAttempt:
			return;
		}

		switch (QueryConnect())
		{
		case State::Failed:
			QMessageBox::warning(
				window,
				"Warning",
				"Query connection failed.\n"
				"Some information may be missing.\n"
			);
		}

		switch (RCONConnect())
		{
		case State::Failed:
			QMessageBox::warning(
				window,
				"Warning",
				"RCON connection failed.\n"
				"Sending commands will not be avilable.\n"
			);
		}

		switch (RCONLogin())
		{
		case State::Failed:
			QMessageBox::warning(
				window,
				"Warning",
				"RCON packet failed.\n"
				"Sending commands will not be avilable.\n"
			);
		}

		reconnected = true;
		dontTry = false;
	});

	if (reconnect_thread.joinable())
		reconnect_thread.detach();
	reconnect_thread = std::thread(std::move(task));
}

void FillOutPacket(std::vector<uint8_t>& packet, int id)
{
	auto packetID = writeVarInt(id);
	packet.insert(packet.begin(), packetID.begin(), packetID.end());

	auto length = writeVarInt(packet.size());
	packet.insert(packet.begin(), length.begin(), length.end());
}

std::vector<uint8_t> ServerPacketHandshake(int status)
{
	auto protcolVersion = writeVarInt(-1);
	auto serverAddress  = writeVarInt(0);
	auto nextState      = writeVarInt(status);

	std::vector<uint8_t> packet;
	packet.insert(packet.end(), protcolVersion.begin(), protcolVersion.end());
	packet.insert(packet.end(), serverAddress .begin(), serverAddress .end());
	packet.push_back(0);
	packet.push_back(0);
	packet.insert(packet.end(), nextState     .begin(), nextState     .end());

	FillOutPacket(packet, 0);

	return packet;
}

std::vector<uint8_t> ServerPacketPing()
{
	std::vector<uint8_t> packet;
	packet.push_back(0);
	packet.push_back(0);
	packet.push_back(0);
	packet.push_back(0);
	packet.push_back(0);
	packet.push_back(0);
	packet.push_back(0);
	packet.push_back(0);

	FillOutPacket(packet, 1);

	return packet;
}

std::vector<uint8_t> ServerPacket(int id)
{
	std::vector<uint8_t> packet;

	FillOutPacket(packet, id);

	return packet;
}

std::string ServerPacketStatus(std::vector<uint8_t> packet)
{
	uint32_t numRead = 0, total = 0;
	int length   = readVarInt(packet.data() + total, numRead);
	total += numRead;
	int packetID = readVarInt(packet.data() + total, numRead);
	total += numRead;

	if (packetID != 0)
		return "";

	length = readVarInt(packet.data() + total, numRead);
	total += numRead;
	std::string json_response;
	json_response.resize(length);
	std::memcpy((char*)json_response.data(), packet.data() + total, length);
	return json_response;
}

void ServerConnection::Update()
{
	if (dontTry)
		return;
	if (!window)
		return;

	if (IPCheck      () != State::Success) return;
	if (ServerConnect() != State::Success) return;

	auto server_ping = ServerPing();
	auto query_ping  = QueryPing ();

	if (reconnected)
	{
		auto server_decode = ServerPingDecode(server_ping);
		
		window->UpdateImage(server_decode.image      );
		window->UpdateType (server_decode.version    );
		window->UpdateMotd (server_decode.motd       );
		window->UpdateMax  (server_decode.max_players);

		if (query_connected)
		{
			auto[mod_name, plugin] = QueryPingGetPlugins(query_ping);

			window->UpdateModName(mod_name);

			for (auto name : plugin)
			{
				window->AddPlugin(name);
				plugins[name] = 1;
    		}
		}

		for(auto it = plugins.begin(); it != plugins.end();)
		{
		  	if (it->second == 0)
		  	{
				window->RemovePlugin(it->first);
				it = plugins.erase(it);
		  	}
		  	else
			{
				it->second = 0;
				++it;
			}
		}
	}

	std::tuple<std::vector<std::string>, int> tuple;
	std::vector<std::string> players;
	int online_players;

	if (query_connected)
	{
		tuple = QueryPingGetPlayers(query_ping);
	}
	else
	{
		tuple = ServerPingGetPlayers(server_ping);
	}

	players        = std::get<0>(tuple);
	online_players = std::get<1>(tuple);

	window->UpdateOnline(online_players);
	
    for (auto name : players)
	{
		window->AddUser(name);
		users[name] = 1;
    }

	for(auto it = users.begin(); it != users.end();)
	{
	  	if (it->second == 0)
	  	{
			window->RemoveUser(it->first);
			it = users.erase(it);
	  	}
	  	else
		{
			it->second = 0;
			++it;
		}
	}
}

void ServerConnection::SendCommand(std::string command)
{
	if (command[0] != '/')
		command = "/" + command;
	window->AddCommand(command);

	if (dontTry)
	{
		window->AddCommand("Trying to connect to server. Please wait.");
		return;
	}

	if (!rcon_connected)
	{
		window->AddCommand("RCON did not connect. Can not send command.");
		return;
	}

	RCONPacket packet;
	memset(&packet, 0, sizeof(packet));
	packet.length = 10 + command.length();
	packet.id     = 1;
	packet.type   = 2;
	memcpy(packet.payload, command.data(), command.length());

	asio::write(rcon_socket, asio::buffer(&packet, packet.length + 4));

	std::vector<char> response;

	asio::read(rcon_socket, asio::buffer(&packet, sizeof(packet)), asio::transfer_at_least(12));

	if (packet.id == 1 && packet.type == 0)
	{
		for (int a = 0; a < packet.length - 10; a++)
			response.push_back(packet.payload[a]);
	}

	packet.length     = 10 ;
	packet.type       = 100;
	packet.payload[0] = 0  ;
	packet.payload[1] = 0  ;

	asio::write(rcon_socket, asio::buffer(&packet, packet.length + 4));

	do
	{
		asio::read(rcon_socket, asio::buffer(&packet, sizeof(packet)), asio::transfer_at_least(12));

		if (packet.id == 1 && packet.type == 0)
		{
			if (memcmp(packet.payload, "Unknown request 64", 18) == 0)
				break;

			for (int a = 0; a < packet.length - 10; a++)
				response.push_back(packet.payload[a]);
		}
	}
	while (packet.type == 0);
	response.push_back(0);

	int pos = command.find("help");
	if (pos == 0 || pos == 1)
	{
		std::vector<char> temp; 
		std::replace(response.begin(), response.end(), '/', '\n');
		temp.insert(temp.end(), response.begin() + 1, response.end());
		response = temp;
	}

	window->AddCommand(response.data());
}

ServerConnection::State ServerConnection::IPCheck()
{
	static std::string pre_ip = "";

	QSettings settings;
	
	auto server_ip = settings.value("server_ip").toString().toStdString();

	if (server_ip == pre_ip)
	{
		return State::NoAttempt;
	}

	std::regex ip_check("^(.+\\..{2,10}|localhost|(?:\\d{1,3}\\.){3}\\d{1,3})\\/?.*?$");
	if (!std::regex_search(server_ip, ip_check))
	{
		pre_ip == server_ip;
		return State::Failed;
	}

	pre_ip = "";
	return State::Success;
}

ServerConnection::State ServerConnection::ServerConnect()
{
	static std::string pre_ip = "";

	QSettings settings;
	
	auto server_ip   = settings.value("server_ip"  ).toString().toStdString();
	auto server_port = settings.value("server_port").toInt   ()              ;

	if (server_ip == pre_ip)
	{
		return State::NoAttempt;
	}

	try 
	{
		server_socket.close();
    	tcp::resolver tcp_resolver(io_context);
    	asio::connect(server_socket, tcp_resolver.resolve(server_ip, std::to_string(server_port)));
	}
	catch(const std::exception& e)
	{
		pre_ip == server_ip;
		return State::Failed;
	}

	pre_ip = "";
	return State::Success;
}

ServerConnection::State ServerConnection::RCONConnect()
{
	static int pre_port = -1;
	static std::string pre_password = "";

	QSettings settings;
	
	auto server_ip = settings.value("server_ip").toString().toStdString();
	auto rcon_port = settings.value("rcon_port").toInt   ()              ;

	if (rcon_port == pre_port)
	{
		return State::NoAttempt;
	}

	try 
	{
		rcon_socket.close();
    	tcp::resolver tcp_resolver(io_context);
    	asio::connect(rcon_socket, tcp_resolver.resolve(server_ip, std::to_string(rcon_port)));
	}
	catch(const std::exception& e)
	{
		rcon_connected = false;
		pre_port = rcon_port;
		return State::Failed;
	}

	pre_port = -1;
	rcon_connected = true;
	return State::Success;
}

ServerConnection::State ServerConnection::RCONLogin()
{
	static std::string pre_password = "";

	QSettings settings;
	
	auto rcon_pass = settings.value("rcon_password").toString().toStdString();

	if (rcon_pass == pre_password || !rcon_connected)
	{
		return State::NoAttempt;
	}

	RCONPacket packet;
	memset(&packet, 0, sizeof(packet));
	packet.length = 10 + rcon_pass.length();
	packet.id     = 1;
	packet.type   = 3;
	memcpy(packet.payload, rcon_pass.data(), rcon_pass.length());

	asio::write(rcon_socket, asio::buffer(&packet, packet.length + 4)                             );
	asio::read (rcon_socket, asio::buffer(&packet, sizeof(packet)   ), asio::transfer_at_least(12));

	if (packet.id == -1)
	{
		rcon_connected = false;
		pre_password = rcon_pass;
		return State::Failed;
	}

	pre_password = "";
	rcon_connected = true;
	return State::Success;
}

ServerConnection::State ServerConnection::QueryConnect()
{
	static int pre_port = -1;
	static std::string pre_password = "";

	QSettings settings;
	
	auto server_ip  = settings.value("server_ip" ).toString().toStdString();
	auto query_port = settings.value("query_port").toInt   ()              ;

	if (query_port == pre_port)
	{
		return State::NoAttempt;
	}

	try 
	{
		query_socket.close();
    	udp::resolver udp_resolver(io_context);
    	asio::connect(query_socket, udp_resolver.resolve(udp::v4(), server_ip, std::to_string(query_port)));
		std::this_thread::sleep_for(500ms);
		std::vector<uint8_t> sessionID{0x00,0x00,0x00,0x00,0x00};
		query_socket.send(asio::buffer(sessionID));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << "\n";

		query_connected = false;
		pre_port = query_port;
		return State::Failed;
	}

	pre_port = -1;
	query_connected = true;
	return State::Success;
}

std::vector<uint8_t> ServerConnection::ServerPing()
{
	auto handshake = ServerPacketHandshake(1);
	auto packet    = ServerPacket         (0);
	auto ping      = ServerPacketPing     ( );

	std::vector<uint8_t> response(32767 + 10);

	asio::write(server_socket, asio::buffer(handshake));
	asio::write(server_socket, asio::buffer(packet   ));
	asio::write(server_socket, asio::buffer(ping     ));
	asio::read (server_socket, asio::buffer(response), asio::transfer_at_least(2));

	return response;
}

std::vector<uint8_t> ServerConnection::QueryPing()
{
	std::vector<uint8_t> sessionID{0xFE,0xFD,0x09,0x00,0x00,0x00,0x01};
	std::vector<uint8_t> getstat  {0xFE,0xFD,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	std::vector<uint8_t> challengeTokenResponse(50  );
	std::vector<uint8_t> query_status          (5000);

	if (!query_connected)
		return query_status;

	try
	{
		query_socket.send   (asio::buffer(sessionID             ));
		query_socket.receive(asio::buffer(challengeTokenResponse));

		int32_t challengeToken = std::stoi(((char*)challengeTokenResponse.data() + 5));
		getstat[ 7] = (challengeToken >> 24) & 0xFF;
		getstat[ 8] = (challengeToken >> 16) & 0xFF;
		getstat[ 9] = (challengeToken >>  8) & 0xFF;
		getstat[10] = (challengeToken >>  0) & 0xFF;

		query_socket.send   (asio::buffer(getstat     ));
		query_socket.receive(asio::buffer(query_status));
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(
			window,
			"Warning",
			"Query connection failed.\n"
			"Some information may be missing.\n"
		);
		query_connected = false;
	}

	return query_status;
}

ServerPingData ServerConnection::ServerPingDecode(std::vector<uint8_t> packet)
{
	auto status = json::parse(ServerPacketStatus(packet));

	ServerPingData data;

	if (status.count("favicon"))
	{
		data.image = status["favicon"].get<std::string>();
		data.image = data.image.substr(std::string("data:image/png;base64,").size());
	}

	data.online_players = status["players"]["online"].get<int>();
	data.max_players    = status["players"]["max"   ].get<int>();

	data.motd = status["description"]["text"].get<std::string>();

	data.version = status["version"]["name"].get<std::string>();

	return data;
}

std::tuple<std::vector<std::string>, int> ServerConnection::ServerPingGetPlayers(std::vector<uint8_t> packet)
{
	auto status = json::parse(ServerPacketStatus(packet));

	int online = status["players"]["online"].get<int>();

	return {{""},online};
}

std::tuple<std::vector<std::string>, int> ServerConnection::QueryPingGetPlayers(std::vector<uint8_t> packet)
{
	VectorStream iss(packet);
	std::vector<std::string> players;

	int online = std::stoi(iss.GetValue("numplayers"));

	iss.MovePast("player_");
	iss.Ignore(1);

	while(iss.Peek() != '\0')
	{
		players.push_back(iss.GetValueDelim('\0'));
	}

	return {players, online};
}

std::tuple<std::string, std::vector<std::string>> ServerConnection::QueryPingGetPlugins(std::vector<uint8_t> packet)
{
	VectorStream iss(packet);
	std::vector<std::string> plugins;
	std::string mod_name;

	iss.MovePast("plugins");
	mod_name = iss.GetValueDelim({':', '\0'});

	while(iss.Peek() == ' ')
	{
		iss.Ignore(1);
		plugins.push_back(iss.GetValueDelim({';', '\0'}));
	}

	return {mod_name, plugins};
}

