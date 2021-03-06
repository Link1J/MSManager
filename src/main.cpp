/*
    MSManager - A Minecraft server manager
    Copyright (C) 2020 Jared Irwin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "mainwindow.hpp"
#include <QApplication>
#include <QSettings>
#include <QStyleFactory>
#include <QSplashScreen>
#include <QSurfaceFormat>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QPixmap pixmap(":/Icons/AppIcon");
    QSplashScreen splash(pixmap);
    splash.show();
    app.processEvents();

    QCoreApplication::setOrganizationName  ("Link1J"   );
    QCoreApplication::setOrganizationDomain("link1j.me");
    QCoreApplication::setApplicationName   ("msmanager");
    
    MainWindow window;
    window.show();
    splash.finish(&window);
    return app.exec();
}