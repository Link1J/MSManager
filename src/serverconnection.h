#pragma once

#define ASIO_STANDALONE

#include <asio.hpp>
#include <unordered_map>

class MainWindow;

class ServerConnection
{
	asio::ip::tcp::socket                 server_socket  ;
	asio::ip::tcp::socket                 rcon_socket    ;
	asio::ip::udp::socket                 query_socket   ;
	asio::ip::udp::resolver::results_type query_endpoints;

	std::string pre_password;
	std::string server_ip_fail;
	std::unordered_map<std::string, int> users;

public:
	MainWindow* window;
	bool dontTry = false;

	ServerConnection(                   );
	void Update     (                   );
	void Reconnect  (                   );
	void SendCommand(std::string command);
};