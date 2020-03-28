#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include <QBuffer>
#include <QImageReader>
#include <QMessageBox>
#include <QSettings>

#include <iostream>

#include "settingsdialog.hpp"

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow), userlist(new UserModel)
{
    ui->setupUi(this);

	ui->users_list->setModel(userlist.data());

	connection.window = this;
	connection.Reconnect();
	startTimer(1000);

	QSettings settings;
	ui->server_ip->setText(settings.value("server_ip").toString());


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

void MainWindow::update_image(std::vector<uint8_t> image)
{
	if (image.size() != 0)
	{
		QByteArray data = QByteArray::fromRawData((const char*)image.data(), image.size());
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

void MainWindow::update_motd(std::string motd)
{
	ui->server_motd->setText(QString::fromStdString(motd));
}

void MainWindow::update_players(int online, int max)
{
	ui->users_online->setText(QString::number(online));
	ui->users_max   ->setText(QString::number(max   ));
}

void MainWindow::update_type(std::string server_type)
{
	ui->server_type->setText(QString::fromStdString(server_type));
}

void MainWindow::add_command(std::string command)
{
	ui->command_list->addItem(QString::fromStdString(command));
	ui->command_list->scrollToBottom();
}

void MainWindow::add_user(std::string user)
{
	userlist->addUser(user);
}

void MainWindow::remove_user(std::string user)
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
	connection.Reconnect();
}

void MainWindow::on_user_command(std::string command_base)
{
	std::string command;
	command.resize(command_base.size() + userSelected.size());
	sprintf((char*)command.data(), command_base.c_str(), userSelected.c_str());

	userSelected = "";

	connection.SendCommand(command);
}

void MainWindow::UserKick     () { on_user_command("kick %s"              ); }
void MainWindow::UserBan      () { on_user_command("ban %s"               ); }
void MainWindow::UserCreative () { on_user_command("gamemode creative %s" ); }
void MainWindow::UserSurvival () { on_user_command("gamemode survival %s" ); }
void MainWindow::UserSpectator() { on_user_command("gamemode spectator %s"); }

void MainWindow::OpenSettings() 
{
	auto settings_dialog = new SettingsDialog(this);
    settings_dialog->exec();

	QSettings settings;
	ui->server_ip->setText(settings.value("server_ip").toString());
}

void MainWindow::OpenAbout() 
{
	QMessageBox::about(this, "About", "MSManager\nCreated by: Link1J\n");
}

