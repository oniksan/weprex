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

#include "timechart.h"

using namespace webstella::gui;

TimeChart::TimeChart(QQuickItem *parent) :
	QQuickPaintedItem(parent),
	m_autoUpdateTime(50),
	m_leftTime(0),
	m_rightTime(0),
	m_zoomCounter(0),
	m_zoomCounterBase(5),
	m_leftMousePressed(false),
	m_rightMousePressed(false),
	m_middleMousePressed(false),
	m_zoomLock(false),
	m_mouseMoveEventDisable(false),
	m_lastTrackingRegime(TrackingRegime::UNDEFINED)
{
	m_render = new DefaultSeriesRenderer(&m_basicSeries, &m_extraSeries);
	connect(&m_zoomTimer, &QTimer::timeout, this, &TimeChart::zoomSlot);
	m_zoomTimer.setInterval(50);
	m_zoomTimer.setSingleShot(true);
	connect(&m_autoUpdateTimer, &QTimer::timeout, this, &TimeChart::autoUpdateSlot);
	m_autoUpdateTimer.start(m_autoUpdateTime);
	setAcceptedMouseButtons(Qt::AllButtons);
	setAcceptHoverEvents(true);
	connect(m_render, &SeriesRenderer::trackingChanged, this, [=]() {trackingRegimeMonitor();});
	connect(m_render, &SeriesRenderer::scalingChanged, this, [=]() {trackingRegimeMonitor();});
	setTrackingRegime(TrackingRegime::TRACKING_AND_SCALING_ALL);
}

TimeChart::~TimeChart() {
	delete m_render;
}

void TimeChart::paint(QPainter *painter) {
	m_render->render(painter);
	// Гарантированная проририсовка последнего кадра при зуммировании
	if (m_zoomCounter == 0) {
		m_mouseMoveEventDisable = false;
	}
}

SeriesRenderer* TimeChart::render() const {
	return m_render;
}

int TimeChart::zoomCounterBase() const {
	return m_zoomCounterBase;
}

void TimeChart::setZoomCounterBase(int value) {
	m_zoomCounterBase = value;
	emit zoomCounterBaseChanged(value);
}

int TimeChart::autoUpdateTime() const {
	return m_autoUpdateTime;
}

void TimeChart::setAutoUpdateTime(int value) {
	if (value <= 0) {
		m_autoUpdateTimer.stop();
		m_autoUpdateTime = 0;
	} else {
		m_autoUpdateTime = value;
		m_autoUpdateTimer.start(m_autoUpdateTime);
	}
	emit autoUpdateTimeChanged(value);
}

void TimeChart::trackingRegimeMonitor() {
	TrackingRegime r = trackingRegime();
	if (r != m_lastTrackingRegime) {
		m_lastTrackingRegime = r;
		emit trackingRegimeChanged();
	}
}

TimeChart::TrackingRegime TimeChart::trackingRegime() {
	/*if (m_render->isScalingCanceled()) {

	}*/
	if (m_render->isTracking()) {
		if (m_render->isAutoRangeValue()) {
			if (!m_render->isAutoRangeTime()) {
				return TrackingRegime::TRACKING_AND_SCALING_VALUE;
			}
		} else {
			if (!m_render->isAutoRangeTime()) {
				return TrackingRegime::TRACKING_WITHOUT_SCALING;
			}
		}
	} else {
		if (m_render->isAutoRangeValue()) {
			if (m_render->isAutoRangeTime()) {
				if (m_render->isScalingCanceled()) {
					return TrackingRegime::TRACKING_AND_SCALING_ALL;
				}
			}
		} else {
			if (!m_render->isAutoRangeTime()) {
				return TrackingRegime::NO_TRACKING_NO_SCALING;
			}
		}
	}
	return TrackingRegime::UNDEFINED;
}

