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

#ifndef WSSERIALINTERFACE_H
#define WSSERIALINTERFACE_H

#include <QtCore>
#include <QSerialPort>
#include "protocols/wsabstractrrprotocol.h"
#include "wspollingrrinterface.h"

enum class WSSerialState : quint8 {
	NONE = 0,
	TRANSMIT = 1,
	RECEIVE = 2
};

class WSSerialInterface : public WSPollingRRInterface {
Q_OBJECT

public:
	WSSerialInterface(const QString &portName, WSAbstractRRProtocol *protocolGet, QThread *mainThread, QObject *parent = nullptr);

	WSInterface type() const override;
	void workerLoop() override;

	QString portName() const;
	void setPortName(const QString &portName);

	qint32 baudRate() const;
	void setBaudRate(qint32 baudRate);

	QSerialPort::DataBits dataBits() const;
	void setDataBits(const QSerialPort::DataBits &dataBits);

	QSerialPort::Parity parity() const;
	void setParity(const QSerialPort::Parity &parity);

	QSerialPort::StopBits stopBits() const;
	void setStopBits(const QSerialPort::StopBits &stopBits);

	static QSerialPort::DataBits dataBitsFromNumber(int dataBits);
	static int dataBitsToNumber(const QSerialPort::DataBits &dataBits);

	static QSerialPort::Parity parityFromString(const QString &parity);
	static QString parityToString(const QSerialPort::Parity &parity);

	static QSerialPort::StopBits stopBitsFromString(const QString &stopBits);
	static QString stopBitsToString(const QSerialPort::StopBits &stopBits);

private:
	QSerialPort m_serial;
	WSSerialState m_state;
	QByteArray m_recvBuffer;
};

#endif // WSSERIALINTERFACE_H
