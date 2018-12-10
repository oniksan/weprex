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

#include "defaultseriesrenderer.h"

using namespace webstella::gui;

DefaultSeriesRenderer::DefaultSeriesRenderer(QList<TimeSeries *> *basicSeries, QList<TimeSeries *> *extraSeries, QObject *parent) :
	SeriesRenderer(basicSeries, extraSeries, parent),
	m_wasPointsAddEvent(false),
	m_firstMouseEvent(false),
	m_boolViewportHeight(20),
	m_boolViewportsCount(0),
	m_bitViewportsCount(0),
	m_commonViewportsCount(0),
	m_oneBitHeight(8),
	m_oneBitIndent(3),
	m_gridColor(QColor(122, 122, 122)),
	m_gridStyle(Qt::SolidLine),
	m_gridWidth(1),
	m_labelsColor(QColor(200, 200, 200)),
	m_labelsBackgroundColor(QColor(60, 60, 60)),
	m_backgroundColor(QColor(48, 48, 48)),
	m_labelsFont(QFont("Open Sans", 10, QFont::Normal)),
	//autoRangeY(false),
	m_leftIndent(5),
	m_verticalIndent(20),
	m_viewfinderWindowBorderColor(QColor(240, 240, 240, 150)),
	m_viewfinderWindowBackgroundColor(QColor(20, 20, 20, 150)),
	m_viewfinderWindowTextColor(QColor(255, 255, 255)),
	m_viewfinderWindowFont(QFont("Open Sans", 10, QFont::Normal)),
	m_selectionBorderColor(QColor(200, 200, 200, 150)),
	m_selectionBorderStyle(Qt::SolidLine),
	m_selectionBorderWidth(1),
	m_selectionBackgroundColor(QColor(255, 130, 70, 100)),
	m_selectionBackgroundStyle(Qt::SolidPattern),
	m_maxPointerBackgroundColor(QColor(250, 50, 50, 130)),
	m_maxPointerBorderColor(QColor(255, 255, 255, 255)),
	m_minPointerBackgroundColor(QColor(50, 50, 250, 130)),
	m_minPointerBorderColor(QColor(255, 255, 255, 255)),
	m_antialiasing(true),
	m_bufferedBasicYK(0),
	m_bufferedXK(0),
	m_overlayRenderEvent(false),
	m_mouseZooming(false),
	m_mouseScrolling(false),
	m_scaling(false)
{
	QFontMetrics fm(m_labelsFont);
	m_minGridXPixels = static_cast<quint32>(fm.width("00.00.00 00:00:00.000") + 2);
	m_minGridYPixels = static_cast<quint32>(fm.height() + 15);
	m_oneBitViewportHeight = m_oneBitHeight + m_oneBitIndent;
	m_bitsFont = QFont("Open Sans", static_cast<int>(m_oneBitViewportHeight - 3), QFont::Normal);
	m_selection.setRect(0, 0, 0, 0);
}

void DefaultSeriesRenderer::calcAnalogHeight() {
	quint32 verticalIndentsCount = 0;
	if (m_boolViewportsCount > 0) {
		verticalIndentsCount += m_boolViewportsCount;
	}
	if (m_bitViewportsCount > 0) {
		verticalIndentsCount += m_bitViewportsCount;
	}
	if (m_commonViewportsCount > 0) {
		verticalIndentsCount++;
	}
	if (verticalIndentsCount > 0) {
		verticalIndentsCount--;
	}
	QFontMetrics fm(m_labelsFont);
	quint32 timeLine = static_cast<quint32>(fm.height() + m_verticalIndent);
	m_graphicAreaHeight = m_viewportDimension.height() - static_cast<qint32>(m_boolViewportHeight * m_boolViewportsCount + m_oneBitViewportHeight * m_allBitsCount + verticalIndentsCount * m_verticalIndent + timeLine);
}

bool DefaultSeriesRenderer::isAntialiasing() const {
	return m_antialiasing;
}

void DefaultSeriesRenderer::setAntialiasing(bool value) {
	m_antialiasing = value;
	emit antialiasingChanged(value);
}

void DefaultSeriesRenderer::update() {
	m_boolViewportsCount = 0;
	m_bitViewportsCount = 0;
	m_allBitsCount = 0;
	m_commonViewportsCount = 0;
	TimeSeries* s;
	for (int i = 0; i < m_basicSeries->size(); i++) {
		s = m_basicSeries->at(i);
		if (s->dataType() == TimeValue::Type::BOOL) {
			m_boolViewportsCount++;
		} else if (s->notation() == TimeSeries::Notation::BITS) {
			m_bitViewportsCount++;
			m_allBitsCount += s->bitsCount();
		} else if (s->notation() == TimeSeries::Notation::LINES || s->notation() == TimeSeries::Notation::STEPS) {
			m_commonViewportsCount++;
		}
	}
}

QColor DefaultSeriesRenderer::minPointerBorderColor() const {
	return m_minPointerBorderColor;
}

void DefaultSeriesRenderer::setMinPointerBorderColor(const QColor &value) {
	m_minPointerBorderColor = value;
	emit minPointerBorderColorChanged(value);
}

QColor DefaultSeriesRenderer::minPointerBackgroundColor() const {
	return m_minPointerBackgroundColor;
}

void DefaultSeriesRenderer::setMinPointerBackgroundColor(const QColor &value) {
	m_minPointerBackgroundColor = value;
	emit minPointerBackgroundColorChanged(value);
}

QColor DefaultSeriesRenderer::maxPointerBorderColor() const {
	return m_maxPointerBorderColor;
}

void DefaultSeriesRenderer::setMaxPointerBorderColor(const QColor &value) {
	m_maxPointerBorderColor = value;
	emit maxPointerBorderColorChanged(value);
}

QColor DefaultSeriesRenderer::maxPointerBackgroundColor() const {
	return m_maxPointerBackgroundColor;
}

void DefaultSeriesRenderer::setMaxPointerBackgroundColor(const QColor &value) {
	m_maxPointerBackgroundColor = value;
	emit maxPointerBackgroundColorChanged(value);
}

void DefaultSeriesRenderer::mouseEventRender(QPainter *p) {
	// Перерисовка буферного изображения
	p->drawPixmap(0, 0, m_bufferedImage);
	// Прорисовка области выделения
	if (m_mouseZooming && m_selection.width() != 0) {
		// Область выделения
		QPixmap pmSelection(m_viewportDimension.width(), m_viewportDimension.height());
		pmSelection.fill(Qt::transparent);
		QPainter bufSelection(&pmSelection);
		QPen selectionPen(m_selectionBorderStyle);
		selectionPen.setWidth(m_selectionBorderWidth);
		selectionPen.setColor(m_selectionBorderColor);
		QBrush selectionBrush(m_selectionBackgroundColor, m_selectionBackgroundStyle);
		bufSelection.setPen(selectionPen);
		bufSelection.setBrush(selectionBrush);
		bufSelection.drawRect(m_selection);
		p->drawPixmap(0, 0, pmSelection);
	} else {
		// Прорисовка визира
		if (m_viewfinderVisible) {
			viewfinderRender(p);
		}
	}
}

void DefaultSeriesRenderer::setScaling(bool value) {
	if (m_scaling != value) {
		m_scaling = value;
		emit scalingChanged(m_scaling);
	}
}