void TimeChart::setTrackingRegime(TrackingRegime regime) {
	if (regime == TrackingRegime::NO_TRACKING_NO_SCALING) {
		m_render->setAutoRangeTime(false);
		m_render->setAutoRangeValue(false);
		m_render->setTracking(false);
		m_render->scalingCancel();
		trackingRegimeMonitor();
	} else if (regime == TrackingRegime::TRACKING_AND_SCALING_ALL) {
		m_render->setAutoRangeTime(true);
		m_render->setAutoRangeValue(true);
		m_render->setTracking(false);
		m_render->scalingCancel();
		trackingRegimeMonitor();
	} else if (regime == TrackingRegime::TRACKING_AND_SCALING_VALUE) {
		m_render->setAutoRangeTime(false);
		m_render->setAutoRangeValue(true);
		m_render->setTracking(true);
		trackingRegimeMonitor();
	} else if (regime == TrackingRegime::TRACKING_WITHOUT_SCALING) {
		m_render->setAutoRangeTime(false);
		m_render->setAutoRangeValue(false);
		m_render->setTracking(true);
		trackingRegimeMonitor();
	}
	m_render->update();
	update();
}

void TimeChart::setExtraSeriesView(bool value) {
	m_render->setExtraSeriesView(value);
	emit extraSeriesViewChanged(value);
}

bool TimeChart::isExtraSeriesView() const {
	return m_render->isExtraSeriesView();
}

void TimeChart::mouseMoveEvent(QMouseEvent* me) {
	if (!m_mouseMoveEventDisable) {
		if ((me->buttons() & Qt::LeftButton) && m_leftMousePressed) {
			m_selection.setWidth(me->pos().x() - m_selection.x());
			m_selection.setHeight(me->pos().y() - m_selection.y());
			m_render->zoomSelectionEvent(me, m_selection, true);
			m_render->mouseHoverEvent(me->pos());
		} else if ((me->buttons() & Qt::RightButton) && m_rightMousePressed) {
			m_selection.setWidth(me->pos().x() - m_selection.x());
			m_selection.setHeight(me->pos().y() - m_selection.y());
			double timeMult = 0.;
			if (m_render->viewportDimension().width() > 0) {
				timeMult = static_cast<double>(m_render->currentTimeBounds().interval()) / m_render->viewportDimension().width();
			}
			if (m_selection.width() > 0) {
				m_render->moveTimeBackEvent(static_cast<quint64>(m_selection.width() * timeMult));
			} else if (m_selection.width() < 0) {
				m_render->moveTimeForwardEvent(static_cast<quint64>(-1 * m_selection.width() * timeMult));
			}
			double valueMult = 0.;
			if (m_render->viewportDimension().height() > 0) {
				valueMult = m_render->currentValueBounds().range() / m_render->viewportDimension().height();
			}
			if (m_selection.height() > 0) {
				m_render->moveValueUpEvent(m_selection.height() * valueMult);
			} else if (m_selection.height() < 0) {
				m_render->moveValueDownEvent(-1 * m_selection.height() * valueMult);
			}
			m_render->scrollEvent(me, m_selection, true);
			m_selection.setRect(me->pos().x(), me->pos().y(), 0, 0);
		}

		update();
	}
}

void TimeChart::hoverMoveEvent(QHoverEvent* he) {
	if (!m_mouseMoveEventDisable) {
		if (he->oldPos() != he->pos()) {
			m_render->mouseHoverEvent(he->pos());
			update();
		}
	}
}

void TimeChart::mousePressEvent(QMouseEvent* me) {
	if (!m_rightMousePressed && !m_leftMousePressed) {
		if (me->buttons() & Qt::LeftButton) {
			m_selection.setRect(me->pos().x(), me->pos().y(), 0, 0);
			m_leftMousePressed = true;
			m_render->setAutoRangeTime(false);
			m_render->setAutoRangeValue(false);
			m_render->zoomSelectionEvent(me, m_selection, true);
		} else if (me->buttons() & Qt::RightButton) {
			m_selection.setRect(me->pos().x(), me->pos().y(), 0, 0);
			m_rightMousePressed = true;
			setCursor(Qt::SizeAllCursor);
			m_render->scrollEvent(me, m_selection, true);
		} else if (me->buttons() & Qt::MiddleButton) {
			m_middleMousePressed = true;
		}
		m_render->mousePressEvent(me);
		//update();
	}
}

void TimeChart::scalingDefault() {
	m_render->setAutoRangeTime(false);
	m_render->setAutoRangeValue(false);
	m_render->scalingDefaultEvent();
	m_render->update();
	update();
}

