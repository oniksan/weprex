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

#include "wsqmlapplication.h"

WSQMLApplication::WSQMLApplication(QObject *parent) :
	QObject(parent),
	m_interfacesCounter(0),
	m_logInterfaceData(false)
{
	m_storeSettings = nullptr;
	refreshAvailablePorts();
}

WSQMLApplication::~WSQMLApplication() {
	if (m_storeSettings != nullptr) {
		delete m_storeSettings;
	}
}

const QString WSQMLApplication::whoIAm = "App core";

QString WSQMLApplication::dataTypeToString(quint8 type) {
	switch (type) {
		case static_cast<quint8>(WSDataType::INTEGER):
			return QString("int");
		case static_cast<quint8>(WSDataType::FLOAT):
			return QString("float");
		case static_cast<quint8>(WSDataType::TEXT):
			return QString("text");
	}
	return "undefined";
}

quint8 WSQMLApplication::stringToDataType(const QString &type) {
	if (type =="int") {
		return static_cast<quint8>(WSDataType::INTEGER);
	} else if (type =="float") {
		return static_cast<quint8>(WSDataType::FLOAT);
	} else if (type =="text") {
		return static_cast<quint8>(WSDataType::TEXT);
	} else {
		return static_cast<quint8>(WSDataType::INTEGER);
	}
}

QString WSQMLApplication::dataByteOrderToString(quint8 order) {
	switch (order) {
		case static_cast<quint8>(WSByteOrder::FORWARD):
			return QString("forward");
		case static_cast<quint8>(WSByteOrder::BACKWARD):
			return QString("backward");
		case static_cast<quint8>(WSByteOrder::FORWARD_WORDS_REVERSE):
			return QString("forward_rev");
		case static_cast<quint8>(WSByteOrder::BACKWARD_WORDS_REVERSE):
			return QString("backward_rev");
	}
	return "undefined";
}

quint8 WSQMLApplication::stringToDataByteOrder(const QString &order) {
		if (order == "forward") {
			return static_cast<quint8>(WSByteOrder::FORWARD);
		} else if (order == "backward") {
			return static_cast<quint8>(WSByteOrder::BACKWARD);
		} else if (order == "forward_rev") {
			return static_cast<quint8>(WSByteOrder::FORWARD_WORDS_REVERSE);
		} else if (order == "backward_rev") {
			return static_cast<quint8>(WSByteOrder::BACKWARD_WORDS_REVERSE);
		} else {
			return static_cast<quint8>(WSByteOrder::FORWARD);
		}
}

QString WSQMLApplication::dataRepresentToString(quint8 represent) {
	switch (represent) {
		case static_cast<quint8>(WSDataRepresent::BIN):
			return QString("bin");
		case static_cast<quint8>(WSDataRepresent::OCT):
			return QString("oct");
		case static_cast<quint8>(WSDataRepresent::DEC):
			return QString("dec");
		case static_cast<quint8>(WSDataRepresent::HEX):
			return QString("hex");
		case static_cast<quint8>(WSDataRepresent::ASCII):
			return QString("ascii");
	}
	return "undefined";
}

quint8 WSQMLApplication::stringToDataRepresent(const QString &represent) {
	if (represent == "bin") {
		return static_cast<quint8>(WSDataRepresent::BIN);
	} else if (represent == "oct") {
		return static_cast<quint8>(WSDataRepresent::OCT);
	} else if (represent == "dec") {
		return static_cast<quint8>(WSDataRepresent::DEC);
	} else if (represent == "hex") {
		return static_cast<quint8>(WSDataRepresent::HEX);
	} else if (represent == "ascii") {
		return static_cast<quint8>(WSDataRepresent::ASCII);
	} else {
		return static_cast<quint8>(WSDataRepresent::DEC);
	}
}

QString WSQMLApplication::pollingTypeToString(quint8 type) {
	switch (type) {
		case static_cast<quint8>(WSPollingType::NO):
			return QString("none");
		case static_cast<quint8>(WSPollingType::ALWAYS):
			return QString("read");
		case static_cast<quint8>(WSPollingType::ONCE_SET):
			return QString("write");
	}
	return "undefined";
}

quint8 WSQMLApplication::stringToPollingType(const QString &type) {
	if (type == "none") {
		return static_cast<quint8>(WSPollingType::NO);
	} else if (type == "read") {
		return static_cast<quint8>(WSPollingType::ALWAYS);
	} else if (type == "write") {
		return static_cast<quint8>(WSPollingType::ONCE_SET);
	} else {
		return static_cast<quint8>(WSPollingType::NO);
	}
}

