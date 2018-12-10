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

#ifndef WSABSTRACTRRPROTOCOL_H
#define WSABSTRACTRRPROTOCOL_H

#include <QtCore>
#include <memory>

enum class WSProtocolParseCode : qint8 {
	INCORRECT = -1,
	INCOMPLETE = 0,
	VALID = 1,
	VALID_BROADCAST = 2
};

enum class WSRRProtocol {
	MODBUS_RTU = 0,
	MODBUS_ASCII = 1,
	MODBUS_TCP = 2,
	DCON = 3
};

enum class WSPollingType : quint8 {
	NO = 0,
	ALWAYS = 1,
	ONCE_SET = 2
};

class WSAbstractRRProtocol : public QObject {
Q_OBJECT

public:
	explicit WSAbstractRRProtocol(quint32 bufferSize, QObject *parent = nullptr);
	virtual ~WSAbstractRRProtocol() {}
	virtual const QByteArray &constructRequest();
	virtual WSProtocolParseCode processResponse(const QByteArray &data) = 0;
	virtual void resetToDefault() = 0;
	virtual WSRRProtocol type() const = 0;
	virtual void timeoutOccurred(quint32 timeout) = 0;
	virtual bool readyToPolling() = 0;

	quint32 bufferSize() const;

protected:
	quint32 m_bufferSize;
	std::unique_ptr<char[]> m_buffer;
	QByteArray m_array;

	virtual qint32 construct(char *data) = 0;
};

#endif // WSABSTRACTRRPROTOCOL_H