void DefaultSeriesRenderer::render(QPainter *p) {
	if (!m_scaling) {
		if (isAutoRangeTime()) {
			m_bufferedTimeBounds = completeTimeBounds();
		}
		if (isAutoRangeValue()) {
			m_bufferedBasicValueBounds = autoValueBounds(m_basicSeries, m_bufferedTimeBounds);
			m_bufferedExtraValueBounds = autoValueBounds(m_extraSeries, m_bufferedTimeBounds);
		}
	}
	// Расчет масштабирующего коэффициента по оси 0x
	if (m_bufferedTimeBounds.right() <= m_bufferedTimeBounds.left()) {
		m_bufferedXK = 0.;
	} else {
		m_bufferedXK = m_viewportDimension.width() / (1. * m_bufferedTimeBounds.interval());
	}
	calcAnalogHeight();
	// Расчет масштабирующего коэффициента по оси 0y
	if (m_bufferedBasicValueBounds.max() <= m_bufferedBasicValueBounds.min()) {
		m_bufferedBasicYK = 0.;
	} else {
		m_bufferedBasicYK = m_graphicAreaHeight / m_bufferedBasicValueBounds.range();
	}
	if (m_bufferedExtraValueBounds.max() <= m_bufferedExtraValueBounds.min()) {
		m_bufferedExtraYK = 0.;
	} else {
		m_bufferedExtraYK = m_graphicAreaHeight / m_bufferedExtraValueBounds.range();
	}

	// Первоначальная прорисовка
	if (m_bufferedImage.width() == 0) {
		m_overlayRenderEvent = false;
	}
	// Была добавлена точка
	if (m_wasPointsAddEvent) {
		m_overlayRenderEvent = false;
	}

	if (m_overlayRenderEvent) {
		mouseEventRender(p);
		// Сброс флага перемещения мыши
		m_overlayRenderEvent = false;
	} else {
		m_bufferedImage = QPixmap(m_viewportDimension.width(), m_viewportDimension.height());
		QPainter bufferedPainter(&m_bufferedImage);
		QBrush backBrush(m_backgroundColor);
		// Заливка фона
		bufferedPainter.fillRect(0, 0, m_viewportDimension.width(), m_viewportDimension.height(), backBrush);
		// Прорисовка сетки оси 0x
		QPixmap pmGrid(m_viewportDimension.width(), m_viewportDimension.height());
		pmGrid.fill(Qt::transparent);
		QPainter bufGrid(&pmGrid);
		if (m_antialiasing) {
			bufGrid.setRenderHint(QPainter::TextAntialiasing);
		}
		gridXRenderer(&bufGrid);
		bufferedPainter.drawPixmap(0, 0, pmGrid);


		// Прорисовка графиков
		int verticalShift = 0;
		if (m_viewportDimension.width() > 0) {
			// Прорисовка аналоговых графиков
			if (m_graphicAreaHeight > 0) {
				QPixmap pm(m_viewportDimension.width(), m_graphicAreaHeight);
				pm.fill(Qt::transparent);
				QPainter buf(&pm);
				if (m_antialiasing) {
					buf.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
				}
				analogRenderer(&buf);
				bufferedPainter.drawPixmap(0, 0, pm);
				verticalShift = m_graphicAreaHeight + m_verticalIndent;
			}

			TimeSeries* s;
			// Прорисовка битовых графиков
			for (int i = 0; i < m_basicSeries->size(); i++) {
				s = m_basicSeries->at(i);
				if (s->notation() == TimeSeries::Notation::BITS) {
					QPixmap pm(m_viewportDimension.width(), static_cast<int>(m_oneBitViewportHeight * s->bitsCount() + 2));
					pm.fill(Qt::transparent);
					QPainter buf(&pm);
					bitRenderer(s, &buf);
					bufferedPainter.drawPixmap(0, verticalShift, pm);
					verticalShift += m_oneBitViewportHeight * s->bitsCount() + m_verticalIndent;
				}
			}

			// Прорисовка булевых графиков
			for (int i = 0; i < m_basicSeries->size(); i++) {
				s = m_basicSeries->at(i);
				if (s->dataType() == TimeValue::Type::BOOL) {
					QPixmap pm(m_viewportDimension.width(), static_cast<int>(m_boolViewportHeight + 2));
					pm.fill(Qt::transparent);
					QPainter buf(&pm);
					boolRenderer(s, &buf);
					bufferedPainter.drawPixmap(0, verticalShift, pm);
					verticalShift += m_boolViewportHeight + m_verticalIndent;
				}
			}
		}
		if (m_wasPointsAddEvent && m_firstMouseEvent) {
			mouseEventRender(p);
			m_wasPointsAddEvent = false;
		} else {
			p->drawPixmap(0, 0, m_bufferedImage);
		}
	}
}

TimeFormat DefaultSeriesRenderer::getTimeFormat(quint64 interval, quint64 minXStep) {
	TimeFormat res;
	if (interval < TimeFormat::STEP_1SEC * 10L) {			// 10 сек
		res.format = "HH:mm:ss.zzz";
		res.step = TimeFormat::STEP_100MS;						// 0.1 сек
		res.startTime = m_bufferedTimeBounds.left();
	} else if (interval < TimeFormat::STEP_1MIN * 10L) {	// 10 мин
		res.format = "HH:mm:ss";
		res.step = TimeFormat::STEP_1SEC;						// 1 сек
		res.startTime = m_bufferedTimeBounds.left();
	} else if (interval < TimeFormat::STEP_1HOUR * 10L) {	// 10 часов
		res.format = "HH:mm";
		res.step = TimeFormat::STEP_1MIN;						// 1 мин
		res.startTime = m_bufferedTimeBounds.left();
	} else if (interval < TimeFormat::STEP_1DAY * 10L) {	// 10 сут
		res.format = "dd.MM.yy HH:mm";
		res.step = TimeFormat::STEP_1HOUR;						// 1 час
		res.startTime = m_bufferedTimeBounds.left();
	} else if (interval < TimeFormat::STEP_1MONTH * 10L) {	// 10 мес
		res.format = "dd.MM.yy";
		res.step = TimeFormat::STEP_1DAY;						// 1 день
		res.startTime = m_bufferedTimeBounds.left();
	} else if (interval < TimeFormat::STEP_1YEAR * 10L) {	// 10 лет
		res.format = "dd.MM.yy";
		res.step = TimeFormat::STEP_1MONTH;						// 1 мес
		if (QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_bufferedTimeBounds.left())).date().year() == 1970 && QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_bufferedTimeBounds.left())).date().month() == 1) {
			res.startTime = 0;
		} else {
			res.startTime = static_cast<quint64>(QDateTime::fromString(QString::number(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_bufferedTimeBounds.left())).date().year()) + QString::number(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_bufferedTimeBounds.left())).date().month()), "yyyyM").toMSecsSinceEpoch());
		}
	} else {												// > 10 лет
		res.format = "yyyy";
		res.step = TimeFormat::STEP_1YEAR;						// 1 год
		if (QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_bufferedTimeBounds.left())).date().year() == 1970) {
			res.startTime = 0;
		} else {
			res.startTime = static_cast<quint64>(QDateTime::fromString(QString::number(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(m_bufferedTimeBounds.left())).date().year()), "yyyy").toMSecsSinceEpoch());
		}
	}
	// Расчет множителя сетки 0x
	res.stepMult = 1;
	if (minXStep != res.step) {
		res.stepMult = static_cast<quint32>(minXStep / res.step + 1);
	} else {
		res.stepMult = static_cast<quint32>(minXStep / res.step);
	}
	// Начальное время кратно множителю (чтобы сетка не "дергалась" при скроллинге)
	res.startTime = (res.startTime / (res.step * res.stepMult)) * (res.step * res.stepMult);
	return res;
}