QStringList WSQMLApplication::getAvailablePortNames() {
	return m_availablePortNames;
}

void WSQMLApplication::refreshAvailablePorts() {
	m_availablePortNames.clear();
	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &info : ports) {
		if (!info.isBusy()) {
			m_availablePortNames.append(info.portName());
		}
	}
	emit availablePortNamesChanged();
}

quint32 WSQMLApplication::addInterface(QJSValue data) {
	quint32 iid = m_interfacesCounter + 1;
	// Protocol instance
	QString protocol = data.property("protocol").toString();
	WSAbstractRRProtocol *proto = nullptr;
	if (protocol == "modbus_rtu") {
		proto = new WSModbusRTUProtocol();
		connect(static_cast<WSModbusRTUProtocol*>(proto), &WSModbusRTUProtocol::parameterChanged, [=](quint32 paramId, WSModbusParameter *param) {onParameterModbusValueChanged(iid, paramId, param);});
		connect(static_cast<WSModbusRTUProtocol*>(proto), &WSModbusRTUProtocol::parameterError, [=](quint32 paramId, WSModbusParameter *param) {onParameterModbusError(iid, paramId, param);});
		connect(static_cast<WSModbusRTUProtocol*>(proto), &WSModbusRTUProtocol::parameterTimeout, [=](quint32 paramId, WSModbusParameter *param) {onParameterTimeout(iid, paramId, param);});
		connect(static_cast<WSModbusRTUProtocol*>(proto), &WSModbusRTUProtocol::parameterRequest, [=](quint32 paramId, WSModbusParameter *param) {onParameterRequest(iid, paramId, param);});
	} else if (protocol == "modbus_ascii") {
		return 0;
	} else if (protocol == "modbus_tcp") {
		proto = new WSModbusTCPProtocol();
		connect(static_cast<WSModbusTCPProtocol*>(proto), &WSModbusTCPProtocol::parameterChanged, [=](quint32 paramId, WSModbusParameter *param) {onParameterModbusValueChanged(iid, paramId, param);});
		connect(static_cast<WSModbusTCPProtocol*>(proto), &WSModbusTCPProtocol::parameterError, [=](quint32 paramId, WSModbusParameter *param) {onParameterModbusError(iid, paramId, param);});
		connect(static_cast<WSModbusTCPProtocol*>(proto), &WSModbusTCPProtocol::parameterTimeout, [=](quint32 paramId, WSModbusParameter *param) {onParameterTimeout(iid, paramId, param);});
		connect(static_cast<WSModbusTCPProtocol*>(proto), &WSModbusTCPProtocol::parameterRequest, [=](quint32 paramId, WSModbusParameter *param) {onParameterRequest(iid, paramId, param);});
	} else if (protocol == "dcon") {
		return 0;
	} else {
		return 0;
	}
	// Transport instance
	WSPollingRRInterface *iface;
	QString transport = data.property("transportType").toString();

	// Serial transport
	if (transport == "serial") {
		iface = new WSSerialInterface(
			data.property("serial").property("port").toString(),
			proto,
			QThread::currentThread()
		);
		proto->setParent(iface);
		(static_cast<WSSerialInterface*>(iface))->setBaudRate(data.property("serial").property("baudrate").toInt());
		(static_cast<WSSerialInterface*>(iface))->setDataBits(WSSerialInterface::dataBitsFromNumber(data.property("serial").property("dataBits").toInt()));
		(static_cast<WSSerialInterface*>(iface))->setParity(WSSerialInterface::parityFromString(data.property("serial").property("parity").toString()));
		(static_cast<WSSerialInterface*>(iface))->setStopBits(WSSerialInterface::stopBitsFromString(data.property("serial").property("stopBits").toString()));

	// TCP transport
	} else if (transport == "tcp") {
		iface = new WSSocketInterface(
			QHostAddress(data.property("tcp").property("ipAddress").toString()),
			static_cast<quint16>(data.property("tcp").property("port").toUInt()),
			proto,
			QThread::currentThread()
		);
		proto->setParent(iface);

	// Undefined transport
	} else {
		delete proto;
		return 0;
	}
	// Common interface parameters
	iface->setTransmitTimeout(data.property("timeout").toUInt());
	iface->setReceiveTimeout(data.property("timeout").toUInt());
	iface->setPollingPause(data.property("pollingPause").toUInt());
	connect(iface, &WSPollingInterface::started, this, [=]() {onInterfacePollingStarted(iid);});
	connect(iface, &WSPollingInterface::connected, this, [=]() {onInterfacePollingConnected(iid);});
	connect(iface, &WSPollingInterface::disconnected, this, [=]() {onInterfacePollingDisconnected(iid);});
	connect(iface, &WSPollingInterface::stopped, this, [=]() {onInterfacePollingStopped(iid);});
	connect(iface, &WSPollingRRInterface::connectionErrorOccurred, this, [=]() {onInterfaceConnectionError(iid);});
	connect(iface, &WSPollingRRInterface::reconnecting, this, [=]() {onInterfaceReconnecting(iid);});
	connect(iface, &WSPollingRRInterface::trasmitted, this, [=](QByteArray transmittedData) {onTransmittedData(iid, transmittedData);});
	connect(iface, &WSPollingRRInterface::received, this, [=](QByteArray receivedData) {onReceivedData(iid, receivedData);});
	connect(iface, &WSPollingRRInterface::transmitTimeoutOccurred, this, [=](quint32 timeout) {onTransmitTimeoutOccurred(iid, timeout);});
	connect(iface, &WSPollingRRInterface::receiveTimeoutOccurred, this, [=](quint32 timeout) {onReceiveTimeoutOccurred(iid, timeout);});
	connect(iface, &WSPollingRRInterface::validDataReceived, this, [=]() {onValidDataReceived(iid);});
	//connect(iface, &WSPollingRRInterface::incompleteDataReceived, this, [=]() {onIncompleteDataReceived(iid);});
	connect(iface, &WSPollingRRInterface::errorDataReceived, this, [=]() {onErrorDataReceived(iid);});

	m_interfaces[iid] = std::unique_ptr<WSPollingInterface>(iface);
	m_interfacesCounter++;
	return iid;
}

