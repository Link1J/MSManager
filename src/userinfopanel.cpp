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

#include "userinfopanel.hpp"
#include "ui_userinfopanel.h"

#include <fmt/format.h>

#include <QInputDialog>
#include <QQuickView>
#include <QQmlEngine>
#include <QQmlContext>

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

UserInfoPanel::UserInfoPanel(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::UserInfoPanel)
	, timer(new QTimer(this))
{
	ui->setupUi(this);

	ui->model->engine()->rootContext()->setContextProperty("_window", ui->model);

	connect(timer, SIGNAL(timeout()), this, SLOT(Update()));

	in_scoreboard_update   = false;
	dont_update_scoreboard = false;
}

UserInfoPanel::~UserInfoPanel()
{
}

void UserInfoPanel::AddServerConnection(ServerConnection* connection)
{
	this->connection = connection;
}

void UserInfoPanel::SetUser(QString user)
{
	selected_user = user.toStdString();

	if(selected_user == "")
	{
		hide();
		timer->stop();
	}
	else
	{
		show();
		timer->start(5000);
	}

	setWindowTitle("User Info - " + user);
	Update();
}

void UserInfoPanel::Update()
{
	std::vector<std::string> values;

	values = split(SendUserCommand("scoreboard players get {} {}", "Air"   ), ' ');
	ui->air   ->setText(QString::fromStdString(values[2]));
	values = split(SendUserCommand("scoreboard players get {} {}", "XP"    ), ' ');
	ui->xp    ->setText(QString::fromStdString(values[2]));
	values = split(SendUserCommand("scoreboard players get {} {}", "Health"), ' ');
	ui->health->setText(QString::fromStdString(values[2]));
	values = split(SendUserCommand("scoreboard players get {} {}", "Armor" ), ' ');
	ui->armor ->setText(QString::fromStdString(values[2]));
	values = split(SendUserCommand("scoreboard players get {} {}", "Food"  ), ' ');
	ui->food  ->setText(QString::fromStdString(values[2]));
	values = split(SendUserCommand("scoreboard players get {} {}", "Deaths"), ' ');
	ui->deaths->setText(QString::fromStdString(values[2]));
}

template <typename... Args>
std::string UserInfoPanel::SendUserCommand(std::string command_base, Args... args)
{
	std::string command = fmt::format(
		command_base,
		selected_user,
		args...
	);
	return connection->SendCommand(command, false);
}

void UserInfoPanel::UserKick() 
{ 
	bool ok;
	QString text = QInputDialog::getText(this, tr("Reason"), tr("Reason for kick:"), QLineEdit::Normal, "", &ok);

	if (!ok)
		return;

	if (!text.isEmpty())
		SendUserCommand("kick {0} {1}", text.toStdString());
	else
		SendUserCommand("kick {0}");
}
void UserInfoPanel::UserBan() 
{ 
	bool ok;
	QString text = QInputDialog::getText(this, tr("Reason"), tr("Reason for ban:"), QLineEdit::Normal, "", &ok); 

	if (!ok)
		return;

	if (!text.isEmpty())
		SendUserCommand("ban {0} {1}", text.toStdString());
	else
		SendUserCommand("ban {0}");
}

void UserInfoPanel::RCONDisabled()
{
	ui->kick_user->setEnabled(false);
	ui->ban_user ->setEnabled(false);
}

void UserInfoPanel::RCONEnabled()
{
	ui->kick_user->setEnabled(true);
	ui->ban_user ->setEnabled(true);
}
