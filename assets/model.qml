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

import QtQuick 2.15
import QtQuick3D 1.15
import QtQuick3D.Materials 1.15
import QtQuick3D.Effects 1.15
import QtQuick3D.Helpers 1.15

import "qrc:/Models/Zombie"

View3D {
	anchors.fill: parent
	camera: camera
	renderMode: View3D.Offscreen

	environment: SceneEnvironment {
		clearColor: "transparent"
		backgroundMode: SceneEnvironment.Transparent
		antialiasingMode: SceneEnvironment.MSAA
		antialiasingQuality: SceneEnvironment.High
	}

	PointLight {
		position: Qt.vector3d(0, -100, 0);
		brightness: 1500
	}

	PerspectiveCamera {
		position: Qt.vector3d(0, 0, 800)
	}

	Node {
		position: Qt.vector3d(0, -400, 0);

		Zombie {
		}
	}
}