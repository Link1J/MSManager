#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Link1J");
    QCoreApplication::setOrganizationDomain("link1j.me");
    QCoreApplication::setApplicationName("msmanager");

    MainWindow main_window;
    main_window.show();

    return app.exec();
}