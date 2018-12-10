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

#include "wsdataconverter.h"

WSDataConverterInterface::~WSDataConverterInterface() {}

std::unique_ptr<WSDataConverterInterface> make_data_converter(char *dataBytes, qint32 dataSize, WSByteOrder order, WSDataType type, quint8 typeSize, bool sign) {
	if (type == WSDataType::INTEGER) {
		if (typeSize == 1) {
			if (sign) {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<int8_t>(dataBytes, dataSize, order));
			} else {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<uint8_t>(dataBytes, dataSize, order));
			}
		} else if (typeSize == 2) {
			if (sign) {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<int16_t>(dataBytes, dataSize, order));
			} else {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<uint16_t>(dataBytes, dataSize, order));
			}
		} else if (typeSize == 4) {
			if (sign) {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<int32_t>(dataBytes, dataSize, order));
			} else {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<uint32_t>(dataBytes, dataSize, order));
			}
		} else if (typeSize == 8) {
			if (sign) {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<int64_t>(dataBytes, dataSize, order));
			} else {
				return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<uint64_t>(dataBytes, dataSize, order));
			}
		}
	} else if (type == WSDataType::FLOAT) {
		if (typeSize == 4) {
			return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<float>(dataBytes, dataSize, order));
		} else if (typeSize == 8) {
			return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<double>(dataBytes, dataSize, order));
		}
	} else if (type == WSDataType::TEXT) {
		return std::unique_ptr<WSDataConverterInterface>(new WSDataConverter<char>(dataBytes, dataSize, order));
	}
	return std::unique_ptr<WSDataConverterInterface>(nullptr);
}

template<>
QString WSDataConverter<float>::valToString(float val, WSDataRepresent represent, quint8 precision) {
	return floatToString(val, represent, precision);
}

template<>
QString WSDataConverter<double>::valToString(double val, WSDataRepresent represent, quint8 precision) {
	return floatToString(val, represent, precision);
}

template<typename T>
QString WSDataConverter<T>::valToString(T val, WSDataRepresent represent, quint8) {
	return intToString(val, represent);
}

QString WSByteArrayConverter::toString(QByteArray arr, WSDataRepresent represent) {
	QString str = "";
	if (represent == WSDataRepresent::BIN) {
		for (int i = 0; i < arr.size(); i++) {
			str.append(QString::number(static_cast<unsigned char>(arr[i]), 2).toUpper() + QString(" "));
		}
	} else if (represent == WSDataRepresent::OCT) {
		for (int i = 0; i < arr.size(); i++) {
			str.append(QString::number(static_cast<unsigned char>(arr[i]), 8).toUpper() + QString(" "));
		}
	} else if (represent == WSDataRepresent::DEC) {
		for (int i = 0; i < arr.size(); i++) {
			str.append(QString::number(static_cast<unsigned char>(arr[i]), 10).toUpper() + QString(" "));
		}
	} else if (represent == WSDataRepresent::HEX) {
		for (int i = 0; i < arr.size(); i++) {
			str.append(((arr[i] >= static_cast<char>(0) && arr[i] < static_cast<char>(16)) ? QString("0") : QString("")) + QString::number(static_cast<unsigned char>(arr[i]), 16).toUpper() + QString(" "));
		}
	} else if (represent == WSDataRepresent::ASCII) {
		std::string stdString(arr.constData(), static_cast<size_t>(arr.length()));
		str = QString::fromStdString(stdString);
	}
	return str;
}