TimeFormat DefaultSeriesRenderer::calcGridX() {
	// Максимальное число линий горизонтальной сетки
	quint32 maxGridLines = static_cast<quint32>(m_viewportDimension.width()) / m_minGridXPixels;
	if (maxGridLines == 0) {
		maxGridLines = 1;
	}
	// Ширина области просмотра в единицах времени
	quint64 intervalX = m_bufferedTimeBounds.interval();
	// Минимальная ширина между линиями сетки в единицах времени
	quint64 minXStep = intervalX / maxGridLines;
	// Формат вывода времени
	TimeFormat format = getTimeFormat(intervalX, minXStep);
	return format;
}

IntegerFormat DefaultSeriesRenderer::calcGridY(qint64 min, qint64 max) {
	// Максимальное число линий вертикальной сетки
	quint32 maxGridLines = static_cast<quint32>(m_graphicAreaHeight) / m_minGridYPixels;
	if (maxGridLines == 0) {
		maxGridLines = 1;
	}
	// Минимальная ширина между линиями сетки в единицах отображаемой величины
	qint64 minYStep = (max - min) / maxGridLines;
	// Определение шага
	IntegerFormat format;
	for (qint64 i = 10; ; i *= 10) {
		if (minYStep / i == 0) {
			format.step = i / 10;
			for (double j = 1; j < 10.1; j += 0.5) {
				if (format.step * j >= minYStep) {
					format.step *= j;
					break;
				}
			}
			break;
		}
	}
	format.start = (min / format.step) * format.step;
	return format;
}

DoubleFormat DefaultSeriesRenderer::calcGridY(double min, double max) {
	// Максимальное число линий вертикальной сетки
	quint32 maxGridLines = static_cast<quint32>(m_graphicAreaHeight) / m_minGridYPixels;
	if (maxGridLines == 0) {
		maxGridLines = 1;
	}
	// Минимальная ширина между линиями сетки в единицах отображаемой величины
	double minYStep = (max - min) / maxGridLines;
	// Определение шага
	DoubleFormat format;
	format.digits = 0;
	if (minYStep >= 10.) {
		for (double i = 10.; ; i *= 10.) {
			if (minYStep / i < 1.) {
				format.step = i / 10.;
				for (double j = 1; j < 10.1; j += 0.5) {
					if (format.step * j >= minYStep) {
						format.step *= j;
						break;
					}
				}
				break;
			}
		}
	} else if (minYStep > 1E-8) {
		quint8 k = 0;
		for (double i = 1.; i < 1E10; i *= 10.) {
			format.digits = k - 1;
			if (minYStep * i > 10.) {
				format.step = 10. / i;
				double j = format.step;
				for (int z = 1; z <= 10; z++) {
					if (j >= minYStep) {
						format.step = j;
						break;
					}
					j += format.step;
				}
				break;
			}
			k++;
		}
	} else {
		format.digits = 8;
		format.step = 1E-8;
	}
	format.start = static_cast<qint32>(min / format.step) * format.step;
	return format;
}

void DefaultSeriesRenderer::viewfinderRender(QPainter* p) {
	QList<TimeSeries*>* currentSeries;
	double yK;
	ValueBounds vb;
	if (m_extraSeriesView) {
		currentSeries = m_extraSeries;
		vb = m_bufferedExtraValueBounds;
		yK = m_bufferedExtraYK;
	} else {
		currentSeries = m_basicSeries;
		vb = m_bufferedBasicValueBounds;
		yK = m_bufferedBasicYK;
	}
	if (currentSeries->size() == 0) {
		return;
	}
	quint64 time = static_cast<quint64>(m_currentCursor.x() / m_bufferedXK + m_bufferedTimeBounds.left());

	TimeSeries* s;
	QFontMetrics fm(m_viewfinderWindowFont);
	int vfWidth, vfHeight;
	int curY = 0;
	vfHeight = fm.height() + fm.descent();
	QString label;
	QString timeLabel, v;

	QPen borderPen, textPen;
	QBrush backBrush, boxBrush;
	borderPen.setColor(m_viewfinderWindowBorderColor);
	borderPen.setStyle(Qt::SolidLine);
	borderPen.setWidth(1);
	textPen.setColor(m_viewfinderWindowTextColor);
	textPen.setStyle(Qt::SolidLine);
	textPen.setWidth(1);
	backBrush.setColor(m_viewfinderWindowBackgroundColor);
	backBrush.setStyle(Qt::SolidPattern);
	boxBrush.setStyle(Qt::SolidPattern);

	QFontMetrics bitFM(m_bitsFont);
	int bitRectWidth = bitFM.width("00");

	QPixmap pm(m_viewportDimension.width(), m_viewportDimension.height());
	pm.fill(Qt::transparent);
	QPainter buf(&pm);
	/*if (antialiasing) {
		buf.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
	}*/

	int verticalShift = m_graphicAreaHeight + m_verticalIndent;

	for (int i = 0; i < currentSeries->size(); i++) {
		QBrush seriesBrush;
		seriesBrush.setStyle(Qt::SolidPattern);
		s = currentSeries->at(i);
		// Определение текущего значения
		bool paintAnalog = false;
		bool paintBits = false;
		if ((s->dataType() == TimeValue::Type::DOUBLE) || (s->dataType() == TimeValue::Type::INT && s->notation() == TimeSeries::Notation::LINES)) {
			InterpolateTimeValue val = s->interpolateValue(time);
			if (!val.isOutOfRange()) {
				v = (val.getValue() > 100000.)?QString::number(val.getValue(), 'e', 3):QString::number(val.getValue(), 'f', 3);
				label = v;
				curY = static_cast<int>(m_graphicAreaHeight - (val.getValue() - vb.min()) * yK);
				if (val.getCode() == 0) {
					seriesBrush.setColor(s->normalLineColor());
				} else {
					label = label % " : #0x" % QString::number(val.getCode(), 16).toUpper();
					seriesBrush.setColor(s->errorLineColor());
				}
				paintAnalog = true;
			}
		} else {
			TimeValue* val = s->leftNearValue(time);
			if (val == nullptr) {
				val = s->firstValue();
				if (val != nullptr) {
					if (time > val->time()) {
						val = s->lastValue();
					}
				}
			}
			if (val != nullptr) {
				if (s->dataType() == TimeValue::Type::INT) {
					v = QString::number(val->intValue());
					label = v;
					curY = static_cast<int>(m_graphicAreaHeight - (val->doubleValue() - vb.min()) * yK);
				}
				if (val->code() == 0) {
					seriesBrush.setColor(s->normalLineColor());
				} else {
					label = label % " : #0x" % QString::number(val->code(), 16).toUpper();
					seriesBrush.setColor(s->errorLineColor());
				}
				if (s->notation() == TimeSeries::Notation::STEPS) {
					paintAnalog = true;
				} else if (s->notation() == TimeSeries::Notation::BITS) {
					paintBits = true;
				}
			} else {
				label = "---";
			}
		}
		// Прорисовка текущего значения
		if (paintAnalog) {
			buf.setPen(borderPen);
			buf.setBrush(seriesBrush);
			vfWidth = fm.width(label) + m_leftIndent * 2;
			buf.drawEllipse(QPoint(m_currentCursor.x(), curY), 4, 4);
			buf.setBrush(backBrush);
			buf.setFont(m_viewfinderWindowFont);
			if ((m_viewportDimension.width() - m_currentCursor.x()) >= vfWidth) {
				buf.drawRect(m_currentCursor.x(), curY, vfWidth - 2, vfHeight - 2);
				buf.setPen(textPen);
				buf.drawText(m_leftIndent + m_currentCursor.x(), curY + vfHeight - fm.descent() * 2, label);
			} else {
				buf.drawRect(m_currentCursor.x() - vfWidth, curY, vfWidth - 2, vfHeight - 2);
				buf.setPen(textPen);
				buf.drawText(m_leftIndent + m_currentCursor.x() - vfWidth, curY + vfHeight - fm.descent() * 2, label);
			}
		} else if (paintBits) {
			QPixmap bitPM(m_viewportDimension.width(), static_cast<int>(m_oneBitViewportHeight * s->bitsCount() + 2));
			bitPM.fill(Qt::transparent);
			QPainter bitBuf(&bitPM);
			// Подписи
			bitBuf.setFont(m_bitsFont);
			int curGrid;
			for (quint8 i = 0; i < s->bitsCount(); i++) {
				label = QString::number(i);
				curGrid = static_cast<int>(m_oneBitViewportHeight * (s->bitsCount() - i));
				if ((m_viewportDimension.width() - m_currentCursor.x()) >= bitRectWidth) {
					bitBuf.setPen(textPen);
					bitBuf.drawText(m_leftIndent + m_currentCursor.x(), curGrid - bitFM.descent(), label);
				} else {
					bitBuf.setPen(textPen);
					bitBuf.drawText(m_leftIndent + m_currentCursor.x() - bitRectWidth, curGrid - bitFM.descent(), label);
				}
			}
			buf.drawPixmap(0, verticalShift, bitPM);
			verticalShift += m_oneBitViewportHeight * s->bitsCount() + m_verticalIndent;
		}
	}
	QDateTime dt;
	dt.setMSecsSinceEpoch(static_cast<qint64>(time));
	timeLabel = dt.toString("HH:mm:ss.zzz dd.MM.yyyy");
	// Прорисовка времени
	buf.setPen(borderPen);
	QColor newBackColor(m_viewfinderWindowBackgroundColor);
	newBackColor.setAlpha(255);
	backBrush.setColor(newBackColor);
	buf.setBrush(backBrush);
	buf.setFont(m_viewfinderWindowFont);
	int timeWidth = fm.width(timeLabel) + m_leftIndent * 2;
	if ((m_viewportDimension.width() - m_currentCursor.x()) >= timeWidth) {
		buf.drawRect(m_currentCursor.x(), m_viewportDimension.height() - vfHeight + 1, timeWidth - 2, vfHeight - 3);
		buf.setPen(textPen);
		buf.drawText(m_leftIndent + m_currentCursor.x(), m_viewportDimension.height() - fm.descent() * 2 + 1, timeLabel);
	} else {
		buf.drawRect(m_currentCursor.x() - timeWidth, m_viewportDimension.height() - vfHeight + 1, timeWidth - 2, vfHeight - 3);
		buf.setPen(textPen);
		buf.drawText(m_leftIndent + m_currentCursor.x() - timeWidth, m_viewportDimension.height() - fm.descent() * 2 + 1, timeLabel);
	}

	// Прорисовка линии визира
	buf.setPen(borderPen);
	buf.drawLine(m_currentCursor.x(), 0, m_currentCursor.x(), m_viewportDimension.height());
	p->drawPixmap(0, 0, pm);
}

