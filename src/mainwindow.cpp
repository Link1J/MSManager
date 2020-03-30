#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QBuffer>
#include <QImageReader>
#include <QMessageBox>
#include <QSettings>

#include <iostream>

#include <fmt/format.h>

#include "settingsdialog.hpp"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
  , userlist(new UserModel)
  , pluginlist(new PluginModel)
{
    ui->setupUi(this);

	ui->users_list  ->setModel(userlist  .data());
	ui->plugins_list->setModel(pluginlist.data());

	ui->mod_name_group->hide();
	ui->version_group ->hide();
	ui->plugins_group ->hide();
	ui->user_info     ->hide();

	QSettings settings;
	ui->server_ip->setText(settings.value("server_ip").toString());

   	connect(ui->users_list->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(UserSelected(QItemSelection)));

	connection.window = this;
	connection.Reconnect();
	startTimer(1000);

	ui->user_info->AddServerConnection(&connection);
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
	QMessageBox::about(this, "About", "MSManager\nCreated by: Link1J\n");
}

void MainWindow::AddPlugin(std::string plugin)
{
	pluginlist->addPlugin(plugin);
	
	if (pluginlist->rowCount() > 0)
		ui->plugins_group->show();
}

void MainWindow::RemovePlugin(std::string plugin)
{
	pluginlist->removePlugin(plugin);

	if (pluginlist->rowCount() <= 0)
		ui->plugins_group->hide();
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
	if(indexes.isEmpty()) 
	{
		ui->user_info->SetUser("");
	} 
	else 
	{
		auto index = indexes.at(0);
		auto selected_user = userlist->getUser(index.row());
		ui->user_info->SetUser(QString::fromStdString(selected_user));
	}
}

void MainWindow::RCONDisabled()
{
	ui->tabs->setTabEnabled(2, false);

	ui->user_info->RCONDisabled();
}

void MainWindow::RCONEnabled()
{
	ui->tabs->setTabEnabled(2, true);

	ui->user_info->RCONEnabled();
}