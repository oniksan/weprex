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

#ifndef WS_CHART_DEFAULTSERIESRENDERER_H
#define WS_CHART_DEFAULTSERIESRENDERER_H

#include <memory>
#include <QtCore>
#include <QtGui>
#include "seriesrenderer.h"

namespace webstella {
	namespace gui {

		// Формат разметки оси времени
		class TimeFormat {

		public:
			TimeFormat() : format(""), step(0), startTime(0), stepMult(0) {}
			static const quint64 STEP_100MS = 100;
			static const quint64 STEP_1SEC = 1000;
			static const quint64 STEP_1MIN = 60000;
			static const quint64 STEP_1HOUR = 3600000;
			static const quint64 STEP_1DAY = 86400000;
			static const quint64 STEP_1MONTH = 2592000000;
			static const quint64 STEP_1YEAR = 31536000000;

			QString format;
			quint64 step;
			quint64 startTime;
			quint32 stepMult;
		};

		class IntegerFormat {

		public:
			IntegerFormat() : step(0), start(0) {}
			qint64 step;
			qint64 start;
		};

		class DoubleFormat {

		public:
			DoubleFormat() : step(0.), start(0.), digits(0) {}
			double step;
			double start;
			quint8 digits;
		};

		class DefaultSeriesRenderer : public SeriesRenderer {
		Q_OBJECT

		private:
			TimeFormat getTimeFormat(quint64 interval, quint64 minXStep);
			TimeFormat calcGridX();
			IntegerFormat calcGridY(qint64 min, qint64 max);
			DoubleFormat calcGridY(double min, double max);
			void lineStepRenderer(TimeSeries* m_basicSeries, QPainter* p, double minY, double yK, bool back, bool step);
			void boolRenderer(TimeSeries* m_basicSeries, QPainter* p);
			void bitRenderer(TimeSeries* m_basicSeries, QPainter* p);
			void analogRenderer(QPainter* p);
			void gridXRenderer(QPainter* p);
			void viewfinderRender(QPainter* p);
			bool bit(qint64 val, quint8 index);
			void calcAnalogHeight();
			void mouseEventRender(QPainter* p);
			bool m_wasPointsAddEvent;
			bool m_firstMouseEvent;
			void setScaling(bool value);

		protected:
			// Высота булева графика
			quint32 m_boolViewportHeight;
			quint32 m_boolViewportsCount;
			quint32 m_bitViewportsCount;
			quint32 m_allBitsCount;
			quint32 m_commonViewportsCount;
			// Высота одной полоски бита битового представления
			quint32 m_oneBitViewportHeight;
			quint32 m_oneBitHeight;
			quint32 m_oneBitIndent;
			QFont m_bitsFont;

			quint32 m_minGridXPixels;
			quint32 m_minGridYPixels;
			QColor m_gridColor;
			Qt::PenStyle m_gridStyle;
			int m_gridWidth;
			QColor m_labelsColor;
			QColor m_labelsBackgroundColor;
			QColor m_backgroundColor;
			QFont m_labelsFont;
			// Горизонтальный отступ при выводе подписей осей
			quint16 m_leftIndent;
			// Вертикальный отступ между графиками
			quint16 m_verticalIndent;
			QColor m_viewfinderWindowBorderColor;
			QColor m_viewfinderWindowBackgroundColor;
			QColor m_viewfinderWindowTextColor;
			QFont m_viewfinderWindowFont;

			// Область выделения
			QRect m_selection;

			// Цвет бордюра области выделения
			QColor m_selectionBorderColor;
			// Стиль бордюра области выделения
			Qt::PenStyle m_selectionBorderStyle;
			// Ширина бордюра области выделения
			int m_selectionBorderWidth;
			// Цвет фона области выделения
			QColor m_selectionBackgroundColor;
			// Стиль фона области выделения
			Qt::BrushStyle m_selectionBackgroundStyle;

			// Цвет фона указателя максимума
			QColor m_maxPointerBackgroundColor;
			// Цвет бордюра указателя максимума
			QColor m_maxPointerBorderColor;
			// Цвет фона указателя минимума
			QColor m_minPointerBackgroundColor;
			// Цвет бордюра указателя минимума
			QColor m_minPointerBorderColor;

			bool m_antialiasing;

			// Буферизированное изображение
			QPixmap m_bufferedImage;
			// Буферизированный коэффициент оси 0y
			double m_bufferedBasicYK;
			double m_bufferedExtraYK;
			// Буферизированный коэффициент оси 0x
			double m_bufferedXK;
			// Буферизированные временные границы
			//TimeBounds m_bufferedTimeBounds;
			// Буферизированные границы значения
			ValueBounds m_bufferedBasicValueBounds;
			ValueBounds m_bufferedExtraValueBounds;
			// Буферизированные временные границы
			TimeBounds m_bufferedTimeBounds;
			// Флаг необходимости рендера поверх основного (без изменения основного)
			bool m_overlayRenderEvent;
			// Текущие координаты мыши
			QPoint m_currentCursor;
			// Флаг перетаскивания
			bool m_mouseZooming;
			bool m_mouseScrolling;
			// Флаг принудительного изменения мастабирования
			bool m_scaling;

		public:
			DefaultSeriesRenderer(QList<TimeSeries*>* m_basicSeries, QList<TimeSeries*>* m_extraSeries, QObject* parent = nullptr);

			int gridWidth() const;
			void setGridWidth(int value);

			int leftIndent() const;
			void setLeftIndent(quint16 value);

			QFont labelsFont() const;
			void setLabelsFont(const QFont &value);

			QColor viewfinderWindowBorderColor() const;
			void setViewfinderWindowBorderColor(const QColor &value);

