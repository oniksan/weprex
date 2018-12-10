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
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.3
import ru.webstella.weprex 1.0
import QtQuick.Dialogs 1.2

ApplicationWindow {
	id: window
	visible: true
	width: 800
	height: 600
	title: qsTr("Application log")
	Material.theme: Material.Dark
	Material.accent: Material.DeepOrange

	property alias textArea: textArea
	property Settings settings
	property App mainApp

	FileDialog {
		id: dialogFileSave
		title: qsTr("Please choose a save location")
		folder: shortcuts.documents
		selectExisting: false
		onAccepted: {
			saveData(dialogFileSave.fileUrl)
		}
		nameFilters: [ qsTr("Text file ") + "(*.txt)", qsTr("All files ") + "(*)" ]
	}

	DialogAlert {
		id: dialogAlert
		x: getDialogCenteredX(width)
		y: getDialogCenteredY(height)
	}

	function getDialogCenteredX(w) {
		return (window.width - w) / 2
	}

	function getDialogCenteredY(h) {
		return (window.contentItem.height - h) / 2
	}

	function showAlert(title, message) {
		dialogAlert.title = title
		dialogAlert.message = message
		dialogAlert.visible = true
	}

	function saveData(fileUrl) {
		var f = mainApp.createFile(fileUrl)
		if (f.openWrite()) {
			f.write(textArea.text)
			f.flush();
			f.close();
		} else {
			log(qsTr("Error"), qsTr("Unable to write log to file: ") + fileUrl)
			showAlert(
				qsTr("Error"),
				qsTr("Unable to write log to file.")
			)
		}
		mainApp.destroyFile(f)
	}

	function log(title, text) {
		textArea.append(Qt.formatDateTime(new Date(), settings.dateTimeFormat) + "\t" + title + ": " + text)
	}

	header: ToolBar {
		implicitHeight: contentHeight
		RowLayout {
			anchors.fill: parent
			WSToolButton {
				iconSource: "qrc:/icon/save_to_file.png"
				iconSourceDisabled: "qrc:/icon/save_to_file_dis.png"
				ToolTip.text: qsTr("Save log to file.")
				onClicked: dialogFileSave.open()
			}
			WSToolButton {
				iconSource: "qrc:/icon/clear_all.png"
				iconSourceDisabled: "qrc:/icon/clear_all.png"
				ToolTip.text: qsTr("Clear log.")
				onClicked: textArea.clear()
			}
			Item {
				Layout.fillWidth: true
			}
		}
	}

	ScrollView {
		clip: true
		ScrollBar.horizontal.policy: ScrollBar.AsNeeded
		ScrollBar.vertical.policy: ScrollBar.AsNeeded
		anchors.fill: parent

		TextArea {
			id: textArea
			selectByKeyboard: true
			selectByMouse: true
			font.pixelSize: 14
			anchors.fill: parent
		}
	}
}