quint32 DefaultSeriesRenderer::boolViewportHeight() const {
	return m_boolViewportHeight;
}

void DefaultSeriesRenderer::setBoolViewportHeight(quint32 value) {
	m_boolViewportHeight = value;
	emit boolViewportHeightChanged(value);
}


QFont DefaultSeriesRenderer::viewfinderWindowFont() const {
	return m_viewfinderWindowFont;
}

void DefaultSeriesRenderer::setViewfinderWindowFont(const QFont &value) {
	m_viewfinderWindowFont = value;
	emit viewfinderWindowFontChanged(value);
}

QColor DefaultSeriesRenderer::viewfinderWindowTextColor() const {
	return m_viewfinderWindowTextColor;
}

void DefaultSeriesRenderer::setViewfinderWindowTextColor(const QColor &value) {
	m_viewfinderWindowTextColor = value;
	emit viewfinderWindowTextColorChanged(value);
}

QColor DefaultSeriesRenderer::viewfinderWindowBackgroundColor() const {
	return m_viewfinderWindowBackgroundColor;
}

void DefaultSeriesRenderer::setViewfinderWindowBackgroundColor(const QColor &value) {
	m_viewfinderWindowBackgroundColor = value;
	emit viewfinderWindowBackgroundColorChanged(value);
}

QColor DefaultSeriesRenderer::viewfinderWindowBorderColor() const {
	return m_viewfinderWindowBorderColor;
}

void DefaultSeriesRenderer::setViewfinderWindowBorderColor(const QColor &value) {
	m_viewfinderWindowBorderColor = value;
	emit viewfinderWindowBorderColorChanged(value);
}

