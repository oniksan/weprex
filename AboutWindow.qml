/****************************************************************************

  This file is part of the Webstella protocols exchange (Weprex) software.

  Copyright (C) 2018 Oleg Malyavkin.
  Contact: weprexsoft@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/

import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Window 2.11
import QtQuick.Layouts 1.3
import ru.webstella.weprex 1.0

ApplicationWindow {
	id: about
	visible: false
	color: "transparent"
	title: "About"
	modality: Qt.ApplicationModal
	flags: Qt.SplashScreen
	x: (Screen.width - splashImage.width) / 2
	y: (Screen.height - splashImage.height) / 2
	width: splashImage.width
	height: splashImage.height

	property App mainApp

	background: Image {
		id: splashImage
		source: "qrc:/icon/splash.jpg"
	}

	function showAbout() {
		about.visible = true
	}

	Item {
		anchors.fill: parent
		ColumnLayout {
			anchors.left: parent.left
			anchors.right: parent.right
			anchors.bottom: parent.bottom
			Repeater {
				model: [
					mainApp.fullName() + " (" + mainApp.name() + ")",
					"Version: " + mainApp.version(),
					"Build: " + mainApp.build(),
					"Copyrights: " + mainApp.copyrights(),
					mainApp.additionalInfo(),
					"License: " + mainApp.license(),
					"Contact us: " + mainApp.contacts()
				]
				Label {
					Layout.fillWidth: true
					horizontalAlignment: Text.AlignLeft
					leftPadding: 200
					font.pixelSize: 12
					color: "white"
					text: modelData
				}
			}
			Item {
				Layout.fillWidth: true
				height: 20
			}
		}
	}

	MouseArea {
		anchors.fill: parent
		onClicked: {
			about.visible = false
		}
	}
}
