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

Page {
	id: interfacePage
	signal mustRemove(int devId)
	signal mustEditInterfaceDialog(int devId)
	property string whoLog: "Interface panel#" + interfaceId
	property alias dataModel: dataModel
	property bool pollingChecked: true
	property bool showTaskIndicator: false
	property bool showTaskAnimation: false
	property string taskIndicatorText: ""
	property bool pollingLockFlag: false
	property bool seriesColorChangedFlag: false
	property App mainApp
	property Settings appSettings
	property LogWindow logWindow
	property ChartWindow chartWindow
	property TableWindow tableWindow
	property DialogAlert alertDialog
	property DialogConfirm confirmDialog
	property int readCount: 0
	property int writeCount: 0
	property int selectedIndex: -1
	property bool minimized: false
	property int interfaceId
	property var parameterDialog: null
	property var interfaceSettings:
		({
			"protocol": "",
			"transportType": "",
			"serial": {
				"port": "",
				"baudrate": "",
				"dataBits": "",
				"parity": "",
				"stopBits": ""
			 },
			"tcp": {
				"ipAddress": "",
				"port": ""
			 },
			"timeout": "",
			"pollingPause": ""
		})
	contentHeight: listView.height

	onMinimizedChanged: {
		if (minimized) {
			minimize()
		} else {
			maximize()
		}
	}

	onParameterDialogChanged: {
		if (parameterDialog !== null) {
			parameterDialog.parameterAdded.connect(performAppendParameter)
			parameterDialog.parameterEdited.connect(performEditParameter)
		}
	}
	
	function seriesColorChanged(seriesName, seriesColor) {
		var splitted = seriesName.split(appSettings.aliasSeparator)
		if (splitted.length > 2) {
			if (splitted[0] === interfaceId.toString()) {
				setParamSettingsInModel(parseInt(splitted[1], 10), {"chart_color": seriesColor})
			}
		}
	}

	function log(text) {
		logWindow.log(whoLog, text)
	}

	function showAlert(title, message) {
		alertDialog.title = title
		alertDialog.message = message
		alertDialog.visible = true
	}

	function showConfirm(title, message, acceptableObject) {
		confirmDialog.title = title
		confirmDialog.message = message
		confirmDialog.acceptableObject = acceptableObject
		confirmDialog.visible = true
	}

	function minimize() {
		listView.implicitHeight = 0
		listView.implicitWidth = 0
		listView.visible = false
		minimized = true
	}

	function maximize() {
		listView.visible = true
		listView.implicitWidth = listView.headerItem.width
		listView.implicitHeight = listView.contentHeight
		minimized = false
	}

	function createUniqueParameterName(parameterId, parameterName) {
		return interfaceId + appSettings.aliasSeparator + parameterId + appSettings.aliasSeparator + parameterName
	}

	function tabNameFromId(parameterId) {
		var paramAlias = getParamSettingsInModel(parameterId).alias
		if (paramAlias !== null) {
			return createUniqueParameterName(parameterId, paramAlias)
		}
		return ""
	}
	
	function addParameterToTable(parameterId) {
		tableWindow.addParameter(tabNameFromId(parameterId))
	}

	function removeParameterFromTable(parameterId) {
		tableWindow.removeParameter(tabNameFromId(parameterId))
	}
	
	function addValueToTable(parameterId, date, value, code) {
		tableWindow.addValue(tabNameFromId(parameterId), appSettings.dateTimeFormat, date, value, code)
	}

	function seriesNameFromId(parameterId) {
		var paramAlias = getParamSettingsInModel(parameterId).alias
		if (paramAlias !== null) {
			return createUniqueParameterName(parameterId, paramAlias)
		}
		return ""
	}
	
	function checkSeriesExists(parameterId, extra) {
		return chartWindow.isSeriesExists(seriesNameFromId(parameterId), extra)
	}
	
	function addParameterToChart(parameterId, extra) {
		var param = getParamSettingsInModel(parameterId)
		var represent = param.view.represent
		if (param.chart_color === "") {
			setParamSettingsInModel(parameterId, {"chart_color": chartWindow.predefinedColor(extra).toString()})
		}
		var res = chartWindow.addSeries(seriesNameFromId(parameterId), mainApp.paramBytesSize(getParamSettingsInModel(parameterId)) * 8, extra, represent, param.chart_color)
		if (res === "ok") {
			if (!seriesColorChangedFlag) {
				chartWindow.seriesColorChanged.connect(interfacePage.seriesColorChanged)
				seriesColorChangedFlag = true
			}
			return true
		} else if (res === "error_exist") {
			return false
		}
	}

	function removeParameterFromChart(parameterId, extra) {
		var res = chartWindow.removeSeries(seriesNameFromId(parameterId), extra)
		if (res === "ok") {
			return true
		} else if (res === "error_exist") {
			return false
		}
		return false
	}

	function addValueToSeries(parameterId, date, value, valueRaw, code) {
		chartWindow.addValue(seriesNameFromId(parameterId), getParamSettingsInModel(parameterId).view, appSettings.dateTimeFormat, date, value, valueRaw, code)
	}

	function getParamSettingsInModel(paramId) {
		for (var i = 0; i < dataModel.count; i++) {
			if (dataModel.get(i).id === paramId) {
				return dataModel.get(i)
			}
		}
		return null
	}
	function setParamSettingsInModel(paramId, values) {
		for (var i = 0; i < dataModel.count; i++) {
			if (dataModel.get(i).id === paramId) {
				dataModel.set(i, values)
				return
			}
		}
	}

	function pollingStarted() {
		pollingLockFlag = true
		taskIndicatorText = qsTr("Connection...")
		showTaskIndicator = true
		showTaskAnimation = true
	}

	function pollingStart() {
		if (listView.model.count > 0) {
			mainApp.startInterfacePolling(interfaceId)
		}
	}

	function pollingStop() {
		if (mainApp.stopInterfacePolling(interfaceId)) {
			taskIndicatorText = qsTr("Disconnection...")
			showTaskIndicator = true
			showTaskAnimation = true
		}
	}

	function connectionError() {
		taskIndicatorText = qsTr("Unable to connect")
		showTaskIndicator = true
		showTaskAnimation = false
	}

	function reconnecting() {
		taskIndicatorText = qsTr("Reconnection...")
		showTaskIndicator = true
		showTaskAnimation = true
	}

	function pollingStopped() {
		pollingLockFlag = false
		taskIndicatorText = ""
		showTaskIndicator = false
		showTaskAnimation = false
	}

	function connectedToDevice() {
		taskIndicatorText = ""
		showTaskIndicator = false
		showTaskAnimation = false
	}

	function errorOccurred(paramId, errCode, errorCounter) {
		if (getParamSettingsInModel(paramId).type === "write") {
			setParamSettingsInModel(paramId, {"status": errCode, "error": errorCounter})
		} else {
			setParamSettingsInModel(paramId, {"val": "-", "status": errCode, "error": errorCounter})
			var date = new Date()
			addValueToSeries(paramId, date, "-", false, errCode)
			addValueToTable(paramId, date, "-", errCode)
		}
	}

	function valueChanged(paramId, value, valueRaw, responseCounter) {
		var textOk = qsTr("OK")
		if (getParamSettingsInModel(paramId).type === "write") {
			setParamSettingsInModel(paramId, {"poll": false, "status": textOk, "response": responseCounter})
		} else {
			setParamSettingsInModel(paramId, {"val": value, "status": textOk, "response": responseCounter})
			var date = new Date()
			addValueToSeries(paramId, date, value, valueRaw, 0)
			addValueToTable(paramId, date, value, 0)
		}
	}

	function timeoutOccurred(paramId, timeoutCounter) {
		var textTimeout = qsTr("Timeout")
		if (getParamSettingsInModel(paramId).type === "write") {
			setParamSettingsInModel(paramId, {"status": textTimeout, "timeout": timeoutCounter})
		} else {
			setParamSettingsInModel(paramId, {"val": "-", "status": textTimeout, "timeout": timeoutCounter})
		}
	}

	function valueRequest(paramId, requestCounter) {
		setParamSettingsInModel(paramId, {"request": requestCounter})
	}

	function resetSelection() {
		for (var i = 0; i < dataModel.count; i++) {
			dataModel.set(i, {"selected": false})
		}
		selectedIndex = -1
	}

	function viewToString(view) {
		var res = ""
		if (view.represent === "dec") {
			if (view.type === "int") {
				if (!view.sign) {
					res += "u"
				}
				res += "int" + (view.bytes * 8).toString(10)
			} else if (view.type === "float") {
				res = "float"
			} else if (view.type === "double") {
				res = "double"
			}
		} else if (view.represent === "hex") {
			res = "hex"
		} else if (view.represent === "bin") {
			res = "bin"
		} else if (view.represent === "oct") {
			res = "oct"
		} else if (view.represent === "ascii") {
			res = "ASCII"
		} else {
			res = qsTr("undefined")
		}
		if (view.order === "forward") {
			res += " [3-2-1-0]"
		} else if (view.order === "backward") {
			res += " [0-1-2-3]"
		} else if (view.order === "forward_rev") {
			res += " [1-0-3-2]"
		} else if (view.order === "backward_rev") {
			res += " [2-3-0-1]"
		}
		return res
	}

	function fcodeToString(fcode) {
		if (fcode <= 0x0F) {
			return "0x0" + fcode.toString(16).toUpperCase()
		} else {
			return "0x" + fcode.toString(16).toUpperCase()
		}
	}

	function valToText(key, val) {
		switch (key) {
			case "alias":
				return val
			case "devadr":
				return val
			case "fcode":
				return fcodeToString(val)
			case "adr":
				return val
			case "count":
				return val
			case "view":
				return viewToString(val)
			case "val":
				return val
			case "status":
				return val
			case "request":
				return val
			case "response":
				return val
			case "error":
				return val
			case "timeout":
				return val.toString(10)
		}
	}

	function constructParameter(settings, pid) {
		var p = {
			"id": pid,
			"poll": settings.hasOwnProperty("poll") ? settings.poll : true,
			"alias": settings.alias,
			"devadr": settings.devadr,
			"fcode": settings.fcode,
			"adr": settings.adr,
			"count": settings.count,
			"view": settings.view,
			"val": ((settings.type === "read")?"":settings.val),
			"status": "",
			"type": settings.type,
			"request": 0,
			"response": 0,
			"error": 0,
			"timeout": 0,
			"selected": false,
			"chart_basic": settings.hasOwnProperty("chart_basic") ? settings.chart_basic : false,
			"chart_extra": settings.hasOwnProperty("chart_extra") ? settings.chart_extra : false,
			"chart_color": settings.hasOwnProperty("chart_color") ? settings.chart_color : "",
			"table": settings.hasOwnProperty("table") ? settings.table : false
		}
		if (settings.type === "write") {
			if (settings.hasOwnProperty("setted_value")) {
				p["setted_value"] = settings.setted_value
			} else {
				p["setted_value"] = mainApp.parseStringValue(settings)
			}
		}
		return p
	}

	function editParameter() {
		// set dialog edit flag
		parameterDialog.newParameter = false
		// fill dialog's fields
		parameterDialog.preloadSettings(dataModel.get(selectedIndex))
		// show dialog
		parameterDialog.visible = true
	}

	function performEditParameter(settings) {
		var param = dataModel.get(selectedIndex)
		settings.poll = param.poll
		var p = constructParameter(settings, param.id)
		if (param.type !== settings.type) {
			if (!deleteChartCheck(param.id)) {
				return
			}
			removeParameterFromTable(param.id)
			if (!mainApp.removeParameter(interfaceId, param.id)) {
				log(qsTr("Internal error. Can`t remove parameter: ") + JSON.stringify(param))
				showAlert(
					qsTr("Internal error"),
					qsTr("Can`t remove parameter.")
					)
				return
			}
			if (settings.type === "read") {
				writeCount--
			} else {
				readCount--
			}
			dataModel.remove(selectedIndex)
			performAppendParameter(settings)
		} else {
			// Checking changes affecting the chart
			var chBasic = param.chart_basic
			var chExtra = param.chart_extra
			var tab = param.table
			if (
				(param.chart_basic || param.chart_extra) &&
				(param.alias !== settings.alias ||
				param.view.type !== settings.view.type ||
				param.view.bytes !== settings.view.bytes)
			) {
				if (!deleteChartCheck(param.id)) {
					return
				}
			}
			// Checking changes affecting the trace table
			if (param.table && param.alias !== settings.alias) {
				removeParameterFromTable(param.id)
			}
			if (!mainApp.editParameter(interfaceId, param.id, p)) {
				log(qsTr("Internal error. Can`t edit parameter: ") + JSON.stringify(p))
				showAlert(
					qsTr("Internal error"),
					qsTr("Can`t edit parameter.")
					)
				return
			}
			dataModel.set(selectedIndex, p)
			if (chBasic) {
				dataModel.setProperty(selectedIndex, "chart_basic", chBasic)
			} else if (chExtra) {
				dataModel.setProperty(selectedIndex, "chart_extra", chExtra)
			}
			if (tab) {
				dataModel.setProperty(selectedIndex, "table", tab)
			}
		}
		resetSelection()
		log(qsTr("Parameter edited: ") + JSON.stringify(dataModel.get(selectedIndex)))
	}

	function newParameter() {
		parameterDialog.newParameter = true
		parameterDialog.visible = true
	}

	function performAppendParameter(settings) {
		var p = constructParameter(settings, 0)
		var pid = mainApp.addParameter(interfaceId, p)
		if (pid === 0) {
			log(qsTr("Internal error. Can`t append parameter: ") + JSON.stringify(p))
			showAlert(
				qsTr("Internal error"),
				qsTr("Can`t append parameter.")
				)
			return
		}
		p.id = pid

		if (settings.type === "read") {
			dataModel.insert(readCount, p)
			readCount++
		} else if (settings.type === "write") {
			dataModel.append(p)
			writeCount++
		}
		log(qsTr("Parameter added: ") + JSON.stringify(p))
	}

	function deleteChartCheck(parameterId) {
		if (checkSeriesExists(parameterId, false)) {
			if (!removeParameterFromChart(parameterId, false)) {
				return false
			}
		} else if (checkSeriesExists(parameterId, true)) {
			if (!removeParameterFromChart(parameterId, true)) {
				return false
			}
		}
		return true
	}

	function performDeleteParameter() {
		if (selectedIndex >= 0) {
			deleteParameter(dataModel.get(selectedIndex).id)
			resetSelection()
		}
	}
	
	function deleteParameter(parameterId) {
		if (!deleteChartCheck(parameterId)) {
			return false
		}
		removeParameterFromTable(parameterId)
		if (!mainApp.removeParameter(interfaceId, parameterId)) {
			log(qsTr("Internal error. Can`t remove parameter#") + parameterId + ".")
			showAlert(
				qsTr("Internal error"),
				qsTr("Can`t remove parameter.")
				)
			return false
		}
		var index = -1
		for (var i = 0; i < dataModel.count; i++) {
			if (dataModel.get(i).id === parameterId) {
				index = i
				break
			}
		}
		if (dataModel.get(index).type === "read") {
			readCount--
		} else if (dataModel.get(index).type === "write") {
			writeCount--
		}
		dataModel.remove(index)
		return true
	}

	function clearParameters() {
		while (dataModel.count > 0) {
			if (!deleteParameter(dataModel.get(0).id)) {
				return false
			}
		}
		return true
	}

	header: ToolBar {
		implicitHeight: contentHeight
		/*background: Rectangle {
			anchors.fill: parent
			color: "#1f1e57"
		}*/

		RowLayout {
			anchors.fill: parent
			CheckDelegate {
				id: pollingCheckBox
				padding: 0
				implicitWidth: 36
				implicitHeight: 36
				indicator.implicitHeight: 25
				indicator.implicitWidth: 25
				checked: pollingChecked
				ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
				ToolTip.visible: hovered
				ToolTip.text: {
					if (checked) {
						qsTr("Click to mark interface polling.")
					} else {
						qsTr("Click to mark interface polling.")
					}
				}
				onCheckedChanged: {
					pollingChecked = checked
				}
			}
			WSToolSeparator {}
			WSToolButton {
				id: btnStartPolling
				iconSource: "qrc:/icon/interface_polling_start.png"
				iconSourceDisabled: "qrc:/icon/interface_polling_start_dis.png"
				enabled: (listView.model.count > 0) && !pollingLockFlag
				ToolTip.text: qsTr("Start interface polling.")
				onClicked: pollingStart()
			}
			WSToolButton {
				id: btnStopPolling
				iconSource: "qrc:/icon/interface_polling_stop.png"
				iconSourceDisabled: "qrc:/icon/interface_polling_stop_dis.png"
				ToolTip.text: qsTr("Stop interface polling.")
				enabled: (listView.model.count > 0) && pollingLockFlag
				onClicked: pollingStop()
			}
			WSToolSeparator {}
			WSToolButton {
				id: btnParameterAdd
				iconSource: "qrc:/icon/parameter_add.png"
				iconSourceDisabled: "qrc:/icon/parameter_add_dis.png"
				ToolTip.text: qsTr("Append polling parameter.")
				enabled: !pollingLockFlag
				onClicked: newParameter()
			}
			WSToolButton {
				id: btnParameterEdit
				iconSource: "qrc:/icon/parameter_edit.png"
				iconSourceDisabled: "qrc:/icon/parameter_edit_dis.png"
				ToolTip.text: qsTr("Edit selected parameter.")
				enabled: (selectedIndex >= 0) && !pollingLockFlag
				onClicked: editParameter()
			}
			WSToolButton {
				id: btnParameterDelete
				iconSource: "qrc:/icon/parameter_delete.png"
				iconSourceDisabled: "qrc:/icon/parameter_delete_dis.png"
				ToolTip.text: qsTr("Remove selected parameter.")
				enabled: (selectedIndex >= 0) && !pollingLockFlag
				property bool accepted: false
				onAcceptedChanged: {
					if (accepted) {
						performDeleteParameter()
					}
				}
				onClicked: {
					showConfirm(qsTr("Remove confirm"), qsTr("Are you sure to remove selected parameter?"), this)
				}
			}

			WSToolSeparator {
				visible: showTaskIndicator
			}
			BusyIndicator {
				running: showTaskAnimation
				implicitHeight: 36
				visible: true
			}
			Label {
				text: taskIndicatorText
				visible: showTaskIndicator
				horizontalAlignment: Qt.AlignHCenter
				verticalAlignment: Qt.AlignVCenter
			}
			WSToolSeparator {
				visible: showTaskIndicator
			}

			Label {
				text: {
					var proto = ""
					switch (interfaceSettings.protocol) {
						case "modbus_rtu":
							proto = qsTr("Modbus RTU")
							break
						case "modbus_ascii":
							proto = qsTr("Modbus ASCII")
							break
						case "modbus_tcp":
							proto = qsTr("Modbus TCP")
							break
						case "dcon":
							proto = qsTr("DCON")
							break
					}
					var str = proto + "<br/>"
					if (interfaceSettings.transportType === "serial") {
						var parity = qsTr("?")
						var stop = qsTr("?")
						switch (interfaceSettings.serial.parity) {
							case "none":
								parity = qsTr("None")
								break
							case "odd":
								parity = qsTr("Odd")
								break
							case "even":
								parity = qsTr("Even")
								break
							case "space":
								parity = qsTr("Space")
								break
							case "mark":
								parity = qsTr("Mark")
								break
						}
						switch (interfaceSettings.serial.stopBits) {
							case "1":
								stop = qsTr("1")
								break
							case "1.5":
								stop = qsTr("1.5")
								break
							case "2":
								stop = qsTr("2")
								break
						}
						str += qsTr("Serial")
						+ " [" + interfaceSettings.serial.port + ": "
						+ interfaceSettings.serial.baudrate + "/"
						+ interfaceSettings.serial.dataBits + "/"
						+ parity + "/"
						+ stop + "]"
					} else if (interfaceSettings.transportType === "tcp") {
						str += qsTr("TCP")
						+ " [" + interfaceSettings.tcp.ipAddress + ":"
						+ interfaceSettings.tcp.port + "]"
					}
					return str
				}

				elide: Label.ElideRight
				horizontalAlignment: Qt.AlignHCenter
				verticalAlignment: Qt.AlignVCenter
				Layout.fillWidth: true
			}
			WSToolButton {
				id: btnInterfaceEdit
				enabled: !pollingLockFlag
				iconSource: "qrc:/icon/interface_edit.png"
				iconSourceDisabled: "qrc:/icon/interface_edit_dis.png"
				ToolTip.text: qsTr("Edit interface properties.")
				onClicked: interfacePage.mustEditInterfaceDialog(interfaceId)
			}
			WSToolButton {
				id: btnInterfaceDelete
				enabled: !pollingLockFlag
				iconSource: "qrc:/icon/interface_delete.png"
				iconSourceDisabled: "qrc:/icon/interface_delete_dis.png"
				ToolTip.text: qsTr("Remove interface.")
				property bool accepted: false
				onAcceptedChanged: {
					if (accepted && !mainApp.isInterfaceStarted(interfaceId)) {
						interfacePage.mustRemove(interfaceId)
					}
				}
				onClicked: {
					showConfirm(qsTr("Remove confirm"), qsTr("Are you sure to remove an interface?"), this)
				}
			}
			WSToolSeparator {}
			ToolButton {
				id: btnCollapse
				implicitWidth: 36
				implicitHeight: 36
				padding: 0
				text: minimized?qsTr("▼"):qsTr("▲")
				onClicked: {
					minimized = !minimized
				}
			}
		}
	}

	TextMetrics {
		id: tableRowTM
	}

	ListModel {
		id: dataModel
	}

	ListModel {
		id: headerModel
		ListElement {name: qsTr("№"); key: ""}
		ListElement {name: qsTr("P"); key: "poll"}
		ListElement {name: qsTr("C1"); key: "chart_basic"}
		ListElement {name: qsTr("C2"); key: "chart_extra"}
		ListElement {name: qsTr("T"); key: "table"}
		ListElement {name: qsTr("Parameter alias"); key: "alias"}
		ListElement {name: qsTr("Address"); key: "devadr"}
		ListElement {name: qsTr("Func code"); key: "fcode"}
		ListElement {name: qsTr("Register"); key: "adr"}
		ListElement {name: qsTr("Count"); key: "count"}
		ListElement {name: qsTr("Type [byte order]"); key: "view"}
		ListElement {name: qsTr("Value"); key: "val"}
		ListElement {name: qsTr("Status"); key: "status"}
		ListElement {name: qsTr("Req"); key: "request"}
		ListElement {name: qsTr("Resp"); key: "response"}
		ListElement {name: qsTr("Err"); key: "error"}
		ListElement {name: qsTr("T.out"); key: "timeout"}
	}

	ListView {
		id: listView

		property int readColor: Material.BlueGrey
		property int writeColor: Material.Teal
		property int hoverShade: Material.Shade400
		property int selectShade: Material.Shade800
		property int titleColor: Material.Indigo

		Behavior on implicitWidth { PropertyAnimation {easing.type: Easing.OutExpo; duration: 400} }
		Behavior on implicitHeight { PropertyAnimation {easing.type: Easing.OutExpo; duration: 300} }

		onContentHeightChanged: {
			if (visible) {
				implicitHeight = contentHeight
			}
		}

		header: Row {
			id: headerRow
			spacing: 1
			topPadding: 1

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
					Component.onCompleted: {
						baseWidth = width
						isBaseWidth = true
					}
					text: name
					font.bold: true
					padding: 10
					height: 30
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
							//isBaseWidth = !isBaseWidth
							parent.width = maxWidth
						}
					}
					ToolTip {
						delay: Qt.styleHints.mousePressAndHoldInterval
						visible: parent.hovered
						text: {
							if (key === "chart_basic") {
								return qsTr("Visualise parameter on chart (Main layer).")
							} else if (key === "chart_extra") {
								return qsTr("Visualise parameter on chart (Background layer).")
							} else if (key === "table") {
								return qsTr("Trace parameter in table.")
							} else if (key === "poll") {
								return qsTr("Enable/disable parameter polling.")
							} else if (key === "devadr") {
								return qsTr("Device address.")
							} else if (key === "fcode") {
								return qsTr("Modbus function code.")
							} else if (key === "adr") {
								return qsTr("Modbus register address.")
							} else if (key === "count") {
								return qsTr("Number of registers or coils.")
							} else if (key === "view") {
								return qsTr("Visualized type and byte order.")
							} else if (key === "request") {
								return qsTr("Number of requests executed.")
							} else if (key === "response") {
								return qsTr("Number of success responses received.")
							} else if (key === "error") {
								return qsTr("Number of responses with errors.")
							} else if (key === "timeout") {
								return qsTr("Number of responses timeouts.")
							}
							return qsTr("Double-click the column header to change the width of the column according to the size of the content.")
						}
					}
					background: Rectangle { color: Material.color(listView.titleColor) }
				}
			}
		}

		model: dataModel
		delegate: Column {
			property int row: index

			Row {
				id: tableRow
				spacing: listView.headerItem.spacing
				property bool rowHovered: false

				function rowWasHovered(hov) {
					rowHovered = hov
				}

				function rowWasSelected(sel) {
					var s = model.selected
					resetSelection()
					model.selected = !s
					if (model.selected) {
						selectedIndex = model.index
					}
				}

				Component.onCompleted: {
					for (var i = 0; i < children.length; i++) {
						if (!children[i].hasOwnProperty('checkedItem')) {
							children[i].onSelected.connect(rowWasSelected)
						}
						children[i].onHovered.connect(rowWasHovered)
					}
				}

				onWidthChanged: {
					rowDivider.width = width
				}

				// Number
				TableItemDelegate {
					text: (type === "read")?(index + 1):(index - readCount + 1)
					width: listView.headerItem.itemAt(0).width
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Enabled
				TableCheckDelegate {
					width: listView.headerItem.itemAt(1).width
					checked: poll
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
					onCheckStateChanged: {
						if (poll != checked) {
							poll = checked
							mainApp.editParameter(interfaceId, model.id, {"poll": poll})
						}
					}
				}

				// Chart basic
				TableCheckDelegate {
					width: listView.headerItem.itemAt(2).width
					checked: chart_basic
					enabled: (type === "read")
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
					onCheckStateChanged: {
						chart_basic = checked
						if (checked) {
							if (checkSeriesExists(model.id, true)) {
								model.chart_extra = false
							}
							addParameterToChart(model.id, false)
						} else {
							removeParameterFromChart(model.id, false)
						}
					}
					ToolTip {
						delay: Qt.styleHints.mousePressAndHoldInterval
						visible: parent.hovered
						text: {
							if (type === "read") {
								return qsTr("Visualise parameter ") + "'" + alias + "'" + qsTr(" in Main layer of chart.")
							} else {
								return qsTr("Write parameters are not visualised.")
							}
						}
					}
				}

				// Chart extra
				TableCheckDelegate {
					width: listView.headerItem.itemAt(3).width
					checked: chart_extra
					enabled: (type === "read")
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
					onCheckStateChanged: {
						chart_extra = checked
						if (checked) {
							if (checkSeriesExists(model.id, false)) {
								model.chart_basic = false
							}
							addParameterToChart(model.id, true)
						} else {
							removeParameterFromChart(model.id, true)
						}
					}
					ToolTip {
						delay: Qt.styleHints.mousePressAndHoldInterval
						visible: parent.hovered
						text: {
							if (type === "read") {
								return qsTr("Visualise parameter ") + "'" + alias + "'" + qsTr(" in Background layer of chart.")
							} else {
								return qsTr("Write parameters are not visualised.")
							}
						}
					}
				}

				// Table trace
				TableCheckDelegate {
					width: listView.headerItem.itemAt(4).width
					checked: table
					enabled: (type === "read")
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
					onCheckStateChanged: {
						table = checked
						if (checked) {
							addParameterToTable(model.id)
						} else {
							removeParameterFromTable(model.id)
						}
					}
					ToolTip {
						delay: Qt.styleHints.mousePressAndHoldInterval
						visible: parent.hovered
						text: {
							if (type === "read") {
								return qsTr("Trace parameter ") + "'" + alias + "'"
							} else {
								return qsTr("Write parameters not traced.")
							}
						}
					}
				}

				// Alias
				TableItemDelegate {
					text: valToText("alias", alias)
					width: listView.headerItem.itemAt(5).width
					horizontalAlignment: Text.AlignLeft
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Device address
				TableItemDelegate {
					text: valToText("devadr", devadr)
					width: listView.headerItem.itemAt(6).width
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Function code
				TableItemDelegate {
					text: valToText("fcode", fcode)
					width: listView.headerItem.itemAt(7).width
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Register address
				TableItemDelegate {
					text: valToText("adr", adr)
					width: listView.headerItem.itemAt(8).width
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Count
				TableItemDelegate {
					text: valToText("count", count)
					width: listView.headerItem.itemAt(9).width
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// View info
				TableItemDelegate {
					text: valToText("view", view)
					width: listView.headerItem.itemAt(10).width
					horizontalAlignment: Text.AlignRight
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Value
				TableItemDelegate {
					text: valToText("val", val)
					width: listView.headerItem.itemAt(11).width
					horizontalAlignment: Text.AlignRight
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Status
				TableItemDelegate {
					text: valToText("status", status)
					width: listView.headerItem.itemAt(12).width
					horizontalAlignment: Text.AlignRight
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Requests count
				TableItemDelegate {
					text: valToText("request", request)
					width: listView.headerItem.itemAt(13).width
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Responses count
				TableItemDelegate {
					text: valToText("response", response)
					width: listView.headerItem.itemAt(14).width
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Errors count
				TableItemDelegate {
					text: valToText("error", error)
					width: listView.headerItem.itemAt(15).width
					color: ((error > 0)?(Material.color(Material.Red)):defaultColor)
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}

				// Timeouts count
				TableItemDelegate {
					text: valToText("timeout", timeout)
					width: listView.headerItem.itemAt(16).width
					color: ((timeout > 0)?(Material.color(Material.Orange)):defaultColor)
					backDefaultColor: (type==="read")?listView.readColor:listView.writeColor
					backgroundColor: model.selected?Material.color(backDefaultColor, listView.selectShade):(tableRow.rowHovered?Material.color(backDefaultColor, listView.hoverShade):Material.color(backDefaultColor))
				}
			}

			Item {
				id: rowDivider
				//color: Material.color(Material.Indigo)
				width: parent.width
				height: 1
			}
		}
	}

}
