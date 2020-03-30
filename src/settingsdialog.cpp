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

#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include <iostream>

#include <QSettings>

SettingsDialog::SettingsDialog(QWidget *parent)
  : QDialog(parent, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint), ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
	
	QSettings settings;

	ui->server_ip    ->setText (settings.value("server_ip"    ).toString());
	ui->server_port  ->setValue(settings.value("server_port"  ).toInt   ());
	ui->query_port   ->setValue(settings.value("query_port"   ).toInt   ());
	ui->rcon_port    ->setValue(settings.value("rcon_port"    ).toInt   ());
	ui->rcon_password->setText (settings.value("rcon_password").toString());
}

SettingsDialog::~SettingsDialog()
{
	QSettings settings;

	settings.setValue("server_ip"    , ui->server_ip    ->text ());
	settings.setValue("server_port"  , ui->server_port  ->value());
	settings.setValue("query_port"   , ui->query_port   ->value());
	settings.setValue("rcon_port"    , ui->rcon_port    ->value());
	settings.setValue("rcon_password", ui->rcon_password->text ());
}