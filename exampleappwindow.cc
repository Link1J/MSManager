/* gtkmm example Copyright (C) 2016 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "exampleappwindow.h"
#include "exampleappprefs.h"
#include <iostream>
#include <stdexcept>
#include <set>

ExampleAppWindow::ExampleAppWindow(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& refBuilder)
: Gtk::ApplicationWindow(cobject),
	refBuilder(refBuilder),
	settings(),
	gears(nullptr),
	online_users(nullptr),
	users_online(nullptr),
	header(nullptr)
{
	// Get widgets from the Gtk::Builder file.
	refBuilder->get_widget("gears", gears);
	if (!gears)
		throw std::runtime_error("No \"gears\" object in window.ui");

	refBuilder->get_widget("online_users", online_users);
	if (!online_users)
		throw std::runtime_error("No \"online_users\" object in window.ui");

	refBuilder->get_widget("header", header);
	if (!header)
		throw std::runtime_error("No \"header\" object in window.ui");

	refBuilder->get_widget("server_icon", server_icon);
	if (!server_icon)
		throw std::runtime_error("No \"server_icon\" object in window.ui");

	refBuilder->get_widget("motd", motd);
	if (!motd)
		throw std::runtime_error("No \"motd\" object in window.ui");

	refBuilder->get_widget("max_players", max_players);
	if (!max_players)
		throw std::runtime_error("No \"max_players\" object in window.ui");	

	refBuilder->get_widget("online_players", online_players);
	if (!online_players)
		throw std::runtime_error("No \"online_players\" object in window.ui");
	
	refBuilder->get_widget("server_type", server_type);
	if (!server_type)
		throw std::runtime_error("No \"server_type\" object in window.ui");	

	// Connect the menu to the MenuButton m_gears, and bind the show-words setting
	// to the win.show-words action and the "Words" menu item.
	// (The connection between action and menu item is specified in gears_menu.ui.)
	auto menu_builder = Gtk::Builder::create_from_resource("/msmanager/app_menu.ui");
	auto object = menu_builder->get_object("appmenu");
	auto menu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(object);
	if (!menu)
		throw std::runtime_error("No \"menu\" object in app_menu.ui");

	gears->set_menu_model(menu);

	// Bind settings.
	settings = Gio::Settings::create("msmanager");
	settings->bind("ip", header->property_subtitle());
	
	// Connect signal handlers.
	//gears->signal_clicked().connect(
	//	sigc::mem_fun(*this, &ExampleAppWindow::open_perfs));

	// Set the window icon.
	set_icon(Gdk::Pixbuf::create_from_resource("/msmanager/msmanager.png"));
}

//static
ExampleAppWindow* ExampleAppWindow::create()
{
	// Load the Builder file and instantiate its widgets.
	auto refBuilder = Gtk::Builder::create_from_resource("/msmanager/window.ui");

	ExampleAppWindow* window = nullptr;
	refBuilder->get_widget_derived("app_window", window);
	if (!window)
		throw std::runtime_error("No \"app_window\" object in window.ui");

	return window;
}

void ExampleAppWindow::update_image(std::vector<uint8_t> image)
{
	auto stream = Gio::MemoryInputStream::create();
	stream->add_data(image.data(), image.size());
	server_icon->set(Gdk::Pixbuf::create_from_stream(stream));
}

void ExampleAppWindow::update_motd(std::string motd)
{
	this->motd->set_text(motd);
}

void ExampleAppWindow::update_users(std::vector<std::string> users)
{
	auto old_children = online_users->get_children();
	for (auto child : old_children)
	{
		online_users->remove(*child);
		delete child;
	}

	for (auto user : users)
	{
		auto row = Gtk::make_managed<Gtk::Box>();
		auto label = Gtk::make_managed<Gtk::Label>(user);

		label->set_alignment(0);
		label->show();

		row->pack_start(*label);
    	row->show();

    	online_users->add(*row);
	}
}

void ExampleAppWindow::update_players(int online, int max)
{
	online_players->set_text(std::to_string(online));
	max_players   ->set_text(std::to_string(max   ));
}

void ExampleAppWindow::update_type(std::string server_type)
{
	this->server_type->set_text(server_type);
}