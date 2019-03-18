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

#include "wsmodbusrtuprotocol.h"

WSModbusRTUProtocol::WSModbusRTUProtocol(quint32 bufferSize) :
	WSAbstractRRProtocol(bufferSize),
	m_hModbusClient(modbus_client_rtu_create(), modbus_client_rtu_destroy),
	m_params([this](quint32 id) {paramCountChanged(id);})
{
	if (m_hModbusClient.get() == nullptr) {
		throw std::bad_alloc();
	}
}

WSRRProtocol WSModbusRTUProtocol::type() const {
	return WSRRProtocol::MODBUS_RTU;
}

qint32 WSModbusRTUProtocol::construct(char *data) {
	quint16 size = 0;
	modbus_client_rtu_request(m_hModbusClient.get(), reinterpret_cast<uint8_t*>(data), &size);
	if (size > 0) {
		quint32 pid;
		WSModbusParameter *p;
		pid = getCurrentParamId();
		p = m_params.get(pid);
		p->setRequestsCount(p->requestsCount() + 1);
		emit parameterRequest(pid, p);
	}
	return size;
}

WSParametersHash<WSModbusParameter> &WSModbusRTUProtocol::params() {
	return m_params;
}

WSProtocolParseCode WSModbusRTUProtocol::processResponse(const QByteArray &data) {
	WSProtocolParseCode code = WSProtocolParseCode::INCORRECT;
	char *d = const_cast<char*>(data.data());
	code = static_cast<WSProtocolParseCode>(modbus_client_rtu_response(m_hModbusClient.get(), reinterpret_cast<uint8_t*>(d), static_cast<uint16_t>(data.size())));
	// Get id from index
	quint32 pid;
	WSModbusParameter *p;
	pid = getCurrentParamId();
	p = m_params.get(pid);
	// Valid data
	if (code == WSProtocolParseCode::VALID || code == WSProtocolParseCode::VALID_BROADCAST) {
		// Responded data
		if (p->param()->err == MB_EC_NO_ERR) {
			p->setResponsesCount(p->responsesCount() + 1);
			emit parameterChanged(pid, p);
		// Parsed error
		} else {
			p->setErrorsCount(p->errorsCount() + 1);
			emit parameterError(pid, p);
		}
	// Invalid data
	} else if (code != WSProtocolParseCode::INCOMPLETE) {
		p->setErrorsCount(p->errorsCount() + 1);
		emit parameterError(pid, p);
	}
	return code;
}

void WSModbusRTUProtocol::resetToDefault() {
	modbus_client_rtu_reset(m_hModbusClient.get());
}

void WSModbusRTUProtocol::paramCountChanged(quint32) {
	struct utils_vect_handle* vect = m_hModbusClient->params;
	// Clear internal (library) parameters list
	utils_vector_clear(vect);
	// Clear indexes vector
	m_paramIdToLibIndex.clear();
	for (auto const& p: m_params) {
		// Add parameter into library
		utils_vect_append(vect, p.second->param());
		// Add index
		m_paramIdToLibIndex.append(p.first);
	}
	resetToDefault();
}

quint32 WSModbusRTUProtocol::getParamIdFromLibIndex(int id) const {
	return m_paramIdToLibIndex.at(id);
}

quint32 WSModbusRTUProtocol::getCurrentParamId() const {
	return getParamIdFromLibIndex(m_hModbusClient->param_counter - 1);
}

void WSModbusRTUProtocol::timeoutOccurred(quint32) {
	// Get id from index
	quint32 pid;
	WSModbusParameter *p;
	pid = getCurrentParamId();
	p = m_params.get(pid);
	p->setTimeoutsCount(p->timeoutsCount() + 1);
	emit parameterTimeout(pid, m_params.get(pid));
}

bool WSModbusRTUProtocol::readyToPolling() {
	return (m_params.begin() != m_params.end());
}

