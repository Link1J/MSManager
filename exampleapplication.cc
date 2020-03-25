/* gtkmm example Copyright (C) 2016 gtkmm development team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "exampleapplication.h"
#include "exampleappwindow.h"
#include "exampleappprefs.h"
#include <iostream>
#include <exception>

ExampleApplication::ExampleApplication()
: Gtk::Application("msmanager.application", Gio::APPLICATION_HANDLES_OPEN)
{
}

Glib::RefPtr<ExampleApplication> ExampleApplication::create()
{
	return Glib::RefPtr<ExampleApplication>(new ExampleApplication());
}

ExampleAppWindow* ExampleApplication::create_appwindow()
{
	auto appwindow = ExampleAppWindow::create();
	server_connection.window = appwindow;
	server_connection.Reconnect();
	add_window(*appwindow);
	appwindow->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this,
		&ExampleApplication::on_hide_window), appwindow));

	sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &ExampleApplication::on_timeout), 0);
  	sigc::connection conn = Glib::signal_timeout().connect(my_slot, 500);

	appwindow->server_connection = &server_connection;

	return appwindow;
}

void ExampleApplication::on_startup()
{
	// Call the base class's implementation.
	Gtk::Application::on_startup();

	// Add actions and keyboard accelerators for the application menu.
	add_action("preferences", sigc::mem_fun(*this, &ExampleApplication::on_action_preferences));
	add_action("update"     , sigc::mem_fun(*this, &ExampleApplication::on_action_update     ));
	add_action("quit"       , sigc::mem_fun(*this, &ExampleApplication::on_action_quit       ));
	add_action("about"      , sigc::mem_fun(*this, &ExampleApplication::on_action_about      ));
	set_accel_for_action("app.quit", "<Ctrl>Q");

	auto refBuilder = Gtk::Builder::create();
	try
	{
		refBuilder->add_from_resource("/msmanager/app_menu.ui");
	}
	catch (const Glib::Error& ex)
	{
		std::cerr << "ExampleApplication::on_startup(): " << ex.what() << std::endl;
		return;
	}

	auto object = refBuilder->get_object("appmenu");
	auto app_menu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(object);
	if (app_menu)
		set_app_menu(app_menu);
	else
		std::cerr << "ExampleApplication::on_startup(): No \"appmenu\" object in app_menu.ui"
							<< std::endl;

	dialog.set_logo(Gdk::Pixbuf::create_from_resource("/msmanager/msmanager.png", -1, 40, true));
	dialog.set_program_name("Minecraft Server Manager");
	dialog.set_version("1.0.0");
	dialog.set_copyright("Link1J");
	dialog.set_comments("A tool for managing a Minecraft server.");
	dialog.set_license("LGPL");

	dialog.set_website("https://link1j.me");
	dialog.set_website_label("Link1J's Website");

	std::vector<Glib::ustring> list_authors;
	list_authors.push_back("Link1J");
	dialog.set_authors(list_authors);

	dialog.signal_response().connect(sigc::mem_fun(*this, &ExampleApplication::on_about_dialog_response));
}

void ExampleApplication::on_activate()
{
	try
	{
		// The application has been started, so let's show a window.
		appwindow = create_appwindow();
		appwindow->present();
		dialog.set_transient_for(*appwindow);
	}
	// If create_appwindow() throws an exception (perhaps from Gtk::Builder),
	// no window has been created, no window has been added to the application,
	// and therefore the application will stop running.
	catch (const Glib::Error& ex)
	{
		std::cerr << "ExampleApplication::on_activate(): " << ex.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "ExampleApplication::on_activate(): " << ex.what() << std::endl;
	}
}

void ExampleApplication::on_open(const Gio::Application::type_vec_files& files,
	const Glib::ustring& /* hint */)
{
	// The application has been asked to open some files,
	// so let's open a new view for each one.
	ExampleAppWindow* appwindow = nullptr;
	auto windows = get_windows();
	if (windows.size() > 0)
		appwindow = dynamic_cast<ExampleAppWindow*>(windows[0]);

	try
	{
		if (!appwindow)
			appwindow = create_appwindow();

		appwindow->present();
	}
	catch (const Glib::Error& ex)
	{
		std::cerr << "ExampleApplication::on_open(): " << ex.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "ExampleApplication::on_open(): " << ex.what() << std::endl;
	}
}

void ExampleApplication::on_hide_window(Gtk::Window* window)
{
	server_connection.dontTry = false;
	server_connection.Reconnect();
	delete window;
}

void ExampleApplication::on_action_preferences()
{
	try
	{
		server_connection.dontTry = true;
		auto prefs_dialog = ExampleAppPrefs::create(*get_active_window());
		prefs_dialog->present();

		// Delete the dialog when it is hidden.
		prefs_dialog->signal_hide().connect(sigc::bind<Gtk::Window*>(sigc::mem_fun(*this,
			&ExampleApplication::on_hide_window), prefs_dialog));
	}
	catch (const Glib::Error& ex)
	{
		std::cerr << "ExampleApplication::on_action_preferences(): " << ex.what() << std::endl;
	}
	catch (const std::exception& ex)
	{
		std::cerr << "ExampleApplication::on_action_preferences(): " << ex.what() << std::endl;
	}
}

void ExampleApplication::on_action_quit()
{
	// Gio::Application::quit() will make Gio::Application::run() return,
	// but it's a crude way of ending the program. The window is not removed
	// from the application. Neither the window's nor the application's
	// destructors will be called, because there will be remaining reference
	// counts in both of them. If we want the destructors to be called, we
	// must remove the window from the application. One way of doing this
	// is to hide the window. See comment in create_appwindow().
	auto windows = get_windows();
	for (auto window : windows)
		window->hide();

	// Not really necessary, when Gtk::Widget::hide() is called, unless
	// Gio::Application::hold() has been called without a corresponding call
	// to Gio::Application::release().
	quit();
}

void ExampleApplication::on_action_update()
{
	server_connection.Reconnect();
}

bool ExampleApplication::on_timeout(int a)
{
	server_connection.Reconnect();
	return true;
}


void ExampleApplication::on_action_about()
{
	dialog.show();
	dialog.present();
}


void ExampleApplication::on_about_dialog_response(int response_id)
{
	 switch (response_id)
	{
	case Gtk::RESPONSE_CLOSE:
	case Gtk::RESPONSE_CANCEL:
	case Gtk::RESPONSE_DELETE_EVENT:
		dialog.hide();
		break;
	default:
		std::cout << "Unexpected response!" << std::endl;
		break;
  }
}