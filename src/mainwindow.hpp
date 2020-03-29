#pragma once

#include <QMainWindow>
#include <QScopedPointer>

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
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

	void UpdateMotd     (std::string motd        );
	void UpdateOnline   (int online              );
	void UpdateMax      (int max                 );
	void UpdateImage    (std::string image       );
	void UpdateType     (std::string server_type );
	void UpdateModName  (std::string mod_name    );

	void AddCommand     (std::string command     );
    void AddUser        (std::string user        );
    void AddPlugin      (std::string plugin      );

    void RemoveUser     (std::string user        );
    void RemovePlugin   (std::string plugin      );

public slots:
    void SendCommand    (      );
    void UserContextMenu(QPoint);

    void UserKick       (      );
    void UserBan        (      );
    void UserCreative   (      );
    void UserSurvival   (      );
    void UserSpectator  (      );

    void OpenSettings   (      );
    void OpenAbout      (      );

protected:
    void timerEvent(QTimerEvent* event);

private:
    template <typename... Args>
    void SendUserCommand(std::string command_base, Args... args);

    QScopedPointer<Ui::MainWindow> ui;
    ServerConnection connection;
    QScopedPointer<UserModel> userlist;
    std::string userSelected;
    QScopedPointer<PluginModel> pluginlist;
};