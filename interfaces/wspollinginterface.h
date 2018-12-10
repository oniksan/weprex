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

#ifndef WSPOLLINGINTERFACE_H
#define WSPOLLINGINTERFACE_H

#include <QObject>
#include <QtCore>
#include "conf.h"

enum class WSInterface {
	TCP = 0,
	SERIAL = 1,
	UDP = 2
};

class WSPollingInterface : public QObject {
Q_OBJECT
	
public:
	explicit WSPollingInterface(QThread *mainThread, QObject *parent = nullptr);
	virtual ~WSPollingInterface() {}
	virtual void start();
	virtual void stop();
	virtual quint32 reconnectPause() const;
	virtual void setReconnectPause(quint32 reconnectPause);
	virtual bool isStarted();

	virtual WSInterface type() const = 0;
	virtual void workerLoop() = 0;

signals:
	void started();
	void stopped();
	void connectionErrorOccurred();
	void disconnectionErrorOccurred();
	void connected();
	void disconnected();
	void reconnecting();
	
protected:
	quint32 m_reconnectPause;
	quint32 m_connectionWaitTime;
	quint32 m_disconnectionWaitTime;
	bool m_stop;
	bool m_started;
	QThread *m_mainThread;
	mutable QMutex m_stopLock;
	mutable QMutex m_lock;
	
	void setStopFlag(bool stop);
	bool getStopFlag();
	void setStarted(bool busy);
	
};

#endif // WSPOLLINGINTERFACE_H
