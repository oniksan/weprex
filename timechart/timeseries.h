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

#ifndef WS_CHART_TIMESERIES_H
#define WS_CHART_TIMESERIES_H

#include <QtCore>
#include <QtGui>
#include "timevalue.h"

namespace webstella {
	namespace gui {

		class TimeSeries : public QObject {
		Q_OBJECT
		Q_ENUMS(Notation)

		public:
			enum class Notation : quint8 {
				// Линейный
				LINES = 1,
				// Ступенчатый
				STEPS = 2,
				// Битовый
				BITS = 3,
				// Ограничение
				BOUND = 4
			};
			
			TimeSeries(QObject *parent = nullptr);
			TimeSeries(const QString &name, QObject *parent = nullptr);

			quint32 maxDataSize() const;
			void setMaxDataSize(quint32 size);
			
			QString name() const;
			void setName(const QString &value);
			
			QColor normalLineColor() const;
			void setNormalLineColor(const QColor &value);
			
			int normalLineWidth() const;
			void setNormalLineWidth(int value);
			
			Qt::PenStyle normalLineStyle() const;
			void setNormalLineStyle(const Qt::PenStyle &value);
			
			QColor errorLineColor() const;
			void setErrorLineColor(const QColor &value);
			
			int errorLineWidth() const;
			void setErrorLineWidth(int value);
			
			Qt::PenStyle errorLineStyle() const;
			void setErrorLineStyle(const Qt::PenStyle &value);
			
			QColor pointBorderColor() const;
			void setPointBorderColor(const QColor &value);
			
			QColor pointBackgroundColor() const;
			void setPointBackgroundColor(const QColor &value);
			
			int pointRadius() const;
			void setPointRadius(int value);
			
			bool isShowPoints() const;
			void setShowPoints(bool value);
			
			Notation notation() const;
			void setNotation(Notation value);
			
			quint8 bitsCount() const;
			void setBitsCount(quint8 value);

			bool needRepaint() const;
			void wasRepainted();
			
			Q_INVOKABLE QList<TimeValue*> maxValues(quint64 leftTime, quint64 rightTime) const;
			Q_INVOKABLE QList<TimeValue*> minValues(quint64 leftTime, quint64 rightTime) const;
			Q_INVOKABLE quint64 maxTime() const;
			Q_INVOKABLE quint64 minTime() const;
			Q_INVOKABLE const QList<TimeValue*>& completeData() const;
			Q_INVOKABLE QList<TimeValue*> intervalData(quint64 leftTime, quint64 rightTime, bool inclusive) const;
			Q_INVOKABLE QList<TimeValue*> intervalDataCoarse(quint16 intervals, quint64 leftTime, quint64 rightTime) const;
			Q_INVOKABLE TimeValue::Type dataType() const;
			Q_INVOKABLE quint32 size() const;
			Q_INVOKABLE double minBoundValue() const;
			Q_INVOKABLE double maxBoundValue() const;
			Q_INVOKABLE void setBoundValues(double min, double max);
			Q_INVOKABLE TimeValue* value(quint64 time) const;
			//Q_INVOKABLE QSharedPointer<DoubleTimeValue> interpolateValue(quint64 time);
			Q_INVOKABLE InterpolateTimeValue interpolateValue(quint64 time);
			Q_INVOKABLE TimeValue* leftNearValue(quint64 time) const;
			Q_INVOKABLE TimeValue* rightNearValue(quint64 time) const;
			Q_INVOKABLE TimeValue* firstValue() const;
			Q_INVOKABLE TimeValue* lastValue() const;
			Q_INVOKABLE bool addIntPoint(quint64 time, qint64 value, quint8 code);
			Q_INVOKABLE bool addDoublePoint(quint64 time, double value, quint8 code);
			Q_INVOKABLE bool addBoolPoint(quint64 time, bool value, quint8 code);
			Q_INVOKABLE bool addIntErrorPoint(quint64 time, quint8 code);
			Q_INVOKABLE bool addDoubleErrorPoint(quint64 time, quint8 code);
			Q_INVOKABLE bool addBoolErrorPoint(quint64 time, quint8 code);
			
