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
import QtQuick.Controls.Material 2.3

ListView {
	id: listView

	property int rowColor: Material.BlueGrey
	property int rowAlternateShade: Material.Shade400
	property int hoverShade: Material.Shade800
	property int titleColor: Material.Indigo

	implicitHeight: contentHeight

	function valToText(key, val) {
		switch (key) {
			case "time":
				return val
			case "value":
				return val
			case "error":
				return val
		}
	}

	/*ListModel {
		id: example
		ListElement {time: qsTr("10.05.2018 02:13:55.285"); value: "154.216"; error: 0}
	}*/

	TextMetrics {
		id: tableRowTM
	}

	ListModel {
		id: headerModel
		ListElement {name: qsTr("№"); key: "number"; presetWidth: "50"}
		ListElement {name: qsTr("Time"); key: "time"; presetWidth: "200"}
		ListElement {name: qsTr("Value"); key: "value"; presetWidth: "150"}
		ListElement {name: qsTr("Error"); key: "error"; presetWidth: "70"}
	}

	header: Row {
		id: headerRow
		spacing: 1

		onWidthChanged: {
			listView.implicitWidth = width
		}

		function itemAt(index) {
			return headerRepeater.itemAt(index)
		}

		Repeater {
			id: headerRepeater
			model: headerModel
			ItemDelegate {

				property real baseWidth
				property bool isBaseWidth

				text: name
				font.bold: true
				padding: 10
				height: 30

				Component.onCompleted: {
					if (presetWidth !== undefined) {
						width = presetWidth
					}
					baseWidth = width
					isBaseWidth = true
				}

				MouseArea {
					width: parent.width
					height: parent.height

					onDoubleClicked: {
						var key = headerModel.get(index).key
						var maxWidth = baseWidth
						if (isBaseWidth) {
							// @TODO font
							tableRowTM.font = font
							for (var i = 0; i < listView.model.count; i++) {
								tableRowTM.text = valToText(key, listView.model.get(i)[key])
								if (tableRowTM.tightBoundingRect.width + parent.padding * 2 > maxWidth) {
									maxWidth = tableRowTM.tightBoundingRect.width + parent.padding * 2
								}
							}
						}
						if (maxWidth > baseWidth) {
							isBaseWidth = false
						} else {
							isBaseWidth = true
						}
						parent.width = maxWidth
					}
				}
				ToolTip {
					delay: Qt.styleHints.mousePressAndHoldInterval
					visible: parent.hovered
					text: qsTr("Double-click the column header to change the width of the column according to the size of the content.")
				}
				background: Rectangle { color: Material.color(listView.titleColor) }
			}
		}
	}

	//model: example
	delegate: Column {
		property int row: index

		Row {
			id: tableRow
			spacing: listView.headerItem.spacing
			property bool rowHovered: false

			Component.onCompleted: {
				for (var i = 0; i < children.length; i++) {
					children[i].onHovered.connect(rowWasHovered)
				}
			}

			onWidthChanged: {
				rowDivider.width = width
			}

			function rowWasHovered(hov) {
				rowHovered = hov
			}

			// Number
			TableItemDelegate {
				text: index + 1
				width: listView.headerItem.itemAt(0).width
				horizontalAlignment: Text.AlignHCenter
				backDefaultColor: listView.rowColor
				backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, listView.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, listView.rowAlternateShade))
			}

			// Time
			TableItemDelegate {
				text: valToText("time", time)
				width: listView.headerItem.itemAt(1).width
				horizontalAlignment: Text.AlignRight
				backDefaultColor: listView.rowColor
				backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, listView.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, listView.rowAlternateShade))
			}

			// Value
			TableItemDelegate {
				text: valToText("value", value)
				width: listView.headerItem.itemAt(2).width
				horizontalAlignment: Text.AlignRight
				backDefaultColor: listView.rowColor
				backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, listView.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, listView.rowAlternateShade))
			}

			// Error
			TableItemDelegate {
				text: valToText("error", error)
				width: listView.headerItem.itemAt(3).width
				horizontalAlignment: Text.AlignHCenter
				backDefaultColor: listView.rowColor
				backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, listView.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, listView.rowAlternateShade))
			}
		}

		Item {
			id: rowDivider
			width: parent.width
			height: 1
		}
	}
}

