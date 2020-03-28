#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QIcon app_icon;
    app_icon.addFile(":/Icons/AppIcon");

    app.setWindowIcon(app_icon);

    MainWindow main_window;
    main_window.show();

    return app.exec();
}