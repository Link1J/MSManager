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