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

#ifndef WS_CHART_SERIESRENDERER_H
#define WS_CHART_SERIESRENDERER_H

#include <QtCore>
#include <QtGui>
#include "timeseries.h"
#include <cfloat>

namespace webstella {
	namespace gui {

		class TimeBounds {
		Q_GADGET
			
		private:
			quint64 m_timeInterval;
			quint64 m_leftTime;
			quint64 m_rightTime;

		public:
			TimeBounds();
			TimeBounds(quint64 leftTime, quint64 rightTime);
			quint64 interval() const;
			quint64 left() const;
			quint64 right() const;
			void setBounds(quint64 leftTime, quint64 rightTime);
			void moveForward(quint64 time);
			void moveBack(quint64 time);

			Q_PROPERTY(quint64 left MEMBER m_leftTime)
			Q_PROPERTY(quint64 right MEMBER m_rightTime)
			Q_PROPERTY(quint64 interval MEMBER m_timeInterval)
		};


		class ValueBounds {
		Q_GADGET

		private:
			double m_maxVal;
			double m_minVal;
			double m_rangeVal;

		public:
			ValueBounds();
			ValueBounds(double min, double max);
			double range() const;
			double max() const;
			double min() const;
			void setBounds(double min, double max);
			void moveUp(double value);
			void moveDown(double value);

			Q_PROPERTY(quint64 range MEMBER m_rangeVal)
			Q_PROPERTY(quint64 max MEMBER m_maxVal)
			Q_PROPERTY(quint64 min MEMBER m_minVal)
		};


		class SeriesRenderer : public QObject {
		Q_OBJECT

		private:
			mutable QMutex m_timeMutex;
			mutable QMutex m_valueMutex;
			TimeBounds m_timeBounds;

		protected:
			QList<TimeSeries*>* m_basicSeries;
			QList<TimeSeries*>* m_extraSeries;
			QSize m_viewportDimension;
			bool m_autoRangeValue;
			bool m_autoRangeTime;
			bool m_tracking;
			double m_autoRangeIndentPercent;
			qint32 m_graphicAreaHeight;
			bool m_extraSeriesView;
			bool m_viewfinderVisible;
			double m_extraSeriesTransparentMultiplier;
			quint32 m_coarseMinPixels;
			quint32 m_coarseMaxPixels;
			ValueBounds defaultValueBounds(QList<TimeSeries *> *series);
			ValueBounds autoValueBounds(QList<TimeSeries *> *series, TimeBounds t);

		public:
			SeriesRenderer(QObject* parent = nullptr);
			SeriesRenderer(QList<TimeSeries*>* m_basicSeries, QList<TimeSeries*>* m_extraSeries = nullptr, QObject *parent = nullptr);

			bool isAutoRangeValue() const;
			void setAutoRangeValue(bool value);
			
			bool isAutoRangeTime() const;
			void setAutoRangeTime(bool value);
			
			double autoRangeIndentPercent() const;
			void setAutoRangeIndentPercent(double value);
			
			QList<TimeSeries *> *basicSeries() const;
			void setBasicSeries(QList<TimeSeries *> *value);
			
			QList<TimeSeries *> *extraSeries() const;
			void setExtraSeries(QList<TimeSeries *> *value);
			
			QSize viewportDimension() const;
			void setViewportDimension(const QSize &value);
			
			void setViewfinderShow(bool show);
			bool isViewfinderShow() const;
			
			double extraSeriesTransparentMultiplier() const;
			void setExtraSeriesTransparentMultiplier(double value);

			quint32 coarseMinPixels() const;
			void setCoarseMinPixels(quint32 value);
			
			quint32 coarseMaxPixels() const;
			void setCoarseMaxPixels(quint32 value);

			bool isExtraSeriesView() const;
			bool setExtraSeriesView(bool set);
			
			bool isTracking() const;
			void setTracking(bool value);

			TimeBounds completeTimeBounds() const;

