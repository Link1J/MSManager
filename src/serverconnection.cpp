#include "serverconnection.h"
#include "mainwindow.hpp"
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <regex>
#include <QMessageBox>

using json = nlohmann::json;
using namespace asio::ip;

asio::io_context io_context;

ServerConnection::ServerConnection()
	: server_socket(io_context)
	, rcon_socket  (io_context)
	, query_socket (io_context, udp::endpoint(udp::v4(), 0)) 
{
	//settings = Gio::Settings::create("msmanager");
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
	if (dontTry)
		return;

	auto server_ip   = (std::string)"192.168.10.100"; //settings->get_string("ip"           );
	auto server_port =              25565           ; //settings->get_int   ("port"         );
	auto query_port  =              25585           ; //settings->get_int   ("query-port"   );
	auto rcon_port   =              25575           ; //settings->get_int   ("rcon-port"    );
	auto rcon_pass   = (std::string)"69a42jw2"      ; //settings->get_string("rcon-password");

    tcp::resolver tcp_resolver(io_context);
	udp::resolver udp_resolver(io_context);

	server_socket.close();
	rcon_socket  .close();
	query_socket .close();
	
	std::regex ip_check("^(.+\\..{2,10}|localhost|(?:\\d{1,3}\\.){3}\\d{1,3})\\/?.*?$");
	if (!std::regex_search(server_ip, ip_check))
		return;

	try 
	{
    	asio::connect(server_socket, tcp_resolver.resolve(           server_ip, std::to_string(server_port)));
	}
	catch(const std::exception& e)
	{
		if (server_ip == server_ip_fail)
			return;

		QMessageBox::critical(
			window,
			"Failed to connect to server.",
			""
		);
		
		if (!window)
			return;

		window->update_motd("");
		window->update_players(0,0);
		window->update_type("");
		window->update_image({});

		server_ip_fail = server_ip;

		return;
	}

	try
	{
    	asio::connect(query_socket , udp_resolver.resolve(udp::v4(), server_ip, std::to_string(query_port )));
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(
			window, 
			"Query connection failed.",
			"Some information may be missing."
		);
	}
	
	Update();

	try
	{
    	asio::connect(rcon_socket  , tcp_resolver.resolve(           server_ip, std::to_string(rcon_port  )));

		RCONPacket login;
		memset(&login, 0, sizeof(login));
		login.length = 10 + rcon_pass.length();
		login.id     = 1;
		login.type   = 3;
		memcpy(login.payload, rcon_pass.data(), rcon_pass.length());
	
		asio::write(rcon_socket, asio::buffer(&login, login.length + 4)                             );
		asio::read (rcon_socket, asio::buffer(&login, sizeof(login)   ), asio::transfer_at_least(12));
	
		if (login.id == -1 && pre_password != rcon_pass)
		{
			QMessageBox::warning(
				window,
				"RCON Login failed.",
				"Sending commands will not be avilable."
			);
		}
	
		pre_password = rcon_pass;
	}
	catch(const std::exception& e)
	{
		QMessageBox::warning(
			window, 
			"RCON connection failed.",
			"Sending commands will not be avilable."
		);
	}	
}

static std::vector<uint8_t> writeVarInt(uint32_t value) 
{
	std::vector<uint8_t> varInt;
    do {
        uint8_t temp = (uint8_t)(value & 0b01111111);
        // Note: >>> means that the sign bit is shifted with the rest of the number rather than being left alone
        value >>= 7;
        if (value != 0) {
            temp |= 0b10000000;
        }
        varInt.push_back(temp);
    } while (value != 0);
	return varInt;
}

static uint32_t readVarInt(uint8_t* data, uint32_t& numRead) 
{
	numRead = 0;
    uint32_t result = 0;
    uint8_t read;
    do {
        read = data[numRead];
        int value = (read & 0b01111111);
        result |= (value << (7 * numRead));

        numRead++;
        if (numRead > 5) {
			return result;
        }
    } while ((read & 0b10000000) != 0);

    return result;
}

void FillOutPacket(std::vector<uint8_t>& packet, int id)
{
	auto packetID = writeVarInt(id);
	packet.insert(packet.begin(), packetID.begin(), packetID.end());

	auto length = writeVarInt(packet.size());
	packet.insert(packet.begin(), length.begin(), length.end());
}

std::vector<uint8_t> ServerHandshake(int status)
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

std::vector<uint8_t> ServerPing()
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

std::string ServerStatus(std::vector<uint8_t> packet)
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

std::vector<uint8_t> base64_decode(std::string const& encoded_string) 
{
	static const std::string base64_chars = 
             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
             "abcdefghijklmnopqrstuvwxyz"
             "0123456789+/";
	
	static auto is_base64 = [](uint8_t c) -> bool {
		return (isalnum(c) || (c == '+') || (c == '/'));
	};

	int in_len = encoded_string.size();
	int i = 0;
	int j = 0;
	int in_ = 0;
	uint8_t char_array_4[4], char_array_3[3];
	std::vector<uint8_t> ret;

	while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
		char_array_4[i++] = encoded_string[in_]; in_++;
		if (i ==4) {
			for (i = 0; i <4; i++)
				char_array_4[i] = base64_chars.find(char_array_4[i]);

			char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
			char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
			char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

			for (i = 0; (i < 3); i++)
					ret.push_back(char_array_3[i]);
			i = 0;
		}
	}

	if (i) {
		for (j = i; j <4; j++)
			char_array_4[j] = 0;

		for (j = 0; j <4; j++)
			char_array_4[j] = base64_chars.find(char_array_4[j]);

		char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
		char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
		char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

		for (j = 0; (j < i - 1); j++) ret.push_back(char_array_3[j]);
	}

	return ret;
}

