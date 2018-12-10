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

#ifndef WSMODBUSPARAMETER_H
#define WSMODBUSPARAMETER_H

#include <QtCore>
#include <new>
#include "modbus.h"
#include "wsdataconverter.h"
#include "wsabstractrrprotocol.h"
#include <memory>

enum class WSModbusExtendedError : qint16 {
	TIMEOUT = -5,
	WRONG_CRC = -4,
	DATA_OVERFLOW = -3,
	INCORRECT_SIZE = -2,
	INCOMPLETE = -1,
	NONE = 0,
	ILLEGAL_FUNCTION = 1,
	ILLEGAL_DATA_ADDRESS = 2,
	ILLEGAL_DATA_VALUE = 3,
	SLAVE_DEVICE_FAILURE = 4
};

class WSModbusParameter {

public:
	WSModbusParameter(QString alias,
		uint8_t deviceAdr,
		uint8_t funcCode,
		uint16_t regAdr,
		uint16_t funcSize,
		WSPollingType pollingType,
		WSDataType dataType,
		WSByteOrder dataByteOrder,
		WSDataRepresent dataRepresent,
		quint8 dataTypeSize,
		bool dataSigned
		);

	quint32 timeoutsCount() const;
	void setTimeoutsCount(const quint32 &timeoutsCount);

	quint32 errorsCount() const;
	void setErrorsCount(const quint32 &errorsCount);

	quint32 responsesCount() const;
	void setResponsesCount(const quint32 &responsesCount);

	quint32 requestsCount() const;
	void setRequestsCount(const quint32 &requestsCount);

	WSByteOrder dataByteOrder() const;
	void setDataByteOrder(const WSByteOrder &dataByteOrder);

	QString alias() const;
	void setAlias(const QString &alias);

	bool enabled() const;
	void setEnabled(bool enabled);

	quint8 dataTypeSize() const;
	void setDataTypeSize(const quint8 &dataTypeSize);

	WSDataType dataType() const;
	void setDataType(const WSDataType &dataType);

	struct modbus_client_parameter *param() const;

	WSDataRepresent dataRepresent() const;
	void setDataRepresent(const WSDataRepresent &dataRepresent);

	bool dataSigned() const;
	void setDataSigned(bool dataSigned);

private:
	bool m_enabled;

	QString m_alias;
	WSDataType m_dataType;
	WSByteOrder m_dataByteOrder;
	WSDataRepresent m_dataRepresent;
	quint8 m_dataTypeSize;
	bool m_dataSigned;
	quint32 m_requestsCount;
	quint32 m_responsesCount;
	quint32 m_errorsCount;
	quint32 m_timeoutsCount;
	std::unique_ptr<struct modbus_client_parameter, void(*)(struct modbus_client_parameter*)> m_param;
	quint8 m_lastPollingType;
};

#endif // WSMODBUSPARAMETER_H
