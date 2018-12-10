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

#include "seriesrenderer.h"

using namespace webstella::gui;

TimeBounds::TimeBounds() :
	m_timeInterval(0),
	m_leftTime(0),
	m_rightTime(0)
{}

TimeBounds::TimeBounds(quint64 leftTime, quint64 rightTime) {
	setBounds(leftTime, rightTime);
}

quint64 TimeBounds::interval() const {
	return m_timeInterval;
}

quint64 TimeBounds::left() const {
	return m_leftTime;
}

quint64 TimeBounds::right() const {
	return m_rightTime;
}

void TimeBounds::setBounds(quint64 leftTime, quint64 rightTime) {
	m_leftTime = leftTime;
	m_rightTime = rightTime;
	m_timeInterval = rightTime - leftTime;
}

void TimeBounds::moveForward(quint64 time) {
	m_rightTime += time;
	if (m_rightTime < m_leftTime) {
		m_rightTime = static_cast<quint64>(18446744073709551615UL);
		m_leftTime = m_rightTime - m_timeInterval;
	} else {
		m_leftTime += time;
	}
}

void TimeBounds::moveBack(quint64 time) {
	m_leftTime -= time;
	m_rightTime -= time;
	if (m_leftTime > m_rightTime) {
		m_leftTime = 0;
		m_rightTime = m_leftTime + m_timeInterval;
	}
}

ValueBounds::ValueBounds() :
	m_maxVal(0.),
	m_minVal(0.),
	m_rangeVal(0.)
{}

ValueBounds::ValueBounds(double min, double max) {
	setBounds(max, min);
}

double ValueBounds::range() const {
	return m_rangeVal;
}

double ValueBounds::max() const {
	return m_maxVal;
}

double ValueBounds::min() const {
	return m_minVal;
}

void ValueBounds::setBounds(double min, double max) {
	m_maxVal = max;
	m_minVal = min;
	m_rangeVal = max - min;
}

void ValueBounds::moveUp(double value) {
	m_minVal += value;
	m_maxVal += value;
}

void ValueBounds::moveDown(double value) {
	m_minVal -= value;
	m_maxVal -= value;
}

SeriesRenderer::SeriesRenderer(QObject *parent) :
	SeriesRenderer(nullptr, nullptr, parent)
{}

SeriesRenderer::SeriesRenderer(QList<TimeSeries *> *basicSeries, QList<TimeSeries*>* extraSeries, QObject* parent) :
	QObject(parent),
	m_basicSeries(basicSeries),
	m_extraSeries(extraSeries),
	m_autoRangeValue(false),
	m_autoRangeTime(false),
	m_tracking(false),
	m_autoRangeIndentPercent(15.),
	m_extraSeriesView(false),
	m_viewfinderVisible(true),
	m_extraSeriesTransparentMultiplier(0.13),
	m_coarseMinPixels(5),
	m_coarseMaxPixels(15)
{}

TimeBounds SeriesRenderer::defaultTimeBounds() const {
	QMutexLocker ml(&m_timeMutex);
	return m_timeBounds;
}

void SeriesRenderer::setDefaultTimeBounds(quint64 leftTime, quint64 rightTime) {
	QMutexLocker ml(&m_timeMutex);
	m_timeBounds.setBounds(leftTime, rightTime);
}

bool SeriesRenderer::isAutoRangeValue() const {
	return m_autoRangeValue;
}

void SeriesRenderer::setAutoRangeValue(bool value) {
	if (m_autoRangeValue != value) {
		m_autoRangeValue = value;
		emit autoRangeValueChanged(value);
	}
}

bool SeriesRenderer::isAutoRangeTime() const {
	return m_autoRangeTime;
}

void SeriesRenderer::setAutoRangeTime(bool value) {
	if (m_autoRangeTime != value) {
		m_autoRangeTime = value;
		emit autoRangeTimeChanged(value);
	}
}

