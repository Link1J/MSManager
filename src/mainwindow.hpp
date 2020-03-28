#pragma once

#include <QMainWindow>
#include <QScopedPointer>

#include "serverconnection.h"
#include "usermodel.hpp"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

	void update_motd   (std::string motd              );
	void update_players(int online, int max           );
	void update_image  (std::vector<uint8_t> image    );
	void update_type   (std::string server_type       );

	void add_command   (std::string command           );
    void add_user      (std::string user              );

    void remove_user   (std::string user              );
    
    void on_user_command  (std::string command_base);


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
    QScopedPointer<Ui::MainWindow> ui;
    ServerConnection connection;
    QScopedPointer<UserModel> userlist;
    std::string userSelected;
};