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

#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QBuffer>
#include <QImageReader>
#include <QMessageBox>
#include <QSettings>

#include <iostream>

#include <fmt/format.h>

#include "settingsdialog.hpp"
#include "userinfopanel.hpp"
#include "aboutthis.hpp"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
	, userlist(new UserModel)
	, pluginlist(new PluginModel)
	, user_info(new UserInfoPanel(this))
{
	ui->setupUi(this);

	ui->users_list  ->setModel(userlist  .data());
	ui->plugins_list->setModel(pluginlist.data());

	ui->mod_name_group->hide();
	ui->version_group ->hide();

	ui->tabs->setTabEnabled(3, false);

	QSettings settings;
	ui->server_ip->setText(settings.value("server_ip").toString());

   	connect(ui->users_list->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(UserSelected(QItemSelection)));

	connection.window = this;
	connection.Reconnect();
	startTimer(1000);

	user_info->AddServerConnection(&connection);
	//user_info->setModal(true);
}

MainWindow::~MainWindow()
{
	// Do nothing
}

void MainWindow::SendCommand()
{
	auto command = ui->command->text();
	connection.SendCommand(command.toStdString());
	ui->command->clear();
}

void MainWindow::UpdateImage(std::string image)
{
	if (image.size() != 0)
	{
		QByteArray string = QByteArray::fromStdString(image);
		QByteArray data = QByteArray::fromBase64(string);
		QBuffer qbuff(&data);
		QImageReader qimg;
		qimg.setDecideFormatFromContent(true);
		qimg.setDevice(&qbuff);
		QImage img = qimg.read();
		ui->server_icon->setPixmap(QPixmap::fromImage(img));
	}
	else
	{
		ui->server_icon->clear();
	}
}

void MainWindow::UpdateMotd(std::string motd)
{
	ui->server_motd->setText(QString::fromStdString(motd));
}

void MainWindow::UpdateOnline(int online)
{
	ui->users_online->setText(QString::number(online));
}

void MainWindow::UpdateMax(int max)
{
	ui->users_max->setText(QString::number(max));
}

void MainWindow::UpdateType(std::string server_type)
{
	if (server_type != "")
		ui->version_group->show();
	else
		ui->version_group->hide();
		
	ui->server_type->setText(QString::fromStdString(server_type));
}

void MainWindow::AddCommand(std::string command)
{
	ui->command_list->addItem(QString::fromStdString(command));
	ui->command_list->scrollToBottom();
}

void MainWindow::AddUser(std::string user)
{
	userlist->addUser(user);
}

void MainWindow::RemoveUser(std::string user)
{
	userlist->removeUser(user);
}

void MainWindow::timerEvent(QTimerEvent* event)
{
	connection.Update();
}

void MainWindow::OpenSettings() 
{
	auto settings_dialog = new SettingsDialog(this);
	settings_dialog->exec();

	QSettings settings;
	ui->server_ip->setText(settings.value("server_ip").toString());
	connection.Reconnect();
}

void MainWindow::OpenAbout() 
{
	auto about = new AboutThis{this};
    about->exec();
}

void MainWindow::OpenAboutQt() 
{
	QMessageBox::aboutQt(this);
}

void MainWindow::AddPlugin(std::string plugin)
{
	pluginlist->addPlugin(plugin);
	
	if (pluginlist->rowCount() > 0)
		ui->tabs->setTabEnabled(3, true);
}

void MainWindow::RemovePlugin(std::string plugin)
{
	pluginlist->removePlugin(plugin);

	if (pluginlist->rowCount() <= 0)
		ui->tabs->setTabEnabled(3, false);
}

void MainWindow::UpdateModName(std::string mod_name)
{
	if (mod_name != "")
		ui->mod_name_group->show();
	else
		ui->mod_name_group->hide();
		
	ui->mod_name->setText(QString::fromStdString(mod_name));	
}

void MainWindow::UserSelected(QItemSelection selection)
{
	auto indexes = selection.indexes();
	if (indexes.isEmpty()) 
	{
		user_info->hide();
	} 
	else 
	{
		auto index = indexes.at(0);
		auto selected_user = userlist->getUser(index.row());
		ui->users_list->clearSelection();
		
		user_info->SetUser(QString::fromStdString(selected_user));

    	user_info->show();
	}
}

void MainWindow::RCONDisabled()
{
	ui->tabs->setTabEnabled(2, false);

	//ui->user_info->RCONDisabled();
}

void MainWindow::RCONEnabled()
{
	ui->tabs->setTabEnabled(2, true);

	//ui->user_info->RCONEnabled();
}

void MainWindow::CreateCriticalMessageBox(QString title, QString message)
{
	QMessageBox::critical(
		this,
		title,
		message
	);
}

void MainWindow::CreateWarningMessageBox(QString title, QString message)
{
	QMessageBox::warning(
		this,
		title,
		message
	);
}