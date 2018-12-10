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

Item {
	visible: false

	property string dateTimeFormat: qsTr("dd.MM.yyyy hh:mm:ss.zzz")
	property int chartMaxDataSize: 20000
	property int traceTableMaxDataSize: 2000
	property string projectFileExtension: "weprex"
	property string csvSeparator: ";"
	property string csvLineEnd: "\r\n"
	property string aliasSeparator: ":"
}
