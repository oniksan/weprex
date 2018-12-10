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

#include "wsmodbusparameter.h"
#include "wsabstractrrprotocol.h"

WSModbusParameter::WSModbusParameter(QString alias,
									 uint8_t deviceAdr,
									 uint8_t funcCode,
									 uint16_t regAdr,
									 uint16_t funcSize,
									 WSPollingType pollingType,
									 WSDataType dataType,
									 WSByteOrder dataByteOrder,
									 WSDataRepresent dataRepresent,
									 quint8 dataTypeSize,
									 bool dataSigned) :
	m_enabled(true),
	m_alias(alias),
	m_dataType(dataType),
	m_dataByteOrder(dataByteOrder),
	m_dataRepresent(dataRepresent),
	m_dataTypeSize(dataTypeSize),
	m_dataSigned(dataSigned),
	m_requestsCount(0),
	m_responsesCount(0),
	m_errorsCount(0),
	m_timeoutsCount(0),
	m_param(modbus_client_param_create(deviceAdr, funcCode, regAdr, funcSize, static_cast<quint8>(pollingType)), modbus_client_param_destroy),
	m_lastPollingType(static_cast<quint8>(pollingType))
{
	if (m_param == nullptr) {
		throw std::bad_alloc();
	}
}

quint32 WSModbusParameter::timeoutsCount() const {
	return m_timeoutsCount;
}

void WSModbusParameter::setTimeoutsCount(const quint32 &timeoutsCount) {
	m_timeoutsCount = timeoutsCount;
}

quint32 WSModbusParameter::errorsCount() const {
	return m_errorsCount;
}

void WSModbusParameter::setErrorsCount(const quint32 &errorsCount) {
	m_errorsCount = errorsCount;
}

quint32 WSModbusParameter::responsesCount() const {
	return m_responsesCount;
}

void WSModbusParameter::setResponsesCount(const quint32 &responsesCount) {
	m_responsesCount = responsesCount;
}

quint32 WSModbusParameter::requestsCount() const {
	return m_requestsCount;
}

void WSModbusParameter::setRequestsCount(const quint32 &requestsCount) {
	m_requestsCount = requestsCount;
}

WSByteOrder WSModbusParameter::dataByteOrder() const {
	return m_dataByteOrder;
}

void WSModbusParameter::setDataByteOrder(const WSByteOrder &dataByteOrder) {
	m_dataByteOrder = dataByteOrder;
}

QString WSModbusParameter::alias() const {
	return m_alias;
}

void WSModbusParameter::setAlias(const QString &alias) {
	m_alias = alias;
}

bool WSModbusParameter::enabled() const {
	return m_enabled;
}

void WSModbusParameter::setEnabled(bool enabled) {
	if (enabled) {
		m_param->type = m_lastPollingType;
	} else {
		m_lastPollingType = m_param->type;
		m_param->type = MB_NO_POLLING;
		m_requestsCount = 0;
		m_responsesCount = 0;
		m_errorsCount = 0;
		m_timeoutsCount = 0;
	}
	m_enabled = enabled;
}

quint8 WSModbusParameter::dataTypeSize() const {
	return m_dataTypeSize;
}

void WSModbusParameter::setDataTypeSize(const quint8 &dataTypeSize) {
	m_dataTypeSize = dataTypeSize;
}

WSDataType WSModbusParameter::dataType() const {
	return m_dataType;
}

void WSModbusParameter::setDataType(const WSDataType &dataType) {
	m_dataType = dataType;
}

modbus_client_parameter *WSModbusParameter::param() const {
	return m_param.get();
}

WSDataRepresent WSModbusParameter::dataRepresent() const {
	return m_dataRepresent;
}

void WSModbusParameter::setDataRepresent(const WSDataRepresent &dataRepresent) {
	m_dataRepresent = dataRepresent;
}

bool WSModbusParameter::dataSigned() const {
	return m_dataSigned;
}

void WSModbusParameter::setDataSigned(bool dataSigned) {
	m_dataSigned = dataSigned;
}
