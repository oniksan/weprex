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

#ifndef WSFILE_H
#define WSFILE_H

#include <QtCore>

class WSFile : public QFile {
Q_OBJECT

public:
	WSFile(const QString &name, QObject *parent = nullptr);
	virtual ~WSFile();
	Q_INVOKABLE inline bool openRead() {
		return QFile::open(QIODevice::ReadOnly);
	}

	Q_INVOKABLE inline bool openWrite() {
		return QFile::open(QIODevice::WriteOnly);
	}

	Q_INVOKABLE inline bool openReadWrite() {
		return QFile::open(QIODevice::ReadWrite);
	}

	Q_INVOKABLE inline void close() {
		QFile::close();
	}

	Q_INVOKABLE inline bool flush() {
		return QFile::flush();
	}

	Q_INVOKABLE inline qint64 write(const QByteArray &byteArray) {
		return QFile::write(byteArray);
	}

	Q_INVOKABLE inline qint64 write(const QString &str) {
		return QFile::write(str.toUtf8());
	}

	Q_INVOKABLE inline QByteArray readAll() {
		return QFile::readAll();
	}

};

Q_DECLARE_METATYPE(WSFile*)

#endif // WSFILEIO_H
