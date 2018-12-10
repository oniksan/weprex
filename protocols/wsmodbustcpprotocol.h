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

#ifndef WSMODBUSTCPPROTOCOL_H
#define WSMODBUSTCPPROTOCOL_H

#include <new>
#include <memory>
#include "modbus.h"
#include "libdefs.h"
#include "wsabstractrrprotocol.h"
#include "wsmodbusparameter.h"
#include "wsparametershash.h"

class WSModbusTCPProtocol : public WSAbstractRRProtocol {
Q_OBJECT

public:
	WSModbusTCPProtocol(quint32 bufferSize = 255);
	virtual WSProtocolParseCode processResponse(const QByteArray &data) override;
	virtual void resetToDefault() override;
	virtual WSRRProtocol type() const override;
	
	WSParametersHash<WSModbusParameter> &params();

	void timeoutOccurred(quint32 timeout) override;
	bool readyToPolling() override;

private:
	std::unique_ptr<struct modbus_tcp_client_handler, void(*)(struct modbus_tcp_client_handler*)> m_hModbusClient;
	WSParametersHash<WSModbusParameter> m_params;
	QVector<quint32> m_paramIdToLibIndex;
	void paramCountChanged(quint32 id);
	quint32 getParamIdFromLibIndex(int id) const;
	quint32 getCurrentParamId() const;

protected:
	virtual qint32 construct(char *data) override;

signals:
	void parameterChanged(quint32 id, WSModbusParameter *param);
	void parameterError(quint32 id, WSModbusParameter *param);
	void parameterTimeout(quint32 id, WSModbusParameter *param);
	void parameterRequest(quint32 id, WSModbusParameter *param);
};

#endif // WSMODBUSTCPPROTOCOL_H