TimeBounds SeriesRenderer::completeTimeBounds() const {
	QMutexLocker ml(&m_timeMutex);
	TimeSeries* s;
	quint64 minT = 0, maxT = 0;
	quint64 exMinT = 0, exMaxT = 0;
	bool first = true;
	for (int i = 0; i < m_basicSeries->size(); i++) {
		s = m_basicSeries->at(i);
		if (s->size() > 0) {
			if (first) {
				minT = s->minTime();
				maxT = s->maxTime();
				first = false;
			} else {
				if (s->maxTime() > maxT) {
					maxT = s->maxTime();
				}
				if (s->minTime() < minT) {
					minT = s->minTime();
				}
			}
		}
	}
	first = true;
	for (int i = 0; i < m_extraSeries->size(); i++) {
		s = m_extraSeries->at(i);
		if (s->size() > 0) {
			if (first) {
				exMinT = s->minTime();
				exMaxT = s->maxTime();
			} else {
				if (s->maxTime() > exMaxT) {
					exMaxT = s->maxTime();
				}
				if (s->minTime() < exMinT) {
					exMinT = s->minTime();
				}
			}
		}
	}
	if (m_basicSeries->size() > 0 && m_extraSeries->size() > 0) {
		return TimeBounds((minT < exMinT)?minT:exMinT, (maxT > exMaxT)?maxT:exMaxT);
	} else if (m_basicSeries->size() > 0 && m_extraSeries->size() == 0) {
		return TimeBounds(minT, maxT);
	} else if (m_basicSeries->size() == 0 && m_extraSeries->size() > 0) {
		return TimeBounds(exMinT, exMaxT);
	} else {
		return TimeBounds();
	}
}

QList<TimeSeries *> *SeriesRenderer::basicSeries() const {
	return m_basicSeries;
}

void SeriesRenderer::setBasicSeries(QList<TimeSeries *> *value) {
	m_basicSeries = value;
	emit basicSeriesChanged(value);
}

QList<TimeSeries *> *SeriesRenderer::extraSeries() const {
	return m_extraSeries;
}

void SeriesRenderer::setExtraSeries(QList<TimeSeries *> *value) {
	m_extraSeries = value;
	emit extraSeriesChanged(value);
}

QSize SeriesRenderer::viewportDimension() const {
	return m_viewportDimension;
}

void SeriesRenderer::setViewportDimension(const QSize &value) {
	m_viewportDimension = value;
	emit viewportDimensionChanged(value);
}

bool SeriesRenderer::isTracking() const {
	return m_tracking;
}

void SeriesRenderer::setTracking(bool value) {
	m_tracking = value;
	emit trackingChanged(value);
}

ValueBounds SeriesRenderer::defaultValueBounds(QList<TimeSeries*>* series) {
	ValueBounds res;
	double minY = DBL_MAX, maxY = - DBL_MAX;
	TimeSeries* s;
	for (int i = 0; i < series->size(); i++) {
		s = series->at(i);
		if (s->notation() == TimeSeries::Notation::LINES || s->notation() == TimeSeries::Notation::STEPS) {
			if (s->dataType() == TimeValue::Type::INT || s->dataType() == TimeValue::Type::DOUBLE) {
				if (s->minBoundValue() < minY) {
					minY = s->minBoundValue();
				}
				if (s->maxBoundValue() > maxY) {
					maxY = s->maxBoundValue();
				}
			}
		}
	}
	if (maxY == - DBL_MAX || minY == DBL_MAX) {
		res.setBounds(0., 0.);
	} else {
		res.setBounds(minY, maxY);
	}
	if (res.max() == res.min()) {
		if (res.max() < 1E-8) {
			res.setBounds(-1E-7, 1E-7);
		} else {
			res.setBounds(res.min() * 0.9, res.max() * 1.1);
		}
	}
	return res;
}