void TimeChart::scalingTimeValue() {
	m_render->setAutoRangeTime(false);
	m_render->setAutoRangeValue(false);
	m_render->scalingTimeValueEvent();
	m_render->update();
	update();
}

void TimeChart::scalingValue() {
	m_render->setAutoRangeTime(false);
	m_render->setAutoRangeValue(false);
	m_render->scalingValueEvent();
	m_render->update();
	update();
}

void TimeChart::mouseReleaseEvent(QMouseEvent* me) {
	bool needUpdate = false;
	unsetCursor();
	if ((me->button() & Qt::LeftButton) && m_leftMousePressed) {
		m_render->zoomSelectionEvent(me, m_selection, false);
		if (m_zoomCounter == 0 && m_selection.width() != 0 && m_selection.height() != 0) {
			m_zoomCounter = m_zoomCounterBase;
			if (m_selection.width() > 0) {
				m_savedSelection.setX(m_selection.x());
				m_savedSelection.setWidth(m_selection.width());
			} else {
				m_savedSelection.setX(m_selection.x() + m_selection.width());
				m_savedSelection.setWidth(m_selection.width() * -1);
			}
			if (m_selection.height() > 0) {
				m_savedSelection.setY(m_selection.y());
				m_savedSelection.setHeight(m_selection.height());
			} else {
				m_savedSelection.setY(m_selection.y() + m_selection.height());
				m_savedSelection.setHeight(m_selection.height() * -1);
			}
			m_mouseMoveEventDisable = true;
			m_zoomTimer.start();
		}
		m_selection.setRect(0, 0, 0, 0);
		m_leftMousePressed = false;
	} else if ((me->button() & Qt::RightButton) && m_rightMousePressed) {
		m_render->scrollEvent(me, m_selection, false);
		if (m_selection.width() != 0 || m_selection.height() != 0) {
			needUpdate = true;
		}
		m_selection.setRect(0, 0, 0, 0);
		m_rightMousePressed = false;
	} else if ((me->button() & Qt::MiddleButton) && m_middleMousePressed) {
		scalingTimeValue();
		m_middleMousePressed = false;
		needUpdate = true;
	}
	m_render->mouseReleaseEvent(me);
	if (needUpdate) {
		update();
	}
}

/*void TimeChart::resizeEvent(QResizeEvent *re) {
	render->setViewportDimension(re->size());
}*/

void TimeChart::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) {
	QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
	QSize s(static_cast<int>(newGeometry.width()), static_cast<int>(newGeometry.height()));
	m_render->setViewportDimension(s);
}

bool TimeChart::setTimeAxisRange(quint64 leftTime, quint64 rightTime) {
	if ((rightTime - leftTime) > 100) {
		this->m_leftTime = leftTime;
		this->m_rightTime = rightTime;
		m_render->setDefaultTimeBounds(leftTime, rightTime);
		update();
		return true;
	}
	return false;
}

quint64 TimeChart::leftTime() const {
	return m_leftTime;
}

quint64 TimeChart::rightTime() const {
	return m_rightTime;
}

void TimeChart::addBasicTimeSeries(TimeSeries* series) {
	m_basicSeries.append(series);
	connect(series, &TimeSeries::bitsCountChanged, m_render, &SeriesRenderer::update);
	connect(series, &TimeSeries::notationChanged, m_render, &SeriesRenderer::update);
	m_render->update();
	update();
}

void TimeChart::addExtraTimeSeries(TimeSeries* series) {
	m_extraSeries.append(series);
	connect(series, &TimeSeries::bitsCountChanged, m_render, &SeriesRenderer::update);
	connect(series, &TimeSeries::notationChanged, m_render, &SeriesRenderer::update);
	m_render->update();
	update();
}

