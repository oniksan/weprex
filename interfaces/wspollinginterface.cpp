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

#include "wspollinginterface.h"

WSPollingInterface::WSPollingInterface(QThread *mainThread, QObject *parent) :
	QObject(parent),
	m_reconnectPause(Conf::DEVICE_RECONNECTION_PAUSE),
	m_connectionWaitTime(Conf::DEVICE_CONNECTION_WAIT_TIME),
	m_disconnectionWaitTime(Conf::DEVICE_DISCONNECTION_WAIT_TIME),
	m_stop(false),
	m_started(false),
	m_mainThread(mainThread)
{}

void WSPollingInterface::start() {
	setStopFlag(false);
	setStarted(true);
	emit started();
	workerLoop();
	if (m_mainThread != nullptr) {
		moveToThread(m_mainThread);
	}
	setStarted(false);
	emit stopped();
}

void WSPollingInterface::stop() {
	setStopFlag(true);
}

quint32 WSPollingInterface::reconnectPause() const {
	QMutexLocker ml(&m_lock);
	return m_reconnectPause;
}

void WSPollingInterface::setReconnectPause(quint32 reconnectPause) {
	QMutexLocker ml(&m_lock);
	if (!m_started) {
		m_reconnectPause = reconnectPause;
	}
}

void WSPollingInterface::setStopFlag(bool stop) {
	QMutexLocker ml(&m_stopLock);
	m_stop = stop;
}

bool WSPollingInterface::getStopFlag() {
	QMutexLocker ml(&m_stopLock);
	return m_stop;
}

void WSPollingInterface::setStarted(bool busy) {
	QMutexLocker ml(&m_lock);
	m_started = busy;
}

bool WSPollingInterface::isStarted() {
	QMutexLocker ml(&m_lock);
	return m_started;
}
