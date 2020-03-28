#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QIcon appIcon;
    appIcon.addFile(":/Icons/AppIcon");

    app.setWindowIcon(appIcon);

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}