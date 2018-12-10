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
	width: 475
	height: 600
	title: qsTr("Table view")
	Material.theme: Material.Dark
	Material.accent: Material.DeepOrange
	
	property string whoLog: "Table window"
	property LogWindow logWindow
	property App mainApp
	property Settings settings
	property bool autoScroll: false

	Timer {
		id: clearValues
		interval: 5000
		running: true
		repeat: true
		onTriggered: {
			removeRedundantData()
		}
	}

	FileDialog {
		id: dialogFileSave
		title: qsTr("Please choose a save location")
		folder: shortcuts.documents
		selectExisting: false
		onAccepted: {
			saveData(tabBar.currentItem.text, dialogFileSave.fileUrl)
		}
		nameFilters: [ qsTr("Comma-Separated Values ") + "(*.csv)", qsTr("All files ") + "(*)" ]
	}

	DialogAlert {
		id: dialogAlert
		x: getDialogCenteredX(width)
		y: getDialogCenteredY(height)
	}

	function getTabData(tabName) {
		for (var i = 0; i < contentModel.count; i++) {
			if (contentModel.get(i).name === tabName) {
				return contentModel.get(i).submodel
			}
		}
		return null
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

	function saveData(tabName, fileUrl) {
		var data = getTabData(tabName)
		if (data !== null) {
			var f = mainApp.createFile(fileUrl)
			if (f.openWrite()) {
				for (var i = 0; i < data.count; i++) {
					f.write(data.get(i).time + settings.csvSeparator +
							data.get(i).value + settings.csvSeparator +
							data.get(i).error + settings.csvLineEnd)
				}
				f.flush();
				f.close();
				log(qsTr("Data '") + tabName + qsTr("' saved to ") + fileUrl)
			} else {
				log(qsTr("Error. Unable to write data to file: ") + fileUrl)
				showAlert(
					qsTr("Error"),
					qsTr("Unable to write data to file.")
				)
			}
			mainApp.destroyFile(f)
		}
	}

	function removeRedundantData() {
		for (var i = 0; i < contentModel.count; i++) {
			var sub = contentModel.get(i).submodel
			var delCount = sub.count - appSettings.traceTableMaxDataSize
			if (delCount > 0) {
				log(qsTr("Clear ") + delCount + qsTr(" values in: ") + contentModel.get(i).name + ".")
				sub.remove(0, delCount)
			}
		}
	}

	function clearTabData(tabName) {
		var data = getTabData(tabName)
		if (data !== null) {
			data.clear()
			log(qsTr("Parameter ") + tabName + qsTr(": data cleared."))
		}
	}

	function clearAllData() {
		for (var i = 0; i < contentModel.count; i++) {
			contentModel.get(i).submodel.clear()
		}
		log(qsTr("All parameters: data cleared."))
	}

	function log(text) {
		logWindow.log(whoLog, text)
	}

	function addParameter(tabName) {
		contentModel.append({name: tabName, submodel: []})
		log(qsTr("Parameter added: ") + tabName + ".")
	}
	
	function removeParameter(tabName) {
		for (var i = 0; i < contentModel.count; i++) {
			if (contentModel.get(i).name === tabName) {
				contentModel.remove(i)
				log(qsTr("Parameter removed: ") + tabName + ".")
				return
			}
		}
	}
	
	function addValue(tabName, dateTimeFormat, date, value, code) {
		var data = getTabData(tabName)
		if (data !== null) {
			data.append({time: Qt.formatDateTime(date, dateTimeFormat), value: value, error: ((code !== 0) ? code : "OK")})
		}
	}

	ListModel {
		id: contentModel
		/*ListElement {name: "param name"; submodel: []}*/
	}

	header: ToolBar {
		implicitHeight: contentHeight
		RowLayout {
			anchors.fill: parent
			WSToolButton {
				iconSource: "qrc:/icon/save_to_file.png"
				iconSourceDisabled: "qrc:/icon/save_to_file_dis.png"
				ToolTip.text: qsTr("Save data to file.")
				onClicked: {
					if (tabBar.currentItem !== null) {
						dialogFileSave.open()
					}
				}
			}
			WSToolButton {
				iconSource: "qrc:/icon/clear_tab.png"
				iconSourceDisabled: "qrc:/icon/clear_tab.png"
				ToolTip.text: qsTr("Clear data in current tab.")
				onClicked: {
					if (tabBar.currentItem !== null) {
						clearTabData(tabBar.currentItem.text)
					}
				}
			}
			WSToolButton {
				iconSource: "qrc:/icon/clear_all.png"
				iconSourceDisabled: "qrc:/icon/clear_all.png"
				ToolTip.text: qsTr("Clear all data.")
				onClicked: clearAllData()
			}
			Item {
				Layout.fillWidth: true
			}
		}
	}

	ColumnLayout {
		anchors.fill: parent
		ScrollView {
			id: tabScroll
			clip: true
			Layout.fillWidth: true
			Layout.alignment: Qt.AlignTop
			ScrollBar.horizontal.policy: ScrollBar.AsNeeded
			ScrollBar.vertical.policy: ScrollBar.AlwaysOff

			TabBar {
				id: tabBar
				anchors.top: parent.top
				anchors.left: parent.left
				anchors.right: parent.right
				onCurrentIndexChanged: {
					if (currentIndex < 0 && count > 0) {
						currentIndex = 0
					}
				}

				Repeater {
					model: contentModel
					TabButton {
						text: name
						ToolTip {
							delay: Qt.styleHints.mousePressAndHoldInterval
							visible: hovered
							text: name
						}
					}
				}
			}
		}
		
		StackLayout {
			id: tableContainer
			Layout.fillWidth: true
			Layout.fillHeight: true
			currentIndex: tabBar.currentIndex

			Repeater {
				model: contentModel
				ScrollView {
					id: tableScroll
					clip: true
					ScrollBar.horizontal.policy: ScrollBar.AsNeeded
					ScrollBar.vertical.policy: ScrollBar.AsNeeded
					onContentHeightChanged: {
						if (autoScroll) {
							ScrollBar.vertical.position = 1 - ScrollBar.vertical.size
						}
					}
					Item {
						implicitWidth: tt.width
						implicitHeight: tt.height
						TableTrace {
							id: tt
							model: submodel
						}
					}
				}
			}
		}
	}
}