void TimeChart::zoomSlot() {
	if (m_zoomCounter > 0) {
		m_zoomLock = true;
		QRect r = QRect(m_savedSelection.x() / m_zoomCounter,
						m_savedSelection.y() / m_zoomCounter,
						m_render->viewportDimension().width() + (m_savedSelection.width() - m_render->viewportDimension().width()) / m_zoomCounter,
						m_render->graphAreaHeight() + (m_savedSelection.height() - m_render->graphAreaHeight()) / m_zoomCounter
						);
		m_render->zoomEvent(r);
		QRect newR(((m_savedSelection.x() - r.x()) * m_render->viewportDimension().width()) / r.width(),
				   ((m_savedSelection.y() - r.y()) * m_render->graphAreaHeight()) / r.height(),
				   (m_savedSelection.width() * m_render->viewportDimension().width()) / r.width(),
				   (m_savedSelection.height() * m_render->graphAreaHeight()) / r.height()
				   );
		m_savedSelection = newR;
		m_zoomCounter--;
		update();
		m_zoomTimer.start();
	} else {
		m_zoomLock = false;
	}
}

void TimeChart::autoUpdateSlot() {
	if (!m_zoomLock) {
		for (int i = 0; i < m_basicSeries.size(); i++) {
			if (m_basicSeries.at(i)->needRepaint()) {
				m_render->newPointsAddedEvent();
				update();
				m_basicSeries.at(i)->wasRepainted();
			}
		}
		for (int i = 0; i < m_extraSeries.size(); i++) {
			if (m_extraSeries.at(i)->needRepaint()) {
				m_render->newPointsAddedEvent();
				update();
				m_extraSeries.at(i)->wasRepainted();
			}
		}
	}
}

void TimeChart::wheelEvent(QWheelEvent*) {
	setExtraSeriesView(!isExtraSeriesView());
}

bool TimeChart::removeBasicSeries(const QString &name) {
	TimeSeries *s;
	bool res = false;
	for (int i = 0; i < m_basicSeries.size(); i++) {
		s = m_basicSeries.at(i);
		if (s->name() == name) {
			m_basicSeries.removeAt(i);
			delete s;
			res = true;
		}
	}
	if (res) {
		m_render->update();
		update();
	}
	return res;
}

bool TimeChart::removeExtraSeries(const QString &name) {
	TimeSeries *s;
	bool res = false;
	for (int i = 0; i < m_extraSeries.size(); i++) {
		s = m_extraSeries.at(i);
		if (s->name() == name) {
			m_extraSeries.removeAt(i);
			delete s;
			res = true;
		}
	}
	if (res) {
		m_render->update();
		update();
	}
	return res;
}

bool TimeChart::removeExtraSeries(int index) {
	TimeSeries *s;
	bool res = false;
	if (index >= 0 && index < m_extraSeries.size()) {
		s = m_extraSeries.at(index);
		m_extraSeries.removeAt(index);
		delete s;
		res = true;
	}
	if (res) {
		m_render->update();
		update();
	}
	return res;
}

bool TimeChart::removeBasicSeries(int index) {
	TimeSeries *s;
	bool res = false;
	if (index >= 0 && index < m_basicSeries.size()) {
		s = m_basicSeries.at(index);
		m_basicSeries.removeAt(index);
		delete s;
		res = true;
	}
	if (res) {
		m_render->update();
		update();
	}
	return res;
}

quint32 TimeChart::basicSeriesCount() const {
	return static_cast<quint32>(m_basicSeries.size());
}

quint32 TimeChart::extraSeriesCount() const {
	return static_cast<quint32>(m_extraSeries.size());
}

void TimeChart::removeAllSerieses() {
	TimeSeries *s;
	for (int i = 0; i < m_extraSeries.size(); i++) {
		s = m_extraSeries.at(i);
		delete s;
	}
	m_extraSeries.clear();
	for (int i = 0; i < m_basicSeries.size(); i++) {
		s = m_basicSeries.at(i);
		delete s;
	}
	m_basicSeries.clear();
	m_render->update();
	update();
}

QColor TimeChart::predefinedColor(quint16 index) {
	QColor res;
	switch (index) {
		case 0:
			res = QColor(70, 190, 230);
			break;
		case 1:
			res = QColor(110, 210, 40);
			break;
		case 2:
			res = QColor(220, 160, 140);
			break;
		case 3:
			res = QColor(160, 170, 190);
			break;
		case 4:
			res = QColor(170, 130, 190);
			break;
		case 5:
			res = QColor(133, 120, 60);
			break;
		case 6:
			res = QColor(60, 80, 220);
			break;
		case 7:
			res = QColor(40, 130, 30);
			break;
		case 8:
			res = QColor(140, 70, 50);
			break;
		case 9:
			res = QColor(40, 40, 40);
			break;
		case 10:
			res = QColor(160, 40, 160);
			break;
		case 11:
			res = QColor(220, 120, 10);
			break;
		default:
			res = QColor(100, 100, 100);
	}
	return res;
}

