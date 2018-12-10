/****************************************************************************

  This file is part of the Webstella GUI library.

  Copyright (C) 2014 - 2018 Oleg Malyavkin.
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

#include "timeseries.h"

using namespace webstella::gui;

TimeSeries::TimeSeries(const QString &name, QObject *parent) :
	QObject(parent),
	m_maxDataSize(0),
	m_name(name),
	m_type(TimeValue::Type::NONE),
	m_notation(TimeSeries::Notation::LINES),
	m_bitsCount(64),
	m_normalLineColor(QColor(0, 200, 100)),
	m_normalLineWidth(2),
	m_normalLineStyle(Qt::SolidLine),
	m_errorLineColor(QColor(255, 50 , 0)),
	m_errorLineWidth(3),
	m_errorLineStyle(Qt::DashLine),
	m_pointBorderColor(255, 255, 255),
	m_pointBackgroundColor(QColor(0, 200, 100)),
	m_pointRadius(3),
	m_showPoints(true),
	m_needRepaint(false)
{}

TimeSeries::TimeSeries(QObject *parent) :
	TimeSeries(QString(), parent)
{}

QColor TimeSeries::normalLineColor() const {
	return m_normalLineColor;
}

void TimeSeries::setNormalLineColor(const QColor &value) {
	m_normalLineColor = value;
	m_needRepaint = true;
	emit normalLineColorChanged(value);
}

int TimeSeries::normalLineWidth() const {
	return m_normalLineWidth;
}

void TimeSeries::setNormalLineWidth(int value) {
	m_normalLineWidth = value;
	m_needRepaint = true;
	emit normalLineWidthChanged(value);
}

Qt::PenStyle TimeSeries::normalLineStyle() const {
	return m_normalLineStyle;
}

void TimeSeries::setNormalLineStyle(const Qt::PenStyle &value) {
	m_normalLineStyle = value;
	m_needRepaint = true;
	emit normalLineStyleChanged(value);
}

QColor TimeSeries::errorLineColor() const {
	return m_errorLineColor;
}

void TimeSeries::setErrorLineColor(const QColor &value) {
	m_errorLineColor = value;
	m_needRepaint = true;
	emit errorLineColorChanged(value);
}

int TimeSeries::errorLineWidth() const {
	return m_errorLineWidth;
}

void TimeSeries::setErrorLineWidth(int value) {
	m_errorLineWidth = value;
	m_needRepaint = true;
	emit errorLineWidthChanged(value);
}

Qt::PenStyle TimeSeries::errorLineStyle() const {
	return m_errorLineStyle;
}

void TimeSeries::setErrorLineStyle(const Qt::PenStyle &value) {
	m_errorLineStyle = value;
	m_needRepaint = true;
	emit errorLineStyleChanged(value);
}

QColor TimeSeries::pointBorderColor() const {
	return m_pointBorderColor;
}

void TimeSeries::setPointBorderColor(const QColor &value) {
	m_pointBorderColor = value;
	m_needRepaint = true;
	emit pointBorderColorChanged(value);
}

QColor TimeSeries::pointBackgroundColor() const {
	return m_pointBackgroundColor;
}

void TimeSeries::setPointBackgroundColor(const QColor &value) {
	m_pointBackgroundColor = value;
	m_needRepaint = true;
	emit pointBackgroundColorChanged(value);
}

int TimeSeries::pointRadius() const {
	return m_pointRadius;
}

void TimeSeries::setPointRadius(int value) {
	m_pointRadius = value;
	m_needRepaint = true;
	emit pointRadiusChanged(value);
}

bool TimeSeries::isShowPoints() const {
	return m_showPoints;
}

void TimeSeries::setShowPoints(bool value) {
	m_showPoints = value;
	m_needRepaint = true;
	emit showPointsChanged(value);
}

double TimeSeries::minBoundValue() const {
	return m_minBoundValue;
}

double TimeSeries::maxBoundValue() const {
	return m_maxBoundValue;
}

void TimeSeries::setBoundValues(double min, double max) {
	m_minBoundValue = min;
	m_maxBoundValue = max;
}

quint8 TimeSeries::bitsCount() const {
	return m_bitsCount;
}

void TimeSeries::setBitsCount(quint8 value) {
	if (value <= 8) {
		m_bitsCount = 8;
	} else if (m_bitsCount <= 16) {
		m_bitsCount = 16;
	} else if (m_bitsCount <= 32) {
		m_bitsCount = 32;
	} else {
		m_bitsCount = 64;
	}
	m_bitsCount = value;
	m_needRepaint = true;
	emit bitsCountChanged(value);
}

QString TimeSeries::name() const {
	return m_name;
}

void TimeSeries::setName(const QString &value) {
	m_name = value;
	emit nameChanged(value);
}

quint64 TimeSeries::maxTime() const {
	if (m_data.size() > 0) {
		return m_data.last()->time();
	} else {
		return 0;
	}
}

quint64 TimeSeries::minTime() const {
	if (m_data.size() > 0) {
		return m_data.first()->time();
	} else {
		return 0;
	}
}

const QList<TimeValue*>& TimeSeries::completeData() const {
	return m_data;
}

// Не оптимизированно
/*QList<TimeValue*> TimeSeries::intervalData(quint64 leftTime, quint64 rightTime, bool inclusive) const {
	QList<TimeValue*> res;
	bool firstFlag = true;
	quint64 t;
	quint32 counter = 0;
	if (leftTime < rightTime) {
		if (inclusive) {
			for (int i = 0, k = m_data.size(); i < k; i++) {
				counter++;
				t = m_data.at(i)->time();
				if (t >= leftTime) {
					if (firstFlag) {
						if (i > 0) {
							res.append(m_data.at(i - 1));
						}
						firstFlag = false;
					}
					res.append(m_data.at(i));
					if (t > rightTime) {
						break;
					}
				}
			}
		} else {
			for (int i = 0, k = m_data.size(); i < k; i++) {
				counter++;
				t = m_data.at(i)->time();
				if (t >= leftTime) {
					if (t > rightTime) {
						break;
					}
					res.append(m_data.at(i));
				}
			}
		}
	}
	return res;
}*/

