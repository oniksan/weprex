#include "wspollingrrinterface.h"

WSPollingRRInterface::WSPollingRRInterface(WSAbstractRRProtocol *protocol, QThread *mainThread, QObject *parent) :
	WSPollingInterface(mainThread, parent),
	m_protocol(protocol),
	m_transmitTimeout(1000),
	m_receiveTimeout(1000),
	m_pollingPause(50),
	m_skipPause(Conf::DEVICE_THREAD_SLEEP_PAUSE),
	m_errorPause(Conf::DEVICE_ERROR_SLEEP_PAUSE),
	m_releaseFlag(false)
{
	connectSlotSignals();
}

WSPollingRRInterface::~WSPollingRRInterface() {
	if (!m_releaseFlag) {
		disconnect(this, &WSPollingRRInterface::transmitTimeoutOccurred, m_protocol.get(), &WSAbstractRRProtocol::timeoutOccurred);
		disconnect(this, &WSPollingRRInterface::receiveTimeoutOccurred, m_protocol.get(), &WSAbstractRRProtocol::timeoutOccurred);
	}
}

void WSPollingRRInterface::connectSlotSignals() {
	connect(this, &WSPollingRRInterface::transmitTimeoutOccurred, m_protocol.get(), &WSAbstractRRProtocol::timeoutOccurred);
	connect(this, &WSPollingRRInterface::receiveTimeoutOccurred, m_protocol.get(), &WSAbstractRRProtocol::timeoutOccurred);
	m_releaseFlag = false;
}

void WSPollingRRInterface::disconnectSlotSignals() {
	if (!m_releaseFlag) {
		disconnect(this, &WSPollingRRInterface::transmitTimeoutOccurred, m_protocol.get(), &WSAbstractRRProtocol::timeoutOccurred);
		disconnect(this, &WSPollingRRInterface::receiveTimeoutOccurred, m_protocol.get(), &WSAbstractRRProtocol::timeoutOccurred);
		m_releaseFlag = true;
	}
}

WSAbstractRRProtocol* WSPollingRRInterface::protocolGet() {
	return m_protocol.get();
}

WSAbstractRRProtocol* WSPollingRRInterface::protocolRelease() {
	disconnectSlotSignals();
	return m_protocol.release();
}

void WSPollingRRInterface::protocolReset(WSAbstractRRProtocol* proto) {
	disconnectSlotSignals();
	m_protocol.reset(proto);
	connectSlotSignals();
}

quint32 WSPollingRRInterface::pollingPause() const {
	QMutexLocker ml(&m_lock);
	return m_pollingPause;
}

void WSPollingRRInterface::setPollingPause(quint32 pollingPause) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_pollingPause = pollingPause;
	}
}

quint32 WSPollingRRInterface::skipPause() const {
	QMutexLocker ml(&m_lock);
	return m_skipPause;
}

void WSPollingRRInterface::setSkipPause(quint32 skipPause) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_skipPause = skipPause;
	}
}

quint32 WSPollingRRInterface::errorPause() const {
	QMutexLocker ml(&m_lock);
	return m_errorPause;
}

void WSPollingRRInterface::setErrorPause(quint32 errorPause) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_errorPause = errorPause;
	}
}

quint32 WSPollingRRInterface::receiveTimeout() const {
	QMutexLocker ml(&m_lock);
	return m_receiveTimeout;
}

void WSPollingRRInterface::setReceiveTimeout(quint32 receiveTimeout) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_receiveTimeout = receiveTimeout;
	}
}

quint32 WSPollingRRInterface::transmitTimeout() const {
	QMutexLocker ml(&m_lock);
	return m_transmitTimeout;
}

void WSPollingRRInterface::setTransmitTimeout(quint32 transmitTimeout) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_transmitTimeout = transmitTimeout;
	}
}
