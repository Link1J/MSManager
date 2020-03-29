#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QBuffer>
#include <QImageReader>
#include <QMessageBox>
#include <QSettings>
#include <QInputDialog>

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

	QSettings settings;
	ui->server_ip->setText(settings.value("server_ip").toString());

	connection.window = this;
	connection.Reconnect();
	startTimer(1000);
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

void MainWindow::UserContextMenu(QPoint point)
{
	QMenu* contextMenu = new QMenu(tr("Context menu"), this);

	QAction* action1 = contextMenu->addAction("Kick");
	QAction* action2 = contextMenu->addAction("Ban" );

	QMenu* gamemode = contextMenu->addMenu("Gamemode");

	QAction* action3 = gamemode->addAction("Creative" );
	QAction* action4 = gamemode->addAction("Survival" );
	QAction* action5 = gamemode->addAction("Spectator");

	connect(action1, SIGNAL(triggered()), this, SLOT(UserKick     ()));
	connect(action2, SIGNAL(triggered()), this, SLOT(UserBan      ()));
	connect(action3, SIGNAL(triggered()), this, SLOT(UserCreative ()));
	connect(action4, SIGNAL(triggered()), this, SLOT(UserSurvival ()));
	connect(action5, SIGNAL(triggered()), this, SLOT(UserSpectator()));

	auto index = ui->users_list->indexAt(point);
	userSelected = userlist->getUser(index.row());

	contextMenu->exec(mapToGlobal(point));
}

void MainWindow::timerEvent(QTimerEvent* event)
{
	connection.Update();
}

template <typename... Args>
void MainWindow::SendUserCommand(std::string command_base, Args... args)
{
	std::string command = fmt::format(
		command_base,
		userSelected,
		args...
	);
	userSelected = "";
	connection.SendCommand(command);
}

void MainWindow::UserKick() 
{ 
	QString text = QInputDialog::getText(this, tr("Reason"), tr("Reason for kick:")); 
	SendUserCommand("kick {0} {1}", text.toStdString()); 
}
void MainWindow::UserBan() 
{ 
	QString text = QInputDialog::getText(this, tr("Reason"), tr("Reason for ban:")); 
	SendUserCommand("ban {0} {1}", text.toStdString()); 
}
void MainWindow::UserCreative () { SendUserCommand("gamemode creative {0}" ); }
void MainWindow::UserSurvival () { SendUserCommand("gamemode survival {0}" ); }
void MainWindow::UserSpectator() { SendUserCommand("gamemode spectator {0}"); }

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