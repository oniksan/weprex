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
import QtQuick.Window 2.11
import QtQuick.Controls.Material 2.3
import ru.webstella.weprex 1.0

Dialog {
	modal: true
	property bool newInterface: true
	title: newInterface?qsTr("New interface"):qsTr("Edit interface")
	property string transportType
	property App mainApp
	property int iid
	signal interfaceAdded(var settings)
	signal interfaceEdited(var settings)

	Component.onCompleted: {
		closePolicy = Popup.CloseOnEscape
	}

	function preloadSettings(settings) {
		iid = settings.id
		protocol.setIndex(settings.protocol)
		transport.setIndex(settings.transportType)
		serialPort.setIndex(settings.serial.port)
		serialBaudrate.setIndex(settings.serial.baudrate)
		serialDataBits.setIndex(settings.serial.dataBits)
		serialParity.setIndex(settings.serial.parity)
		serialStopBits.setIndex(settings.serial.stopBits)
		tcpAddress.text = settings.tcp.ipAddress
		tcpPort.value = settings.tcp.port
		maxTimeout.value = settings.timeout
		pollingPause.value = settings.pollingPause
	}

	contentItem:
		GridLayout {
			columns: 2
			rowSpacing: 0
			columnSpacing: 10

			LabelDialog {
				id: labelProtocol
				title: qsTr("Protocol")
				baseTooltipText: qsTr("Available protocols are presented in the drop-down list.")
				errorTooltipText: qsTr("Sorry, Modbus ASCII and DCON unsupported in current version.")
			}
			ComboBox {
				id: protocol
				textRole: "text"
				model: ListModel {
					ListElement {text: qsTr("Modbus RTU"); type: "modbus_rtu"}
					ListElement {text: qsTr("Modbus ASCII"); type: "modbus_ascii"}
					ListElement {text: qsTr("Modbus TCP"); type: "modbus_tcp"}
					ListElement {text: qsTr("DCON"); type: "dcon"}
				}
				enabled: newInterface
				Layout.fillWidth: true

				function setIndex(type) {
					for (var i = 0; i < model.count; i++) {
						if (model.get(i).type === type) {
							currentIndex = i
						}
					}
				}
			}

			LabelDialog {
				id: labelTransport
				title: qsTr("Media/Transport layer")
				baseTooltipText: qsTr("Media/Transport layers are presented in the drop-down list.")
			}
			ComboBox {
				id: transport
				textRole: "text"
				model: ListModel {
					ListElement {text: qsTr("Serial"); type: "serial"}
					ListElement {text: qsTr("TCP"); type: "tcp"}
				}
				Layout.fillWidth: true

				function setIndex(type) {
					for (var i = 0; i < model.count; i++) {
						if (model.get(i).type === type) {
							currentIndex = i
						}
					}
				}

				function showSettings() {
					if (transportType === "serial") {
						tcpSettings.visible = false
						serialSettings.visible = true
					} else if (transportType === "tcp") {
						serialSettings.visible = false
						tcpSettings.visible = true
					} else {
						tcpSettings.visible = false
						serialSettings.visible = false
					}
				}
				function determineTransportType() {
					if (currentIndex >= 0) {
						transportType = model.get(currentIndex).type
					}
				}
				onActivated: {
					determineTransportType()
					showSettings()
				}
				onVisibleChanged: {
					determineTransportType()
					showSettings()
				}
			}

			Frame {
				id: serialSettings
				visible: false
				Layout.columnSpan: 2
				Layout.fillWidth: true
				height: contentHeight
				GridLayout {
					anchors.fill: parent
					//rows: 3
					columns: 4
					rowSpacing: 0
					columnSpacing: 10
					LabelDialog {
						id: labelSerialPort
						title: qsTr("Port")
						baseTooltipText: qsTr("Select the serial port available on the system.")
						errorTooltipText: qsTr("Serial port is not set.")
					}
					ComboBox {
						id: serialPort
						model: mainApp.availablePortNames
						Layout.fillWidth: true
						//editable: true
						wheelEnabled: true
						validator: RegExpValidator {
							regExp: /^\S+.*$/
						}

						function setIndex(port) {
							for (var i = 0; i < model.length; i++) {
								if (model[i] === port) {
									currentIndex = i
								}
							}
						}
					}

					Button {
						text: qsTr("Refresh")
						Layout.columnSpan: 2
						Layout.fillWidth: true
						ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
						ToolTip.visible: hovered
						ToolTip.text: qsTr("Refresh list of available ports in system.")
						onClicked: {
							mainApp.refreshAvailablePorts()
						}
					}

					LabelDialog {
						id: labelSerialBaudrate
						title: qsTr("Baudrate")
						baseTooltipText: qsTr("Data exchange rate in bps.")
					}
					ComboBox {
						id: serialBaudrate
						model: [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200]
						currentIndex: 3
						Layout.fillWidth: true

						function setIndex(baud) {
							for (var i = 0; i < model.length; i++) {
								if (model[i] === baud) {
									currentIndex = i
								}
							}
						}
					}

					LabelDialog {
						id: labelSerialDataBits
						title: qsTr("Data bits")
						baseTooltipText: qsTr("Number of data bits.")
					}
					ComboBox {
						id: serialDataBits
						model: [5, 6, 7, 8]
						currentIndex: 3
						Layout.fillWidth: true

						function setIndex(bits) {
							for (var i = 0; i < model.length; i++) {
								if (model[i] === bits) {
									currentIndex = i
								}
							}
						}
					}

					LabelDialog {
						id: labelSerialParity
						title: qsTr("Parity")
						baseTooltipText: qsTr("Parity or check bit type.")
					}
					ComboBox {
						id: serialParity
						textRole: "text"
						model: ListModel {
							ListElement {text: qsTr("None"); type: "none"}
							ListElement {text: qsTr("Odd"); type: "odd"}
							ListElement {text: qsTr("Even"); type: "even"}
							ListElement {text: qsTr("Space"); type: "space"}
							ListElement {text: qsTr("Mark"); type: "mark"}
						}
						Layout.fillWidth: true

						function setIndex(type) {
							for (var i = 0; i < model.count; i++) {
								if (model.get(i).type === type) {
									currentIndex = i
								}
							}
						}
					}

					LabelDialog {
						id: labelSerialStopBits
						title: qsTr("Stop bit")
						baseTooltipText: qsTr("Stop bit type.")
					}
					ComboBox {
						id: serialStopBits
						textRole: "text"
						model: ListModel {
							ListElement {text: qsTr("1"); type: "1"}
							ListElement {text: qsTr("1.5"); type: "1.5"}
							ListElement {text: qsTr("2"); type: "2"}
						}
						Layout.fillWidth: true

						function setIndex(type) {
							for (var i = 0; i < model.count; i++) {
								if (model.get(i).type === type) {
									currentIndex = i
								}
							}
						}
					}
				}
			}

			Frame {
				id: tcpSettings
				visible: false
				Layout.columnSpan: 2
				Layout.fillWidth: true
				height: contentHeight
				RowLayout {
					anchors.fill: parent
					spacing: 10
					LabelDialog {
						id: labelTcpAddress
						title: qsTr("IP address")
						baseTooltipText: qsTr("Server IP v4 address.")
						errorTooltipText: qsTr("Incorrect IP address format entered.")
					}
					TextField {
						id: tcpAddress
						validator: RegExpValidator {
							regExp: /^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$/
						}
						selectByMouse: true
						placeholderText: "127.0.0.1"
						Layout.fillWidth: true
					}

					LabelDialog {
						id: labelTcpPort
						title: qsTr("Port")
						baseTooltipText: qsTr("Server port.")
						errorTooltipText: qsTr("Port is not set.")
					}
					SpinBox {
						id: tcpPort
						wheelEnabled: true
						from: 1
						to: 65535
						value: 502
						editable: true
						wrap: true
						Layout.fillWidth: true
						validator: IntValidator {
							bottom: tcpPort.from
							top: tcpPort.to
						}
					}
				}
			}

			LabelDialog {
				id: labelMaxTimeout
				title: qsTr("Timeout")
				baseTooltipText: qsTr("Maximum response time from the device on interface in ms.")
				errorTooltipText: qsTr("Timeout is not set.")
			}
			SpinBox {
				id: maxTimeout
				wheelEnabled: true
				from: 0
				to: 60000
				value: 500
				editable: true
				wrap: true
				validator: IntValidator {
					bottom: maxTimeout.from
					top: maxTimeout.to
				}
			}

			LabelDialog {
				id: labelPollingPause
				title: qsTr("Polling pause")
				baseTooltipText: qsTr("Pause after polling each parameter in ms.<br/>Some devices do not provide normal operation without this pause.")
				errorTooltipText: qsTr("Polling pause is not set.")
			}
			SpinBox {
				id: pollingPause
				wheelEnabled: true
				from: 0
				to: 60000
				value: 100
				editable: true
				wrap: true
				validator: IntValidator {
					bottom: pollingPause.from
					top: pollingPause.to
				}
			}

		}

	standardButtons: Dialog.Ok | Dialog.Cancel

	function checkData() {
		var res = true
		// !!! Unsupported yet!
		if (protocol.model.get(protocol.currentIndex).type === "modbus_ascii" ||
			protocol.model.get(protocol.currentIndex).type === "dcon") {
			labelProtocol.error = true
			res = false
		}

		if (transportType === "tcp") {
			if (!tcpAddress.acceptableInput) {
				labelTcpAddress.error = true
				res = false
			}
			if (!tcpPort.contentItem.acceptableInput) {
				labelTcpPort.error = true
				res = false
			}
		}

		if (transportType === "serial" && !serialPort.acceptableInput) {
			labelSerialPort.error = true
			res = false
		}

		if (!maxTimeout.contentItem.acceptableInput) {
			labelMaxTimeout.error = true
			res = false
		}

		if (!pollingPause.contentItem.acceptableInput) {
			labelPollingPause.error = true
			res = false
		}

		return res
	}

	function clearErrors() {
		labelTcpAddress.error = false
		labelTcpPort.error = false
		labelSerialPort.error = false
		labelMaxTimeout.error = false
		labelPollingPause.error = false
	}

	onAccepted: {
		clearErrors()
		if (!checkData()) {
			visible = true
		} else {
			var settings = {
				"id": (newInterface ? 0 : iid),
				"protocol": protocol.model.get(protocol.currentIndex).type,
				"transportType": transport.model.get(transport.currentIndex).type,
				"serial": {
					"port": serialPort.currentText,
					"baudrate": parseInt(serialBaudrate.currentText, 10),
					"dataBits": parseInt(serialDataBits.currentText, 10),
					"parity": serialParity.model.get(serialParity.currentIndex).type,
					"stopBits": serialStopBits.model.get(serialStopBits.currentIndex).type
				 },
				"tcp": {
					"ipAddress": tcpAddress.text,
					"port": tcpPort.value
				 },
				"timeout": maxTimeout.value,
				"pollingPause": pollingPause.value
			}

			if (newInterface) {
				interfaceAdded(settings)
			} else {
				interfaceEdited(settings)
			}
		}
	}

	onRejected: {
		clearErrors()
	}
}