WSConversionState WSStringConverter::toArray(QByteArray &result, const QString &data, WSDataRepresent represent, WSByteOrder order, WSDataType type, quint8 typeSize, quint16 bytesSize, bool sign) {
	quint16 count = bytesSize / typeSize;
	bool ok;
	QStringList values = data.trimmed().split(QString(Conf::MULTI_DATA_DIVIDER));
	if (values.size() == count || type == WSDataType::TEXT) {
		if (type == WSDataType::INTEGER) {
			int base;
			if (represent == WSDataRepresent::BIN) {
				base = 2;
			} else if (represent == WSDataRepresent::OCT) {
				base = 8;
			} else if (represent == WSDataRepresent::DEC) {
				base = 10;
			} else if (represent == WSDataRepresent::HEX) {
				base = 16;
			} else {
				return WSConversionState::ERR_REPRESENT;
			}
			if (typeSize == 1) {
				if (sign) {
					QVector<qint8> v;
					short cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toShort(&ok, base);
						if (ok) {
							if (cur > std::numeric_limits<qint8>::max() || cur < std::numeric_limits<qint8>::min()) {
								return WSConversionState::ERR_LIMITS;
							}
							v.append(static_cast<qint8>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<qint8> conv(v, order);
					result.append(conv.bytes(), conv.size());
				} else {
					QVector<quint8> v;
					unsigned short cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toUShort(&ok, base);
						if (ok) {
							if (cur > std::numeric_limits<quint8>::max()) {
								return WSConversionState::ERR_LIMITS;
							}
							v.append(static_cast<quint8>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<quint8> conv(v, order);
					result.append(conv.bytes(), conv.size());
				}
			} else if (typeSize == 2) {
				if (sign) {
					QVector<qint16> v;
					short cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toShort(&ok, base);
						if (ok) {
							v.append(static_cast<qint16>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<qint16> conv(v, order);
					result.append(conv.bytes(), conv.size());
				} else {
					QVector<quint16> v;
					unsigned short cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toUShort(&ok, base);
						if (ok) {
							v.append(static_cast<quint16>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<quint16> conv(v, order);
					result.append(conv.bytes(), conv.size());
				}
			} else if (typeSize == 4) {
				if (sign) {
					QVector<qint32> v;
					int cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toInt(&ok, base);
						if (ok) {
							v.append(static_cast<qint32>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<qint32> conv(v, order);
					result.append(conv.bytes(), conv.size());
				} else {
					QVector<quint32> v;
					unsigned int cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toUInt(&ok, base);
						if (ok) {
							v.append(static_cast<quint32>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<quint32> conv(v, order);
					result.append(conv.bytes(), conv.size());
				}
			} else if (typeSize == 8) {
				if (sign) {
					QVector<qint64> v;
					long long cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toLongLong(&ok, base);
						if (ok) {
							v.append(static_cast<qint64>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<qint64> conv(v, order);
					result.append(conv.bytes(), conv.size());
				} else {
					QVector<quint64> v;
					unsigned long long cur;
					for (int i = 0; i < count; i++) {
						cur = values[i].toULongLong(&ok, base);
						if (ok) {
							v.append(static_cast<quint64>(cur));
						} else {
							return WSConversionState::ERR_CONVERT;
						}
					}
					WSDataConverter<quint64> conv(v, order);
					result.append(conv.bytes(), conv.size());
				}
			} else {
				return WSConversionState::ERR_TYPE_SIZE;
			}
		} else if (type == WSDataType::FLOAT) {
			if (typeSize == 4) {
				QVector<float> v;
				float cur;
				for (int i = 0; i < count; i++) {
					cur = values[i].toFloat(&ok);
					if (ok) {
						v.append(cur);
					} else {
						return WSConversionState::ERR_CONVERT;
					}
				}
				WSDataConverter<float> conv(v, order);
				result.append(conv.bytes(), conv.size());
			} else if (typeSize == 8) {
				QVector<double> v;
				double cur;
				for (int i = 0; i < count; i++) {
					cur = values[i].toDouble(&ok);
					if (ok) {
						v.append(cur);
					} else {
						return WSConversionState::ERR_CONVERT;
					}
				}
				WSDataConverter<double> conv(v, order);
				result.append(conv.bytes(), conv.size());
			} else {
				return WSConversionState::ERR_TYPE_SIZE;
			}
		} else if (type == WSDataType::TEXT) {
			QVector<char> v;
			QByteArray ascii = data.toLatin1();
			if (ascii.size() != count) {
				return WSConversionState::ERR_COUNT;
			}
			for (int i = 0; i < count; i++) {
				v.append(ascii[i]);
			}
			WSDataConverter<char> conv(v, order);
			result.append(conv.bytes(), conv.size());
		} else {
			return WSConversionState::ERR_TYPE;
		}
	} else {
		return WSConversionState::ERR_COUNT;
	}

	return WSConversionState::OK;
}