bool WSQMLApplication::editInterface(quint32 id, QJSValue data) {
	if (m_interfaces.find(id) != m_interfaces.end()) {
		WSPollingInterface *iface = m_interfaces[id].get();
		// PollingRRInterface
		if (iface->type() == WSInterface::TCP || iface->type() == WSInterface::SERIAL) {
			WSPollingRRInterface *ifaceRR = static_cast<WSPollingRRInterface*>(iface);
			// Check protocols change
			WSRRProtocol protocolType = ifaceRR->protocolGet()->type();
			if (data.hasProperty("protocol")) {
				QString protocol = data.property("protocol").toString();
				if (protocolType == WSRRProtocol::MODBUS_TCP && protocol != "modbus_tcp") {
					return false;
				} else if (protocolType == WSRRProtocol::MODBUS_RTU && protocol != "modbus_rtu") {
					return false;
				} else if (protocolType == WSRRProtocol::MODBUS_ASCII && protocol == "modbus_ascii") {
					return false;
				} else if (protocolType == WSRRProtocol::DCON && protocol == "dcon") {
					return false;
				}
			}
			// Check transport
			WSInterface transportType = ifaceRR->type();
			if (data.hasProperty("transportType")) {
				bool protocolChanged = false;
				QString transport = data.property("transportType").toString();
				if (transport == "serial" && transportType == WSInterface::TCP) {
					WSAbstractRRProtocol *proto = ifaceRR->protocolRelease();
					proto->setParent(this);
					m_interfaces[id].reset(nullptr);
					iface = new WSSerialInterface(
						data.property("serial").property("port").toString(),
						proto,
						QThread::currentThread()
					);
					proto->setParent(iface);
					(static_cast<WSSerialInterface*>(iface))->setBaudRate(data.property("serial").property("baudrate").toInt());
					(static_cast<WSSerialInterface*>(iface))->setDataBits(WSSerialInterface::dataBitsFromNumber(data.property("serial").property("dataBits").toInt()));
					(static_cast<WSSerialInterface*>(iface))->setParity(WSSerialInterface::parityFromString(data.property("serial").property("parity").toString()));
					(static_cast<WSSerialInterface*>(iface))->setStopBits(WSSerialInterface::stopBitsFromString(data.property("serial").property("stopBits").toString()));
					protocolChanged = true;
				} else if (transport == "tcp" && transportType == WSInterface::SERIAL) {
					WSAbstractRRProtocol *proto = ifaceRR->protocolRelease();
					proto->setParent(this);
					m_interfaces[id].reset(nullptr);
					iface = new WSSocketInterface(
						QHostAddress(data.property("tcp").property("ipAddress").toString()),
						static_cast<quint16>(data.property("tcp").property("port").toUInt()),
						proto,
						QThread::currentThread()
					);
					proto->setParent(iface);
					protocolChanged = true;
				} else if (transportType == WSInterface::SERIAL) {
					(static_cast<WSSerialInterface*>(iface))->setBaudRate(data.property("serial").property("baudrate").toInt());
					(static_cast<WSSerialInterface*>(iface))->setDataBits(WSSerialInterface::dataBitsFromNumber(data.property("serial").property("dataBits").toInt()));
					(static_cast<WSSerialInterface*>(iface))->setParity(WSSerialInterface::parityFromString(data.property("serial").property("parity").toString()));
					(static_cast<WSSerialInterface*>(iface))->setStopBits(WSSerialInterface::stopBitsFromString(data.property("serial").property("stopBits").toString()));
				} else if (transportType == WSInterface::TCP) {
					(static_cast<WSSocketInterface*>(iface))->setIpAddress(QHostAddress(data.property("tcp").property("ipAddress").toString()));
					(static_cast<WSSocketInterface*>(iface))->setPort(static_cast<quint16>(data.property("tcp").property("port").toUInt()));
				}
				if (protocolChanged) {
					ifaceRR = static_cast<WSPollingRRInterface*>(iface);
					connect(ifaceRR, &WSPollingInterface::started, this, [=]() {onInterfacePollingStarted(id);});
					connect(ifaceRR, &WSPollingInterface::connected, this, [=]() {onInterfacePollingConnected(id);});
					connect(ifaceRR, &WSPollingInterface::disconnected, this, [=]() {onInterfacePollingDisconnected(id);});
					connect(ifaceRR, &WSPollingInterface::stopped, this, [=]() {onInterfacePollingStopped(id);});
					connect(ifaceRR, &WSPollingRRInterface::connectionErrorOccurred, this, [=]() {onInterfaceConnectionError(id);});
					connect(ifaceRR, &WSPollingRRInterface::reconnecting, this, [=]() {onInterfaceReconnecting(id);});
					connect(ifaceRR, &WSPollingRRInterface::trasmitted, this, [=](QByteArray transmittedData) {onTransmittedData(id, transmittedData);});
					connect(ifaceRR, &WSPollingRRInterface::received, this, [=](QByteArray receivedData) {onReceivedData(id, receivedData);});
					connect(ifaceRR, &WSPollingRRInterface::transmitTimeoutOccurred, this, [=](quint32 timeout) {onTransmitTimeoutOccurred(id, timeout);});
					connect(ifaceRR, &WSPollingRRInterface::receiveTimeoutOccurred, this, [=](quint32 timeout) {onReceiveTimeoutOccurred(id, timeout);});
					connect(ifaceRR, &WSPollingRRInterface::validDataReceived, this, [=]() {onValidDataReceived(id);});
					//connect(ifaceRR, &WSPollingRRInterface::incompleteDataReceived, this, [=]() {onIncompleteDataReceived(id);});
					connect(ifaceRR, &WSPollingRRInterface::errorDataReceived, this, [=]() {onErrorDataReceived(id);});
					m_interfaces[id] = std::unique_ptr<WSPollingInterface>(iface);
				}
			}
			// Common interface parameters
			ifaceRR->setTransmitTimeout(data.property("timeout").toUInt());
			ifaceRR->setReceiveTimeout(data.property("timeout").toUInt());
			ifaceRR->setPollingPause(data.property("pollingPause").toUInt());
			return true;
		}
	}
	return false;
}

