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
import QtQuick.Controls.Material 2.12

ItemDelegate {
	id: control
	padding: 5
	height: 30
	Behavior on backgroundColor { PropertyAnimation {easing.type: Easing.Linear; duration: 300} }

	property int backDefaultColor : Material.Indigo
	property color backgroundColor : Material.color(backDefaultColor)
	property alias horizontalAlignment: content.horizontalAlignment
	property alias verticalAlignment: content.verticalAlignment
	property alias color: content.color
	property color defaultColor : Material.foreground
	property bool selected : false
	signal onHovered(var hov)
	onHoveredChanged: {
		onHovered(hovered)
	}
	signal onSelected(var sel)
	onClicked: {
		selected = !selected
		onSelected(selected)
	}
	contentItem: Text {
		id: content
		text: control.text
		font: control.font
		color: defaultColor
		elide: Text.ElideRight
		horizontalAlignment: Text.AlignHCenter
	}

	background: Rectangle {
		width: parent.width
		color: backgroundColor
	}
	ToolTip.visible: hovered
	ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
	ToolTip.text: text
}
