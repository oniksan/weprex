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
import QtQuick.Controls.Material 2.12

RowLayout {
	property alias number: number
	property alias type: type
	SpinBox {
		id: number
		Connections {
			target: number.contentItem
			onTextEdited: {
				var v = number.vft(number.contentItem.text)
				if (v > number.to) {
					number.contentItem.text = number.tfv(number.to)
				} else if (v < number.from) {
					number.contentItem.text = number.tfv(number.from)
				}
				number.value = number.vft(number.contentItem.text)
			}
		}

		function tfv(v) {
			if (represent === "Hex") {
				return "0x" + v.toString(16).toUpperCase()
			} else if (represent === "Dec") {
				return v.toString(10)
			} else {
				return "undefined"
			}
		}

		function vft(t) {
			if (represent === "Hex") {
				return parseInt(t, 16)
			} else if (represent === "Dec") {
				return parseInt(t, 10)
			} else {
				return 0
			}
		}

		textFromValue: tfv
		valueFromText: vft

		validator: RegExpValidator {
			regExp: {
				new RegExp("((0x)([0-9]|[A-F]|[a-f]){1,4})|(([0-9]){1,5})", "i")
			}
		}

		wheelEnabled: true
		from: 0
		to: 65535
		editable: true
		wrap: true

		ToolTip.visible: hovered
		ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
		ToolTip.text: {
			if (represent === "Dec") {
				return qsTr("In Hex: ") + value.toString(16).toUpperCase()
			} else if (represent === "Hex") {
				return qsTr("In Dec: ") + value.toString(10)
			} else {
				return "undefined"
			}
		}
		property string represent

		onValueModified: {
			contentItem.text = tfv(value)
		}
		Layout.fillWidth: true

	}
	ComboBox {
		id: type
		model: ["Hex", "Dec"]
		onActivated: {
			number.represent = currentText
			number.contentItem.text = number.tfv(number.value)
		}
		onVisibleChanged: {
			number.represent = currentText
			number.contentItem.text = number.tfv(number.value)
		}
		//Layout.fillWidth: true
	}
}