			QColor viewfinderWindowBackgroundColor() const;
			void setViewfinderWindowBackgroundColor(const QColor &value);

			QColor viewfinderWindowTextColor() const;
			void setViewfinderWindowTextColor(const QColor &value);

			QFont viewfinderWindowFont() const;
			void setViewfinderWindowFont(const QFont &value);

			quint32 boolViewportHeight() const;
			void setBoolViewportHeight(quint32 value);

			bool isAntialiasing() const;
			void setAntialiasing(bool value);

			QColor maxPointerBackgroundColor() const;
			void setMaxPointerBackgroundColor(const QColor &value);

			QColor maxPointerBorderColor() const;
			void setMaxPointerBorderColor(const QColor &value);

			QColor minPointerBackgroundColor() const;
			void setMinPointerBackgroundColor(const QColor &value);

			QColor minPointerBorderColor() const;
			void setMinPointerBorderColor(const QColor &value);

			Q_INVOKABLE TimeBounds currentTimeBounds() const override;
			Q_INVOKABLE ValueBounds currentValueBounds() const override;
			Q_INVOKABLE void update() override;
			Q_INVOKABLE void render(QPainter* p) override;
			Q_INVOKABLE void mouseHoverEvent(const QPoint &pos) override;
			Q_INVOKABLE void mousePressEvent(QMouseEvent*) override;
			Q_INVOKABLE void mouseReleaseEvent(QMouseEvent*) override;
			Q_INVOKABLE void zoomSelectionEvent(QMouseEvent*, QRect m_selection, bool zooming) override;
			Q_INVOKABLE void scrollEvent(QMouseEvent*, QRect m_selection, bool scrolling) override;
			Q_INVOKABLE void zoomEvent(QRect m_selection) override;
			Q_INVOKABLE bool isScalingCanceled() override;
			Q_INVOKABLE void scalingCancel() override;
			Q_INVOKABLE void scalingDefaultEvent() override;
			Q_INVOKABLE void scalingTimeValueEvent() override;
			Q_INVOKABLE void scalingValueEvent() override;
			Q_INVOKABLE void moveTimeForwardEvent(quint64 time) override;
			Q_INVOKABLE void moveTimeBackEvent(quint64 time) override;
			Q_INVOKABLE void setTimeBoundsEvent(quint64 leftTime, quint64 rightTime) override;
			Q_INVOKABLE void moveValueUpEvent(double value) override;
			Q_INVOKABLE void moveValueDownEvent(double value) override;
			Q_INVOKABLE void setValueBoundsEvent(double min, double max) override;
			Q_INVOKABLE void mouseWheelEvent(QWheelEvent*) override;
			Q_INVOKABLE void newPointsAddedEvent() override;

			Q_PROPERTY(int gridWidth READ gridWidth WRITE setGridWidth NOTIFY gridWidthChanged)
			Q_PROPERTY(int leftIndent READ leftIndent WRITE setLeftIndent NOTIFY leftIndentChanged)
			Q_PROPERTY(QFont labelsFont READ labelsFont WRITE setLabelsFont NOTIFY labelsFontChanged)
			Q_PROPERTY(QColor viewfinderWindowBorderColor READ viewfinderWindowBorderColor WRITE setViewfinderWindowBorderColor NOTIFY viewfinderWindowBorderColorChanged)
			Q_PROPERTY(QColor viewfinderWindowBackgroundColor READ viewfinderWindowBackgroundColor WRITE setViewfinderWindowBackgroundColor NOTIFY viewfinderWindowBackgroundColorChanged)
			Q_PROPERTY(QColor viewfinderWindowTextColor READ viewfinderWindowTextColor WRITE setViewfinderWindowTextColor NOTIFY viewfinderWindowTextColorChanged)
			Q_PROPERTY(QFont viewfinderWindowFont READ viewfinderWindowFont WRITE setViewfinderWindowFont NOTIFY viewfinderWindowFontChanged)
			Q_PROPERTY(quint32 boolViewportHeight READ boolViewportHeight WRITE setBoolViewportHeight NOTIFY boolViewportHeightChanged)
			Q_PROPERTY(bool antialiasing READ isAntialiasing WRITE setAntialiasing NOTIFY antialiasingChanged)
			Q_PROPERTY(QColor maxPointerBackgroundColor READ maxPointerBackgroundColor WRITE setMaxPointerBackgroundColor NOTIFY maxPointerBackgroundColorChanged)
			Q_PROPERTY(QColor maxPointerBorderColor READ maxPointerBorderColor WRITE setMaxPointerBorderColor NOTIFY maxPointerBorderColorChanged)
			Q_PROPERTY(QColor minPointerBackgroundColor READ minPointerBackgroundColor WRITE setMinPointerBackgroundColor NOTIFY minPointerBackgroundColorChanged)
			Q_PROPERTY(QColor minPointerBorderColor READ minPointerBorderColor WRITE setMinPointerBorderColor NOTIFY minPointerBorderColorChanged)

		signals:
			void gridWidthChanged(int value);
			void leftIndentChanged(int value);
			void labelsFontChanged(const QFont &value);
			void viewfinderWindowBorderColorChanged(const QColor &value);
			void viewfinderWindowBackgroundColorChanged(const QColor &value);
			void viewfinderWindowTextColorChanged(const QColor &value);
			void viewfinderWindowFontChanged(const QFont &value);
			void boolViewportHeightChanged(quint32 value);
			void antialiasingChanged(bool value);
			void maxPointerBackgroundColorChanged(const QColor &value);
			void maxPointerBorderColorChanged(const QColor &value);
			void minPointerBackgroundColorChanged(const QColor &value);
			void minPointerBorderColorChanged(const QColor &value);
		};
	}
}

#endif // WS_CHART_DEFAULTSERIESRENDERER_H
