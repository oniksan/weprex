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

#ifndef WSQMLAPPLICATION_H
#define WSQMLAPPLICATION_H

#include <memory>
#include <map>
#include <QObject>
#include <QtCore>
#include <QtSerialPort>
#include <QJSValue>
#include <QJSValueIterator>
#include <QDesktopServices>
#include "interfaces/wssocketinterface.h"
#include "interfaces/wsserialinterface.h"
#include "protocols/wsabstractrrprotocol.h"
#include "protocols/wsmodbustcpprotocol.h"
#include "protocols/wsmodbusrtuprotocol.h"
#include "protocols/wsmodbusparameter.h"
#include "protocols/modbus.h"
#include "protocols/wsdataconverter.h"
#include "utils/wssettings.h"
#include "utils/wsfile.h"
#include "conf.h"

class WSQMLApplication : public QObject {
Q_OBJECT

public:
	explicit WSQMLApplication(QObject *parent = nullptr);
	virtual ~WSQMLApplication();
	
	Q_PROPERTY(QStringList availablePortNames READ getAvailablePortNames NOTIFY availablePortNamesChanged)

	QStringList getAvailablePortNames();
	Q_INVOKABLE void refreshAvailablePorts();

	Q_INVOKABLE bool startInterfacePolling(quint32 id);
	Q_INVOKABLE bool stopInterfacePolling(quint32 id);
	Q_INVOKABLE bool isInterfaceStarted(quint32 id);
	
	Q_INVOKABLE quint32 addInterface(QJSValue data);
	Q_INVOKABLE bool editInterface(quint32 id, QJSValue data);
	Q_INVOKABLE bool removeInterface(quint32 id);
	
	Q_INVOKABLE quint32 addParameter(quint32 interfaceId, QJSValue data);
	Q_INVOKABLE bool editParameter(quint32 interfaceId, quint32 id, QJSValue data);
	Q_INVOKABLE bool removeParameter(quint32 interfaceId, quint32 id);

	Q_INVOKABLE static QString dataTypeToString(quint8 type);
	Q_INVOKABLE static quint8 stringToDataType(const QString &type);

	Q_INVOKABLE static QString dataByteOrderToString(quint8 order);
	Q_INVOKABLE static quint8 stringToDataByteOrder(const QString &order);

	Q_INVOKABLE static QString dataRepresentToString(quint8 represent);
	Q_INVOKABLE static quint8 stringToDataRepresent(const QString &represent);

	Q_INVOKABLE static QString pollingTypeToString(quint8 type);
	Q_INVOKABLE static quint8 stringToPollingType(const QString &type);

	Q_INVOKABLE WSSettings* createSettingsInstance(const QUrl &url);
	Q_INVOKABLE void setLogInterfaceData(bool enabled);
	Q_INVOKABLE bool logInterfaceData();
	Q_INVOKABLE quint16 paramBytesSize(QJSValue data);
	Q_INVOKABLE QVariant parseStringValue(QJSValue data);
	Q_INVOKABLE QString arrayToString(QJSValue data);
	Q_INVOKABLE WSFile* createFile(const QUrl &url);
	Q_INVOKABLE void destroyFile(WSFile *file);

	Q_INVOKABLE bool showManual();

	Q_INVOKABLE QString name();
	Q_INVOKABLE QString fullName();
	Q_INVOKABLE QString version();
	Q_INVOKABLE QString license();
	Q_INVOKABLE QString build();
	Q_INVOKABLE QString copyrights();
	Q_INVOKABLE QString additionalInfo();
	Q_INVOKABLE QString contacts();

	static const QString whoIAm;

private:
	QStringList m_availablePortNames;
	std::map<quint32, std::unique_ptr<WSPollingInterface> > m_interfaces;
	std::map<quint32, std::unique_ptr<QThread> > m_interfacesThreads;
	quint32 m_interfacesCounter;
	WSSettings *m_storeSettings;
	bool m_logInterfaceData;

	QString getFilePath(const QUrl &url);

	void onParameterModbusValueChanged(quint32 interfaceId, quint32 paramId, WSModbusParameter *param);
	void onParameterModbusError(quint32 interfaceId, quint32 paramId, WSModbusParameter *param);
	void onParameterTimeout(quint32 interfaceId, quint32 paramId, WSModbusParameter*p);
	void onParameterRequest(quint32 interfaceId, quint32 paramId, WSModbusParameter*p);
	void onInterfacePollingStarted(quint32 interfaceId);
	void onInterfacePollingConnected(quint32 interfaceId);
	void onInterfacePollingDisconnected(quint32 interfaceId);
	void onInterfacePollingStopped(quint32 interfaceId);
	void onInterfaceConnectionError(quint32 interfaceId);
	void onInterfaceReconnecting(quint32 interfaceId);
	void onTransmittedData(quint32 interfaceId, QByteArray transmittedData);
	void onReceivedData(quint32 interfaceId, QByteArray receivedData);
	void onTransmitTimeoutOccurred(quint32 interfaceId, quint32 timeout);
	void onReceiveTimeoutOccurred(quint32 interfaceId, quint32 timeout);
	void onValidDataReceived(quint32 interfaceId);
	//void onIncompleteDataReceived(quint32 interfaceId);
	void onErrorDataReceived(quint32 interfaceId);
	
signals:
	void availablePortNamesChanged();
	void valueChanged(quint32 interfaceId, quint32 paramId, QString value, QString valueRaw, quint32 responseCounter);
	void valueError(quint32 interfaceId, quint32 paramId, QString errCode, quint32 errorCounter);
	void valueTimeout(quint32 interfaceId, quint32 paramId, quint32 timeoutCounter);
	void valueRequest(quint32 interfaceId, quint32 paramId, quint32 requestCounter);
	//void interfaceStatusChanged(quint32 interfaceId);
	void interfacePollingStarted(quint32 interfaceId);
	void interfaceConnected(quint32 interfaceId);
	void interfacePollingStopped(quint32 interfaceId);
	void interfaceConnectionError(quint32 interfaceId);
	void interfaceReconnecting(quint32 interfaceId);
	void info(const QString &who, const QString &message);

public slots:

};

#endif // WSQMLAPPLICATION_H