bool WSQMLApplication::removeInterface(quint32 id) {
	if (m_interfaces.find(id) != m_interfaces.end()) {
		if (m_interfacesThreads.find(id) != m_interfacesThreads.end()) {
			m_interfaces[id]->stop();
		}
		m_interfaces.erase(id);
		if (m_interfaces.size() == 0) {
			m_interfacesCounter = 0;
		}
		return true;
	}
	return false;
}

quint32 WSQMLApplication::addParameter(quint32 interfaceId, QJSValue data) {
	// Find interface
	if (m_interfaces.find(interfaceId) != m_interfaces.end()) {
		WSPollingInterface *iface = m_interfaces[interfaceId].get();
		// PollingRRInterface
		if (iface->type() == WSInterface::TCP || iface->type() == WSInterface::SERIAL) {
			WSRRProtocol protocolType = static_cast<WSPollingRRInterface*>(iface)->protocolGet()->type();
			// ***** Modbus TCP/RTU *****
			if (protocolType == WSRRProtocol::MODBUS_TCP || protocolType == WSRRProtocol::MODBUS_RTU) {
				WSAbstractRRProtocol *prot = static_cast<WSAbstractRRProtocol*>(static_cast<WSPollingRRInterface*>(iface)->protocolGet());
				WSModbusParameter *p = new WSModbusParameter(
					data.property("alias").toString(),
					static_cast<quint8>(data.property("devadr").toUInt()),
					static_cast<quint8>(data.property("fcode").toUInt()),
					static_cast<quint16>(data.property("adr").toUInt()),
					static_cast<quint16>(data.property("count").toUInt()),
					static_cast<WSPollingType>(stringToPollingType(data.property("type").toString())),
					static_cast<WSDataType>(stringToDataType(data.property("view").property("type").toString())),
					static_cast<WSByteOrder>(stringToDataByteOrder(data.property("view").property("order").toString())),
					static_cast<WSDataRepresent>(stringToDataRepresent(data.property("view").property("represent").toString())),
					static_cast<quint8>(data.property("view").property("bytes").toUInt()),
					data.property("view").property("sign").toBool()
					);
				p->setEnabled(data.property("poll").toBool());
				if (data.hasProperty("setted_value")) {
					memcpy(p->param()->value, data.property("setted_value").toVariant().toByteArray().constData(), p->param()->size);
				}
				quint32 id = 0;
				if (protocolType == WSRRProtocol::MODBUS_TCP) {
					id = static_cast<WSModbusTCPProtocol*>(prot)->params().lastId() + 1;
					 static_cast<WSModbusTCPProtocol*>(prot)->params().append(id, p);
				} else if (protocolType == WSRRProtocol::MODBUS_RTU) {
					id =  static_cast<WSModbusRTUProtocol*>(prot)->params().lastId() + 1;
					 static_cast<WSModbusRTUProtocol*>(prot)->params().append(id, p);
				}
				return id;
			}
		}
	}
	return 0;
}

