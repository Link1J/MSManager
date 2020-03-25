#include "serverconnection.h"
#include "exampleappwindow.h"
#include <vector>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <regex>

using json = nlohmann::json;

boost::asio::io_context io_context;

using namespace boost;
using namespace boost::asio::ip;

ServerConnection::ServerConnection()
	: server_socket(io_context)
	, rcon_socket  (io_context)
	, query_socket (io_context, udp::endpoint(udp::v4(), 0)) 
{
	settings = Gio::Settings::create("msmanager");
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

	auto server_ip   = (std::string)settings->get_string("ip"           );
	auto server_port =              settings->get_int   ("port"         );
	auto query_port  =              settings->get_int   ("query-port"   );
	auto rcon_port   =              settings->get_int   ("rcon-port"    );
	auto rcon_pass   = (std::string)settings->get_string("rcon-password");

    tcp::resolver tcp_resolver(io_context);
	udp::resolver udp_resolver(io_context);

	server_socket.close();
	rcon_socket  .close();
	query_socket .close();

	try 
	{
		std::regex ip_check("^(.+\..{2,10}|localhost|(?:\d{1,3}\.){3}\d{1,3})\/?.*?$");
		if (!std::regex_search(server_ip, ip_check))
			return;

    	asio::connect(server_socket, tcp_resolver.resolve(           server_ip, std::to_string(server_port)));
    	asio::connect(rcon_socket  , tcp_resolver.resolve(           server_ip, std::to_string(rcon_port  )));
    	asio::connect(query_socket , udp_resolver.resolve(udp::v4(), server_ip, std::to_string(query_port )));

		Update();

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
			Gtk::MessageDialog error("RCON Login failed.", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, false);
			error.set_secondary_text("Sending commands will no be avilable.");
			error.run();
		}

		pre_password = rcon_pass;
	}
	catch(std::exception e)
	{
		if (!window)
			return;

		window->update_motd("Could not connect to server.");
		window->update_players(0,0);
		window->update_type("");
		window->update_image({});
		window->update_users({});
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

void extractKey(std::istringstream& iss, const char* expected) 
{
    std::string temp;
	do {
    	getline(iss, temp, '\0');
	} while(temp.compare(expected) && iss.good());
}


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

	std::istringstream iss;
    iss.rdbuf()->pubsetbuf(reinterpret_cast<char*>(&query_status[5]), query_status.size()-5);

	auto status = json::parse(ServerStatus(response));

	if (status.count("favicon"))
	{
		auto image = status["favicon"].get<std::string>();
		image = image.substr(std::string("data:image/png;base64,").size());
		window->update_image(base64_decode(image));
	}
	
	window->update_motd(status["description"]["text"].get<std::string>());

	int online = status["players"]["online"].get<int>();
	int max    = status["players"]["max"   ].get<int>();

	window->update_players(online, max);

	std::string game_id;
    std::string version;
    std::string gametype;

	extractKey(iss, "gametype");
    getline(iss, gametype, '\0');

    extractKey(iss, "game_id");
    getline(iss, game_id, '\0');

    extractKey(iss, "version");
    getline(iss, version, '\0');

	window->update_type(game_id + " " + version + " " + gametype);

	std::vector<std::string> players;
	extractKey(iss, "\001player_");
    iss.ignore(1);

    std::string name;
	bool newName = false;


	for(auto it = users.begin(); it != users.end(); it++)
	{
		it->second = 0;
	}

    while(iss.good() && iss.peek()!='\0') 
	{
        getline(iss, name, '\0');
        players.push_back(name);

		if (!users.contains(name))
			newName = true;
		
		users[name] = 1;
    }

	for(auto it = users.begin(); it != users.end();)
	{
	  	if (it->second == 0)
	  	{
			it = users.erase(it); // previously this was something like m_map.erase(it++);
			newName = true;
	  	}
	  	else
		{
			++it;
		}
	}

	//for (auto& item : status["players"]["sample"])
	//{
	//	players.push_back(item["name"]);
	//}

	if (newName)
		window->update_users(players);
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

	if (command == "help")
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