TimeSeries* TimeChart::addBasicSeries(const QString& name) {
	auto *s = new TimeSeries(name, this);
	addBasicTimeSeries(s);
	return s;
}

TimeSeries* TimeChart::addExtraSeries(const QString& name) {
	auto *s = new TimeSeries(name, this);
	addExtraTimeSeries(s);
	return s;
}

TimeSeries* TimeChart::getBasicSeries(const QString& name) {
	TimeSeries* s;
	for (int i = 0; i < m_basicSeries.size(); i++) {
		s = m_basicSeries.at(i);
		if (s->name() == name) {
			return s;
		}
	}
	return nullptr;
}

TimeSeries* TimeChart::getExtraSeries(const QString& name) {
	TimeSeries* s;
	for (int i = 0; i < m_extraSeries.size(); i++) {
		s = m_extraSeries.at(i);
		if (s->name() == name) {
			return s;
		}
	}
	return nullptr;
}

void TimeChart::repaint() {
	m_render->update();
	update();
}

TimeBounds TimeChart::currentTimeBounds() {
	return m_render->currentTimeBounds();
}

ValueBounds TimeChart::currentValueBounds() {
	return m_render->currentValueBounds();
}

void TimeChart::moveTimeForward(quint64 time) {
	m_render->moveTimeForwardEvent(time);
	m_render->update();
	update();
	trackingRegimeMonitor();
}

void TimeChart::moveTimeBack(quint64 time) {
	m_render->moveTimeBackEvent(time);
	m_render->update();
	update();
	trackingRegimeMonitor();
}

void TimeChart::moveValueUp(double value) {
	m_render->moveValueUpEvent(value);
	m_render->update();
	update();
	trackingRegimeMonitor();
}

void TimeChart::moveValueDown(double value) {
	m_render->moveValueDownEvent(value);
	m_render->update();
	update();
	trackingRegimeMonitor();
}

void TimeChart::setTimeBounds(quint64 leftTime, quint64 rightTime) {
	m_render->setAutoRangeTime(false);
	m_render->setAutoRangeValue(false);
	if (rightTime - leftTime >= 1000) {
		m_render->setTimeBoundsEvent(leftTime, rightTime);
		m_render->update();
		update();
	}
	trackingRegimeMonitor();
}

void TimeChart::setValueBounds(double min, double max) {
	m_render->setAutoRangeTime(false);
	m_render->setAutoRangeValue(false);
	m_render->setValueBoundsEvent(min, max);
	m_render->update();
	update();
	trackingRegimeMonitor();
}

void TimeChart::scaleTimeBounds(double mult) {
	if (mult > 0) {
		TimeBounds cur = currentTimeBounds();
		quint64 offset;
		quint64 newInterval = static_cast<quint64>(cur.interval() * mult);
		if (mult > 1.) {
			offset = (newInterval - cur.interval()) / 2;
			if (cur.left() <= offset) {
				setTimeBounds(0, cur.right() + offset);
			} else {
				setTimeBounds(cur.left() - offset, cur.right() + offset);
			}
		} else if (mult < 1.) {
			offset = (cur.interval() - newInterval) / 2;
			setTimeBounds(cur.left() + offset, cur.right() - offset);
		}
		trackingRegimeMonitor();
	}
}

void TimeChart::scaleValueBounds(double mult) {
	if (mult > 0) {
		ValueBounds cur = currentValueBounds();
		double offset;
		double newRange = cur.range() * mult;
		if (mult > 1.) {
			offset = (newRange - cur.range()) / 2;
			setValueBounds(cur.min() - offset, cur.max() + offset);
		} else if (mult < 1.) {
			offset = (cur.range() - newRange) / 2;
			setValueBounds(cur.min() + offset, cur.max() - offset);
		}
		trackingRegimeMonitor();
	}
}
