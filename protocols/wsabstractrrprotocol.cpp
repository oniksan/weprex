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

#include "wsabstractrrprotocol.h"

WSAbstractRRProtocol::WSAbstractRRProtocol(quint32 bufferSize, QObject *parent) :
	QObject(parent),
	m_bufferSize(bufferSize),
	m_buffer(new char[bufferSize])
{}

const QByteArray& WSAbstractRRProtocol::constructRequest() {
	qint32 size = construct(m_buffer.get());
	if (size > 0) {
		m_array = QByteArray::fromRawData(m_buffer.get(), size);
	} else {
		m_array = QByteArray();
	}
	return m_array;
}

quint32 WSAbstractRRProtocol::bufferSize() const {
	return m_bufferSize;
}

