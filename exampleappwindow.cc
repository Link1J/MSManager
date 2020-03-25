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
#include "serverconnection.h"
#include <iostream>
#include <stdexcept>
#include <set>
#include <chrono>
#include <thread>

using namespace std::literals::chrono_literals;

ExampleAppWindow::ExampleAppWindow(BaseObjectType* cobject,
	const Glib::RefPtr<Gtk::Builder>& refBuilder)
: Gtk::ApplicationWindow(cobject),
	refBuilder(refBuilder),
	settings(),
	gears(nullptr),
	online_users(nullptr),
	header(nullptr)
{
	// Get widgets from the Gtk::Builder file.
	refBuilder->get_widget("gears", gears);
	if (!gears)
		throw std::runtime_error("No \"gears\" object in window.ui");

	refBuilder->get_widget("online_users", online_users);
	if (!online_users)
		throw std::runtime_error("No \"online_users\" object in window.ui");

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

	refBuilder->get_widget("messages_list", messages_list);
	if (!messages_list)
		throw std::runtime_error("No \"messages_list\" object in window.ui");

	refBuilder->get_widget("command", command);
	if (!command)
		throw std::runtime_error("No \"command\" object in window.ui");
	
	refBuilder->get_widget("messages", messages);
	if (!messages)
		throw std::runtime_error("No \"messages\" object in window.ui");

	refBuilder->get_widget("hostname", hostname);
	if (!hostname)
		throw std::runtime_error("No \"hostname\" object in window.ui");

	// Connect the menu to the MenuButton m_gears
	auto menu_builder = Gtk::Builder::create_from_resource("/msmanager/app_menu.ui");
	auto menu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(menu_builder->get_object("appmenu"));
	if (!menu)
		throw std::runtime_error("No \"menu\" object in app_menu.ui");

	gears->set_menu_model(menu);

	menu_builder = Gtk::Builder::create_from_resource("/msmanager/right_click_menus.ui");
	menu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(menu_builder->get_object("user_menu"));
	if (!menu)
		throw std::runtime_error("No \"user_menu\" object in right_click_menus.ui");

	user_menu = new Gtk::Menu(menu);

	adjustment = messages_list->get_adjustment();

	// Bind settings.
	settings = Gio::Settings::create("msmanager");
	settings->bind("ip", hostname->property_label());
	
	// Connect signal handlers.
	command      ->signal_activate     ().connect(sigc::mem_fun(*this, &ExampleAppWindow::on_command_enter));
	messages_list->signal_size_allocate().connect(sigc::mem_fun(*this, &ExampleAppWindow::on_scroll_down  ));

	auto user_menu_group = Gio::SimpleActionGroup::create();

	user_menu_group->add_action("kick"     , sigc::mem_fun(*this, &ExampleAppWindow::on_user_kick     ));
	user_menu_group->add_action("ban"      , sigc::mem_fun(*this, &ExampleAppWindow::on_user_ban      ));
	user_menu_group->add_action("creative" , sigc::mem_fun(*this, &ExampleAppWindow::on_user_creative ));
	user_menu_group->add_action("survival" , sigc::mem_fun(*this, &ExampleAppWindow::on_user_survival ));
	user_menu_group->add_action("spectator", sigc::mem_fun(*this, &ExampleAppWindow::on_user_spectator));

	user_menu->insert_action_group("user_menu", user_menu_group);

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
	if (image.size() != 0)
	{
		auto stream = Gio::MemoryInputStream::create();
		stream->add_data(image.data(), image.size());
		server_icon->set(Gdk::Pixbuf::create_from_stream(stream));
	}
	else
	{
		server_icon->clear();
	}
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
		auto row   = Gtk::make_managed<Gtk::EventBox>(    );
		auto box   = Gtk::make_managed<Gtk::Box     >(    );
		auto label = Gtk::make_managed<Gtk::Label   >(user);

		label->set_alignment(0);
		label->show();

		box->pack_start(*label);
		box->show();

		row->add(*box);
		row->show();

		row->signal_button_press_event().connect(sigc::mem_fun(*this, &ExampleAppWindow::on_button_press));

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

void ExampleAppWindow::add_command(std::string command)
{
	auto label = Gtk::make_managed<Gtk::Label>(command);

	label->set_alignment(0);
	label->show();

    messages_list->add(*label);

	adjustment->set_value(adjustment->get_upper()); 
}

void ExampleAppWindow::on_command_enter()
{
	server_connection->SendCommand(command->get_text());
	command->set_text("");
}

void ExampleAppWindow::on_scroll_down(Gtk::Allocation allocation)
{
	auto upper = adjustment->get_upper();
	adjustment->set_value(upper);
}

bool ExampleAppWindow::on_button_press(GdkEventButton* event)
{
	if((event->type == GDK_BUTTON_PRESS) && (event->button == 3))
	{
		if(!user_menu->get_attach_widget())
			user_menu->attach_to_widget(*this);

		auto row = online_users->get_row_at_y(event->y);
		if (row)
		{
			auto box = (Gtk::EventBox*)row->get_children()[0];
			user_selected = (Gtk::Box*)box->get_child();
			user_menu->popup(event->button, event->time);
		}
		return true;
	}
	else
	{
		return false;
	}
}

void ExampleAppWindow::on_user_command(std::string command_base)
{
	Gtk::Label* label = (Gtk::Label*)user_selected->get_children()[0];
	std::string user_name = label->get_text();

	user_selected = nullptr;

	std::string command;
	command.resize(command_base.size() + user_name.size());
	sprintf((char*)command.data(), command_base.c_str(), user_name.c_str());

	server_connection->SendCommand(command);
}

void ExampleAppWindow::on_user_kick     () { on_user_command("kick %s"              ); }
void ExampleAppWindow::on_user_ban      () { on_user_command("ban %s"               ); }
void ExampleAppWindow::on_user_creative () { on_user_command("gamemode creative %s" ); }
void ExampleAppWindow::on_user_survival () { on_user_command("gamemode survival %s" ); }
void ExampleAppWindow::on_user_spectator() { on_user_command("gamemode spectator %s"); }