			Q_PROPERTY(quint32 maxDataSize READ maxDataSize WRITE setMaxDataSize NOTIFY maxDataSizeChanged)
			Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
			Q_PROPERTY(QColor normalLineColor READ normalLineColor WRITE setNormalLineColor NOTIFY normalLineColorChanged)
			Q_PROPERTY(int normalLineWidth READ normalLineWidth WRITE setNormalLineWidth NOTIFY normalLineWidthChanged)
			Q_PROPERTY(Qt::PenStyle normalLineStyle READ normalLineStyle WRITE setNormalLineStyle NOTIFY normalLineStyleChanged)
			Q_PROPERTY(QColor errorLineColor READ errorLineColor WRITE setErrorLineColor NOTIFY errorLineColorChanged)
			Q_PROPERTY(int errorLineWidth READ errorLineWidth WRITE setErrorLineWidth NOTIFY errorLineWidthChanged)
			Q_PROPERTY(Qt::PenStyle errorLineStyle READ errorLineStyle WRITE setErrorLineStyle NOTIFY errorLineStyleChanged)
			Q_PROPERTY(QColor pointBorderColor READ pointBorderColor WRITE setPointBorderColor NOTIFY pointBorderColorChanged)
			Q_PROPERTY(QColor pointBackgroundColor READ pointBackgroundColor WRITE setPointBackgroundColor NOTIFY pointBackgroundColorChanged)
			Q_PROPERTY(int pointRadius READ pointRadius WRITE setPointRadius NOTIFY pointRadiusChanged)
			Q_PROPERTY(bool isShowPoints READ isShowPoints WRITE setShowPoints NOTIFY showPointsChanged)
			Q_PROPERTY(Notation notation READ notation WRITE setNotation NOTIFY notationChanged)
			Q_PROPERTY(quint8 bitsCount READ bitsCount WRITE setBitsCount NOTIFY bitsCountChanged)

		private:
			void seriesChangeEventSender();
			bool addPoint(TimeValue* value);

		protected:
			// Точки графика
			QList<TimeValue*> m_data;
			// Максимальное число точек графика
			quint32 m_maxDataSize;
			// Имя графика
			QString m_name;
			// Подпись оси 0x
			QString m_xAxisName;
			// Подпись оси 0y
			QString m_yAxisName;
			// Тип данных (определяется первой добавленной записью)
			TimeValue::Type m_type;
			// Представление данных
			Notation m_notation;
			// Количество бит при рендеринге в битовом представлении
			quint8 m_bitsCount;

			QColor m_normalLineColor;
			int m_normalLineWidth;
			Qt::PenStyle m_normalLineStyle;
			QColor m_errorLineColor;
			int m_errorLineWidth;
			Qt::PenStyle m_errorLineStyle;
			QColor m_pointBorderColor;
			QColor m_pointBackgroundColor;
			int m_pointRadius;
			bool m_showPoints;
			double m_minBoundValue;
			double m_maxBoundValue;
			bool m_needRepaint;
		
		signals:
			void maxDataSizeChanged(quint32 value);
			void nameChanged(const QString &value);
			void normalLineColorChanged(const QColor &value);
			void normalLineWidthChanged(int value);
			void normalLineStyleChanged(const Qt::PenStyle &value);
			void errorLineColorChanged(const QColor &value);
			void errorLineWidthChanged(int value);
			void errorLineStyleChanged(const Qt::PenStyle &value);
			void pointBorderColorChanged(const QColor &value);
			void pointBackgroundColorChanged(const QColor &value);
			void pointRadiusChanged(int value);
			void showPointsChanged(bool value);
			void notationChanged(Notation value);
			void bitsCountChanged(quint8 value);
		};
	}
}

Q_DECLARE_METATYPE(webstella::gui::TimeSeries::Notation)

#endif // WS_CHART_TIMESERIES_H
