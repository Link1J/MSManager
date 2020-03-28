#include "settingsdialog.hpp"
#include "ui_settingsdialog.h"

#include <iostream>

SettingsDialog::SettingsDialog(QWidget *parent)
  : QDialog(parent, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint), ui(new Ui::SettingsDialog)
{
	ui->setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);
}

SettingsDialog::~SettingsDialog()
{
	// Do nothing
}