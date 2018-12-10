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
import ru.webstella.gui.chart 1.0
import QtQuick.Dialogs 1.2

ApplicationWindow {
	id: appWindow
	visible: true
	width: 1090
	height: 800
	title: app.name() + " " + app.version()
	Material.theme: Material.Dark
	Material.accent: Material.DeepOrange

	property var interfaces: ({})
	property string whoLog: "Main window"

	Timer {
		id: closeApplicationTask
		interval: 100
		running: false
		repeat: true
		onTriggered: {
			if (!hasStartedInterfaces()) {
				appWindow.close()
			}
		}
	}

	onClosing: {
		saveProject(null)
		showWaitOnExit()
		if (hasStartedInterfaces()) {
			stopPollingAll()
			close.accepted = false
			closeApplicationTask.start()
		}
	}

	Settings {
		id: appSettings
	}

	SplashWindow {
		id: splashWindow
		mainApp: app
	}

	ChartWindow {
		id: chartWindow
		visible: false
		logWindow: logWindow
		settings: appSettings
	}

	TableWindow {
		id: tableWindow
		visible: false
		logWindow: logWindow
		settings: appSettings
		mainApp: app
	}

	LogWindow {
		id: logWindow
		visible: false
		settings: appSettings
		mainApp: app
	}

	App {
		id: app
	}

	DialogAlert {
		id: dialogAlert
		x: getDialogCenteredX(width)
		y: getDialogCenteredY(height)
	}

	DialogConfirm {
		id: dialogConfirm
		x: getDialogCenteredX(width)
		y: getDialogCenteredY(height)
	}

	DialogExit {
		id: dialogExit
		x: getDialogCenteredX(width)
		y: getDialogCenteredY(height)
	}

	FileDialog {
		id: dialogFileSave
		title: qsTr("Please choose a save location")
		folder: shortcuts.home
		selectExisting: false
		onAccepted: {
			saveProject(dialogFileSave.fileUrl)
		}
		nameFilters: [ qsTr("Weprex session file ") + "(*." + appSettings.projectFileExtension + ")", qsTr("All files ") + "(*)" ]
	}

	FileDialog {
		id: dialogFileLoad
		title: qsTr("Please choose a session location")
		folder: shortcuts.home
		selectExisting: true
		onAccepted: {
			if (!loadProject(dialogFileLoad.fileUrl)) {
				log(whoLog, qsTr("Error. Session file is corrupted or in incompatible format."))
				showAlert(qsTr("Error."), qsTr("Session file is corrupted or in incompatible format."))
			}
		}
		nameFilters: [ qsTr("Weprex session file ") + "(*." + appSettings.projectFileExtension + ")", qsTr("All files ") + "(*)" ]
	}

	Connections {
		target: app
		onValueError: {
			var iface = interfaces[interfaceId]
			iface["interface"].errorOccurred(paramId, errCode, errorCounter)
		}
		onValueChanged: {
			var iface = interfaces[interfaceId]
			iface["interface"].valueChanged(paramId, value, valueRaw, responseCounter)
		}
		onValueTimeout: {
			var iface = interfaces[interfaceId]
			iface["interface"].timeoutOccurred(paramId, timeoutCounter)
		}
		onValueRequest: {
			var iface = interfaces[interfaceId]
			iface["interface"].valueRequest(paramId, requestCounter)
		}

		onInfo: {
			log(who, message);
		}

		onInterfacePollingStarted: {
			var iface = interfaces[interfaceId]
			iface["interface"].pollingStarted()
		}
		onInterfacePollingStopped: {
			var iface = interfaces[interfaceId]
			iface["interface"].pollingStopped()
		}
		onInterfaceConnected: {
			var iface = interfaces[interfaceId]
			iface["interface"].connectedToDevice()
		}
		onInterfaceConnectionError: {
			var iface = interfaces[interfaceId]
			iface["interface"].connectionError()
		}
		onInterfaceReconnecting: {
			var iface = interfaces[interfaceId]
			iface["interface"].reconnecting()
		}
	}

	Component.onCompleted: {
		dialogInterface.interfaceAdded.connect(appendInterface)
		dialogInterface.interfaceEdited.connect(editInterface)
		log(whoLog, qsTr("Application started."))
		loadProject(null)
	}

	function hasModified() {
		if (Object.keys(interfaces).length > 0) {
			return true
		}
		return false
	}

	function valToBool(val) {
		if (typeof val === "string") {
			if (val === "true") {
				return true
			} else {
				return false
			}
		} else if (typeof val === "boolean") {
			return val
		} else if (typeof val === "undefined") {
			return false
		} else if (typeof val === "number") {
			if (val === 0) {
				return false
			} else {
				return true
			}
		}
	}

	function objectToStore(store, obj) {
		for (var k in obj) {
			if (typeof obj[k] === "object") {
				store.beginGroup(k)
				objectToStore(store, obj[k])
				store.endGroup()
			} else {
				store.setValue(k, obj[k])
			}
		}
	}

	// Restore object from template
	function objectFromStore(store, template) {
		var object = {}
		for (var k in template) {
			if (typeof template[k] === "object") {
				store.beginGroup(k)
				var valGrouped = objectFromStore(store, template[k])
				store.endGroup()
				if (valGrouped === null) {
					return null
				}
				object[k] = valGrouped
			} else {
				var val = store.value(k)
				if (typeof val === "undefined") {
					return null
				} else if (typeof val === "string") {
					if (template[k] === "boolean") {
						object[k] = valToBool(val)
						/*if (val === "true") {
							object[k] = true
						} else {
							object[k] = false
						}*/
					} else if (template[k] === "int") {
						object[k] = parseInt(val, 10)
					} else if (template[k] === "float") {
						object[k] = parseFloat(val)
					} else {
						object[k] = val
					}
				} else {
					object[k] = val
				}
			}
		}
		return object
	}

	function interfaceFromStore(store) {
		var template = {
			"protocol": "string",
			"transportType": "string",
			"serial": {
				"port": "string",
				"baudrate": "int",
				"dataBits": "int",
				"parity": "string",
				"stopBits": "string"
				},
			"tcp": {
				"ipAddress": "string",
				"port": "int"
				},
			"timeout": "int",
			"pollingPause": "int"
		}
		return objectFromStore(store, template)
	}
	
	function parameterFromStore(store, protocol) {
		var template = {}
		if (protocol === "modbus_rtu" || protocol === "modbus_tcp" || protocol === "modbus_ascii") {
			template = {
				"id": "int",
				"poll": "boolean",
				"alias": "string",
				"devadr": "int",
				"fcode": "int",
				"adr": "int",
				"count": "int",
				"view": {
					"type": "string",
					"represent": "string",
					"bytes": "int",
					"sign": "boolean",
					"order": "string"
				},
				"val": "string",
				"type": "string",
				"request": "int",
				"response": "int",
				"error": "int",
				"timeout": "int",
				"selected": "boolean",
				"chart_basic": "boolean",
				"chart_extra": "boolean",
				"chart_color": "string",
				"table": "boolean"
			}
		}
		return objectFromStore(store, template)
	}

	function saveProject(fileUrl) {
		var s
		if (fileUrl !== null) {
			log(whoLog, qsTr("Save session to file \""+ fileUrl + "\"."))
			s = app.createSettingsInstance(fileUrl)
		} else {
			log(whoLog, qsTr("Save default session."))
			s = app.createSettingsInstance("")
		}
		if (!s.isWritable()) {
			log(whoLog, qsTr("Error. Unable to write data to file."))
			showAlert(
				qsTr("Error"),
				qsTr("Unable to write data to file.")
			)
			return
		}

		s.clear()
		// Store common
		s.beginWriteArray("common")
			s.setValue("log_interface_data", app.logInterfaceData())
			s.setValue("auto_scroll_table_trace", miAutoScrollTableTrace.checked)
		s.endArray()
		// Store interfaces
		s.beginWriteArray("interfaces")
			var index = 0
			for (var ifaceId in interfaces) {
				s.setArrayIndex(index)
				s.beginGroup("data")
					var curIface = interfaces[ifaceId].interface
					objectToStore(s, curIface.interfaceSettings);
				s.endGroup()
				s.beginGroup("gui")
					s.setValue("polling", interfaces[ifaceId].interface.pollingChecked)
					s.setValue("minimized", interfaces[ifaceId].interface.minimized)
				s.endGroup()
				// Store parameters
				s.beginWriteArray("param")
					for (var i = 0; i < curIface.dataModel.count; i++) {
						s.setArrayIndex(i)
						objectToStore(s, curIface.dataModel.get(i));
					}
				s.endArray()
				index++
			}
		s.endArray();
	}

	function loadProject(fileUrl) {
		if (hasStartedInterfaces()) {
			showAlert(qsTr("Info."), qsTr("You need to stop polling all interfaces before loading."))
		} else {
			clearApplicationState()
			var s
			if (fileUrl !== null) {
				log(whoLog, qsTr("Load session from file \""+ fileUrl + "\"."))
				s = app.createSettingsInstance(fileUrl)
			} else {
				log(whoLog, qsTr("Load default session."))
				s = app.createSettingsInstance("")
			}
			// Load common
			s.beginReadArray("common")
				miLogInterfaceData.checked = valToBool(s.value("log_interface_data"))
				miAutoScrollTableTrace.checked = valToBool(s.value("auto_scroll_table_trace"))
			s.endArray()
			// Load interfaces
			var size = s.beginReadArray("interfaces")
				var iface
				for (var i = 0; i < size; i++) {
					s.setArrayIndex(i)
					s.beginGroup("data")
						iface = interfaceFromStore(s)
						if (iface === null) {
							return false
						}
					s.endGroup()
					appendInterface(iface)
					s.beginGroup("gui")
						interfaces[iface.id].interface.pollingChecked = valToBool(s.value("polling"))
						interfaces[iface.id].interface.minimized = valToBool(s.value("minimized"))
					s.endGroup()
					// Load parameters
					var psize = s.beginReadArray("param")
						var param
						for (var j = 0; j < psize; j++) {
							s.setArrayIndex(j)
							param = parameterFromStore(s, iface["protocol"])
							if (param === null) {
								return false
							}
							interfaces[iface["id"]].interface.performAppendParameter(param)
						}
					s.endArray()
				}
			s.endArray();
		}
		return true
	}

	function newProject() {
		if (hasStartedInterfaces()) {
			showAlert(qsTr("Info."), qsTr("You need to stop polling all interfaces before create new session."))
		} else {
			clearApplicationState()
		}
	}

	/*onAfterSynchronizing: {

	}*/
	
	function hasStartedInterfaces() {
		for (var i in interfaces) {
			if (app.isInterfaceStarted(i)) {
				return true
			}
		}
		return false
	}

	function startPollingAll() {
		for (var i in interfaces) {
			if (!app.isInterfaceStarted(i) && interfaces[i].interface.pollingChecked) {
				interfaces[i].interface.pollingStart()
			}
		}
	}

	function stopPollingAll() {
		for (var i in interfaces) {
			if (app.isInterfaceStarted(i)) {
				interfaces[i].interface.pollingStop()
			}
		}
	}

	function getDialogCenteredX(w) {
		return (appWindow.width - w) / 2
	}

	function getDialogCenteredY(h) {
		return ((appWindow.contentItem.height > h)?((appWindow.contentItem.height - h) / 2):((appWindow.menuBar.height + appWindow.header.height) * -1))
	}

	function showAlert(title, message) {
		dialogAlert.title = title
		dialogAlert.message = message
		dialogAlert.visible = true
	}
	
	function showConfirm(title, message, acceptableObject) {
		dialogConfirm.title = title
		dialogConfirm.message = message
		dialogConfirm.acceptableObject = acceptableObject
		dialogConfirm.visible = true
	}

	function showWaitOnExit() {
		dialogExit.message = qsTr("Completes all tasks...")
		dialogExit.visible = true
	}

	function log(title, text) {
		logWindow.log(title, text)
	}

	function showChart() {
		chartWindow.visible = !chartWindow.visible
	}

	function showTable() {
		tableWindow.visible = !tableWindow.visible
	}

	function showLog() {
		logWindow.visible = !logWindow.visible
	}
	
	function newInterface() {
		// reset dialog edit flag
		dialogInterface.newInterface = true
		dialogInterface.visible = true
	}

	function reconfigInterface(interfaceId) {
		// set dialog edit flag
		dialogInterface.newInterface = false
		// fill dialog's fields
		dialogInterface.preloadSettings(interfaces[interfaceId].interface.interfaceSettings)
		// show dialog
		dialogInterface.visible = true
	}

	function clearApplicationState() {
		// Remove interfaces
		for (var iface in interfaces) {
			removeInterface(iface)
		}
	}

	function removeInterface(interfaceId) {
		var iface = interfaces[interfaceId]
		if (!iface.interface.clearParameters()) {
			log(whoLog, qsTr("Internal error. Can`t remove parameters in interface#") + interfaceId + ".")
			showAlert(
				qsTr("Internal error"),
				qsTr("Can`t remove parameters in interface.")
				)
		}
		for (var key in iface) {
			iface[key].destroy()
			delete iface[key]
		}
		delete interfaces[interfaceId]
		if (!app.removeInterface(interfaceId)) {
			log(whoLog, qsTr("Internal error. Can`t remove interface#") + interfaceId + ".")
			showAlert(
				qsTr("Internal error"),
				qsTr("Can`t remove interface.")
				)
		}
		log(whoLog, qsTr("Interface#") + interfaceId + qsTr(" removed."))
	}

	function editInterface(settings) {
		if (!app.editInterface(settings.id, settings)) {
			log(whoLog, qsTr("Internal error. Can`t edit interface: ") + JSON.stringify(settings))
			showAlert(
				qsTr("Internal error"),
				qsTr("Can`t edit interface.")
				)
			return
		}
		interfaces[settings.id].interface.interfaceSettings = settings
		log(whoLog, qsTr("Interface edited: ") + JSON.stringify(settings))
	}

	function appendInterface(settings) {
		var iid = app.addInterface(settings)
		if (iid === 0) {
			log(whoLog, qsTr("Internal error. Can`t append interface: ") + JSON.stringify(settings))
			showAlert(
				qsTr("Internal error"),
				qsTr("Can`t append interface.")
				)
			return
		}
		settings.id = iid

		var ifaceDependency = {}
		var iface = Qt.createQmlObject("InterfacePanel {interfaceId: " + iid + "}", interfaceContainer)
		ifaceDependency["interface"] = iface
		iface.mustRemove.connect(removeInterface)
		iface.mustEditInterfaceDialog.connect(reconfigInterface)
		iface.interfaceSettings = settings
		iface.mainApp = app
		iface.logWindow = logWindow
		iface.appSettings = appSettings
		iface.chartWindow = chartWindow
		iface.tableWindow = tableWindow
		iface.alertDialog = dialogAlert
		iface.confirmDialog = dialogConfirm

		var dialogWin = iface.parameterDialog = Qt.createQmlObject("
			DialogModbusRegister {
				visible: false
				x: (appWindow.width - width) / 2
				y: ((appWindow.contentItem.height > height)?((appWindow.contentItem.height - height) / 2):((appWindow.menuBar.height + appWindow.header.height) * -1))
			}
		", interfaceContainer)
		ifaceDependency["edit_dialog"] = dialogWin
		dialogWin.mainApp = app
		interfaces[iid] = ifaceDependency
		//counter += 1
		log(whoLog, qsTr("Interface added: ") + JSON.stringify(settings))
	}

	DialogInterface {
		id: dialogInterface
		visible: false
		x: (appWindow.width - width) / 2
		y: ((appWindow.contentItem.height > height)?((appWindow.contentItem.height - height) / 2):((appWindow.menuBar.height + appWindow.header.height) * -1))
		mainApp: app
	}

	menuBar: MenuBar {
		Menu {
			title: qsTr("File")
			MenuItem {
				text: qsTr("New session")
				property bool accepted: false
				onAcceptedChanged: {
					if (accepted) {
						newProject()
					}
				}
				onTriggered: {
					if (hasModified()) {
						showConfirm(qsTr("New session confirm"), qsTr("Create a new session?<br/>All unsaved data will be deleted."), this)
					} else {
						newProject()
					}
				}
			}
			MenuItem {
				text: qsTr("Open session...")
				property bool accepted: false
				onAcceptedChanged: {
					if (accepted) {
						dialogFileLoad.open()
					}
				}
				onTriggered: {
					if (hasModified()) {
						showConfirm(qsTr("Open session confirm"), qsTr("Open a session?<br/>All unsaved data will be deleted."), this)
					} else {
						dialogFileLoad.open()
					}
				}
			}
			MenuItem {
				text: qsTr("Save session as...")
				onTriggered: dialogFileSave.open()
			}
			MenuItem {
				text: qsTr("Save session")
				onTriggered: saveProject(null)
			}
			MenuSeparator {
				padding: 0
				topPadding: 10
				bottomPadding: 10
			}
			MenuItem {text: qsTr("Exit"); onTriggered: close()}
		}

		Menu {
			title: qsTr("Edit")
			MenuItem {
				text: qsTr("New interface...")
				onTriggered: newInterface()
			}
			MenuSeparator {
				padding: 0
				topPadding: 10
				bottomPadding: 10
			}
			MenuItem {
				text: qsTr("Start polling interfaces")
				onTriggered: startPollingAll()
			}
			MenuItem {
				text: qsTr("Stop polling interfaces")
				onTriggered: stopPollingAll()
			}
			MenuSeparator {
				padding: 0
				topPadding: 10
				bottomPadding: 10
			}
			MenuItem {
				id: miLogInterfaceData
				text: qsTr("Log interface data")
				checkable: true
				onCheckedChanged: {
					app.setLogInterfaceData(checked)
				}
				checked: false
			}
			MenuItem {
				id: miAutoScrollTableTrace
				text: qsTr("Scroll table data")
				checkable: true
				onCheckedChanged: {
					tableWindow.autoScroll = checked
				}
				checked: false
			}
		}

		Menu {
			title: qsTr("View")
			MenuItem {text: qsTr("Chart..."); onTriggered: showChart()}
			MenuItem {text: qsTr("Table..."); onTriggered: showTable()}
			MenuItem {text: qsTr("Application log..."); onTriggered: showLog()}
		}

		Menu {
			title: qsTr("Help")
			MenuItem {text: qsTr("Help..."); onTriggered: {app.showManual()}}
			MenuSeparator {
				padding: 0
				topPadding: 10
				bottomPadding: 10
			}
			MenuItem {text: qsTr("About..."); onTriggered: {splashWindow.showAbout()}}
		}
	}

	header: ToolBar {
		/*background: Rectangle {
			anchors.fill: parent
			color: "#1f1e57"
		}*/
		RowLayout {
			anchors.fill: parent
			WSToolButton {
				iconSource: "qrc:/icon/new_session.png"
				iconSourceDisabled: "qrc:/icon/new_session_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				property bool accepted: false
				onAcceptedChanged: {
					if (accepted) {
						newProject()
					}
				}
				onClicked: {
					if (hasModified()) {
						showConfirm(qsTr("New session confirm"), qsTr("Create a new session?<br/>All unsaved data will be deleted."), this)
					} else {
						newProject()
					}
				}
				ToolTip.text: qsTr("Create new session.")
			}
			WSToolButton {
				iconSource: "qrc:/icon/open_session.png"
				iconSourceDisabled: "qrc:/icon/open_session_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				property bool accepted: false
				onAcceptedChanged: {
					if (accepted) {
						dialogFileLoad.open()
					}
				}
				onClicked: {
					if (hasModified()) {
						showConfirm(qsTr("Open session confirm"), qsTr("Open a session?<br/>All unsaved data will be deleted."), this)
					} else {
						dialogFileLoad.open()
					}
				}
				ToolTip.text: qsTr("Open session from file.")
			}
			WSToolButton {
				iconSource: "qrc:/icon/save_session.png"
				iconSourceDisabled: "qrc:/icon/save_session_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				onClicked: dialogFileSave.open()
				ToolTip.text: qsTr("Save session to file.")
			}

			ToolSeparator {}
			WSToolButton {
				iconSource: "qrc:/icon/interface_add.png"
				iconSourceDisabled: "qrc:/icon/interface_add.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				onClicked: newInterface()
				ToolTip.text: qsTr("Create new polling interface.")
			}
			ToolSeparator {}
			WSToolButton {
				iconSource: "qrc:/icon/view_chart.png"
				iconSourceDisabled: "qrc:/icon/view_chart_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				onClicked: showChart()
				ToolTip.text: qsTr("Show/hide chart window.")
			}
			WSToolButton {
				iconSource: "qrc:/icon/view_table.png"
				iconSourceDisabled: "qrc:/icon/view_table_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				onClicked: showTable()
				ToolTip.text: qsTr("Show/hide table window.")
			}
			WSToolButton {
				iconSource: "qrc:/icon/view_log.png"
				iconSourceDisabled: "qrc:/icon/view_log_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				onClicked: showLog()
				ToolTip.text: qsTr("Show/hide application log window.")
			}
			ToolSeparator {}
			WSToolButton {
				iconSource: "qrc:/icon/all_polling_start.png"
				iconSourceDisabled: "qrc:/icon/all_polling_start_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				onClicked: startPollingAll()
				ToolTip.text: qsTr("Start polling all marked interfaces.")
			}
			WSToolButton {
				iconSource: "qrc:/icon/all_polling_stop.png"
				iconSourceDisabled: "qrc:/icon/all_polling_stop_dis.png"
				iconSize: 40
				implicitSize: parent.implicitHeight
				onClicked: stopPollingAll()
				ToolTip.text: qsTr("Stop polling all interfaces.")
			}
			Item {
				Layout.fillWidth: true
			}
		}
	}

	ScrollView {
		clip: true
		topPadding: 5
		anchors.fill: parent
		ColumnLayout {
			id: interfaceContainer
			width: parent.parent.width
		}
		ScrollBar.horizontal.policy: ScrollBar.AsNeeded
		ScrollBar.vertical.policy: ScrollBar.AsNeeded
	}
}