void DefaultSeriesRenderer::lineStepRenderer(TimeSeries* series, QPainter* p, double minY, double yK, bool back, bool step = false) {
	// Значения графика за временной интервал
	QList<TimeValue*> data = series->intervalData(m_bufferedTimeBounds.left(), m_bufferedTimeBounds.right(), true);
	if (data.size() == 0) {
		return;
	}
	if (static_cast<quint32>(m_viewportDimension.width() / data.size()) < m_coarseMinPixels) {
		data = series->intervalDataCoarse(static_cast<quint16>(static_cast<quint32>(m_viewportDimension.width()) / m_coarseMaxPixels), m_bufferedTimeBounds.left(), m_bufferedTimeBounds.right());
	}
	if (data.size() == 0) {
		return;
	}
	int cX, cY = 0, prevX = 0, prevY = 0;
	quint8 prevCode = 0;
	TimeValue* v;
	QPen normal, error, pointPen;
	QBrush pointBrush;
	int pointRadius = series->pointRadius();
	if (pointRadius == 0) {
		pointRadius = 1;
	}
	normal.setColor(series->normalLineColor());
	normal.setStyle(series->normalLineStyle());
	normal.setWidth(series->normalLineWidth());
	error.setColor(series->errorLineColor());
	error.setStyle(series->errorLineStyle());
	error.setWidth(series->errorLineWidth());
	pointPen.setColor(series->pointBorderColor());
	pointPen.setStyle(Qt::SolidLine);
	pointPen.setWidth(1);
	pointBrush.setColor(series->pointBackgroundColor());
	pointBrush.setStyle(Qt::SolidPattern);
	if (back) {
		QColor tmpColor = normal.color();
		tmpColor.setAlpha(static_cast<int>(normal.color().alpha() * m_extraSeriesTransparentMultiplier));
		normal.setColor(tmpColor);
		normal.setWidth(normal.width() + 2);
		tmpColor = error.color();
		tmpColor.setAlpha(static_cast<int>(error.color().alpha() * m_extraSeriesTransparentMultiplier));
		error.setColor(tmpColor);
		error.setWidth(error.width() + 2);
		tmpColor = pointPen.color();
		tmpColor.setAlpha(static_cast<int>(pointPen.color().alpha() * m_extraSeriesTransparentMultiplier));
		pointPen.setColor(tmpColor);
		pointPen.setWidth(pointPen.width() + 2);
		tmpColor = pointBrush.color();
		tmpColor.setAlpha(static_cast<int>(pointBrush.color().alpha() * m_extraSeriesTransparentMultiplier));
		pointBrush.setColor(tmpColor);
	}
	// Перебор значений и отображение на канве
	for (int i = 0; i < data.size(); i++) {
		v = data.at(i);
		// Определение преобразованных координат точки
		if (m_bufferedTimeBounds.left() > v->time()) {
			cX = static_cast<int>((m_bufferedTimeBounds.left() - v->time()) * m_bufferedXK) * -1;
		} else {
			cX = static_cast<int>((v->time() - m_bufferedTimeBounds.left()) * m_bufferedXK);
		}
		if (series->dataType() == TimeValue::Type::INT) {
			cY = static_cast<int>(m_graphicAreaHeight - (v->intValue() - minY) * yK);
		} else if (series->dataType() == TimeValue::Type::DOUBLE) {
			cY = static_cast<int>(m_graphicAreaHeight - (v->doubleValue() - minY) * yK);
		} else {
			return;
		}
		if (data.size() == 1) {
			p->setBrush(pointBrush);
			p->setPen(pointPen);
			p->drawEllipse(QPoint(cX, cY), pointRadius, pointRadius);
			return;
		}
		// Установка цвета и типа линий
		if (prevCode == 0) {
			p->setPen(normal);
		} else {
			p->setPen(error);
		}
		// Прорисовка графика
		if (i > 0) {
			if (step) {
				p->drawLine(prevX, prevY, cX, prevY);
				p->drawLine(cX, prevY, cX, cY);
			} else {
				p->drawLine(prevX, prevY, cX, cY);
			}
			if (series->isShowPoints()) {
				p->setBrush(pointBrush);
				p->setPen(pointPen);
				p->drawEllipse(QPoint(prevX, prevY), pointRadius, pointRadius);
				// Последняя точка
				if (i == (data.size() - 1)) {
					p->drawEllipse(QPoint(cX, cY), pointRadius, pointRadius);
				}
			}
		}
		prevX = cX;
		prevY = cY;
		prevCode = v->code();
	}
	// Прорисовка минимума и максимума
	QList<TimeValue*> tvs;
	QPen maxPen, minPen, labelsPen;
	QBrush maxBrush, minBrush;
	p->setFont(m_labelsFont);
	QString label;
	QFontMetrics fm(m_labelsFont);
	int rectWidth, rectHeight = fm.height();

	maxPen.setColor(m_maxPointerBorderColor);
	maxPen.setStyle(Qt::SolidLine);
	maxPen.setWidth(1);
	minPen.setColor(m_minPointerBorderColor);
	minPen.setStyle(Qt::SolidLine);
	minPen.setWidth(1);
	labelsPen.setColor(m_labelsColor);
	labelsPen.setStyle(Qt::SolidLine);
	labelsPen.setWidth(1);
	maxBrush.setColor(m_maxPointerBackgroundColor);
	maxBrush.setStyle(Qt::SolidPattern);
	minBrush.setColor(m_minPointerBackgroundColor);
	minBrush.setStyle(Qt::SolidPattern);
	if (back) {
		QColor tmpColor = maxPen.color();
		tmpColor.setAlpha(static_cast<int>(maxPen.color().alpha() * m_extraSeriesTransparentMultiplier));
		maxPen.setColor(tmpColor);
		tmpColor = minPen.color();
		tmpColor.setAlpha(static_cast<int>(minPen.color().alpha() * m_extraSeriesTransparentMultiplier));
		minPen.setColor(tmpColor);
		tmpColor = maxBrush.color();
		tmpColor.setAlpha(static_cast<int>(maxBrush.color().alpha() * m_extraSeriesTransparentMultiplier));
		maxBrush.setColor(tmpColor);
		tmpColor = minBrush.color();
		tmpColor.setAlpha(static_cast<int>(minBrush.color().alpha() * m_extraSeriesTransparentMultiplier));
		minBrush.setColor(tmpColor);
		tmpColor = labelsPen.color();
		tmpColor.setAlpha(static_cast<int>(labelsPen.color().alpha() * m_extraSeriesTransparentMultiplier));
		labelsPen.setColor(tmpColor);
	}

	QPoint pnt[5];
	for (int z = 0; z < 2; z++) {
		if (z == 0) {
			tvs = series->maxValues(m_bufferedTimeBounds.left(), m_bufferedTimeBounds.right());
			p->setBrush(maxBrush);
			p->setPen(maxPen);
		} else {
			tvs = series->minValues(m_bufferedTimeBounds.left(), m_bufferedTimeBounds.right());
			p->setBrush(minBrush);
			p->setPen(minPen);
		}

		for (int  i = 0; i < tvs.size(); i++) {
			v = tvs.at(i);
			cX = static_cast<int>((v->time() - m_bufferedTimeBounds.left()) * m_bufferedXK);
			if (v->type() == TimeValue::Type::INT) {
				label = QString::number(v->intValue());
				cY = static_cast<int>(m_graphicAreaHeight - (v->intValue() - minY) * yK);
				p->drawEllipse(QPoint(cX, cY), pointRadius, pointRadius);
			} else if (v->type() == TimeValue::Type::DOUBLE) {
				label = (v->doubleValue() > 100000.)?QString::number(v->doubleValue(), 'e', 3):QString::number(v->doubleValue(), 'f', 3);
				cY = static_cast<int>(m_graphicAreaHeight - (v->doubleValue() - minY) * yK);
				p->drawEllipse(QPoint(cX, cY), pointRadius, pointRadius);
			}
			rectWidth = fm.width(label) + m_leftIndent * 2;
			if (z == 0) {
				pnt[0] = QPoint(cX - rectWidth / 2, cY - 3 * rectHeight / 2);
				pnt[1] = QPoint(cX + rectWidth / 2, cY - 3 * rectHeight / 2);
				pnt[2] = QPoint(cX + rectWidth / 2, cY - rectHeight / 2);
				pnt[3] = QPoint(cX, cY);
				pnt[4] = QPoint(cX - rectWidth / 2, cY - rectHeight / 2);
			} else {
				pnt[0] = QPoint(cX - rectWidth / 2, cY + rectHeight / 2);
				pnt[1] = QPoint(cX, cY);
				pnt[2] = QPoint(cX + rectWidth / 2, cY + rectHeight / 2);
				pnt[3] = QPoint(cX + rectWidth / 2, cY + 3 * rectHeight / 2);

				pnt[4] = QPoint(cX - rectWidth / 2, cY + 3 * rectHeight / 2);
			}
			p->drawPolygon(pnt, 5);
			p->setPen(labelsPen);
			if (z == 0) {
				p->drawText(m_leftIndent + cX - rectWidth / 2, cY - fm.descent() - rectHeight / 2, label);
			} else {
				p->drawText(m_leftIndent + cX - rectWidth / 2, cY - fm.descent() + 3 * rectHeight / 2, label);
			}
		}
	}
}

