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

#pragma once

#include <QDialog>
#include <QIcon>
#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QStackedLayout>
#include <QScrollArea>

class AboutThis : public QDialog
{
	Q_OBJECT

public:
	AboutThis (QWidget* parent = nullptr);
	~AboutThis(                         );

public slots:
	void UpdateStackLayout();

protected:

private:
	QIcon           icon        ;
	QLabel        * icon_label  ;
	QBoxLayout    * columns     ;
	QBoxLayout    * info_rows   ;
	QBoxLayout    * button_list ;
	QPushButton   * about_button;
	QPushButton   * libs_button ;
	QStackedLayout* info_panel  ;
	QScrollArea   * about_panel ;
	QScrollArea   * libs_panel  ;
	QLabel        * about_label ;
};