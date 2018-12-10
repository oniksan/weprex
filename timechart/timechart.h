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

#ifndef WS_CHART_TIMECHART_H
#define WS_CHART_TIMECHART_H

#include <QtCore>
#include <QtGui>
#include <QtQuick/QQuickPaintedItem>
#include "timevalue.h"
#include "timeseries.h"
#include "defaultseriesrenderer.h"

namespace webstella {
	namespace gui {

		class TimeChart : public QQuickPaintedItem {
		Q_OBJECT
		Q_ENUMS(TrackingRegime)

		public:
			enum class TrackingRegime : quint8 {
				UNDEFINED = 0,
				NO_TRACKING_NO_SCALING = 1,
				TRACKING_AND_SCALING_ALL = 2,
				TRACKING_AND_SCALING_VALUE = 3,
				TRACKING_WITHOUT_SCALING = 4
			};

			explicit TimeChart(QQuickItem *parent = nullptr);
			virtual ~TimeChart() override;
			TimeChart(const TimeChart &obj) = delete;
			TimeChart& operator=(TimeChart &obj) = delete;
			TimeChart(const TimeChart &&obj) = delete;
			TimeChart& operator=(TimeChart &&obj) = delete;

			int zoomCounterBase() const;
			void setZoomCounterBase(int value);

			int autoUpdateTime() const;
			void setAutoUpdateTime(int value);

			TrackingRegime trackingRegime();
			void setTrackingRegime(TrackingRegime regime);

			void setExtraSeriesView(bool value);
			bool isExtraSeriesView() const;

			Q_INVOKABLE void paint(QPainter* painter) override;
			Q_INVOKABLE bool setTimeAxisRange(quint64 m_leftTime, quint64 m_rightTime);
			Q_INVOKABLE quint64 leftTime() const;
			Q_INVOKABLE quint64 rightTime() const;
			Q_INVOKABLE SeriesRenderer* render() const;
			Q_INVOKABLE void scalingDefault();
			Q_INVOKABLE void scalingTimeValue();
			Q_INVOKABLE void scalingValue();
			Q_INVOKABLE QColor predefinedColor(quint16 index);
			Q_INVOKABLE TimeSeries* addBasicSeries(const QString& name);
			Q_INVOKABLE TimeSeries* addExtraSeries(const QString& name);
			Q_INVOKABLE TimeSeries* getBasicSeries(const QString& name);
			Q_INVOKABLE TimeSeries* getExtraSeries(const QString& name);
			Q_INVOKABLE bool removeBasicSeries(const QString &name);
			Q_INVOKABLE bool removeExtraSeries(const QString &name);
			Q_INVOKABLE bool removeBasicSeries(int index);
			Q_INVOKABLE bool removeExtraSeries(int index);
			Q_INVOKABLE quint32 basicSeriesCount() const;
			Q_INVOKABLE quint32 extraSeriesCount() const;
			Q_INVOKABLE void removeAllSerieses();
			Q_INVOKABLE void repaint();
			Q_INVOKABLE TimeBounds currentTimeBounds();
			Q_INVOKABLE ValueBounds currentValueBounds();
			Q_INVOKABLE void moveTimeForward(quint64 time);
			Q_INVOKABLE void moveTimeBack(quint64 time);
			Q_INVOKABLE void moveValueUp(double value);
			Q_INVOKABLE void moveValueDown(double value);
			Q_INVOKABLE void setTimeBounds(quint64 leftTime, quint64 rightTime);
			Q_INVOKABLE void setValueBounds(double min, double max);
			Q_INVOKABLE void scaleTimeBounds(double mult);
			Q_INVOKABLE void scaleValueBounds(double mult);

			Q_PROPERTY(int zoomCounterBase READ zoomCounterBase WRITE setZoomCounterBase NOTIFY zoomCounterBaseChanged)
			Q_PROPERTY(int autoUpdateTime READ autoUpdateTime WRITE setAutoUpdateTime NOTIFY autoUpdateTimeChanged)
			Q_PROPERTY(TrackingRegime trackingRegime READ trackingRegime WRITE setTrackingRegime NOTIFY trackingRegimeChanged)
			Q_PROPERTY(bool extraSeriesView READ isExtraSeriesView WRITE setExtraSeriesView NOTIFY extraSeriesViewChanged)

		private:
			// Данные (основные)
			QList<TimeSeries*> m_basicSeries;
			// Данные (дополнительные)
			QList<TimeSeries*> m_extraSeries;
			// Рендер
			SeriesRenderer* m_render;
			int m_autoUpdateTime;

			// Левая временная граница
			quint64 m_leftTime;
			// Правая временная граница
			quint64 m_rightTime;

			// Таймер обновления значений
			QTimer *m_autoUpdateTimer;

			// Таймер перерисовки зуммирования
			QTimer *m_zoomTimer;
			// Счетчик таймера зуммирования
			qint32 m_zoomCounter;
			// Количество визуализированных шагов зуммирования
			int m_zoomCounterBase;

			// Область выделения
			QRect m_selection;
			// Сохраненная область выделения
			QRect m_savedSelection;
			// Флаг зажатой левой кнопки мыши
			bool m_leftMousePressed;
			bool m_rightMousePressed;
			bool m_middleMousePressed;

			bool m_zoomLock;
			bool m_mouseMoveEventDisable;
			TrackingRegime m_lastTrackingRegime;

			void addBasicTimeSeries(TimeSeries* series);
			void addExtraTimeSeries(TimeSeries* series);
			void trackingRegimeMonitor();

		protected:
			virtual void mouseMoveEvent(QMouseEvent* me) override;
			virtual void hoverMoveEvent(QHoverEvent* he) override;
			virtual void mousePressEvent(QMouseEvent* me) override;
			virtual void mouseReleaseEvent(QMouseEvent* me) override;
			//virtual void resizeEvent(QResizeEvent *re);
			virtual void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;
			virtual void wheelEvent(QWheelEvent *we) override;


		public slots:
			void zoomSlot();
			void autoUpdateSlot();

		signals:
			void zoomCounterBaseChanged(int value);
			void autoUpdateTimeChanged(int value);
			void trackingRegimeChanged();
			void extraSeriesViewChanged(bool value);
		};
	}
}

Q_DECLARE_METATYPE(webstella::gui::TimeChart::TrackingRegime)

#endif // WS_CHART_TIMECHART_H
