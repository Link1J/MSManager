#pragma once

#define ASIO_STANDALONE

#include <asio.hpp>
#include <unordered_map>
#include <thread>

class MainWindow;

struct ServerPingData
{
	std::string image;
	int max_players;
	int online_players;
	std::string version;
	std::string motd;
};

class ServerConnection
{
public:
	enum class State {
		Success, Failed, NoAttempt
	};

private:
	asio::ip::tcp::socket                 server_socket  ;
	asio::ip::tcp::socket                 rcon_socket    ;
	asio::ip::udp::socket                 query_socket   ;
	asio::ip::udp::resolver::results_type query_endpoints;

	std::unordered_map<std::string, int> users;
	std::unordered_map<std::string, int> plugins;
	bool rcon_connected;
	bool query_connected;
	bool reconnected;

	std::thread reconnect_thread;

	State IPCheck      ();
	State ServerConnect();
	State QueryConnect ();
	State RCONConnect  ();
	State RCONLogin    ();

	std::vector<uint8_t> ServerPing();
	std::vector<uint8_t> QueryPing ();

	ServerPingData ServerPingDecode(std::vector<uint8_t>);
	std::tuple<std::vector<std::string>, int        > ServerPingGetPlayers(std::vector<uint8_t>);
	std::tuple<std::vector<std::string>, int        > QueryPingGetPlayers (std::vector<uint8_t>);
	std::tuple<std::string, std::vector<std::string>> QueryPingGetPlugins (std::vector<uint8_t>);

public:
	MainWindow* window;
	bool dontTry = false;

	ServerConnection (                   );
	~ServerConnection(                   );
	void Update      (                   );
	void Reconnect   (                   );
	void SendCommand (std::string command);
};