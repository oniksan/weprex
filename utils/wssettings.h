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

#ifndef WSSETTINGS_H
#define WSSETTINGS_H

#include <QSettings>

class WSSettings : public QSettings {
Q_OBJECT

public:
	explicit WSSettings(const QString &fileName, QObject *parent = nullptr);
	Q_INVOKABLE inline void setValue(const QString &key, const QVariant &value) {
		QSettings::setValue(key, value);
	}

	Q_INVOKABLE inline QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) {
		return QSettings::value(key, defaultValue);
	}

	Q_INVOKABLE inline void beginGroup(const QString &prefix) {
		QSettings::beginGroup(prefix);
	}

	Q_INVOKABLE inline void endGroup() {
		QSettings::endGroup();
	}

	Q_INVOKABLE inline int beginReadArray(const QString &prefix) {
		return QSettings::beginReadArray(prefix);
	}

	Q_INVOKABLE inline void beginWriteArray(const QString &prefix, int size = -1) {
		QSettings::beginWriteArray(prefix, size);
	}

	Q_INVOKABLE inline void setArrayIndex(int i) {
		QSettings::setArrayIndex(i);
	}

	Q_INVOKABLE inline void endArray() {
		QSettings::endArray();
	}

	Q_INVOKABLE inline void clear() {
		QSettings::clear();
	}

	Q_INVOKABLE inline bool isWritable() {
		return QSettings::isWritable();
	}
};

Q_DECLARE_METATYPE(WSSettings*)

#endif // WSSETTINGS_H