			TimeBounds defaultTimeBounds() const;
			Q_INVOKABLE void setDefaultTimeBounds(quint64 leftTime, quint64 rightTime);
			Q_INVOKABLE virtual TimeBounds currentTimeBounds() const = 0;
			Q_INVOKABLE virtual ValueBounds currentValueBounds() const = 0;
			Q_INVOKABLE virtual void update() = 0;
			Q_INVOKABLE virtual void render(QPainter* p) = 0;
			Q_INVOKABLE virtual void mouseHoverEvent(const QPoint &pos) = 0;
			Q_INVOKABLE virtual void mousePressEvent(QMouseEvent* me) = 0;
			Q_INVOKABLE virtual void mouseReleaseEvent(QMouseEvent* me) = 0;
			Q_INVOKABLE virtual void zoomSelectionEvent(QMouseEvent* me, QRect selection, bool zooming) = 0;
			Q_INVOKABLE virtual void scrollEvent(QMouseEvent* me, QRect selection, bool scrolling) = 0;
			Q_INVOKABLE virtual void zoomEvent(QRect selection) = 0;
			Q_INVOKABLE virtual bool isScalingCanceled() = 0;
			Q_INVOKABLE virtual void scalingCancel() = 0;
			Q_INVOKABLE virtual void scalingDefaultEvent() = 0;
			Q_INVOKABLE virtual void scalingTimeValueEvent() = 0;
			Q_INVOKABLE virtual void scalingValueEvent() = 0;
			Q_INVOKABLE virtual void moveTimeForwardEvent(quint64 time) = 0;
			Q_INVOKABLE virtual void moveTimeBackEvent(quint64 time) = 0;
			Q_INVOKABLE virtual void setTimeBoundsEvent(quint64 leftTime, quint64 rightTime) = 0;
			Q_INVOKABLE virtual void moveValueUpEvent(double value) = 0;
			Q_INVOKABLE virtual void moveValueDownEvent(double value) = 0;
			Q_INVOKABLE virtual void setValueBoundsEvent(double min, double max) = 0;
			Q_INVOKABLE virtual void mouseWheelEvent(QWheelEvent *we) = 0;
			Q_INVOKABLE virtual void newPointsAddedEvent() = 0;
			Q_INVOKABLE qint32 graphAreaHeight() const;

			Q_PROPERTY(bool autoRangeValue READ isAutoRangeValue WRITE setAutoRangeValue NOTIFY autoRangeValueChanged)
			Q_PROPERTY(bool autoRangeTime READ isAutoRangeTime WRITE setAutoRangeTime NOTIFY autoRangeTimeChanged)
			Q_PROPERTY(double autoRangeIndentPercent READ autoRangeIndentPercent WRITE setAutoRangeIndentPercent NOTIFY autoRangeIndentPercentChanged)
			Q_PROPERTY(QList<TimeSeries *> * basicSeries READ basicSeries WRITE setBasicSeries NOTIFY basicSeriesChanged)
			Q_PROPERTY(QList<TimeSeries *> * extraSeries READ extraSeries WRITE setExtraSeries NOTIFY extraSeriesChanged)
			Q_PROPERTY(QSize viewportDimension READ viewportDimension WRITE setViewportDimension NOTIFY viewportDimensionChanged)
			Q_PROPERTY(bool viewfinderShow READ isViewfinderShow WRITE setViewfinderShow NOTIFY viewfinderShowChanged)
			Q_PROPERTY(double extraSeriesTransparentMultiplier READ extraSeriesTransparentMultiplier WRITE setExtraSeriesTransparentMultiplier NOTIFY extraSeriesTransparentMultiplierChanged)
			Q_PROPERTY(quint32 coarseMinPixels READ coarseMinPixels WRITE setCoarseMinPixels NOTIFY coarseMinPixelsChanged)
			Q_PROPERTY(quint32 coarseMaxPixels READ coarseMaxPixels WRITE setCoarseMaxPixels NOTIFY coarseMaxPixelsChanged)
			Q_PROPERTY(bool extraSeriesView READ isExtraSeriesView WRITE setExtraSeriesView NOTIFY extraSeriesViewChanged)
			Q_PROPERTY(bool tracking READ isTracking WRITE setTracking NOTIFY trackingChanged)

		signals:
			void autoRangeValueChanged(bool value);
			void autoRangeTimeChanged(bool value);
			void autoRangeIndentPercentChanged(double value);
			void basicSeriesChanged(QList<TimeSeries *> *value);
			void extraSeriesChanged(QList<TimeSeries *> *value);
			void viewportDimensionChanged(const QSize &value);
			void viewfinderShowChanged(bool value);
			void extraSeriesTransparentMultiplierChanged(double value);
			void coarseMinPixelsChanged(quint32 value);
			void coarseMaxPixelsChanged(quint32 value);
			void extraSeriesViewChanged(bool set);
			void trackingChanged(bool value);
			void scalingChanged(bool value);
		};
	}
}

#endif // WS_CHART_SERIESRENDERER_H
