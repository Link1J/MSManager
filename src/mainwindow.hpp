/*
    MSManager - A Minecraft server manager
    Copyright (C) 2020 Link1J

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

#include <QMainWindow>
#include <QScopedPointer>
#include <QItemSelection>

#include "serverconnection.h"
#include "usermodel.hpp"
#include "pluginmodel.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow (QWidget *parent = 0);
    ~MainWindow(                   );

	void UpdateMotd            (std::string motd        );
	void UpdateOnline          (int online              );
	void UpdateMax             (int max                 );
	void UpdateImage           (std::string image       );
	void UpdateType            (std::string server_type );
	void UpdateModName         (std::string mod_name    );

	void AddCommand            (std::string command     );
    void AddUser               (std::string user        );
    void AddPlugin             (std::string plugin      );

    void RemoveUser            (std::string user        );
    void RemovePlugin          (std::string plugin      );

    void RCONDisabled          (                        );
    void RCONEnabled           (                        );

public slots:
    void SendCommand (                 );

    void OpenSettings(                 );
    void OpenAbout   (                 );

    void UserSelected(QItemSelection   );

protected:
    void timerEvent(QTimerEvent* event);

private:
    QScopedPointer<Ui::MainWindow> ui;
    QScopedPointer<UserModel> userlist;
    QScopedPointer<PluginModel> pluginlist;

    ServerConnection connection;
};