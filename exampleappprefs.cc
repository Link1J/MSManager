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

#include "exampleappprefs.h"
#include "exampleappwindow.h"
#include <stdexcept>

ExampleAppPrefs::ExampleAppPrefs(BaseObjectType* cobject,
  const Glib::RefPtr<Gtk::Builder>& refBuilder)
: Gtk::Dialog(cobject),
  m_refBuilder(refBuilder),
  m_settings(),
  m_ip(nullptr),
  m_port(nullptr),
  m_query_port(nullptr),
  m_rcon_port(nullptr),
  m_rcon_password(nullptr)
{
  m_refBuilder->get_widget("ip", m_ip);
  if (!m_ip)
    throw std::runtime_error("No \"ip\" object in prefs.ui");

  m_refBuilder->get_widget("port", m_port);
  if (!m_port)
    throw std::runtime_error("No \"port\" object in prefs.ui");

  m_refBuilder->get_widget("query_port", m_query_port);
  if (!m_query_port)
    throw std::runtime_error("No \"query_port\" object in prefs.ui");

  m_refBuilder->get_widget("rcon_port", m_rcon_port);
  if (!m_rcon_port)
    throw std::runtime_error("No \"rcon_port\" object in prefs.ui");

  m_refBuilder->get_widget("rcon_password", m_rcon_password);
  if (!m_rcon_password)
    throw std::runtime_error("No \"rcon_password\" object in prefs.ui");

  m_settings = Gio::Settings::create("msmanager");
  m_settings->bind("ip", m_ip->property_text());
  m_settings->bind("port", m_port->property_value());
  m_settings->bind("query-port", m_query_port->property_value());
  m_settings->bind("rcon-port", m_rcon_port->property_value());
  m_settings->bind("rcon-password", m_rcon_password->property_text());
}

//static
ExampleAppPrefs* ExampleAppPrefs::create(Gtk::Window& parent)
{
  // Load the Builder file and instantiate its widgets.
  auto refBuilder = Gtk::Builder::create_from_resource("/msmanager/prefs.ui");

  ExampleAppPrefs* dialog = nullptr;
  refBuilder->get_widget_derived("prefs_dialog", dialog);
  if (!dialog)
    throw std::runtime_error("No \"prefs_dialog\" object in prefs.ui");

  dialog->set_transient_for(parent);

  return dialog;
}
