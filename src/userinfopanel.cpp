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

#include "userinfopanel.hpp"
#include "ui_userinfopanel.h"

#include <fmt/format.h>

#include <QInputDialog>

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
	: QGroupBox(parent)
	, timer(new QTimer(this))
	, ui(new Ui::UserInfoPanel)
{
	ui->setupUi(this);

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

	setTitle(user);
	UpdatePlayerScoreboard();
}

void UserInfoPanel::Update()
{
	UpdatePlayerScoreboard();
}

void UserInfoPanel::ScoreboardValue(QTableWidgetItem* value_item)
{
	if (in_scoreboard_update)
		return;
	if (value_item->column() != 1)
		return;

	int row = value_item->row();
	auto name_item = ui->scoreboard_table->item(row, 0);

	auto name  = name_item ->text().toStdString();
	auto value = value_item->text().toStdString();

	SendUserCommand("scoreboard players set {} {} {}", name, value);

	dont_update_scoreboard = false;

	UpdatePlayerScoreboard();
}

void UserInfoPanel::ScoreboardEnter(QTableWidgetItem* value_item)
{
	if (value_item->column() != 1)
		return;

	dont_update_scoreboard = true;
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

void UserInfoPanel::UpdatePlayerScoreboard()
{
	if (selected_user == "")
		return;
	if (dont_update_scoreboard)
		return;

	in_scoreboard_update = true;

	std::vector<std::string> scoreboard_objectives = split(SendUserCommand("scoreboard players list {0}"), '[');
	scoreboard_objectives.erase(scoreboard_objectives.begin());

	ui->scoreboard_table->setRowCount(scoreboard_objectives.size());

	int row = 0;
	for (auto objective : scoreboard_objectives)
	{
		auto values = split(objective, ':');
		values[0].erase(values[0].end  () - 1);
		values[1].erase(values[1].begin()    );

		auto name  = new QTableWidgetItem(QString::fromStdString(values[0]));
		auto value = new QTableWidgetItem(QString::fromStdString(values[1]));

		name->setFlags(name->flags() & ~Qt::ItemIsEditable);

		ui->scoreboard_table->setItem(row, 0, name );
		ui->scoreboard_table->setItem(row, 1, value);

		row++;
	}

	in_scoreboard_update = false;
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