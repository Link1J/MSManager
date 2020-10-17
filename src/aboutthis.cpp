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

#include "aboutthis.hpp"
#include <iostream>

AboutThis::AboutThis(QWidget *parent)
	: QDialog(parent, Qt::MSWindowsFixedSizeDialogHint | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
	, icon_label    (new QLabel        {                          this    })
	, columns       (new QBoxLayout    { QBoxLayout::LeftToRight, this    })
	, info_rows     (new QBoxLayout    { QBoxLayout::TopToBottom          })
	, button_list   (new QBoxLayout    { QBoxLayout::LeftToRight          })
	, about_button  (new QPushButton   { "About"                , this    })
	, libs_button   (new QPushButton   { "Libs"                 , this    })
	, info_panel    (new QStackedLayout{                          this    })
	, about_panel   (new QScrollArea   {                          this    })
	, libs_panel    (new QScrollArea   {                          this    })
	, about_label   (new QLabel        {                          this    })
{
	const QSize icon_size { 64, 64 };

	setWindowTitle("About");
	setAttribute(Qt::WA_DeleteOnClose);

	about_label->setText(
		"<h2>About MSManager</h2>"
		"<p>Version 0.1.0.0</p>"
		"<p>A Minecraft Server Manager</p>"
		"<p>The license for external libs used is under the Libs tab. "
		"Click the Libs button to see them.</p>"

		"<p>Copyright (C) 2020 Jared Irwin</p>"

		"<p>This program is free software: you can redistribute it and/or modify "
		"it under the terms of the GNU General Public License as published by "
		"the Free Software Foundation, either version 3 of the License, or "
		"(at your option) any later version.</p>"

		"<p>This program is distributed in the hope that it will be useful, "
		"but WITHOUT ANY WARRANTY; without even the implied warranty of "
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
		"GNU General Public License for more details.</p>"
		
		"<p>You should have received a copy of the GNU General Public License "
		"along with this program.  If not, see <a>https://www.gnu.org/licenses/</a>.</p>"
	);
	about_label->setWordWrap         (true    );
	about_label->setOpenExternalLinks(true    );

	icon = parent->windowIcon();
	icon_label->setPixmap(icon.pixmap(icon.actualSize(icon_size)));
	icon_label->setFixedSize(icon_size);

	auto temp = new QBoxLayout{ QBoxLayout::TopToBottom };
	temp->setAlignment(Qt::AlignTop);
	temp->addWidget   (icon_label  );

	button_list->setAlignment(Qt::AlignRight);
	button_list->addWidget   (about_button  );
	button_list->addWidget   (libs_button   );
	
	about_panel->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	about_panel->setFixedWidth(500                          );
	about_panel->setWidget    (about_label                  );

	libs_panel->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	libs_panel->setFixedWidth(500                          );

	info_panel->setSizeConstraint (QLayout::SetFixedSize);
	info_panel->setContentsMargins(10, 10, 10, 10       );
	info_panel->addWidget         (about_panel          );
	info_panel->addWidget         (libs_panel           );
	
	info_rows->addItem(info_panel );
	info_rows->addItem(button_list);
	
	columns->addItem(temp     );
	columns->addItem(info_rows);

	QObject::connect(about_button, SIGNAL(pressed()), this, SLOT(UpdateStackLayout()));
	QObject::connect(libs_button , SIGNAL(pressed()), this, SLOT(UpdateStackLayout()));
}

AboutThis::~AboutThis()
{
}

void AboutThis::UpdateStackLayout()
{
	QObject* obj = sender();
	if (obj == about_button) { info_panel->setCurrentWidget(about_panel); }
	if (obj == libs_button ) { info_panel->setCurrentWidget(libs_panel ); }
}
