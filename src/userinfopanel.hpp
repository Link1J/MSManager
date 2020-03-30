#pragma once

#include <QGroupBox>
#include <QScopedPointer>
#include <QTableWidgetItem>
#include <QTimer>

#include "serverconnection.h"

namespace Ui {
    class UserInfoPanel;
}

class UserInfoPanel : public QGroupBox
{
    Q_OBJECT

public:
    UserInfoPanel (QWidget *parent = 0);
    ~UserInfoPanel(                   );

    void AddServerConnection(ServerConnection*);

    void RCONDisabled();
    void RCONEnabled ();

public slots:
	void SetUser        (QString          );
    
    void UserKick       (                 );
    void UserBan        (                 );

    void ScoreboardValue(QTableWidgetItem*);
    void ScoreboardEnter(QTableWidgetItem*);

    void Update         (                 );

private:    
    template <typename... Args>
    std::string SendUserCommand(std::string command_base, Args... args);

	void UpdatePlayerScoreboard();

    QScopedPointer<Ui::UserInfoPanel> ui;
    ServerConnection* connection;
    QTimer* timer;

    std::string selected_user;

    bool in_scoreboard_update;
    bool dont_update_scoreboard;
};