void DefaultSeriesRenderer::boolRenderer(TimeSeries* series, QPainter* p) {
	// Значения графика за временной интервал
	QList<TimeValue*> data = series->intervalData(m_bufferedTimeBounds.left(), m_bufferedTimeBounds.right(), true);
	if (data.size() == 0) {
		return;
	}
	int halfViewport = m_boolViewportHeight / 2;
	int cX, prevX = 0;
	bool cY, prevY = false;
	quint8 prevCode = 0;
	TimeValue* v;
	QPen normal, error;
	QBrush pointBrush;
	normal.setColor(series->normalLineColor());
	normal.setStyle(series->normalLineStyle());
	normal.setWidth(1);
	error.setColor(series->errorLineColor());
	error.setStyle(series->errorLineStyle());
	error.setWidth(1);
	pointBrush.setColor(series->pointBackgroundColor());
	pointBrush.setStyle(Qt::SolidPattern);
	p->setBrush(pointBrush);
	// Перебор значений и отображение на канве
	for (int i = 0; i < data.size(); i++) {
		v = data.at(i);
		// Определение преобразованных координат точки
		if (m_bufferedTimeBounds.left() > v->time()) {
			cX = static_cast<int>((m_bufferedTimeBounds.left() - v->time()) * m_bufferedXK) * -1;
		} else {
			cX = static_cast<int>((v->time() - m_bufferedTimeBounds.left()) * m_bufferedXK);
		}
		cY = v->boolValue();
		// Установка цвета и типа линий
		if (prevCode == 0) {
			p->setPen(normal);
		} else {
			p->setPen(error);
		}
		if (data.size() == 1) {
			if (cY) {
				p->drawRect(m_leftIndent, 0, m_viewportDimension.width() - m_leftIndent * 2, static_cast<int>(m_boolViewportHeight));
			} else {
				p->drawLine(m_leftIndent, halfViewport, m_viewportDimension.width() - m_leftIndent, halfViewport);
			}
			return;
		}
		// Прорисовка графика
		if (i > 0) {
			if (prevY) {
				p->drawRect(prevX, 0, cX - prevX, static_cast<int>(m_boolViewportHeight));
			} else {
				p->drawLine(prevX, halfViewport, cX, halfViewport);
			}
			if (i == data.size() - 1) {
				if (cY) {
					p->drawRect(cX, 0, m_viewportDimension.width() - cX - m_leftIndent, static_cast<int>(m_boolViewportHeight));
				} else {
					p->drawLine(cX, halfViewport, m_viewportDimension.width() - m_leftIndent, halfViewport);
				}
			}
		}
		prevX = cX;
		prevY = cY;
		prevCode = v->code();
	}
}

bool DefaultSeriesRenderer::bit(qint64 val, quint8 index) {
	return (val & (static_cast<qint64>(1) << index)) ? true : false;
}

void DefaultSeriesRenderer::bitRenderer(TimeSeries* series, QPainter* p) {
	// Значения графика за временной интервал
	QList<TimeValue*> data = series->intervalData(m_bufferedTimeBounds.left(), m_bufferedTimeBounds.right(), true);
	if (data.size() == 0) {
		return;
	}
	int cX, prevX = 0;
	qint64 cY, prevY = 0;
	quint8 prevCode = 0;
	TimeValue* v;
	QPen normal, error;
	QBrush pointBrush;
	normal.setColor(series->normalLineColor());
	normal.setStyle(series->normalLineStyle());
	normal.setWidth(1);
	error.setColor(series->errorLineColor());
	error.setStyle(series->errorLineStyle());
	error.setWidth(1);
	pointBrush.setColor(series->pointBackgroundColor());
	pointBrush.setStyle(Qt::SolidPattern);
	p->setBrush(pointBrush);
	QFontMetrics fm(m_bitsFont);
	p->setFont(m_bitsFont);
	int rectWidth = fm.width("00");
	// Перебор значений и отображение на канве
	for (int i = 0; i < data.size(); i++) {
		v = data.at(i);
		// Определение преобразованных координат точки
		if (m_bufferedTimeBounds.left() > v->time()) {
			cX = static_cast<int>((m_bufferedTimeBounds.left() - v->time()) * m_bufferedXK) * -1;
		} else {
			cX = static_cast<int>((v->time() - m_bufferedTimeBounds.left()) * m_bufferedXK);
		}
		cY = v->intValue();
		if (i > 0 || data.size() == 1) {
			for (quint8 j = 0; j < series->bitsCount(); j++) {
				// Установка цвета и типа линий
				if (prevCode == 0) {
					p->setPen(normal);
				} else {
					p->setPen(error);
				}
				// Прорисовка графика
				if (data.size() == 1) {
					if (bit(cY, j)) {
						p->drawRect(rectWidth + m_leftIndent * 3, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j)), m_viewportDimension.width() - (rectWidth + m_leftIndent * 4), static_cast<int>(m_oneBitHeight));
					} else {
						p->drawLine(rectWidth + m_leftIndent * 3, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j) + m_oneBitHeight / 2), m_viewportDimension.width() - m_leftIndent, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j) + m_oneBitHeight / 2));
					}
				} else {
					if (bit(prevY, j)) {
						p->drawRect(prevX, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j)), cX - prevX, static_cast<int>(m_oneBitHeight));
					} else {
						p->drawLine(prevX, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j) + m_oneBitHeight / 2), cX, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j) + m_oneBitHeight / 2));
					}
					if (i == data.size() - 1) {
						if (bit(cY, j)) {
							p->drawRect(cX, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j)), m_viewportDimension.width() - cX - m_leftIndent, static_cast<int>(m_oneBitHeight));
						} else {
							p->drawLine(cX, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j) + m_oneBitHeight / 2), m_viewportDimension.width() - m_leftIndent, static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - 1 - j) + m_oneBitHeight / 2));
						}
					}
				}
			}
		}
		if (data.size() == 1) {
			break;
		}
		prevX = cX;
		prevY = cY;
		prevCode = v->code();
	}

	// Подписи
	QString label;
	QPen rectPen;
	rectPen.setColor(m_gridColor);
	rectPen.setStyle(Qt::SolidLine);
	rectPen.setWidth(1);
	QBrush rectBrush(m_labelsBackgroundColor);
	p->setBrush(rectBrush);
	int curGrid;
	for (quint8 i = 0; i < series->bitsCount(); i++) {
		label = QString::number(i);
		curGrid = static_cast<int>(m_oneBitViewportHeight * (series->bitsCount() - i));
		p->setPen(rectPen);
		p->drawRect(0, curGrid - static_cast<int>(m_oneBitViewportHeight) + 1 - fm.descent(), rectWidth + m_leftIndent * 2, static_cast<int>(m_oneBitViewportHeight));
		p->setPen(m_labelsColor);
		p->drawText(m_leftIndent, curGrid - fm.descent(), label);
	}
}

