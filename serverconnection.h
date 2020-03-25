#pragma once

#include <boost/asio.hpp>
#include <gtkmm.h>

class ExampleAppWindow;

class ServerConnection
{
	boost::asio::ip::tcp::socket                 server_socket  ;
	boost::asio::ip::tcp::socket                 rcon_socket    ;
	boost::asio::ip::udp::socket                 query_socket   ;
	boost::asio::ip::udp::resolver::results_type query_endpoints;

	Glib::RefPtr<Gio::Settings> settings;
	std::string pre_password;

public:
	ExampleAppWindow* window;
	bool dontTry = false;

	ServerConnection(                   );
	void Update     (                   );
	void Reconnect  (                   );
	void SendCommand(std::string command);
};