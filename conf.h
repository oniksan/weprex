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

#ifndef WSCONF_H
#define WSCONF_H

#include <QtCore>

class Conf {
public:
	static const quint8 FLOAT_DATA_PRECISION;
	static const char* MULTI_DATA_DIVIDER;
	static const quint32 DEVICE_RECONNECTION_PAUSE;
	static const quint32 DEVICE_CONNECTION_WAIT_TIME;
	static const quint32 DEVICE_DISCONNECTION_WAIT_TIME;
	static const bool DEVICE_TCP_AUTO_RECONNECT;
	static const quint32 DEVICE_THREAD_SLEEP_PAUSE;
	static const quint32 DEVICE_ERROR_SLEEP_PAUSE;
	static const QString DEFAULT_STORE_SETTINGS_FILE;
	static const QString MANUAL_FILE_PATH;

	static const QString storeSettingsPath();
};

#endif // WSCONF_H