class VectorStream
{
	std::vector<uint8_t>& vector;
	int index = 0;

public:
	VectorStream(std::vector<uint8_t>& vector)
		: vector(vector)
	{
	}

	std::string GetValue(std::string key)
	{
		MovePast(key);
		std::string value((char*)&vector[index]);
		index += value.size();
		return value;
	}

	void MovePast(std::string key)
	{
		int i;

		for (; index < vector.size(); index++)
		{
			for (i = 0; i < key.size(); i++)
			{
				if (vector[index + i] != key[i])
					break;
			}

			if (i == key.size())
				break;
		}

		index += key.size() + 1;
	}

	std::string GetValueDelim(uint8_t delim)
	{
		std::vector<char> value;

		for (; index < vector.size(); index++)
		{
			value.push_back(vector[index]);

			if (vector[index] == delim)
				break;
		}

		return value.data();
	}

	void Ignore(int count)
	{
		index += count;
	}

	uint8_t Peek()
	{
		return vector[index];
	}
};

void ServerConnection::Update()
{
	if (dontTry)
		return;
	if (!window)
		return;

	auto handshake = ServerHandshake(1);
	asio::write(server_socket, asio::buffer(handshake));
	auto packet = ServerPacket(0);
	asio::write(server_socket, asio::buffer(packet   ));
	auto ping = ServerPing();
	asio::write(server_socket, asio::buffer(ping     ));
	std::vector<uint8_t> response(32767 + 10);
	asio::read (server_socket, asio::buffer(response), asio::transfer_at_least(2));

	std::vector<uint8_t> sessionID{0xFE,0xFD,0x09,0x00,0x00,0x00,0x01};
	query_socket.send(asio::buffer(sessionID));
	std::vector<uint8_t> challengeTokenResponse(50);
	query_socket.receive(asio::buffer(challengeTokenResponse));
	int32_t challengeToken = std::stoi(((char*)challengeTokenResponse.data() + 5));
	std::vector<uint8_t> getstat{0xFE,0xFD,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	getstat[ 7] = (challengeToken >> 24) & 0xFF;
	getstat[ 8] = (challengeToken >> 16) & 0xFF;
	getstat[ 9] = (challengeToken >>  8) & 0xFF;
	getstat[10] = (challengeToken >>  0) & 0xFF;
	query_socket.send(asio::buffer(getstat));
	std::vector<uint8_t> query_status(5000);
	query_socket.receive(asio::buffer(query_status));

	VectorStream iss(query_status);

	auto status = json::parse(ServerStatus(response));

	std::string image;

	if (status.count("favicon"))
	{
		image = status["favicon"].get<std::string>();
		image = image.substr(std::string("data:image/png;base64,").size());
	}

	int online = status["players"]["online"].get<int>();
	int max    = status["players"]["max"   ].get<int>();

	std::string gametype = iss.GetValue("gametype");
	std::string game_id  = iss.GetValue("game_id");
	std::string version  = iss.GetValue("version" );

	window->update_motd(status["description"]["text"].get<std::string>());
	window->update_players(online, max);
	window->update_type(game_id + " " + version + " " + gametype);
	window->update_image(base64_decode(image));

	std::vector<std::string> players;
	iss.MovePast("\001player_");
    iss.Ignore(1);

    std::string name;
	bool newName = false;

	for(auto it = users.begin(); it != users.end(); it++)
	{
		it->second = 0;
	}

    while(iss.Peek() != '\0') 
	{
        name = iss.GetValueDelim('\0');
		if (!users.contains(name))
			window->add_user(name);
		users[name] = 1;
    }

	for(auto it = users.begin(); it != users.end();)
	{
	  	if (it->second == 0)
	  	{
			window->remove_user(it->first);
			it = users.erase(it);
	  	}
	  	else
		{
			++it;
		}
	}
}

void ServerConnection::SendCommand(std::string command)
{
	RCONPacket login;
	memset(&login, 0, sizeof(login));
	login.length = 10 + command.length();
	login.id     = 1;
	login.type   = 2;
	memcpy(login.payload, command.data(), command.length());

	asio::write(rcon_socket, asio::buffer(&login, login.length + 4));

	std::vector<char> response;

	asio::read(rcon_socket, asio::buffer(&login, sizeof(login)), asio::transfer_at_least(12));

	if (login.id == 1 && login.type == 0)
	{
		for (int a = 0; a < login.length - 10; a++)
			response.push_back(login.payload[a]);
	}

	login.length     = 10 ;
	login.type       = 100;
	login.payload[0] = 0  ;
	login.payload[1] = 0  ;

	asio::write(rcon_socket, asio::buffer(&login, login.length + 4));

	do
	{
		asio::read(rcon_socket, asio::buffer(&login, sizeof(login)), asio::transfer_at_least(12));

		if (login.id == 1 && login.type == 0)
		{
			if (memcmp(login.payload, "Unknown request 64", 18) == 0)
				break;

			for (int a = 0; a < login.length - 10; a++)
				response.push_back(login.payload[a]);
		}
	}
	while (login.type == 0);
	response.push_back(0);

	int pos = command.find("help");
	if (pos == 0 || pos == 1)
	{
		std::vector<char> temp; 
		std::replace(response.begin(), response.end(), '/', '\n');
		temp.insert(temp.end(), response.begin() + 1, response.end());
		response = temp;
	}

	if (command[0] != '/')
		command = "/" + command;

	window->add_command(command);
	window->add_command(response.data());
}