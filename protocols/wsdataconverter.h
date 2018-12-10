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

#ifndef WSDATACONVERTER_H
#define WSDATACONVERTER_H

#include <QtCore>
#include <memory>
#include <limits>
#include "conf.h"

enum class WSConversionState : quint8 {
	OK = 0,
	ERR_COUNT = 1,
	ERR_LIMITS = 2,
	ERR_CONVERT = 3,
	ERR_REPRESENT = 4,
	ERR_TYPE_SIZE = 5,
	ERR_TYPE = 6
};

enum class WSByteOrder : quint8 {
	FORWARD = 0,
	BACKWARD = 1,
	FORWARD_WORDS_REVERSE = 2,
	BACKWARD_WORDS_REVERSE = 3
};

enum class WSDataRepresent : quint8 {
	BIN = 0,
	OCT = 1,
	DEC = 2,
	HEX = 3,
	ASCII = 4
};

enum class WSDataType : quint8 {
	INTEGER = 0,
	FLOAT = 1,
	TEXT = 2
};

class WSDataConverterInterface {

public:
	virtual ~WSDataConverterInterface();
	virtual qint32 count() = 0;
	virtual qint32 size() = 0;
	virtual void reconvert() = 0;
	virtual QStringList toStringList(WSDataRepresent represent, quint8 precision, const char *divider) = 0;
};

template <typename T>
class WSDataConverter : public WSDataConverterInterface {

public:
	WSDataConverter(char *bytes, qint32 size, WSByteOrder order) :
		m_bytes(bytes),
		m_size(size),
		m_order(order),
		m_need_delete(false)
	{
		m_converted_bytes = new char[m_size];
		convert();
		calcCount();
	}

	WSDataConverter(QVector<T> values, WSByteOrder order) :
		m_size(sizeof(T) * values.size()),
		m_order(order),
		m_count(values.size()),
		m_need_delete(true)
	{
		m_bytes = new char[m_size];
		m_converted_bytes = new char[m_size];
		for (qint32 i = 0; i < m_count; i++) {
			memcpy(m_bytes + (i * sizeof(T)), &values[i], sizeof(T));
		}
		convert();
	}

	virtual ~WSDataConverter() {
		delete m_converted_bytes;
		if (m_need_delete) {
			delete m_bytes;
		}
	}
	
	WSDataConverter(const WSDataConverter &obj) = delete;
	WSDataConverter& operator=(WSDataConverter &obj) = delete;
	WSDataConverter(const WSDataConverter &&obj) = delete;
	WSDataConverter& operator=(WSDataConverter &&obj) = delete;

	const char* bytes() {
		return m_converted_bytes;
	}

	qint32 count() {
		return m_count;
	}

	qint32 size() {
		return m_size;
	}

	void reconvert() {
		convert();
	}

	T value() {
		//return *((T*) m_converted_bytes);
		return *(reinterpret_cast<T*>(m_converted_bytes));
	}

	QVector<T> values() {
		T val;
		QVector<T> result;
		for (qint32 i = 0; i < m_count; i++) {
			val = (reinterpret_cast<T*>(m_converted_bytes))[i];
			result.append(val);
		}
		return result;
	}

	QStringList toStringList(WSDataRepresent represent, quint8 precision, const char *divider = " ") {
		QStringList result;
		if (m_count < 2) {
			QString res = valToString(value(), represent, precision);
			result.append(res);
			result.append(res);
		} else {
			QVector<T> vals = values();
			QVectorIterator<T> i(vals);
			QString result1, result2, res;
			while (i.hasNext()) {
				res = valToString(i.next(), represent, precision);
				result1 = result1 % res;
				result2 = result2 % res;
				if (i.hasNext()) {
					result1 = result1 % divider;
				}
			}
			if (represent == WSDataRepresent::ASCII) {
				result.append(result2);
			} else {
				result.append(result1);
			}
			if (represent == WSDataRepresent::BIN || represent == WSDataRepresent::ASCII) {
				result.append(result2);
			} else {
				result.append(result1);
			}
		}
		return result;
	}

private:
	char *m_bytes;
	qint32 m_size;
	WSByteOrder m_order;
	char *m_converted_bytes;
	qint32 m_count;
	bool m_need_delete;

	void convert() {
		qint32 i;
		if (m_size == 1) {
			m_converted_bytes[0] = m_bytes[0];
		} else {
			if (m_order == WSByteOrder::FORWARD) {
				memcpy(m_converted_bytes, m_bytes, m_size);
			} else if (m_order == WSByteOrder::BACKWARD) {
				for (i = 0; i < m_size; i++) {
					m_converted_bytes[i] = m_bytes[m_size - i - 1];
				}
			} else if (m_order == WSByteOrder::FORWARD_WORDS_REVERSE) {
				for (i = 0; i < m_size; i++) {
					m_converted_bytes[i] = ((i % 2 == 0)?m_bytes[i + 1]:m_bytes[i - 1]);
				}
			} else if (m_order == WSByteOrder::BACKWARD_WORDS_REVERSE) {
				for (i = 0; i < m_size; i++) {
					m_converted_bytes[i] = ((i % 2 == 0)?m_bytes[m_size - i - 2]:m_bytes[m_size - i]);
				}
			}
		}
	}

	void calcCount() {
		if (m_size == 0 || m_size % sizeof(T) != 0) {
			m_count = 0;
		} else {
			m_count = m_size / sizeof(T);
		}
	}

	QString intToString(T val, WSDataRepresent represent) {
		QString res;
		if (represent == WSDataRepresent::BIN) {
			res = QString::number(val, 2);
			int n = res.length();
			switch (n) {
				case 0:
					res = "00000000";
					break;
				case 1:
					res = "0000000" + res;
					break;
				case 2:
					res = "000000" + res;
					break;
				case 3:
					res = "00000" + res;
					break;
				case 4:
					res = "0000" + res;
					break;
				case 5:
					res = "000" + res;
					break;
				case 6:
					res = "00" + res;
					break;
				case 7:
					res = "0" + res;
					break;
			}
			return res;
		} else if (represent == WSDataRepresent::OCT) {
			return QString::number(val, 8);
		} else if (represent == WSDataRepresent::DEC) {
			return QString::number(val, 10);
		} else if (represent == WSDataRepresent::HEX) {
			res = QString::number(val, 16).toUpper();
			if (res.length() == 1) {
				res = "0" + res;
			}
			return res;
		} else if (represent == WSDataRepresent::ASCII) {
			QChar c = static_cast<char>(val);
			return QString(c);
		}
		return QString();
	}

	QString floatToString(T val, WSDataRepresent represent, quint8 precision) {
		if (represent == WSDataRepresent::DEC) {
			return QString::number(val, 'g', precision);
		}
		return QString();
	}

	QString valToString(T val, WSDataRepresent represent, quint8);
};

std::unique_ptr<WSDataConverterInterface> make_data_converter(char *dataBytes, qint32 dataSize, WSByteOrder order, WSDataType type, quint8 typeSize, bool sign);

class WSByteArrayConverter {
public:
	static QString toString(QByteArray arr, WSDataRepresent represent);
};

class WSStringConverter {
public:
	static WSConversionState toArray(QByteArray &result, const QString &data, WSDataRepresent represent, WSByteOrder order, WSDataType type, quint8 typeSize, quint16 bytesSize, bool sign);
};

#endif // WSDATACONVERTER_H
