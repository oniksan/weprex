#include "wsserialinterface.h"

WSSerialInterface::WSSerialInterface(const QString &portName, WSAbstractRRProtocol *protocol, QThread *mainThread, QObject *parent) :
	WSPollingRRInterface(protocol, mainThread, parent),
	m_serial(portName, this)
{
	m_serial.setFlowControl(QSerialPort::NoFlowControl);
	m_serial.setBaudRate(QSerialPort::Baud9600);
	m_serial.setDataBits(QSerialPort::Data8);
	m_serial.setParity(QSerialPort::NoParity);
	m_serial.setStopBits(QSerialPort::OneStop);
}

WSInterface WSSerialInterface::type() const {
	return WSInterface::SERIAL;
}

QString WSSerialInterface::portName() const {
	QMutexLocker ml(&m_lock);
	return m_serial.portName();
}

void WSSerialInterface::setPortName(const QString &portName) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_serial.setPortName(portName);
	}
}

qint32 WSSerialInterface::baudRate() const {
	QMutexLocker ml(&m_lock);
	return m_serial.baudRate();
}

void WSSerialInterface::setBaudRate(qint32 baudRate) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_serial.setBaudRate(baudRate);
	}
}

QSerialPort::DataBits WSSerialInterface::dataBits() const {
	QMutexLocker ml(&m_lock);
	return m_serial.dataBits();
}

void WSSerialInterface::setDataBits(const QSerialPort::DataBits &dataBits) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_serial.setDataBits(dataBits);
	}
}

QSerialPort::Parity WSSerialInterface::parity() const {
	QMutexLocker ml(&m_lock);
	return m_serial.parity();
}

void WSSerialInterface::setParity(const QSerialPort::Parity &parity) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_serial.setParity(parity);
	}
}

QSerialPort::StopBits WSSerialInterface::stopBits() const {
	QMutexLocker ml(&m_lock);
	return m_serial.stopBits();
}

void WSSerialInterface::setStopBits(const QSerialPort::StopBits &stopBits) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_serial.setStopBits(stopBits);
	}
}

QSerialPort::DataBits WSSerialInterface::dataBitsFromNumber(int dataBits) {
	if (dataBits == 5) {
		return QSerialPort::Data5;
	} else if (dataBits == 6) {
		return QSerialPort::Data6;
	} else if (dataBits == 7) {
		return QSerialPort::Data7;
	} else if (dataBits == 8) {
		return QSerialPort::Data8;
	} else {
		return QSerialPort::UnknownDataBits;
	}
}

int WSSerialInterface::dataBitsToNumber(const QSerialPort::DataBits &dataBits) {
	if (dataBits == QSerialPort::Data5) {
		return 5;
	} else if (dataBits == QSerialPort::Data6) {
		return 6;
	} else if (dataBits == QSerialPort::Data7) {
		return 7;
	} else if (dataBits == QSerialPort::Data8) {
		return 8;
	}
	return -1;
}

QSerialPort::Parity WSSerialInterface::parityFromString(const QString &parity) {
	if (parity == "none") {
		return QSerialPort::NoParity;
	} else if (parity == "odd") {
		return QSerialPort::OddParity;
	} else if (parity == "even") {
		return QSerialPort::EvenParity;
	} else if (parity == "space") {
		return QSerialPort::SpaceParity;
	} else if (parity == "mark") {
		return QSerialPort::MarkParity;
	}
	return QSerialPort::UnknownParity;
}

QString WSSerialInterface::parityToString(const QSerialPort::Parity &parity) {
	if (parity == QSerialPort::NoParity) {
		return QString("none");
	} else if (parity == QSerialPort::OddParity) {
		return QString("odd");
	} else if (parity == QSerialPort::EvenParity) {
		return QString("even");
	} else if (parity == QSerialPort::SpaceParity) {
		return QString("space");
	} else if (parity == QSerialPort::MarkParity) {
		return QString("mark");
	}
	return QString("");
}

QSerialPort::StopBits WSSerialInterface::stopBitsFromString(const QString &stopBits) {
	if (stopBits == "1") {
		return QSerialPort::OneStop;
	} else if (stopBits == "1.5") {
		return QSerialPort::OneAndHalfStop;
	} else if (stopBits == "2") {
		return QSerialPort::TwoStop;
	}
	return QSerialPort::UnknownStopBits;
}

QString WSSerialInterface::stopBitsToString(const QSerialPort::StopBits &stopBits) {
	if (stopBits == QSerialPort::OneStop) {
		return QString("1");
	} else if (stopBits == QSerialPort::OneAndHalfStop) {
		return QString("1.5");
	} else if (stopBits == QSerialPort::TwoStop) {
		return QString("2");
	}
	return QString("");
}

void WSSerialInterface::workerLoop() {
	if (m_serial.open(QIODevice::ReadWrite)) {
		emit connected();
		m_state = WSSerialState::TRANSMIT;
		while (true) {
			QCoreApplication::processEvents();
			if (getStopFlag()) {
				m_serial.close();
				break;
			}
			if (m_state == WSSerialState::TRANSMIT) {
				QByteArray data = m_protocol->constructRequest();
				if (data.size() > 0) {
					m_serial.write(data);
					if (m_serial.waitForBytesWritten(static_cast<int>(m_transmitTimeout))) {
						emit trasmitted(data);
						m_recvBuffer.clear();
						m_state = WSSerialState::RECEIVE;
					} else {
						emit transmitTimeoutOccurred(m_transmitTimeout);
						QThread::msleep(m_skipPause);
						continue;
					}
				} else {
					emit dataSkipped();
					QThread::msleep(m_skipPause);
					continue;
				}
			} else if (m_state == WSSerialState::RECEIVE) {
				if (m_serial.bytesAvailable() > 0 || m_serial.waitForReadyRead(static_cast<int>(m_receiveTimeout))) {
					QByteArray readData = m_serial.readAll();
					m_recvBuffer.append(readData);
					emit received(readData);
					WSProtocolParseCode code = m_protocol->processResponse(m_recvBuffer);
					if (code == WSProtocolParseCode::INCOMPLETE) {
						emit incompleteDataReceived(m_recvBuffer);
						QThread::msleep(m_skipPause);
						continue;
					} else if (code == WSProtocolParseCode::VALID || code == WSProtocolParseCode::VALID_BROADCAST) {
						emit validDataReceived(m_recvBuffer);
						m_state = WSSerialState::TRANSMIT;
						QThread::msleep(m_pollingPause);
						continue;
					} else {
						emit errorDataReceived(m_recvBuffer);
						QThread::msleep(m_pollingPause);
						QThread::msleep(m_errorPause);
						m_recvBuffer.clear();
						m_state = WSSerialState::TRANSMIT;
						continue;
					}
				} else {
					emit receiveTimeoutOccurred(m_receiveTimeout);
					QThread::msleep(m_skipPause);
					m_recvBuffer.clear();
					m_state = WSSerialState::TRANSMIT;
					continue;
				}
			}
		}
		emit disconnected();
	} else {
		emit connectionErrorOccurred();
	}
}
