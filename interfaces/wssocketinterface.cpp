#include "wssocketinterface.h"

WSSocketInterface::WSSocketInterface(QHostAddress ipAddress, quint16 port, WSAbstractRRProtocol *protocol, QThread *mainThread, QObject *parent) :
	WSPollingRRInterface(protocol, mainThread, parent),
	m_socket(this),
	m_ipAddress(ipAddress),
	m_port(port),
	m_autoReconnect(Conf::DEVICE_TCP_AUTO_RECONNECT),
	m_state(WSSocketState::NONE)
{
	m_socket.setProxy(QNetworkProxy::NoProxy);
}

WSInterface WSSocketInterface::type() const {
	return WSInterface::TCP;
}

bool WSSocketInterface::autoReconnect() const {
	QMutexLocker ml(&m_lock);
	return m_autoReconnect;
}

void WSSocketInterface::setAutoReconnect(bool autoReconnect) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_autoReconnect = autoReconnect;
	}
}

QHostAddress WSSocketInterface::ipAddress() const {
	QMutexLocker ml(&m_lock);
	return m_ipAddress;
}

void WSSocketInterface::setIpAddress(const QHostAddress &ipAddress) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_ipAddress = ipAddress;
	}
}

quint16 WSSocketInterface::port() const {
	QMutexLocker ml(&m_lock);
	return m_port;
}

void WSSocketInterface::setPort(quint16 port) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_port = port;
	}
}

void WSSocketInterface::workerLoop() {
	bool tryDisconnect = false;
	while (true) {
		QCoreApplication::processEvents();
		m_socket.connectToHost(m_ipAddress, m_port);
		if (m_socket.waitForConnected(static_cast<int>(m_connectionWaitTime))) {
			emit connected();
			m_state = WSSocketState::TRANSMIT;
			while (true) {
				QCoreApplication::processEvents();
				if (getStopFlag()) {
					tryDisconnect = true;
					break;
				}
				if (m_state == WSSocketState::TRANSMIT) {
					QByteArray data = m_protocol->constructRequest();
					if (data.size() > 0) {
						m_socket.write(data);
						if (m_socket.waitForBytesWritten(static_cast<int>(m_transmitTimeout))) {
							emit trasmitted(data);
							m_recvBuffer.clear();
							m_state = WSSocketState::RECEIVE;
						} else {
							emit transmitTimeoutOccurred(m_transmitTimeout);
							tryDisconnect = true;
							break;
						}
					} else {
						emit dataSkipped();
						QThread::msleep(m_skipPause);
						continue;
					}
				} else if (m_state == WSSocketState::RECEIVE) {
					if (m_socket.bytesAvailable() > 0 || m_socket.waitForReadyRead(static_cast<int>(m_receiveTimeout))) {
						QByteArray readData = m_socket.readAll();
						m_recvBuffer.append(readData);
						emit received(readData);
						WSProtocolParseCode code = m_protocol->processResponse(m_recvBuffer);
						if (code == WSProtocolParseCode::INCOMPLETE) {
							emit incompleteDataReceived(m_recvBuffer);
							QThread::msleep(m_skipPause);
							continue;
						} else if (code == WSProtocolParseCode::VALID || code == WSProtocolParseCode::VALID_BROADCAST) {
							emit validDataReceived(m_recvBuffer);
							m_state = WSSocketState::TRANSMIT;
							QThread::msleep(m_pollingPause);
							continue;
						} else {
							emit errorDataReceived(m_recvBuffer);
							tryDisconnect = true;
							break;
						}
					} else {
						emit receiveTimeoutOccurred(m_receiveTimeout);
						tryDisconnect = true;
						break;
					}
				}
			}
		} else {
			emit connectionErrorOccurred();
		}
		if (tryDisconnect) {
			tryDisconnect = false;
			if (m_socket.state() != QAbstractSocket::UnconnectedState && m_socket.state() != QAbstractSocket::ClosingState) {
				m_socket.disconnectFromHost();
				if (m_socket.state() == QAbstractSocket::UnconnectedState || m_socket.waitForDisconnected(static_cast<int>(m_disconnectionWaitTime))) {
					emit disconnected();
				} else {
					emit disconnectionErrorOccurred();
					break;
				}
			}
		}
		if (!getStopFlag() && m_autoReconnect) {
			m_state = WSSocketState::NONE;
			QThread::msleep(m_reconnectPause);
			emit reconnecting();
		} else {
			break;
		}
	}
}
