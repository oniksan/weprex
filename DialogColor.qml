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

Dialog {
	id: colorDialog
	title: qsTr("Pick a color")
	visible: false
	property var acceptableObject

	property color colorValue: hsba(hueSlider.value, colorPicker.saturation, colorPicker.brightness, 1.0)
	property color initColorValue

	onVisibleChanged: {
		if (acceptableObject !== null) {
			acceptableObject.accepted = false
		}
	}

	onRejected: {
		acceptableObject.accepted = false
	}

	onAccepted: {
		acceptableObject.accepted = true
	}

	onInitColorValueChanged: {
		var lightness = initColorValue.hslLightness * 2
		var sb = initColorValue.hslSaturation * ((lightness <= 1) ? lightness : 2 - lightness)
		var b = (lightness + sb) / 2
		var s = sb / b
		hueSlider.value = initColorValue.hslHue
		pickerCursor.x = s * colorPicker.width
		pickerCursor.y = (1 - b) * colorPicker.height
	}

	Component.onCompleted: {
		closePolicy = Popup.CloseOnEscape
	}

	function hsba(h, s, b, a) {
		var lightness = (2 - s) * b
		var satHSL = s * b / ((lightness <= 1) ? lightness : 2 - lightness)
		lightness /= 2
		var c = Qt.hsla(h, satHSL, lightness, a)
		return Qt.hsla(h, satHSL, lightness, a)
	}

	contentItem: ColumnLayout {
		Item {
			id: colorPicker
			property color hueColor : hsba(hueSlider.value, 1.0, 1.0, 1.0)
			property real saturation : pickerCursor.x/width
			property real brightness : 1 - pickerCursor.y/height

			Layout.alignment: Qt.AlignHCenter
			width: 200
			height: 200

			Rectangle {
				anchors.fill: parent
				rotation: -90
				gradient: Gradient {
					GradientStop { position: 0.0; color: "#FFFFFF" }
					GradientStop { position: 1.0; color: colorPicker.hueColor }
				}
			}
			Rectangle {
				anchors.fill: parent
				gradient: Gradient {
					GradientStop { position: 1.0; color: "#FF000000" }
					GradientStop { position: 0.0; color: "#00000000" }
				}
			}
			Item {
				id: pickerCursor
				property int r : 8
				Rectangle {
					x: -parent.r
					y: -parent.r
					width: parent.r*2
					height: parent.r*2
					radius: parent.r
					border.color: "black"
					border.width: 2
					color: "transparent"
					Rectangle {
						anchors.fill: parent
						anchors.margins: 2
						border.color: "white"
						border.width: 2
						radius: width/2
						color: "transparent"
					}
				}
			}
			MouseArea {
				anchors.fill: parent
				function handleMouse(mouse) {
					if (mouse.buttons & Qt.LeftButton) {
						pickerCursor.x = Math.max(0, Math.min(width,  mouse.x))
						pickerCursor.y = Math.max(0, Math.min(height, mouse.y))
					}
				}
				onPositionChanged: handleMouse(mouse)
				onPressed: handleMouse(mouse)
			}
		}

		Slider {
			id: hueSlider
			
			Layout.alignment: Qt.AlignHCenter
			leftPadding: 0
			rightPadding: 0
			background: Rectangle {
				height: colorPicker.width
				width: 8
				implicitWidth: colorPicker.width
				implicitHeight: 8
				rotation: 90
				radius: 4
				anchors.centerIn: parent
				gradient: Gradient {
					GradientStop { position: 1.0;  color: "#FF0000" }
					GradientStop { position: 0.85; color: "#FFFF00" }
					GradientStop { position: 0.76; color: "#00FF00" }
					GradientStop { position: 0.5;  color: "#00FFFF" }
					GradientStop { position: 0.33; color: "#0000FF" }
					GradientStop { position: 0.16; color: "#FF00FF" }
					GradientStop { position: 0.0;  color: "#FF0000" }
				}
			}
			handle: Rectangle {
				x: hueSlider.leftPadding + hueSlider.visualPosition * (hueSlider.availableWidth - width)
				y: hueSlider.topPadding + hueSlider.availableHeight / 2 - height / 2
				implicitWidth: 18
				implicitHeight: 18
				radius: 9
				color: hueSlider.hovered ? hsba(hueSlider.value, 1.0, 1.0, 1.0) : hsba(hueSlider.value, 1.0, 1.0, 0.7)
				border.color: "white"
				Behavior on color { PropertyAnimation {easing.type: Easing.Linear; duration: 300} }
			}
		}

		RowLayout {
			Rectangle {
				implicitWidth: 30
				implicitHeight: 30
				color: colorValue
				border.color: "white"
			}
			Label {
				text: colorValue
				horizontalAlignment: TextInput.AlignHCenter
			}
		}
	}

	standardButtons: Dialog.Ok | Dialog.Cancel
}
