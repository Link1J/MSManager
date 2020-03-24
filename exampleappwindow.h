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

#ifndef GTKMM_EXAMPLEAPPWINDOW_H_
#define GTKMM_EXAMPLEAPPWINDOW_H_

#include <gtkmm.h>

class ExampleAppWindow : public Gtk::ApplicationWindow
{
public:
	ExampleAppWindow(BaseObjectType* cobject,
		const Glib::RefPtr<Gtk::Builder>& refBuilder);

	static ExampleAppWindow* create();

	void update_users  (std::vector<std::string> users);
	void update_motd   (std::string motd              );
	void update_players(int online, int max           );
	void update_image  (std::vector<uint8_t> image    );
	void update_type   (std::string server_type       );

protected:
	Glib::RefPtr<Gtk::Builder> refBuilder;
	Glib::RefPtr<Gio::Settings> settings;

	Gtk::MenuButton* gears;
	Gtk::ListBox* online_users;
	Gtk::ListBox* users_online;
	Gtk::HeaderBar* header;
	Gtk::Image* server_icon;
	Gtk::Label* motd;
	Gtk::Label* online_players;
	Gtk::Label* max_players;
	Gtk::Label* server_type;
};

#endif /* GTKMM_EXAMPLEAPPWINDOW_H */
