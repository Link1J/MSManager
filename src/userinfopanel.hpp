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
#include <QTableWidgetItem>
#include <QTimer>

#include "serverconnection.h"

namespace Ui {
    class UserInfoPanel;
}

class UserInfoPanel : public QMainWindow
{
    Q_OBJECT

public:
    UserInfoPanel (QWidget *parent = 0);
    ~UserInfoPanel(                   );

    void AddServerConnection(ServerConnection*);

    void RCONDisabled();
    void RCONEnabled ();

public slots:
	void SetUser (QString);
    
    void UserKick(       );
    void UserBan (       );

    void Update  (       );

private:    
    template <typename... Args>
    std::string SendUserCommand(std::string command_base, Args... args);

    QScopedPointer<Ui::UserInfoPanel> ui;
    ServerConnection* connection;
    QTimer* timer;

    std::string selected_user;

    bool in_scoreboard_update;
    bool dont_update_scoreboard;
};