// Оптимизирован поиск границ (время-индекс представляется уравнением прямой,
// выбирается средняя точка, затем поиск ведется слева или справа)
QList<TimeValue*> TimeSeries::intervalData(quint64 leftTime, quint64 rightTime, bool inclusive) const {
	QList<TimeValue*> res;
	quint64 t, t0, tN, timeCur;
	int i0, iN, iCur, iL, iR;
	iL = 0;
	iR = 0;
	timeCur = leftTime;
	quint32 counter = 0;
	if (m_data.size() > 1
			&& leftTime < rightTime
			&& leftTime <= m_data.constLast()->time()
			&& rightTime > m_data.constFirst()->time()) {
		// Поиск границы (на первой итерации - левой, на второй- правой)
		for (int i = 0; i < 2; i++) {
			i0 = 0;
			iN = m_data.size() - 1;
			t0 = m_data.constFirst()->time();
			tN = m_data.constLast()->time();
			while (true) {
				counter++;
				// Проверка вхождения всего интервала в указанные границы
				if (i == 0 && leftTime <= t0) {
					iCur = 0;
					break;
				} else if (i == 1 && rightTime >= tN) {
					iCur = iN;
					break;
				}
				// Текущий индекс (по уравнению прямой)
				iCur = static_cast<int>(static_cast<quint64>(i0) + ((timeCur - t0) * static_cast<quint64>((iN - i0))) / (tN - t0));
				t = m_data.at(iCur)->time();
				// Искомый индекс лежит левее
				if (t > timeCur) {
					iN = iCur - 1;
					tN = m_data.at(iN)->time();
					if (tN < timeCur) {
						// Включая запредельные интервалы
						if (inclusive) {
							// Индекс для левой границы
							if (i == 0) {
								iCur = iN;
							}
							// Для правой iCur
							break;
						// Не включая запредельные интервалы
						} else {
							// Индекс для правой границы
							if (i == 1) {
								iCur = iN;
							}
							// Для левой iCur
							break;
						}
					} else if (tN == timeCur) {
						break;
					}
				// Искомый индекс лежит правее
				} else if (t < timeCur) {
					i0 = iCur + 1;
					t0 = m_data.at(i0)->time();
					if (t0 > timeCur) {
						// Включая запредельные интервалы
						if (inclusive) {
							// Индекс для правой границы
							if (i == 1) {
								iCur = i0;
							}
							// Для левой iCur
							break;
						// Не включая запредельные интервалы
						} else {
							// Индекс для левой границы
							if (i == 0) {
								iCur = i0;
							}
							// Для правой iCur
							break;
						}
					} else if (t0 == timeCur){
						break;
					}
				// Искомый индекс в этой точке
				} else {
					break;
				}
			}
			// Сохранение данных поиска по границе
			if (i == 0) {
				iL = iCur;
			} else {
				iR = iCur;
			}
			// Переход к правой границе
			timeCur = rightTime;
		}
		// Выборка интервала
		if (iR > iL) {
			return m_data.mid(iL, iR- iL + 1);
		}
	}
	return res;
}

