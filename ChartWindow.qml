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
import ru.webstella.gui.chart 1.0
import ru.webstella.weprex 1.0

ApplicationWindow {
	id: window
	visible: true
	width: 970
	height: 800
	title: qsTr("Chart view")
	Material.theme: Material.Dark
	Material.accent: Material.DeepOrange
	
	signal seriesColorChanged(string seriesName, string seriesColor)
	property string whoLog: "Chart window"
	property LogWindow logWindow
	property Settings settings
	property alias timeChart: timeChart
	property alias chartTableModel: chartTableModel
	property alias chartTable: chartTable
	
	Connections {
		target: timeChart
		onTrackingRegimeChanged: {
			changeTrackingStatusText()
		}
		onExtraSeriesViewChanged: {
			changeLayerStatusText()
		}
	}

	Component.onCompleted: {
		changeTrackingStatusText()
		changeLayerStatusText()
	}

	function changeTrackingStatusText() {
		if (timeChart.trackingRegime === TimeChart.TRACKING_AND_SCALING_ALL) {
			trackingStatusText.text = qsTr("All axes scaling and tracking")
		} else if (timeChart.trackingRegime === TimeChart.TRACKING_AND_SCALING_VALUE) {
			trackingStatusText.text = qsTr("Value scaling and tracking")
		} else if (timeChart.trackingRegime === TimeChart.TRACKING_WITHOUT_SCALING) {
			trackingStatusText.text = qsTr("Tracking without scaling")
		} else if (timeChart.trackingRegime === TimeChart.NO_TRACKING_NO_SCALING) {
			trackingStatusText.text = qsTr("No tracking and scaling")
		} else {
			trackingStatusText.text = qsTr("No scaling settings")
		}
	}

	function changeLayerStatusText() {
		layerStatusText.text = qsTr("Layer: ") + (timeChart.extraSeriesView ? qsTr("Back") : qsTr("Main"))
	}

	function log(text) {
		logWindow.log(whoLog, text)
	}

	function valToText(key, val) {
		switch (key) {
			case "time":
				return val
			case "value":
				return val
			case "error":
				return val
			default:
				return val
		}
	}

	function isSeriesExists(seriesName, extra) {
		if (extra) {
			if (timeChart.getExtraSeries(seriesName) !== null) {
				return true
			}
		} else {
			if (timeChart.getBasicSeries(seriesName) !== null) {
				return true
			}
		}
		return false
	}
	
	function setSeriesColor(series, seriesColor) {
		series.normalLineColor = seriesColor
		series.pointBackgroundColor = seriesColor
	}

	function setSeriesColorByName(seriesName, seriesColor) {
		var series = timeChart.getBasicSeries(seriesName)
		if (series === null) {
			series = timeChart.getExtraSeries(seriesName)
			if (series === null) {
				return false
			}
		}
		setSeriesColor(series, seriesColor)
		window.seriesColorChanged(seriesName, seriesColor)
		return true
	}

	function predefinedColor(extra) {
		if (extra) {
			return timeChart.predefinedColor(timeChart.extraSeriesCount())
		} else {
			return timeChart.predefinedColor(timeChart.basicSeriesCount())
		}
	}

	function addSeries(seriesName, bits, extra, represent, seriesColor) {
		var series
		if (extra) {
			if (timeChart.getExtraSeries(seriesName) !== null) {
				log(qsTr("Internal error. Added series exists: ") + seriesName + ".")
				return "error_exist"
			}
			series = timeChart.addExtraSeries(seriesName)
		} else {
			if (timeChart.getBasicSeries(seriesName) !== null) {
				log(qsTr("Internal error. Added series exists: ") + seriesName + ".")
				return "error_exist"
			}
			series = timeChart.addBasicSeries(seriesName)
		}
		series.bitsCount = bits
		if (represent === "bin") {
			series.notation = TimeSeries.BITS
		} else {
			series.notation = TimeSeries.LINES
		}
		series.maxDataSize = settings.chartMaxDataSize
		setSeriesColor(series, seriesColor)
		chartTableModel.append({
			"color": seriesColor.toString(),
			"onlayer": extra ? qsTr("Back") : qsTr("Main"),
			"alias": seriesName,
			"render": (series.notation === TimeSeries.BITS)?qsTr("Bits"):qsTr("Line"),
			"time": "",
			"value": "",
			"error": ""
			})
		log(qsTr("Series added: ") + JSON.stringify(chartTableModel.get(chartTableModel.count - 1)))
		return "ok"
	}
	
	function removeSeries(seriesName, extra) {
		if (extra) {
			if (!timeChart.removeExtraSeries(seriesName)) {
				log(qsTr("Internal error. Removed series not exists: ") + seriesName + ".")
				return "error_exist"
			}
		} else {
			if (!timeChart.removeBasicSeries(seriesName)) {
				log(qsTr("Internal error. Removed series not exists: ") + seriesName + ".")
				return "error_exist"
			}
		}
		for (var i = 0; i < chartTableModel.count; i++) {
			if (chartTableModel.get(i).alias === seriesName) {
				chartTableModel.remove(i)
				break
			}
		}
		log(qsTr("Series removed: ") + seriesName + ".")
		return "ok"
	}
	
	function addValue(seriesName, view, dateTimeFormat, date, value, valueRaw, code) {
		var series = timeChart.getBasicSeries(seriesName)
		if (series === null) {
			series = timeChart.getExtraSeries(seriesName)
			if (series === null) {
				return false
			}
		}
		if (view.type === "int") {
			if (typeof valueRaw === "boolean") {
				series.addIntErrorPoint(date.getTime(), parseInt(code))
			} else {
				if (view.represent === "hex") {
					series.addIntPoint(date.getTime(), parseInt(valueRaw, 16), parseInt(code))
				} else if (view.represent === "oct") {
					series.addIntPoint(date.getTime(), parseInt(valueRaw, 8), parseInt(code))
				} else if (view.represent === "bin") {
					series.addIntPoint(date.getTime(), parseInt(valueRaw, 2), parseInt(code))
				} else {
					series.addIntPoint(date.getTime(), parseInt(valueRaw, 10), parseInt(code))
				}
			}
		} else if (view.type === "float") {
			if (typeof valueRaw === "boolean") {
				series.addDoubleErrorPoint(date.getTime(), parseInt(code))
			} else {
				series.addDoublePoint(date.getTime(), parseFloat(valueRaw), parseInt(code))
			}
		}
		for (var i = 0; i < chartTableModel.count; i++) {
			if (seriesName === chartTableModel.get(i).alias) {
				chartTableModel.set(i, {time: Qt.formatDateTime(date, dateTimeFormat), value: value, error: ((code !== 0) ? code : "OK")})
				break
			}
		}
		return true
	}
	
	function getDialogCenteredX(w) {
		return (window.width - w) / 2
	}

	function getDialogCenteredY(h) {
		return ((window.contentItem.height > h)?((window.contentItem.height - h) / 2):((window.menuBar.height + window.header.height) * -1))
	}

	function showColorDialog(acceptableObject, curColor) {
		dialogColor.acceptableObject = acceptableObject
		dialogColor.initColorValue = curColor
		dialogColor.visible = true
	}

	DialogColor {
		id: dialogColor
		x: getDialogCenteredX(width)
		y: getDialogCenteredY(height)
	}

	WSSplitView {
		anchors.fill: parent
		orientation: Qt.Vertical

		TimeChart {
			id: timeChart
			implicitWidth: parent.width
			Layout.fillHeight: true
			Layout.minimumHeight: 400
			visible: true
		}
		Item {
			//Layout.fillWidth: true
			implicitWidth: parent.width
			height: 120
			//color: "lightblue"
			ColumnLayout {
				spacing: 1
				anchors.fill: parent
				ToolBar {
					id: toolBar
					Layout.topMargin: 1
					Layout.alignment: Qt.AlignTop
					implicitHeight: 40
					Layout.fillWidth: true
					RowLayout {
						anchors.fill: parent
						WSToolButton {
							iconSource: "qrc:/icon/tracking_and_scaling_all.png"
							iconSourceDisabled: "qrc:/icon/tracking_and_scaling_all.png"
							onClicked: timeChart.trackingRegime = TimeChart.TRACKING_AND_SCALING_ALL
							ToolTip.text: qsTr("Auto scale all axes and tracking")
						}
						WSToolButton {
							iconSource: "qrc:/icon/tracking_and_scaling_value.png"
							iconSourceDisabled: "qrc:/tracking_and_scaling_value.png"
							onClicked: timeChart.trackingRegime = TimeChart.TRACKING_AND_SCALING_VALUE
							ToolTip.text: qsTr("Auto scale value axis and tracking")
						}
						WSToolButton {
							iconSource: "qrc:/icon/tracking_wo_scaling.png"
							iconSourceDisabled: "qrc:/icon/tracking_wo_scaling.png"
							onClicked: timeChart.trackingRegime = TimeChart.TRACKING_WITHOUT_SCALING
							ToolTip.text: qsTr("No scale and tracking")
						}
						WSToolButton {
							iconSource: "qrc:/icon/no_tracking_no_scaling.png"
							iconSourceDisabled: "qrc:/icon/no_tracking_no_scaling.png"
							onClicked: timeChart.trackingRegime = TimeChart.NO_TRACKING_NO_SCALING
							ToolTip.text: qsTr("No scale, no tracking")
						}
						WSToolButton {
							iconSource: "qrc:/icon/show_all_chart.png"
							iconSourceDisabled: "qrc:/icon/show_all_chart.png"
							onClicked: timeChart.scalingTimeValue()
							ToolTip.text: qsTr("Show all points")
						}
						WSToolButton {
							iconSource: "qrc:/icon/scale_values.png"
							iconSourceDisabled: "qrc:/icon/scale_values.png"
							onClicked: timeChart.scalingValue()
							ToolTip.text: qsTr("Fit points to value scale")
						}
						WSToolSeparator {}
						Label {
							id: layerStatusText
							text: ""
							elide: Label.ElideRight
							horizontalAlignment: Qt.AlignHCenter
							verticalAlignment: Qt.AlignVCenter
							Layout.fillWidth: true
						}
						WSToolSeparator {}
						Label {
							id: trackingStatusText
							text: ""
							elide: Label.ElideRight
							horizontalAlignment: Qt.AlignHCenter
							verticalAlignment: Qt.AlignVCenter
							Layout.fillWidth: true
						}
						WSToolSeparator {}
						WSToolButton {
							iconSource: "qrc:/icon/switch_charts_layer.png"
							iconSourceDisabled: "qrc:/icon/switch_charts_layer.png"
							onClicked: timeChart.extraSeriesView = !timeChart.extraSeriesView
							ToolTip.text: qsTr("Switch chart's view layer")
						}
						WSToolSeparator {}
						WSToolButton {
							iconSource: "qrc:/icon/chart_time_expand.png"
							iconSourceDisabled: "qrc:/icon/chart_time_expand.png"
							onClicked: timeChart.scaleTimeBounds(0.9)
							ToolTip.text: qsTr("Time axis expand")
						}
						WSToolButton {
							iconSource: "qrc:/icon/chart_time_narrow.png"
							iconSourceDisabled: "qrc:/icon/chart_time_narrow.png"
							onClicked: timeChart.scaleTimeBounds(1.1)
							ToolTip.text: qsTr("Time axis narrow")
						}
						WSToolButton {
							iconSource: "qrc:/icon/chart_value_expand.png"
							iconSourceDisabled: "qrc:/icon/chart_value_expand.png"
							onClicked: timeChart.scaleValueBounds(0.9)
							ToolTip.text: qsTr("Value axis expand")
						}
						WSToolButton {
							iconSource: "qrc:/icon/chart_value_narrow.png"
							iconSourceDisabled: "qrc:/icon/chart_value_narrow.png"
							onClicked: timeChart.scaleValueBounds(1.1)
							ToolTip.text: qsTr("Value axis narrow")
						}
					}
				}

				ListModel {
					id: chartTableModel
					/*ListElement {
						color: "red"
						onlayer: 1
						alias: "Parameter 1"
						render: "Line"
						time: "10.10.2018 10:42:48.000"
						value: 145.5
						error: "OK"
					}
					ListElement {
						color: "black"
						onlayer: 2
						alias: "Parameter 2"
						render: "Step"
						time: "10.11.2018 10:42:48.000"
						value: 165.5
						error: "OK"
					}*/
				}

				ScrollView {
					Layout.fillWidth: true
					Layout.fillHeight: true
					clip: true
					ScrollBar.horizontal.policy: ScrollBar.AsNeeded
					ScrollBar.vertical.policy: ScrollBar.AsNeeded

					Item {
						implicitWidth: chartTable.width
						implicitHeight: chartTable.height
						ListView {
							id: chartTable

							property int rowColor: Material.BlueGrey
							property int rowAlternateShade: Material.Shade400
							property int hoverShade: Material.Shade800
							property int titleColor: Material.Indigo

							implicitHeight: contentHeight

							/*ListModel {
								id: example
								ListElement {time: qsTr("10.05.2018 02:13:55.285"); value: "154.216"; error: 0}
							}*/

							TextMetrics {
								id: tableRowTM
							}

							ListModel {
								id: headerModel
								ListElement {name: qsTr("Color"); key: "color"; presetWidth: "60"}
								ListElement {name: qsTr("Parameter"); key: "alias"; presetWidth: "250"}
								ListElement {name: qsTr("Layer"); key: "onlayer"; presetWidth: "60"}
								ListElement {name: qsTr("Render"); key: "render"; presetWidth: "70"}
								ListElement {name: qsTr("Time"); key: "time"; presetWidth: "180"}
								ListElement {name: qsTr("Value"); key: "value"; presetWidth: "150"}
								ListElement {name: qsTr("State"); key: "error"; presetWidth: "195"}
							}

							header: Row {
								id: headerRow
								spacing: 1

								onWidthChanged: {
									chartTable.implicitWidth = width
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
													for (var i = 0; i < chartTable.model.count; i++) {
														tableRowTM.text = valToText(key, chartTable.model.get(i)[key])
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
											text: {
												if (key === "color") {
													return qsTr("Series color.")
												}
												qsTr("Double-click the column header to change the width of the column according to the size of the content.")
											}
										}
										background: Rectangle { color: Material.color(chartTable.titleColor) }
									}
								}
							}

							model: chartTableModel
							delegate: Column {
								property int row: index

								Row {
									id: tableRow
									spacing: chartTable.headerItem.spacing
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

									// Color
									TableColorDelegate {
										coloring: color
										width: chartTable.headerItem.itemAt(0).width
										property bool accepted: false
										onAcceptedChanged: {
											if (accepted) {
												model.color = dialogColor.colorValue.toString()
												setSeriesColorByName(model.alias, model.color)
											}
										}
										onClicked: {
											showColorDialog(this, model.color)
										}
									}

									// Alias
									TableItemDelegate {
										text: valToText("alias", alias)
										width: chartTable.headerItem.itemAt(1).width
										horizontalAlignment: Text.AlignLeft
										backDefaultColor: chartTable.rowColor
										backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, chartTable.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, chartTable.rowAlternateShade))
									}

									// Layer
									TableItemDelegate {
										text: valToText("onlayer", onlayer)
										width: chartTable.headerItem.itemAt(2).width
										horizontalAlignment: Text.AlignHCenter
										backDefaultColor: chartTable.rowColor
										backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, chartTable.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, chartTable.rowAlternateShade))
									}

									// Render
									TableItemDelegate {
										text: valToText("render", render)
										width: chartTable.headerItem.itemAt(3).width
										horizontalAlignment: Text.AlignHCenter
										backDefaultColor: chartTable.rowColor
										backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, chartTable.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, chartTable.rowAlternateShade))
									}

									// Time
									TableItemDelegate {
										text: valToText("time", time)
										width: chartTable.headerItem.itemAt(4).width
										horizontalAlignment: Text.AlignRight
										backDefaultColor: chartTable.rowColor
										backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, chartTable.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, chartTable.rowAlternateShade))
									}

									// Value
									TableItemDelegate {
										text: valToText("value", value)
										width: chartTable.headerItem.itemAt(5).width
										horizontalAlignment: Text.AlignRight
										backDefaultColor: chartTable.rowColor
										backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, chartTable.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, chartTable.rowAlternateShade))
									}

									// State
									TableItemDelegate {
										text: valToText("error", error)
										width: chartTable.headerItem.itemAt(6).width
										horizontalAlignment: Text.AlignHCenter
										backDefaultColor: chartTable.rowColor
										backgroundColor:tableRow.rowHovered ? Material.color(backDefaultColor, chartTable.hoverShade) : ((index % 2 === 0) ? Material.color(backDefaultColor) : Material.color(backDefaultColor, chartTable.rowAlternateShade))
									}
								}

								Item {
									id: rowDivider
									width: parent.width
									height: 1
								}
							}
						}
					}
				}
			}
		}
	}
}
