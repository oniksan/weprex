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

RowLayout {
	property string title
	property color baseColor: Material.foreground
	property color errorColor: Material.color(Material.Orange)
	property url baseIconUrl: "qrc:/icon/info.png"
	property url errorIconUrl: "qrc:/icon/err.png"
	property string baseTooltipText
	property string errorTooltipText
	property bool error: false

	onErrorChanged: {
		if (error) {
			label.color = errorColor
			tooltip.text = baseTooltipText + "<br/><br/><font color=\"" + errorColor + "\">" + errorTooltipText + "</font>"
			img.source = errorIconUrl
		} else {
			label.color = baseColor
			tooltip.text = baseTooltipText
			img.source = baseIconUrl
		}
	}

	Label {
		id: label
		text: title
		font.bold: true
		color: baseColor
	}
	Control {
		width: img.width
		height: img.height
		Image {
			id: img
			fillMode: Image.Pad
			smooth: false
			mipmap: false
			autoTransform: false
			source: baseIconUrl
		}
		ToolTip {
			id: tooltip
			delay: Qt.styleHints.mousePressAndHoldInterval
			visible: parent.hovered
			text: baseTooltipText
		}
	}
}