QList<TimeValue*> TimeSeries::intervalDataCoarse(quint16 intervals, quint64 leftTime, quint64 rightTime) const {
	QList<TimeValue*> res;
	TimeValue* tv;
	tv = leftNearValue(leftTime);
	if (tv != nullptr) {
		res.append(tv);
	}
	if (leftTime < rightTime && intervals > 0) {
		quint64 step = (rightTime - leftTime) / intervals;
		for (quint64 i = leftTime; i < rightTime; i+=step) {
			QList<TimeValue*> max = maxValues(i, i + step);
			QList<TimeValue*> min = minValues(i, i + step);
			QList<TimeValue*> minmax;
			int iMin = 0, iMax = 0;
			while (true) {
				if (iMin < min.size()) {
					if (iMax < max.size()) {
						if (min.at(iMin)->time() < max.at(iMax)->time()) {
							minmax.append(min.at(iMin));
							iMin++;
						} else if (min.at(iMin)->time() > max.at(iMax)->time()) {
							minmax.append(max.at(iMax));
							iMax++;
						} else {
							minmax.append(max.at(iMax));
							iMin++;
							iMax++;
						}
					} else {
						minmax.append(min.at(iMin));
						iMin++;
					}
				} else {
					if (iMax < max.size()) {
						minmax.append(max.at(iMax));
						iMax++;
					} else {
						break;
					}
				}
			}

			for (int j = 0; j < minmax.size(); j++) {
				res.append(minmax.at(j));
			}
		}
	}
	tv = rightNearValue(rightTime);
	if (tv != nullptr) {
		res.append(tv);
	}

	return res;
}

quint32 TimeSeries::maxDataSize() const {
	return m_maxDataSize;
}

quint32 TimeSeries::size() const {
	return static_cast<quint32>(m_data.size());
}

void TimeSeries::setMaxDataSize(quint32 size) {
	 m_maxDataSize = size;
	 emit maxDataSizeChanged(size);
}

bool TimeSeries::addPoint(TimeValue* value) {
	if (value->time() > maxTime() || m_data.size() == 0) {
		// Переполнение буфера
		if (m_maxDataSize != 0 && static_cast<quint32>(m_data.size()) >= m_maxDataSize) {
			m_data.removeFirst();
		}
		// Проверка типа данных
		if (m_data.size() > 0) {
			if (m_data.first()->type() != value->type()) {
				return false;
			}
		} else {
			m_type = value->type();
		}
		m_data.append(value);
		m_needRepaint = true;
		return true;
	}
	return false;
}

TimeValue::Type TimeSeries::dataType() const {
	return m_type;
}

TimeSeries::Notation TimeSeries::notation() const {
	return m_notation;
}

void TimeSeries::setNotation(Notation value) {
	m_notation = value;
	m_needRepaint = true;
	emit notationChanged(value);
}

QList<TimeValue*> TimeSeries::maxValues(quint64 leftTime = 0, quint64 rightTime = 0) const {
	QList<TimeValue*> res;
	QList<TimeValue*> localData;
	if (leftTime == 0 && rightTime == 0) {
		localData = completeData();
	} else {
		localData = intervalData(leftTime, rightTime, false);
	}
	double max;
	TimeValue* v;
	int lastMaxIndex = 0;
	if (localData.size() > 0) {
		max = localData.first()->doubleValue();
		for (int i = 0; i < localData.size(); i++) {
			v = localData.at(i);
			if (v->doubleValue() > max) {
				max = v->doubleValue();
				lastMaxIndex = i;
			}
		}

		for (int i = 0; i < localData.size(); i++) {
			v = localData.at(i);
			if (v->doubleValue() == max) {
				if ((lastMaxIndex + 1) != i) {
					res.append(v);
				}
				lastMaxIndex = i;
			}
		}
	}

	return res;
}

QList<TimeValue*> TimeSeries::minValues(quint64 leftTime = 0, quint64 rightTime = 0) const {
	QList<TimeValue*> res;
	QList<TimeValue*> localData;
	if (leftTime == 0 && rightTime == 0) {
		localData = completeData();
	} else {
		localData = intervalData(leftTime, rightTime, false);
	}
	double min;
	TimeValue* v;
	int lastMinIndex = 0;
	if (localData.size() > 0) {
		min = localData.first()->doubleValue();
		for (int i = 0; i < localData.size(); i++) {
			v = localData.at(i);
			if (v->doubleValue() < min) {
				min = v->doubleValue();
				lastMinIndex = i;
			}
		}

		for (int i = 0; i < localData.size(); i++) {
			v = localData.at(i);
			if (v->doubleValue() == min) {
				if ((lastMinIndex + 1) != i) {
					res.append(v);
				}
				lastMinIndex = i;
			}
		}
	}

	return res;
}

TimeValue* TimeSeries::value(quint64 time) const {
	for (int i = 0; i < m_data.size(); i++) {
		if (m_data.at(i)->time() == time) {
			return m_data.at(i);
		}
	}
	return nullptr;
}

