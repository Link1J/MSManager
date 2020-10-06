/*
    MSManager - A Minecraft server manager
    Copyright (C) 2020 Jared Irwin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

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

	ServerConnection ();
	~ServerConnection();

	void Update   ();
	void Reconnect();

	std::string SendCommand(std::string command, bool print = true);
};