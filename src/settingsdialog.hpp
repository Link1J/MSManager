#pragma once

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog (QWidget *parent = 0);
    ~SettingsDialog(                   );

public slots:

protected:

private:
    QScopedPointer<Ui::SettingsDialog> ui;
};