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

#ifndef WSPOLLINGRRINTERFACE_H
#define WSPOLLINGRRINTERFACE_H

#include <QtCore>
#include "wspollinginterface.h"
#include "protocols/wsabstractrrprotocol.h"

class WSPollingRRInterface : public WSPollingInterface {
Q_OBJECT

public:
	WSPollingRRInterface(WSAbstractRRProtocol *protocolGet, QThread *mainThread, QObject *parent = nullptr);
	virtual ~WSPollingRRInterface();
	virtual WSAbstractRRProtocol* protocolGet();
	virtual WSAbstractRRProtocol* protocolRelease();
	virtual void protocolReset(WSAbstractRRProtocol* proto);

	virtual quint32 transmitTimeout() const;
	virtual void setTransmitTimeout(quint32 transmitTimeout);

	virtual quint32 receiveTimeout() const;
	virtual void setReceiveTimeout(quint32 receiveTimeout);

	virtual quint32 pollingPause() const;
	virtual void setPollingPause(quint32 pollingPause);

	virtual quint32 skipPause() const;
	virtual void setSkipPause(quint32 skipPause);

	virtual quint32 errorPause() const;
	virtual void setErrorPause(quint32 errorPause);

signals:
	void transmitTimeoutOccurred(quint32 timeout);
	void receiveTimeoutOccurred(quint32 timeout);
	void trasmitted(QByteArray transmittedData);
	void received(QByteArray receivedData);
	void dataSkipped();
	void validDataReceived(QByteArray validData);
	void incompleteDataReceived(QByteArray incompleteData);
	void errorDataReceived(QByteArray errorData);

protected:
	std::unique_ptr<WSAbstractRRProtocol> m_protocol;
	quint32 m_transmitTimeout;
	quint32 m_receiveTimeout;
	quint32 m_pollingPause;
	quint32 m_skipPause;
	quint32 m_errorPause;
	bool m_releaseFlag;

	virtual void connectSlotSignals();
	virtual void disconnectSlotSignals();
};

#endif // WSPOLLINGRRINTERFACE_H
