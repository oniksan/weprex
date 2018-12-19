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

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

RowLayout {
	property alias dataType : dataType
	property alias signType : signType

	function setIndexes(view) {
		var i
		for (i = 0; i < dataType.model.count; i++) {
			var item = dataType.model.get(i)
			if (item.type === view.type &&
				item.represent === view.represent &&
				item.bytes === view.bytes
				) {
				dataType.currentIndex = i
			}
		}
		for (i = 0; i < signType.model.count; i++) {
			if (signType.model.get(i).sign === view.sign) {
				signType.currentIndex = i
			}
		}
	}

	ComboBox {
		id: dataType
		textRole: "text"
		model: ListModel {
			ListElement {text: qsTr("Integer 8 bit"); type: "int"; represent: "dec"; bytes: 1; sign: true; signedDefault: true}
			ListElement {text: qsTr("Integer 16 bit"); type: "int"; represent: "dec"; bytes: 2; sign: true; signedDefault: true}
			ListElement {text: qsTr("Integer 32 bit"); type: "int"; represent: "dec"; bytes: 4; sign: true; signedDefault: true}
			ListElement {text: qsTr("Integer 64 bit"); type: "int"; represent: "dec"; bytes: 8; sign: true; signedDefault: true}
			ListElement {text: qsTr("Hexidecimal"); type: "int"; represent: "hex"; bytes: 1; sign: false; signedDefault: false}
			ListElement {text: qsTr("Octal"); type: "int"; represent: "oct"; bytes: 1; sign: false; signedDefault: false}
			ListElement {text: qsTr("Binary"); type: "int"; represent: "bin"; bytes: 1; sign: false; signedDefault: false}
			ListElement {text: qsTr("Float IEEE-754"); type: "float"; represent: "dec"; bytes: 4; sign: false; signedDefault: true}
			ListElement {text: qsTr("Double IEEE-754"); type: "float"; represent: "dec"; bytes: 8; sign: false; signedDefault: true}
			ListElement {text: qsTr("ASCII character"); type: "text"; represent: "ascii"; bytes: 1; sign: false; signedDefault: false}
		}
		ToolTip.visible: hovered
		ToolTip.delay: 0
		ToolTip.text: {
			return model.get(currentIndex).bytes + " " + qsTr("byte")
		}
		Layout.fillWidth: true
		function signCheck() {
			if (currentIndex >= 0) {

				if (model.get(currentIndex).sign) {
					signType.enabled = true
				} else {
					if (model.get(currentIndex).signedDefault) {
						signType.currentIndex = 0
					} else {
						signType.currentIndex = 1
					}
					signType.enabled = false
				}
			}
		}
		onActivated: {
			signCheck()
		}
		onVisibleChanged: {
			signCheck()
		}
	}
	ComboBox {
		id: signType
		textRole: "text"
		model: ListModel {
			ListElement {text: qsTr("Signed"); sign: true}
			ListElement {text: qsTr("Unsigned"); sign: false}
		}
		//model: [qsTr("Signed"), qsTr("Unsigned")]
	}
}
