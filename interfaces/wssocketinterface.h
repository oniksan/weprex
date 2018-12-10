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

#ifndef WSSOCKETINTERFACE_H
#define WSSOCKETINTERFACE_H

#include <memory>
#include <QObject>
#include <QtCore>
#include <QtNetwork>
#include <QTcpSocket>
#include "protocols/wsabstractrrprotocol.h"
#include "wspollingrrinterface.h"

enum class WSSocketState : quint8 {
	NONE = 0,
	TRANSMIT = 1,
	RECEIVE = 2
};

class WSSocketInterface : public WSPollingRRInterface {
Q_OBJECT

public:
	WSSocketInterface(QHostAddress ipAddress, quint16 port, WSAbstractRRProtocol *protocolGet, QThread *mainThread, QObject *parent = nullptr);

	WSInterface type() const override;
	virtual void workerLoop() override;

	QHostAddress ipAddress() const;
	void setIpAddress(const QHostAddress &ipAddress);

	quint16 port() const;
	void setPort(quint16 port);

	bool autoReconnect() const;
	void setAutoReconnect(bool autoReconnect);

private:
	QTcpSocket m_socket;
	QHostAddress m_ipAddress;
	quint16 m_port;
	bool m_autoReconnect;
	WSSocketState m_state;
	QByteArray m_recvBuffer;
};

#endif // WSSOCKETINTERFACE_H
