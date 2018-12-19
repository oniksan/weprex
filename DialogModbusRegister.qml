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
import QtQuick.Window 2.12
import QtQuick.Controls.Material 2.12
import ru.webstella.weprex 1.0

Dialog {
	modal: true
	property bool newParameter: true
	property string parameterType: "read"
	property App mainApp
	title: newParameter?qsTr("New Modbus Register(s)"):qsTr("Edit Modbus Register(s)")
	signal parameterAdded(var settings)
	signal parameterEdited(var settings)

	Component.onCompleted: {
		closePolicy = Popup.CloseOnEscape
	}

	function preloadSettings(settings) {
		parameterAlias.text = settings.alias
		deviceAddress.number.value = settings.devadr
		functionCode.setIndex(settings.fcode)
		registerAddress.number.value = settings.adr
		registersCount.number.value = settings.count
		view.setIndexes(settings.view)
		byteOrder.setIndex(settings.view.order)
		settedValue.text = settings.val
	}

	contentItem:
		GridLayout {
			columns: 2
			rowSpacing: 0
			columnSpacing: 10

			LabelDialog {
				id: labelParameterAlias
				title: qsTr("Parameter alias")
				baseTooltipText: qsTr("Symbolic parameter name for easy operation of the parameters list.")
				errorTooltipText: qsTr("Parameter alias can not be empty.")
			}
			TextField {
				id: parameterAlias
				selectByMouse: true
				validator: RegExpValidator {
					regExp: /^\S+.*$/
				}
				text: qsTr("New parameter")
				Layout.fillWidth: true
			}

			LabelDialog {
				id: labelDeviceAddress
				title: qsTr("Device address")
				baseTooltipText: qsTr("Address of the device.")
				errorTooltipText: qsTr("Device address is not set.")
			}
			SpinBoxCustom {
				id: deviceAddress
				number.from: 0
				number.to: 255
				number.value: 1
				number.validator: RegExpValidator {
					regExp: new RegExp("((0x)([0-9]|[A-F]|[a-f]){1,2})|(([0-9]){1,3})", "i")
				}
			}

			LabelDialog {
				id: labelFunctionCode
				title: qsTr("Function code")
				baseTooltipText: qsTr("Available Modbus function codes are presented in the drop-down list.")
			}
			ComboBox {
				id: functionCode
				textRole: "text"
				model: ListModel {
					ListElement {text: qsTr("[0x01] Read coils"); code: 0x01}
					ListElement {text: qsTr("[0x02] Read discrete inputs"); code: 0x02}
					ListElement {text: qsTr("[0x03] Read holding registers"); code: 0x03}
					ListElement {text: qsTr("[0x04] Read input registers"); code: 0x04}
					ListElement {text: qsTr("[0x05] Write single coil"); code: 0x05}
					ListElement {text: qsTr("[0x06] Write single register"); code: 0x06}
					ListElement {text: qsTr("[0x0F] Write multiple coils"); code: 0x0F}
					ListElement {text: qsTr("[0x10] Write multiple registers"); code: 0x10}
				}
				Layout.fillWidth: true
				function showSettings() {
					if (parameterType === "read") {
						settedValue.visible = false
						labelSettedValue.visible = false
					} else if (parameterType === "write") {
						settedValue.visible = true
						labelSettedValue.visible = true
					} else {
						settedValue.visible = false
						labelSettedValue.visible = false
					}
				}
				function setIndex(code) {
					for (var i = 0; i < model.count; i++) {
						if (model.get(i).code === code) {
							currentIndex = i
						}
					}
				}
				function determineType() {
					if (currentIndex >= 0) {
						switch (model.get(currentIndex).code) {
							case 0x01:
							case 0x02:
							case 0x03:
							case 0x04:
								parameterType = "read"
								break
							case 0x05:
							case 0x06:
							case 0x0F:
							case 0x10:
								parameterType = "write"
								break
						}
					}
				}
				function determineCount() {
					if (currentIndex >= 0) {
						switch (model.get(currentIndex).code) {
							case 0x01:
							case 0x02:
								registersCount.number.to = 0x07D0
								break
							case 0x0F:
								if (registersCount.number.value > 0x07B0) {
									registersCount.number.value = 1
									registersCount.number.contentItem.text = "1"
								}
								registersCount.number.to = 0x07B0
								break
							case 0x03:
							case 0x04:
								if (registersCount.number.value > 0x7D) {
									registersCount.number.value = 1
									registersCount.number.contentItem.text = "1"
								}
								registersCount.number.to = 0x7D
								break
							case 0x10:
								if (registersCount.number.value > 0x7B) {
									registersCount.number.value = 1
									registersCount.number.contentItem.text = "1"
								}
								registersCount.number.to = 0x7B
								break
							case 0x05:
							case 0x06:
								registersCount.number.value = 1
								registersCount.number.contentItem.text = "1"
								registersCount.number.from = 1
								registersCount.number.to = 1
								break
						}
					}
				}

				onActivated: {
					determineType()
					showSettings()
					determineCount()
				}
				onVisibleChanged: {
					if (visible) {
						determineType()
						showSettings()
						determineCount()
					} else {
						registersCount.number.to = 0x07D0
					}
				}
			}

			LabelDialog {
				id: labelRegisterAddress
				title: qsTr("Register address")
				baseTooltipText: qsTr("Starting address of the modbus register.")
				errorTooltipText: qsTr("Register address is not set.")
			}
			SpinBoxCustom {
				id: registerAddress
			}

			LabelDialog {
				id: labelRegistersCount
				title: qsTr("Count")
				baseTooltipText: qsTr("Number of registers or coils.")
				errorTooltipText: qsTr("Count is not set.")
			}
			SpinBoxCustom {
				id: registersCount
				number.from: 1
				number.to: 0x07D0
				number.validator: RegExpValidator {
					regExp: new RegExp("((0x)([0-9]|[A-F]|[a-f]){1,3})|(([0-9]){1,4})", "i")
				}
			}

			LabelDialog {
				id: labelView
				title: qsTr("View")
				baseTooltipText: qsTr("Data representation format.")
			}
			ComboBoxView {
				id: view
			}

			LabelDialog {
				id: labelByteOrder
				title: qsTr("Byte order")
				baseTooltipText: qsTr("Byte order for placing or retrieving data from a package.")
			}
			ComboBox {
				id: byteOrder
				textRole: "text"
				model: ListModel {
					ListElement {text: qsTr("[3-2-1-0] Forward"); type: "forward"}
					ListElement {text: qsTr("[0-1-2-3] Backward"); type: "backward"}
					ListElement {text: qsTr("[1-0-3-2] Forward pair reverse"); type: "forward_rev"}
					ListElement {text: qsTr("[2-3-0-1] Backward pair reverse"); type: "backward_rev"}
				}
				Layout.fillWidth: true

				function setIndex(order) {
					for (var i = 0; i < model.count; i++) {
						if (model.get(i).type === order) {
							currentIndex = i
						}
					}
				}
			}

			LabelDialog {
				id: labelSettedValue
				title: qsTr("Value(s)")
				baseTooltipText: qsTr("Enter one or more values separated by a space in the format of the View.<br/>Exapmple: '10.2 55.3 887.6 85' or 'A4 F5 E3 22'.")
			}
			TextField {
				id: settedValue
				selectByMouse: true
				validator: RegExpValidator {
					regExp: /^\S+.*$/
				}
				Layout.fillWidth: true
			}
		}

	standardButtons: Dialog.Ok | Dialog.Cancel

	function checkData() {
		var res = true
		var bytes = view.dataType.model.get(view.dataType.currentIndex).bytes
		var code = functionCode.model.get(functionCode.currentIndex).code
		var count = registersCount.number.value
		var errText = ""
		if (code === 0x01 || code === 0x02 || code === 0x05 || code === 0x0F) {
			count = Math.floor(count / 8) + ((count % 8 === 0)?0:1)
		} else if (code === 0x03 || code === 0x04 || code === 0x06 || code === 0x10) {
			count = count * 2;
		}

		if (count % bytes != 0) {
			errText = qsTr("You need to fix: <br/>View value '" + view.dataType.currentText + "'<br/>")
			errText += qsTr("requires that Count be a multiple of ")
			if (code === 0x01 || code === 0x02 || code === 0x05 || code === 0x0F) {
				errText += qsTr((bytes * 8) + " bits (Count >= " + (((bytes - 1) * 8) + 1) + ").<br/>")
			} else {
				errText += qsTr((bytes.toString(10) / 2) + " registers (One register - two bytes).<br/>")
			}
			errText += qsTr("Change Count and/or View values to the appropriate ones.")
			labelView.errorTooltipText = errText
			labelView.error = true
			res = false
		}
		if (!parameterAlias.acceptableInput) {
			labelParameterAlias.error = true
			res = false
		}
		if (!deviceAddress.number.contentItem.acceptableInput) {
			labelDeviceAddress.error = true
			res = false
		}
		if (!registerAddress.number.contentItem.acceptableInput) {
			labelRegisterAddress.error = true
			res = false
		}
		if (!registersCount.number.contentItem.acceptableInput) {
			labelRegistersCount.error = true
			res = false
		}
		if (parameterType === "write" && !settedValue.acceptableInput) {
			labelSettedValue.errorTooltipText = qsTr("Value is not set.")
			labelSettedValue.error = true
			res = false
		}
		return res
	}

	function clearErrors() {
		labelParameterAlias.error = false
		labelDeviceAddress.error = false
		labelFunctionCode.error = false
		labelRegisterAddress.error = false
		labelRegistersCount.error = false
		labelView.error = false
		labelByteOrder.error = false
		labelSettedValue.error = false
	}

	onAccepted: {
		clearErrors()
		if (!checkData()) {
			visible = true
		} else {
			var settings = {
				"alias": parameterAlias.text.trim(),
				"devadr": deviceAddress.number.value,
				"fcode": functionCode.model.get(functionCode.currentIndex).code,
				"adr": registerAddress.number.value,
				"count": registersCount.number.value,
				"view": {
					"type": view.dataType.model.get(view.dataType.currentIndex).type,
					"represent": view.dataType.model.get(view.dataType.currentIndex).represent,
					"bytes": view.dataType.model.get(view.dataType.currentIndex).bytes,
					"sign": view.signType.model.get(view.signType.currentIndex).sign,
					"order": byteOrder.model.get(byteOrder.currentIndex).type
				 },
				"val": settedValue.text,
				"type": parameterType
			}
			if (parameterType === "write") {
				var parseValue = mainApp.parseStringValue(settings)
				if (typeof parseValue === "number") {
					// See WSConversionState
					if (parseValue === 1) {
						labelSettedValue.errorTooltipText = qsTr("Invalid amount of data entered.")
					} else if (parseValue === 2) {
						labelSettedValue.errorTooltipText = qsTr("Value limit exceeded.")
					} else if (parseValue === 3) {
						labelSettedValue.errorTooltipText = qsTr("Invalid data type.")
					} else if (parseValue === 4) {
						labelSettedValue.errorTooltipText = qsTr("Internal error (represent).")
					} else if (parseValue === 5) {
						labelSettedValue.errorTooltipText = qsTr("Internal error (type size).")
					} else if (parseValue === 6) {
						labelSettedValue.errorTooltipText = qsTr("Internal error (type).")
					} else {
						labelSettedValue.errorTooltipText = qsTr("Internal error (undefined).")
					}

					labelSettedValue.error = true
					visible = true
					return
				} else {
					settings["setted_value"] = parseValue
					settings["val"] = mainApp.arrayToString(settings)
				}
			}

			if (newParameter) {
				parameterAdded(settings)
			} else {
				parameterEdited(settings)
			}
		}
	}

	onRejected: {
		clearErrors()
	}
}