InterpolateTimeValue TimeSeries::interpolateValue(quint64 time) {
	if (m_data.size() < 1 || time < m_data.first()->time()) {
		return InterpolateTimeValue();
	}
	TimeValue* v;
	quint64 left = 0;
	TimeValue* val;
	for (int i = 0; i < m_data.size(); i++) {
		v = m_data.at(i);
		if (v->time() == time) {
			return InterpolateTimeValue(v->time(), v->doubleValue(), v->code());
		} else if (v->time() > time && left > 0) {
			val = value(left);
			if (val != nullptr) {
				return InterpolateTimeValue(time, (((time - left) * (v->doubleValue() - val->doubleValue())) / (v->time() - left)) + val->doubleValue(), val->code());
			} else {
				return InterpolateTimeValue();
			}
		}
		left = v->time();
	}
	return InterpolateTimeValue();
}

/*QSharedPointer<DoubleTimeValue> TimeSeries::interpolateValue(quint64 time) {
	if (m_data.size() < 1 || time < m_data.first()->time()) {
		return QSharedPointer<DoubleTimeValue>(new DoubleTimeValue(nullptr));
	}
	TimeValue* v;
	quint64 left = 0;
	TimeValue* val;
	for (int i = 0; i < m_data.size(); i++) {
		v = m_data.at(i);
		if (v->time() == time) {
			return QSharedPointer<DoubleTimeValue>(new DoubleTimeValue(v->time(), v->doubleValue(), v->code(), nullptr));
		} else if (v->time() > time && left > 0) {
			val = value(left);
			if (val != nullptr) {
				return QSharedPointer<DoubleTimeValue>(new DoubleTimeValue(time, (((time - left) * (v->doubleValue() - val->doubleValue())) / (v->time() - left)) + val->doubleValue(), val->code(), nullptr));
			} else {
				return QSharedPointer<DoubleTimeValue>(new DoubleTimeValue(nullptr));
			}
		}
		left = v->time();
	}
	return QSharedPointer<DoubleTimeValue>(new DoubleTimeValue(nullptr));
}*/

TimeValue* TimeSeries::leftNearValue(quint64 time) const {
	if (m_data.size() < 1 || time < m_data.first()->time()) {
		return nullptr;
	}
	quint64 left = 0;
	TimeValue* v;
	for (int i = 0; i < m_data.size(); i++) {
		v = m_data.at(i);
		if (v->time() == time) {
			return v;
		} else if (v->time() > time && left > 0) {
			return value(left);
		}
		left = v->time();
	}
	return nullptr;
}

TimeValue* TimeSeries::rightNearValue(quint64 time) const {
	TimeValue* v;
	for (int i = 0; i < m_data.size(); i++) {
		v = m_data.at(i);
		if (v->time() >= time) {
			return v;
		}
	}
	return nullptr;
}

TimeValue* TimeSeries::firstValue() const {
	if (m_data.size() == 0) {
		return nullptr;
	} else {
		return m_data.first();
	}
}

TimeValue* TimeSeries::lastValue() const {
	if (m_data.size() == 0) {
		return nullptr;
	} else {
		return m_data.last();
	}
}

bool TimeSeries::addIntPoint(quint64 time, qint64 value, quint8 code) {
	auto *p = new IntTimeValue(time, value, code, this);
	return addPoint(p);
}

bool TimeSeries::addDoublePoint(quint64 time, double value, quint8 code) {
	auto *p = new DoubleTimeValue(time, value, code, this);
	return addPoint(p);
}

bool TimeSeries::addBoolPoint(quint64 time, bool value, quint8 code) {
	auto *p = new BoolTimeValue(time, value, code, this);
	return addPoint(p);
}

bool TimeSeries::addIntErrorPoint(quint64 time, quint8 code) {
	TimeValue *p = lastValue();
	qint64 value = 0;
	if (p != nullptr) {
		value = p->intValue();
	}
	return addIntPoint(time, value, code);
}

bool TimeSeries::addDoubleErrorPoint(quint64 time, quint8 code) {
	TimeValue *p = lastValue();
	double value = 0.;
	if (p != nullptr) {
		value = p->doubleValue();
	}
	return addDoublePoint(time, value, code);
}

bool TimeSeries::addBoolErrorPoint(quint64 time, quint8 code) {
	TimeValue *p = lastValue();
	bool value = false;
	if (p != nullptr) {
		value = p->boolValue();
	}
	return addBoolPoint(time, value, code);
}

bool TimeSeries::needRepaint() const {
	return m_needRepaint;
}

void TimeSeries::wasRepainted() {
	m_needRepaint = false;
}
