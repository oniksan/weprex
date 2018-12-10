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

#include "timevalue.h"

using namespace webstella::gui;

TimeValue::TimeValue(QObject *parent) :
	QObject(parent),
	m_time(0),
	m_code(0),
	m_range(Range::ERROR_RANGE)
{}

TimeValue::TimeValue(quint64 time, quint8 code, QObject *parent) :
	QObject(parent),
	m_time(time),
	m_code(code),
	m_range(Range::IN_RANGE)
{}

TimeValue::Range TimeValue::range() const {
	return m_range;
}

void TimeValue::setRange(const Range &value) {
	m_range = value;
}

const quint64& TimeValue::time() const {
	return m_time;
}

const quint8& TimeValue::code() const {
	return m_code;
}

const QString& TimeValue::toString() const {
	return m_formattedVal;
}

qint64 TimeValue::intValue() const {
	return 0;
}

double TimeValue::doubleValue() const {
	return 0.;
}

bool TimeValue::boolValue() const {
	return false;
}

TimeValue::Type TimeValue::type() const {
	return Type::NONE;
}

bool TimeValue::operator==(TimeValue const &value) {
	if (this->time() == value.time() && this->code() == value.code()) {
		return true;
	}
	return false;
}

IntTimeValue::IntTimeValue(QObject *parent) :
	TimeValue(parent),
	m_value(0)
{}

IntTimeValue::IntTimeValue(quint64 time, qint64 value, quint8 code, QObject *parent) :
	TimeValue(time, code, parent),
	m_value(value)

{
	m_formattedVal = QString::number(m_value);
}

qint64 IntTimeValue::intValue() const {
	return m_value;
}

double IntTimeValue::doubleValue() const {
	return static_cast<double>(m_value);
}

bool IntTimeValue::boolValue() const {
	return (m_value == 0)?false:true;
}

TimeValue::Type IntTimeValue::type() const {
	return Type::INT;
}

bool IntTimeValue::operator==(IntTimeValue const &value) {
	if (this->intValue() == value.intValue() && this->time() == value.time() && this->code() == value.code()) {
		return true;
	}
	return false;
}

DoubleTimeValue::DoubleTimeValue(QObject *parent) :
	TimeValue(parent),
	m_value(0.)
{}

DoubleTimeValue::DoubleTimeValue(quint64 time, double value, quint8 code, QObject *parent) :
	TimeValue(time, code, parent),
	m_value(value)
{
	if (m_value > 100000.) {
		m_formattedVal = QString::number(m_value, 'e', 3);
	} else {
		m_formattedVal = QString::number(m_value, 'f', 3);
	}
}

qint64 DoubleTimeValue::intValue() const {
	return static_cast<qint64>(m_value);
}

double DoubleTimeValue::doubleValue() const {
	return m_value;
}

bool DoubleTimeValue::boolValue() const {
	return (m_value == 0.)?false:true;
}

TimeValue::Type DoubleTimeValue::type() const {
	return Type::DOUBLE;
}

bool DoubleTimeValue::operator==(DoubleTimeValue const &value) {
	if (this->doubleValue() == value.doubleValue() && this->time() == value.time() && this->code() == value.code()) {
		return true;
	}
	return false;
}

BoolTimeValue::BoolTimeValue(QObject *parent) :
	TimeValue(parent),
	m_value(false)
{}

BoolTimeValue::BoolTimeValue(quint64 time, bool value, quint8 code, QObject *parent) :
	TimeValue(time, code, parent),
	m_value(value)
{
	if (m_value) {
		m_formattedVal = QString("1");
	} else {
		m_formattedVal = QString("0");
	}
}

qint64 BoolTimeValue::intValue() const {
	return (m_value)?1:0;
}

double BoolTimeValue::doubleValue() const {
	return (m_value)?1.:0.;
}

bool BoolTimeValue::boolValue() const {
	return m_value;
}

TimeValue::Type BoolTimeValue::type() const {
	return Type::BOOL;
}

bool BoolTimeValue::operator==(BoolTimeValue const &value) {
	if (this->boolValue() == value.boolValue() && this->time() == value.time() && this->code() == value.code()) {
		return true;
	}
	return false;
}