ValueBounds SeriesRenderer::autoValueBounds(QList<TimeSeries*>* series, TimeBounds t) {
	ValueBounds res;
	double minY = DBL_MAX, maxY = - DBL_MAX;
	TimeSeries* s;
	QList<TimeValue*> extr;
	for (int i = 0; i < series->size(); i++) {
		s = series->at(i);
		if (s->size() > 0 && (s->notation() == TimeSeries::Notation::LINES || s->notation() == TimeSeries::Notation::STEPS)) {
			if (s->dataType() == TimeValue::Type::INT || s->dataType() == TimeValue::Type::DOUBLE) {
				extr = s->minValues(t.left(), t.right());
				if (extr.size() > 0) {
					if (extr.first()->doubleValue() < minY) {
						minY = extr.first()->doubleValue();
					}
				}
				extr = s->maxValues(t.left(), t.right());
				if (extr.size() > 0) {
					if (extr.first()->doubleValue() > maxY) {
						maxY = extr.first()->doubleValue();
					}
				}
			}
		}
	}
	if (maxY == - DBL_MAX || minY == DBL_MAX) {
		res.setBounds(0., 0.);
	} else {
		double diff = (maxY - minY) * m_autoRangeIndentPercent / 100.;
		maxY += diff;
		minY -= diff;
		res.setBounds(minY, maxY);
	}
	if (res.max() == res.min()) {
		if (res.max() < 0.) {
			if (res.max() > -1E-8) {
				res.setBounds(-1E-7, 1E-7);
			} else {
				res.setBounds(res.min() * 1.1, res.max() * 0.9);
			}
		} else {
			if (res.max() < 1E-8) {
				res.setBounds(-1E-7, 1E-7);
			} else {
				res.setBounds(res.min() * 0.9, res.max() * 1.1);
			}
		}
	}
	return res;
}

double SeriesRenderer::extraSeriesTransparentMultiplier() const {
	return m_extraSeriesTransparentMultiplier;
}

void SeriesRenderer::setExtraSeriesTransparentMultiplier(double value) {
	if (value < 0.01 || value > 0.9) {
		m_extraSeriesTransparentMultiplier = value;
		emit extraSeriesTransparentMultiplierChanged(value);
	}
}

double SeriesRenderer::autoRangeIndentPercent() const {
	return m_autoRangeIndentPercent;
}

void SeriesRenderer::setAutoRangeIndentPercent(double value) {
	m_autoRangeIndentPercent = value;
	autoRangeIndentPercentChanged(value);
}

qint32 SeriesRenderer::graphAreaHeight() const {
	return m_graphicAreaHeight;
}

void SeriesRenderer::setViewfinderShow(bool show) {
	m_viewfinderVisible = show;
	emit viewfinderShowChanged(show);
}

bool SeriesRenderer::isViewfinderShow() const {
	return m_viewfinderVisible;
}

quint32 SeriesRenderer::coarseMaxPixels() const {
	return m_coarseMaxPixels;
}

void SeriesRenderer::setCoarseMaxPixels(quint32 value) {
	m_coarseMaxPixels = value;
	emit coarseMaxPixelsChanged(value);
}

quint32 SeriesRenderer::coarseMinPixels() const {
	return m_coarseMinPixels;
}

void SeriesRenderer::setCoarseMinPixels(quint32 value) {
	m_coarseMinPixels = value;
	emit coarseMinPixelsChanged(value);
}

bool SeriesRenderer::isExtraSeriesView() const {
	return m_extraSeriesView;
}

bool SeriesRenderer::setExtraSeriesView(bool set) {
	if (set) {
		for (int i = 0; i < m_extraSeries->size(); i++) {
			if ((m_extraSeries->at(i)->dataType() == TimeValue::Type::DOUBLE || m_extraSeries->at(i)->dataType() == TimeValue::Type::INT)
				&& (m_extraSeries->at(i)->notation() == TimeSeries::Notation::LINES || m_extraSeries->at(i)->notation() == TimeSeries::Notation::STEPS)) {
				m_extraSeriesView = true;
				emit extraSeriesViewChanged(set);
				return true;
			}
		}
	} else {
		m_extraSeriesView = false;
		emit extraSeriesViewChanged(set);
		return true;
	}
	return false;
}