void DefaultSeriesRenderer::analogRenderer(QPainter* p) {
	bool isDouble = false;
	DoubleFormat fD;
	IntegerFormat fI;
	int curGridY;
	ValueBounds vb;
	double yK;
	bool extraSeriesRender;
	// Текущая группа графиков
	QList<TimeSeries*>* currentSeries;
	// Выбор текущей группы графиков для прорисовки
	for (char counter = (m_extraSeries->size() > 0)?1:0; counter >= 0; counter--) {
		if (m_extraSeriesView) {
			if (counter == 1) {
				currentSeries = m_basicSeries;
				vb = m_bufferedBasicValueBounds;
				yK = m_bufferedBasicYK;
				extraSeriesRender = false;
			} else {
				currentSeries = m_extraSeries;
				vb = m_bufferedExtraValueBounds;
				yK = m_bufferedExtraYK;
				extraSeriesRender = true;
			}
		} else {
			if (counter == 1) {
				currentSeries = m_extraSeries;
				vb = m_bufferedExtraValueBounds;
				yK = m_bufferedExtraYK;
				extraSeriesRender = true;
			} else {
				currentSeries = m_basicSeries;
				vb = m_bufferedBasicValueBounds;
				yK = m_bufferedBasicYK;
				extraSeriesRender = false;
			}
		}

		// Расчет разметки оси 0y и визуализация
		if (counter == 0) {
			for (int i = 0; i < currentSeries->size(); i++) {
				if (currentSeries->at(i)->dataType() == TimeValue::Type::DOUBLE) {
					isDouble = true;
					break;
				}
			}
			QPixmap gridPM(m_viewportDimension.width(), m_graphicAreaHeight);
			gridPM.fill(Qt::transparent);
			QPainter gridBuf(&gridPM);

			QPen grid;
			grid.setColor(m_gridColor);
			grid.setStyle(m_gridStyle);
			grid.setWidth(m_gridWidth);
			gridBuf.setPen(grid);

			if (isDouble) {
				fD = calcGridY(vb.min(), vb.max());
				for (double j = fD.start; j < vb.max(); j += fD.step) {
					curGridY = static_cast<int>(m_graphicAreaHeight - ((j - vb.min()) * yK));
					gridBuf.drawLine(0, curGridY, static_cast<int>(m_viewportDimension.width()), curGridY);
				}
			} else {
				fI = calcGridY(static_cast<qint64>(vb.min()), static_cast<qint64>(vb.max()));
				for (qint64 j = fI.start; j < vb.max(); j += fI.step) {
					curGridY = static_cast<int>(m_graphicAreaHeight - ((j - vb.min()) * yK));
					gridBuf.drawLine(0, curGridY, static_cast<int>(m_viewportDimension.width()), curGridY);
				}
			}
			p->drawPixmap(0, 0, gridPM);
		}

		// Визуализация графиков
		TimeSeries* s;
		for (int i = 0; i < currentSeries->size(); i++) {
			s = currentSeries->at(i);
			if (s->notation() == TimeSeries::Notation::LINES || s->notation() == TimeSeries::Notation::STEPS) {
				if (extraSeriesRender) {
					lineStepRenderer(s, p, m_bufferedExtraValueBounds.min(), m_bufferedExtraYK, counter, (s->notation() == TimeSeries::Notation::STEPS)?true:false);
				} else {
					lineStepRenderer(s, p, m_bufferedBasicValueBounds.min(), m_bufferedBasicYK, counter, (s->notation() == TimeSeries::Notation::STEPS)?true:false);
				}
			}
		}

		// Подписи оси 0y
		if (counter == 0) {
			QPixmap labelsPM(m_viewportDimension.width(), m_graphicAreaHeight);
			labelsPM.fill(Qt::transparent);
			QPainter labelsBuf(&labelsPM);

			labelsBuf.setFont(m_labelsFont);
			QString label;
			QPen rectPen;
			rectPen.setColor(m_gridColor);
			rectPen.setStyle(Qt::SolidLine);
			rectPen.setWidth(1);
			QBrush rectBrush(m_labelsBackgroundColor);
			labelsBuf.setBrush(rectBrush);
			QFontMetrics fm(m_labelsFont);
			int rectWidth;

			if (isDouble) {
				for (double j = fD.start; j < vb.max(); j += fD.step) {
					label = QString::number(j, 'f', fD.digits);
					curGridY = static_cast<int>(m_graphicAreaHeight - ((j - vb.min()) * yK));
					rectWidth = fm.width(label);
					// Метка
					if (curGridY < 0) {
						curGridY = 0;
					}
					labelsBuf.setPen(rectPen);
					labelsBuf.drawRect(0, curGridY - static_cast<int>(fm.height()) / 2, static_cast<int>(rectWidth) + m_leftIndent * 2, fm.height());
					labelsBuf.setPen(m_labelsColor);
					labelsBuf.drawText(m_leftIndent, curGridY - fm.descent() + static_cast<int>(fm.height()) / 2, label);
				}
			} else {
				for (qint64 j = fI.start; j < vb.max(); j += fI.step) {
					label = QString::number((j));
					curGridY = static_cast<int>(m_graphicAreaHeight - ((j - vb.min()) * yK));
					rectWidth = fm.width(label);
					// Метка
					labelsBuf.setPen(rectPen);
					labelsBuf.drawRect(0, curGridY - static_cast<int>(fm.height()) / 2, static_cast<int>(rectWidth) + m_leftIndent * 2, fm.height());
					labelsBuf.setPen(m_labelsColor);
					labelsBuf.drawText(m_leftIndent, curGridY - fm.descent() + static_cast<int>(fm.height()) / 2, label);
				}
			}
			p->drawPixmap(0, 0, labelsPM);
		}
	}
}

void DefaultSeriesRenderer::gridXRenderer(QPainter* p) {
	TimeFormat format = calcGridX();
	int curGridX = 0;
	QDateTime leftDate;
	quint64 stepGridX = format.step * format.stepMult;
	p->setFont(m_labelsFont);
	QString label;
	QPen grid, rectPen;
	grid.setColor(m_gridColor);
	grid.setStyle(m_gridStyle);
	grid.setWidth(m_gridWidth);
	rectPen.setColor(m_gridColor);
	rectPen.setStyle(Qt::SolidLine);
	rectPen.setWidth(1);
	QBrush rectBrush(m_labelsBackgroundColor);
	QFontMetrics fm(m_labelsFont);
	int rectWidth;
	p->setBrush(rectBrush);
	p->setRenderHints(QPainter::Antialiasing, false);

	int j = 0;
	for (quint64 i = format.startTime; i < m_bufferedTimeBounds.right(); i += stepGridX) {
		if (format.step == TimeFormat::STEP_1YEAR) {
			quint64 ij = static_cast<quint64>((QDateTime::fromString(QString::number(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(format.startTime)).date().year() + static_cast<qint32>(format.stepMult) * j), "yyyy")).toMSecsSinceEpoch());
			if (ij >= m_bufferedTimeBounds.left()) {
				curGridX = static_cast<int>((ij - m_bufferedTimeBounds.left()) * m_bufferedXK);
			} else {
				j++;
				continue;
			}
			leftDate.setMSecsSinceEpoch(static_cast<qint64>(ij));
			j++;
		} else if (format.step == TimeFormat::STEP_1MONTH) {
			int m = QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(format.startTime)).date().month() + static_cast<qint32>(format.stepMult) * j;
			int res = m % 12;
			quint64 ij = static_cast<quint64>((QDateTime::fromString(QString::number(QDateTime::fromMSecsSinceEpoch(static_cast<qint64>(format.startTime)).date().year() + ((res == 0)?(m / 12 - 1):(m / 12))) + QString::number((res == 0)?12:res), "yyyyM")).toMSecsSinceEpoch());
			if (ij >= m_bufferedTimeBounds.left()) {
				curGridX = static_cast<int>((ij - m_bufferedTimeBounds.left()) * m_bufferedXK);
			} else {
				j++;
				continue;
			}
			leftDate.setMSecsSinceEpoch(static_cast<qint64>(ij));
			j++;
		} else {
			if (i >= m_bufferedTimeBounds.left()) {
				curGridX = static_cast<int>((i - m_bufferedTimeBounds.left()) * m_bufferedXK);
			} else {
				j++;
				continue;
			}
			leftDate.setMSecsSinceEpoch(static_cast<qint64>(i));
		}
		label = leftDate.toString(format.format);
		rectWidth = fm.width(label);
		// Линия сетки
		p->setPen(grid);
		p->drawLine(curGridX, 0, curGridX, m_viewportDimension.height());
		// Метка
		p->setPen(rectPen);
		if (curGridX < 0) {
			curGridX = 0;
		}
		p->drawRect(curGridX, m_viewportDimension.height() - fm.height() - 2, rectWidth + m_leftIndent * 2, fm.height());
		p->setPen(m_labelsColor);
		p->drawText(curGridX + m_leftIndent, m_viewportDimension.height() - fm.descent() - 2, label);
	}
}

