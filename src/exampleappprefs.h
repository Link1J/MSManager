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

#ifndef GTKMM_EXAMPLEAPPPREFS_H_
#define GTKMM_EXAMPLEAPPPREFS_H_

#include <gtkmm.h>

class ExampleAppPrefs : public Gtk::Dialog
{
public:
  ExampleAppPrefs(BaseObjectType* cobject,
    const Glib::RefPtr<Gtk::Builder>& refBuilder);

  static ExampleAppPrefs* create(Gtk::Window& parent);

protected:
  Glib::RefPtr<Gtk::Builder> m_refBuilder;
  Glib::RefPtr<Gio::Settings> m_settings;
  Gtk::Entry* m_ip;
  Gtk::SpinButton* m_port;
  Gtk::SpinButton* m_query_port;
  Gtk::SpinButton* m_rcon_port;
  Gtk::Entry* m_rcon_password;
};

#endif /* GTKMM_EXAMPLEAPPPREFS_H_ */
