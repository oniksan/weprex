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

#include <conf.h>

const quint8 Conf::FLOAT_DATA_PRECISION = 6;
const char* Conf::MULTI_DATA_DIVIDER = " ";
const quint32 Conf::DEVICE_RECONNECTION_PAUSE = 2000;
const quint32 Conf::DEVICE_CONNECTION_WAIT_TIME = 10000;
const quint32 Conf::DEVICE_DISCONNECTION_WAIT_TIME = 20000;
const bool Conf::DEVICE_TCP_AUTO_RECONNECT = true;
const quint32 Conf::DEVICE_THREAD_SLEEP_PAUSE = 1;
const quint32 Conf::DEVICE_ERROR_SLEEP_PAUSE = 50;
const QString Conf::DEFAULT_STORE_SETTINGS_FILE = "appset.ini";
const QString Conf::MANUAL_FILE_PATH = "weprex_0.1.1_manual.pdf";

const QString Conf::storeSettingsPath() {
	return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + DEFAULT_STORE_SETTINGS_FILE;
}


