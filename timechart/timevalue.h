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

#ifndef WS_CHART_TIMEVALUE_H
#define WS_CHART_TIMEVALUE_H

#include <QtCore>

namespace webstella {
	namespace gui {

		class InterpolateTimeValue {
		Q_GADGET

		private:
			quint64 m_time;
			double m_value;
			quint8 m_code;
			bool m_outOfRange;
		public:
			InterpolateTimeValue() :
				m_time(0),
				m_value(0.),
				m_code(0),
				m_outOfRange(true)
			{}
			InterpolateTimeValue(quint64 time, double value, quint8 code, bool outOfRange = false) :
				m_time(time),
				m_value(value),
				m_code(code),
				m_outOfRange(outOfRange)
			{}

			quint64 getTime() const {
				return m_time;
			}

			double getValue() const {
				return m_value;
			}

			quint64 getCode() const {
				return m_code;
			}

			bool isOutOfRange() const {
				return m_outOfRange;
			}

			Q_PROPERTY(quint64 time MEMBER m_time)
			Q_PROPERTY(double value MEMBER m_value)
			Q_PROPERTY(quint8 code MEMBER m_code)
			Q_PROPERTY(quint8 outOfRange MEMBER m_outOfRange)
		};

		class TimeValue : public QObject {
		Q_OBJECT
		Q_ENUMS(Type)
		Q_ENUMS(Range)
			
		public:
			enum class Type : quint8 {
				NONE = 0,
				INT = 1,
				DOUBLE = 2,
				BOOL = 3
			};

			enum class Range : quint8 {
				IN_RANGE = 0,
				OUT_LEFT = 1,
				OUT_RIGHT = 2,
				ERROR_RANGE = 3
			};
			
			TimeValue(QObject *parent = nullptr);
			TimeValue(quint64 time, quint8 code, QObject *parent = nullptr);
			Q_INVOKABLE virtual const quint64& time() const;
			Q_INVOKABLE virtual const quint8& code() const;
			Q_INVOKABLE virtual const QString& toString() const;
			Q_INVOKABLE Range range() const;
			Q_INVOKABLE void setRange(const Range &value);

			Q_INVOKABLE virtual qint64 intValue() const;
			Q_INVOKABLE virtual double doubleValue() const;
			Q_INVOKABLE virtual bool boolValue() const;
			Q_INVOKABLE virtual Type type() const;
			Q_INVOKABLE virtual bool operator==(TimeValue const &value);

		protected:
			quint64 m_time;
			quint8 m_code;
			QString m_formattedVal;
			Range m_range;
		};

		class IntTimeValue : public TimeValue {
		Q_OBJECT

		private:
			qint64 m_value;

		public:
			IntTimeValue(QObject *parent = nullptr);
			IntTimeValue(quint64 time, qint64 value, quint8 code, QObject *parent = nullptr);
			Q_INVOKABLE qint64 intValue() const override;
			Q_INVOKABLE double doubleValue() const override;
			Q_INVOKABLE bool boolValue() const override;
			Q_INVOKABLE Type type() const override;
			Q_INVOKABLE bool operator==(IntTimeValue const &value);
		};

		class DoubleTimeValue : public TimeValue {
		Q_OBJECT

		private:
			double m_value;

		public:
			DoubleTimeValue(QObject *parent = nullptr);
			DoubleTimeValue(quint64 time, double value, quint8 code, QObject *parent = nullptr);
			Q_INVOKABLE qint64 intValue() const override;
			Q_INVOKABLE double doubleValue() const override;
			Q_INVOKABLE bool boolValue() const override;
			Q_INVOKABLE Type type() const override;
			Q_INVOKABLE bool operator==(DoubleTimeValue const &value);
		};

		class BoolTimeValue : public TimeValue {
		Q_OBJECT

		private:
			bool m_value;

		public:
			BoolTimeValue(QObject *parent = nullptr);
			BoolTimeValue(quint64 time, bool value, quint8 code, QObject *parent = nullptr);
			Q_INVOKABLE qint64 intValue() const override;
			Q_INVOKABLE double doubleValue() const override;
			Q_INVOKABLE bool boolValue() const override;
			Q_INVOKABLE Type type() const override;
			Q_INVOKABLE bool operator==(BoolTimeValue const &value);
		};
	}
}

Q_DECLARE_METATYPE(webstella::gui::TimeValue::Type)
Q_DECLARE_METATYPE(webstella::gui::TimeValue::Range)

#endif // WS_CHART_TIMEVALUE_H