bool WSQMLApplication::editParameter(quint32 interfaceId, quint32 id, QJSValue data) {
	// Find interface
	if (m_interfaces.find(interfaceId) != m_interfaces.end()) {
		WSPollingInterface *iface = m_interfaces[interfaceId].get();
		// PollingRRInterface
		if (iface->type() == WSInterface::TCP || iface->type() == WSInterface::SERIAL) {
			WSRRProtocol protocolType = static_cast<WSPollingRRInterface*>(iface)->protocolGet()->type();
			// ***** Modbus TCP / RTU *****
			if (protocolType == WSRRProtocol::MODBUS_TCP || protocolType == WSRRProtocol::MODBUS_RTU) {
				WSModbusParameter *p = nullptr;
				if (protocolType == WSRRProtocol::MODBUS_TCP) {
					WSModbusTCPProtocol *mtcp = static_cast<WSModbusTCPProtocol*>(static_cast<WSPollingRRInterface*>(iface)->protocolGet());
					if (mtcp->params().contains(id)) {
						p = mtcp->params().get(id);
					}
				} else if (protocolType == WSRRProtocol::MODBUS_RTU) {
					WSModbusRTUProtocol *mrtu = static_cast<WSModbusRTUProtocol*>(static_cast<WSPollingRRInterface*>(iface)->protocolGet());
					if (mrtu->params().contains(id)) {
						p = mrtu->params().get(id);
					}
				}
				// Edit parameter
				if (p != nullptr) {
					if (data.hasProperty("alias")) {
						p->setAlias(data.property("alias").toString());
					}
					if (data.hasProperty("devadr")) {
						p->param()->device_adr = static_cast<quint8>(data.property("devadr").toUInt());
					}
					if (data.hasProperty("fcode")) {
						p->param()->function_code = static_cast<quint8>(data.property("fcode").toUInt());
					}
					if (data.hasProperty("adr")) {
						p->param()->register_adr = static_cast<quint16>(data.property("adr").toUInt());
					}
					if (data.hasProperty("count")) {
						p->param()->func_size = static_cast<quint16>(data.property("count").toUInt());
						// Recalculate and reallocate value size
						if (modbus_client_calc_param_size(p->param()) == 0) {
							return false;
						}
					}
					if (data.hasProperty("type")) {
						p->param()->type = stringToPollingType(data.property("type").toString());
					}
					if (data.hasProperty("view")) {
						if (data.property("view").hasProperty("type")) {
							p->setDataType(static_cast<WSDataType>(stringToDataType(data.property("view").property("type").toString())));
						}
						if (data.property("view").hasProperty("order")) {
							p->setDataByteOrder(static_cast<WSByteOrder>(stringToDataByteOrder(data.property("view").property("order").toString())));
						}
						if (data.property("view").hasProperty("represent")) {
							p->setDataRepresent(static_cast<WSDataRepresent>(stringToDataRepresent(data.property("view").property("represent").toString())));
						}
						if (data.property("view").hasProperty("bytes")) {
							p->setDataTypeSize(static_cast<quint8>(data.property("view").property("bytes").toUInt()));
						}
						if (data.property("view").hasProperty("sign")) {
							p->setDataSigned(data.property("view").property("sign").toBool());
						}
					}
					if (data.hasProperty("poll")) {
						p->setEnabled(data.property("poll").toBool());
					}
					if (data.hasProperty("setted_value")) {
						memcpy(p->param()->value, data.property("setted_value").toVariant().toByteArray().constData(), p->param()->size);
						QByteArray arrr = data.property("setted_value").toVariant().toByteArray();
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool WSQMLApplication::removeParameter(quint32 interfaceId, quint32 id) {
	// Find interface
	if (m_interfaces.find(interfaceId) != m_interfaces.end()) {
		WSPollingInterface *iface = m_interfaces[interfaceId].get();
		// PollingRRInterface
		if (iface->type() == WSInterface::TCP || iface->type() == WSInterface::SERIAL) {
			WSRRProtocol protocolType = static_cast<WSPollingRRInterface*>(iface)->protocolGet()->type();
			// ***** Modbus TCP/RTU *****
			WSAbstractRRProtocol *prot = static_cast<WSAbstractRRProtocol*>(static_cast<WSPollingRRInterface*>(iface)->protocolGet());
			// Delete parameter
			if (protocolType == WSRRProtocol::MODBUS_TCP) {
				if (static_cast<WSModbusTCPProtocol*>(prot)->params().contains(id)) {
					static_cast<WSModbusTCPProtocol*>(prot)->params().remove(id);
					return true;
				}
			} else if (protocolType == WSRRProtocol::MODBUS_RTU) {
				if (static_cast<WSModbusRTUProtocol*>(prot)->params().contains(id)) {
					static_cast<WSModbusRTUProtocol*>(prot)->params().remove(id);
					return true;
				}
			}
		}
	}
	return false;
}

bool WSQMLApplication::startInterfacePolling(quint32 id) {
	emit info(whoIAm, QString("Start interface#") + QString::number(id) + " polling.");
	// Find interface
	if (m_interfaces.find(id) != m_interfaces.end()) {
		// Make sure the thread is destroyed
		if (m_interfacesThreads.find(id) == m_interfacesThreads.end()) {
			// Create thread
			m_interfacesThreads[id] = std::unique_ptr<QThread>(new QThread());
			// Move interface to thread and start
			m_interfaces[id]->moveToThread(m_interfacesThreads[id].get());
			connect(m_interfacesThreads[id].get(), &QThread::started, m_interfaces[id].get(), &WSPollingInterface::start);
			// Thread start
			m_interfacesThreads[id]->start();
			emit info(whoIAm, QString("Interface#") + QString::number(id) + " thread started.");
			return true;
		}
	}
	return false;
}

bool WSQMLApplication::stopInterfacePolling(quint32 id) {
	emit info(whoIAm, QString("Stop interface#") + QString::number(id) + " polling.");
	// Find interface and his thread
	if (m_interfaces.find(id) != m_interfaces.end() && m_interfacesThreads.find(id) != m_interfacesThreads.end()) {
		m_interfaces[id]->stop();
		return true;
	}
	return false;
}

bool WSQMLApplication::isInterfaceStarted(quint32 id) {
	if (m_interfaces.find(id) != m_interfaces.end()) {
		return m_interfaces[id]->isStarted();
	}
	return false;
}

void WSQMLApplication::onParameterModbusValueChanged(quint32 interfaceId, quint32 paramId, WSModbusParameter *p) {
	auto dc = make_data_converter(
		reinterpret_cast<char*>(p->param()->value),
		p->param()->size,
		p->dataByteOrder(),
		p->dataType(),
		p->dataTypeSize(),
		p->dataSigned()
		);
	QStringList strListValue;
	if (dc.get() != nullptr) {
		strListValue = dc->toStringList(p->dataRepresent(), Conf::FLOAT_DATA_PRECISION, Conf::MULTI_DATA_DIVIDER);
	}
	emit valueChanged(interfaceId, paramId, strListValue[0], strListValue[1], p->responsesCount());
}

void WSQMLApplication::onParameterModbusError(quint32 interfaceId, quint32 paramId, WSModbusParameter *p) {
	uint8_t err = p->param()->err;
	QString errStr = "";
	if (err == MB_EC_RESPONSE_ERR) {
		errStr = "0xFF - Corrupt package";
	} else if (err == MB_EC_ILLEGAL_FUNCTION) {
		errStr = "0x01 - Illegal function";
	} else if (err == MB_EC_ILLEGAL_DATA_ADDRESS) {
		errStr = "0x02 - Illegal data address";
	} else if (err == MB_EC_ILLEGAL_DATA_VALUE) {
		errStr = "0x03 - Illegal data value";
	} else if (err == MB_EC_SLAVE_DEVICE_FAILURE) {
		errStr = "0x04 - Slave device failure";
	}
	emit valueError(interfaceId, paramId, errStr, p->errorsCount());
}

void WSQMLApplication::onParameterTimeout(quint32 interfaceId, quint32 paramId, WSModbusParameter* p) {
	emit valueTimeout(interfaceId, paramId, p->timeoutsCount());
}

void WSQMLApplication::onParameterRequest(quint32 interfaceId, quint32 paramId, WSModbusParameter* p) {
	emit valueRequest(interfaceId, paramId, p->requestsCount());
}

void WSQMLApplication::onInterfacePollingStarted(quint32 interfaceId) {
	emit info(whoIAm, QString("Interface#") + QString::number(interfaceId) + " polling started.");
	emit interfacePollingStarted(interfaceId);
}

void WSQMLApplication::onInterfacePollingConnected(quint32 interfaceId) {
	emit info(whoIAm, QString("Interface#") + QString::number(interfaceId) + " connected to host.");
	emit interfaceConnected(interfaceId);
}

void WSQMLApplication::onInterfacePollingDisconnected(quint32 interfaceId) {
	emit info(whoIAm, QString("Interface#") + QString::number(interfaceId) + " disconnected from host.");
}

void WSQMLApplication::onInterfacePollingStopped(quint32 interfaceId) {
	m_interfacesThreads[interfaceId]->quit();
	m_interfacesThreads[interfaceId]->wait(5000);
	// If something strange
	if (!m_interfacesThreads[interfaceId]->isFinished()) {
		m_interfacesThreads[interfaceId]->terminate();
		m_interfacesThreads[interfaceId]->wait();
	}
	// Remove thread from collection and destroy
	m_interfacesThreads.erase(interfaceId);
	emit interfacePollingStopped(interfaceId);
	emit info(whoIAm, QString("Interface#") + QString::number(interfaceId) + " thread stopped and destroyed.");
}

void WSQMLApplication::onInterfaceConnectionError(quint32 interfaceId) {
	emit info(whoIAm, QString("Interface#") + QString::number(interfaceId) + " connection error.");
	emit interfaceConnectionError(interfaceId);
}

void WSQMLApplication::onInterfaceReconnecting(quint32 interfaceId) {
	emit info(whoIAm, QString("Interface#") + QString::number(interfaceId) + " trying to reconnect.");
	emit interfaceReconnecting(interfaceId);
}

QString WSQMLApplication::getFilePath(const QUrl &url) {
	QString path;
	path = url.path();
// Qt Windows path bug
#ifdef Q_OS_WIN
	if (path.startsWith("/")) {
		path = path.mid(1);
	}
#endif
	return path;
}

WSSettings* WSQMLApplication::createSettingsInstance(const QUrl &url) {
	if (m_storeSettings != nullptr) {
		delete m_storeSettings;
	}
	if (url.isEmpty()) {
		m_storeSettings = new WSSettings(Conf::storeSettingsPath(), this);
	} else {
		m_storeSettings = new WSSettings(getFilePath(url), this);
	}
	return m_storeSettings;
}

WSFile* WSQMLApplication::createFile(const QUrl &url) {
	return new WSFile(getFilePath(url), this);
}

void WSQMLApplication::destroyFile(WSFile *file) {
	if (file != nullptr) {
		delete file;
	}
}

void WSQMLApplication::onTransmittedData(quint32 interfaceId, QByteArray transmittedData) {
	if (logInterfaceData()) {
		emit info(whoIAm, QString("Data from interface#") + QString::number(interfaceId) + QString(" >> ") + WSByteArrayConverter::toString(transmittedData, WSDataRepresent::HEX));
	}
}

void WSQMLApplication::onReceivedData(quint32 interfaceId, QByteArray receivedData) {
	if (logInterfaceData()) {
		emit info(whoIAm, QString("Data to interface#") + QString::number(interfaceId) + QString(" << ") + WSByteArrayConverter::toString(receivedData, WSDataRepresent::HEX));
	}
}

void WSQMLApplication::onTransmitTimeoutOccurred(quint32 interfaceId, quint32 timeout) {
	if (logInterfaceData()) {
		emit info(whoIAm, QString("Data from interface#") + QString::number(interfaceId) + QString(" >> Transmit timeout ") + QString::number(timeout) + QString(" ms."));
	}
}

void WSQMLApplication::onReceiveTimeoutOccurred(quint32 interfaceId, quint32 timeout) {
	if (logInterfaceData()) {
		emit info(whoIAm, QString("Data to interface#") + QString::number(interfaceId) + QString(" << Receive timeout ") + QString::number(timeout) + QString(" ms."));
	}
}

void WSQMLApplication::onValidDataReceived(quint32 interfaceId) {
	if (logInterfaceData()) {
		emit info(whoIAm, QString("Data to interface#") + QString::number(interfaceId) + QString(" << Package is valid."));
	}
}

/*void WSQMLApplication::onIncompleteDataReceived(quint32 interfaceId) {
	if (logInterfaceData()) {
		emit info(whoIAm, QString("Data to interface#") + QString::number(interfaceId) + QString(" << Package is incomplete."));
	}
}*/

void WSQMLApplication::onErrorDataReceived(quint32 interfaceId) {
	if (logInterfaceData()) {
		emit info(whoIAm, QString("Data to interface#") + QString::number(interfaceId) + QString(" << Error package."));
	}
}

void WSQMLApplication::setLogInterfaceData(bool enabled) {
	m_logInterfaceData = enabled;
}

bool WSQMLApplication::logInterfaceData() {
	return m_logInterfaceData;
}

quint16 WSQMLApplication::paramBytesSize(QJSValue data) {
	quint16 bytesSize = 0;
	quint8 fcode = static_cast<quint8>(data.property("fcode").toUInt());
	quint16 count = static_cast<quint16>(data.property("count").toUInt());
	if (fcode == 0x01 || fcode == 0x02 || fcode == 0x05 || fcode == 0x0F) {
		bytesSize = count / 8 + ((count % 8 == 0) ? 0 : 1);
	} else if (fcode == 0x03 || fcode == 0x04 || fcode == 0x06 || fcode == 0x10) {
		bytesSize = count * 2;
	}
	return bytesSize;
}

QVariant WSQMLApplication::parseStringValue(QJSValue data) {
	QByteArray byteValue;
	quint16 bytesSize = paramBytesSize(data);
	WSConversionState convRes = WSStringConverter::toArray(
		byteValue,
		data.property("val").toString(),
		static_cast<WSDataRepresent>(stringToDataRepresent(data.property("view").property("represent").toString())),
		static_cast<WSByteOrder>(stringToDataByteOrder(data.property("view").property("order").toString())),
		static_cast<WSDataType>(stringToDataType(data.property("view").property("type").toString())),
		static_cast<quint8>(data.property("view").property("bytes").toUInt()),
		bytesSize,
		data.property("view").property("sign").toBool());
	if (convRes == WSConversionState::OK) {
		return QVariant(byteValue);
	} else {
		return QVariant(static_cast<quint8>(convRes));
	}
}

QString WSQMLApplication::arrayToString(QJSValue data) {
	QByteArray arr = data.property("setted_value").toVariant().toByteArray();
	auto dc = make_data_converter(
		arr.data(),
		arr.size(),
		static_cast<WSByteOrder>(stringToDataByteOrder(data.property("view").property("order").toString())),
		static_cast<WSDataType>(stringToDataType(data.property("view").property("type").toString())),
		static_cast<quint8>(data.property("view").property("bytes").toUInt()),
		data.property("view").property("sign").toBool()
		);
	QStringList strListValue;
	if (dc.get() != nullptr) {
		strListValue = dc->toStringList(
			static_cast<WSDataRepresent>(stringToDataRepresent(data.property("view").property("represent").toString())),
			Conf::FLOAT_DATA_PRECISION,
			Conf::MULTI_DATA_DIVIDER);
	}
	return strListValue[0];
}

QString WSQMLApplication::name() {
	return "Weprex";
}

QString WSQMLApplication::fullName() {
	return "Webstella protocols exchange";
}

QString WSQMLApplication::version() {
	return "0.1.1";
}

QString WSQMLApplication::license() {
	return "GNU General Public License v3";
}

QString WSQMLApplication::build() {
	return QString("%1 %2").arg(__DATE__).arg(__TIME__);
}

QString WSQMLApplication::copyrights() {
	return "© 2018 Oleg Malyavkin";
}

QString WSQMLApplication::additionalInfo() {
	return "Logo & icons design by Yuri Ponomarev";
}

QString WSQMLApplication::contacts() {
	return "weprexsoft@gmail.com<br/>https://github.com/oniksan/weprex";
}

bool WSQMLApplication::showManual() {
	QUrl url(Conf::MANUAL_FILE_PATH);
	return QDesktopServices::openUrl(url);
}
