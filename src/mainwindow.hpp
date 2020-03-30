#pragma once

#include <QMainWindow>
#include <QScopedPointer>
#include <QItemSelection>

#include "serverconnection.h"
#include "usermodel.hpp"
#include "pluginmodel.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow (QWidget *parent = 0);
    ~MainWindow(                   );

	void UpdateMotd            (std::string motd        );
	void UpdateOnline          (int online              );
	void UpdateMax             (int max                 );
	void UpdateImage           (std::string image       );
	void UpdateType            (std::string server_type );
	void UpdateModName         (std::string mod_name    );

	void AddCommand            (std::string command     );
    void AddUser               (std::string user        );
    void AddPlugin             (std::string plugin      );

    void RemoveUser            (std::string user        );
    void RemovePlugin          (std::string plugin      );

    void RCONDisabled          (                        );
    void RCONEnabled           (                        );

public slots:
    void SendCommand (                 );

    void OpenSettings(                 );
    void OpenAbout   (                 );

    void UserSelected(QItemSelection   );

protected:
    void timerEvent(QTimerEvent* event);

private:
    QScopedPointer<Ui::MainWindow> ui;
    QScopedPointer<UserModel> userlist;
    QScopedPointer<PluginModel> pluginlist;

    ServerConnection connection;
};