QFont DefaultSeriesRenderer::labelsFont() const {
	return m_labelsFont;
}

void DefaultSeriesRenderer::setLabelsFont(const QFont &value) {
	m_labelsFont = value;
	emit labelsFontChanged(value);
}

int DefaultSeriesRenderer::leftIndent() const {
	return m_leftIndent;
}

void DefaultSeriesRenderer::setLeftIndent(quint16 value) {
	m_leftIndent = value;
	emit leftIndentChanged(value);
}

int DefaultSeriesRenderer::gridWidth() const {
	return m_gridWidth;
}

void DefaultSeriesRenderer::setGridWidth(int value) {
	m_gridWidth = value;
	emit gridWidthChanged(value);
}

void DefaultSeriesRenderer::mouseHoverEvent(const QPoint &pos) {
	/*if ((me->buttons() & Qt::LeftButton) && mousePressed) {
		selection.setWidth(me->pos().x() - selection.x());
		selection.setHeight(me->pos().y() - selection.y());
	}*/
	m_currentCursor = pos;
	m_overlayRenderEvent = true;
	m_firstMouseEvent = true;
}

void DefaultSeriesRenderer::mousePressEvent(QMouseEvent*) {

}

void DefaultSeriesRenderer::mouseReleaseEvent(QMouseEvent*) {

}

void DefaultSeriesRenderer::zoomSelectionEvent(QMouseEvent*, QRect selection, bool zooming) {
	m_selection = selection;
	m_mouseZooming = zooming;
	setTracking(false);
}

void DefaultSeriesRenderer::scrollEvent(QMouseEvent*, QRect selection, bool scrolling) {
	m_selection = selection;
	m_mouseScrolling = scrolling;
	setTracking(false);
}

void DefaultSeriesRenderer::zoomEvent(QRect selection) {
	if (selection.width() / m_bufferedXK > TimeFormat::STEP_1SEC) {
		m_bufferedTimeBounds = TimeBounds(
			static_cast<quint64>(m_bufferedTimeBounds.left() + selection.x() / m_bufferedXK),
			static_cast<quint64>(m_bufferedTimeBounds.left() + (selection.x() + selection.width()) / m_bufferedXK)
		);
		m_bufferedBasicValueBounds.setBounds(
			m_bufferedBasicValueBounds.max() - (selection.y() + selection.height()) / m_bufferedBasicYK,
			m_bufferedBasicValueBounds.max() - selection.y() / m_bufferedBasicYK
		);
		m_bufferedExtraValueBounds.setBounds(
			m_bufferedExtraValueBounds.max() - (selection.y() + selection.height()) / m_bufferedExtraYK,
			m_bufferedExtraValueBounds.max() - selection.y() / m_bufferedExtraYK
		);
		setScaling(true);
	}
}

bool DefaultSeriesRenderer::isScalingCanceled() {
	return !m_scaling;
}

void DefaultSeriesRenderer::scalingCancel() {
	setScaling(false);
}

void DefaultSeriesRenderer::scalingDefaultEvent() {
	m_bufferedTimeBounds = defaultTimeBounds();
	m_bufferedBasicValueBounds = defaultValueBounds(m_basicSeries);
	m_bufferedExtraValueBounds = defaultValueBounds(m_extraSeries);
	setScaling(true);
	setTracking(false);
}

void DefaultSeriesRenderer::scalingTimeValueEvent() {
	m_bufferedTimeBounds = completeTimeBounds();
	if (m_extraSeriesView) {
		m_bufferedExtraValueBounds = autoValueBounds(m_extraSeries, m_bufferedTimeBounds);
	} else {
		m_bufferedBasicValueBounds = autoValueBounds(m_basicSeries, m_bufferedTimeBounds);
	}
	setScaling(true);
	setTracking(false);
}

void DefaultSeriesRenderer::scalingValueEvent() {
	m_bufferedBasicValueBounds = autoValueBounds(m_basicSeries, m_bufferedTimeBounds);
	m_bufferedExtraValueBounds = autoValueBounds(m_extraSeries, m_bufferedTimeBounds);
	setScaling(true);
	setTracking(false);
}

TimeBounds DefaultSeriesRenderer::currentTimeBounds() const {
	return m_bufferedTimeBounds;
}

ValueBounds DefaultSeriesRenderer::currentValueBounds() const {
	if (m_extraSeriesView) {
		return m_bufferedExtraValueBounds;
	} else {
		return m_bufferedBasicValueBounds;
	}
}

void DefaultSeriesRenderer::moveTimeForwardEvent(quint64 time) {
	m_bufferedTimeBounds.moveForward(time);
	setScaling(true);
}

void DefaultSeriesRenderer::moveTimeBackEvent(quint64 time) {
	m_bufferedTimeBounds.moveBack(time);
	setScaling(true);
}

void DefaultSeriesRenderer::setTimeBoundsEvent(quint64 leftTime, quint64 rightTime) {
	m_bufferedTimeBounds.setBounds(leftTime, rightTime);
	setScaling(true);
}

void DefaultSeriesRenderer::setValueBoundsEvent(double min, double max) {
	if (m_extraSeriesView) {
		m_bufferedExtraValueBounds.setBounds(min, max);
	} else {
		m_bufferedBasicValueBounds.setBounds(min, max);
	}
	setScaling(true);
}

void DefaultSeriesRenderer::moveValueUpEvent(double value) {
	if (m_extraSeriesView) {
		m_bufferedExtraValueBounds.moveUp(value);
	} else {
		m_bufferedBasicValueBounds.moveUp(value);
	}
	setScaling(true);
}

void DefaultSeriesRenderer::moveValueDownEvent(double value) {
	if (m_extraSeriesView) {
		m_bufferedExtraValueBounds.moveDown(value);
	} else {
		m_bufferedBasicValueBounds.moveDown(value);
	}
	setScaling(true);
}

void DefaultSeriesRenderer::mouseWheelEvent(QWheelEvent*) {}

void DefaultSeriesRenderer::newPointsAddedEvent() {
	if (m_tracking) {
		TimeBounds tb = completeTimeBounds();
		if (m_autoRangeValue) {
			m_bufferedBasicValueBounds = autoValueBounds(m_basicSeries, m_bufferedTimeBounds);
			m_bufferedExtraValueBounds = autoValueBounds(m_extraSeries, m_bufferedTimeBounds);
		}
		if (m_bufferedTimeBounds.right() < tb.right()) {
			m_bufferedTimeBounds = TimeBounds(tb.right() - m_bufferedTimeBounds.interval(), tb.right());
		}
		setScaling(true);
	}
	m_wasPointsAddEvent = true;
}
