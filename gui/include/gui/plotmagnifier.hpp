/*
 * Copyright (c) 2024 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see https://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef PLOTMAGNIFIER_H
#define PLOTMAGNIFIER_H

#include <QwtPlot>
#include <qwt_scale_map.h>
#include "scopy-gui_export.h"

namespace scopy {

class SCOPY_GUI_EXPORT PlotMagnifier : public QObject
{
	Q_OBJECT

public:
	explicit PlotMagnifier(QwtPlot *plot);
	~PlotMagnifier();

	QwtPlot *plot();

	void zoom(double factor, QPointF cursorPos);
	void silentZoom(double factor, QPointF cursorPos);
	void pan(double factor);
	void silentPan(double factor);
	bool isZoomed() const;

	void setBaseRect(const QRectF &rect);
	void setBaseRect();
	QRectF zoomBase() const;
	QRectF getCurrentRect();

	void setEnabled(bool en);
	bool isEnabled() const;

	void setBounded(bool en);
	bool isBounded();

	void setFactor(double factor);
	double getFactor() const;

	void setBlockZoomResetEn(bool en);
	bool isBlockZoomResetEn();

	void setZoomModifier(Qt::KeyboardModifier modifier);
	Qt::KeyboardModifier getZoomModifier();
	void setPanModifier(Qt::KeyboardModifier modifier);
	Qt::KeyboardModifier getPanModifier();

	QwtAxisId getXAxis();
	QwtAxisId getYAxis();
	void setXAxis(QwtAxisId axisId);
	void setYAxis(QwtAxisId axisId);

	void setXAxisEn(bool en);
	void setYAxisEn(bool en);
	bool isXAxisEn() const;
	bool isYAxisEn() const;

	static double scaleToFactor(double scale, QwtAxisId axisId, QwtPlot *plot);
	static double factorToScale(double factor, QwtAxisId axisId, QwtPlot *plot);

Q_SIGNALS:
	void reset();
	void zoomed(double factor, QPointF cursorPos = QPointF());
	void zoomedRect(const QRectF &rect);
	void panned(double factor);
	void pannedRect(const QRectF &rect);

protected:
	virtual bool eventFilter(QObject *object, QEvent *event) QWT_OVERRIDE;
	void panRescale(double factor);
	void zoomRescale(double factor);

private Q_SLOTS:
	void zoomToBase();

private:
	QwtPlot *m_plot;
	QWidget *m_canvas;
	double m_factor;
	QPointF m_cursorPos;
	QRectF m_baseRect;
	QwtAxisId m_xAxis, m_yAxis;
	bool m_xAxisEn, m_yAxisEn;
	bool m_isZoomed;
	bool m_en;
	bool m_bounded;
	bool m_blockReset;
	Qt::KeyboardModifier m_zoomModifier, m_panModifier;
};
} // namespace scopy

#endif // PLOTMAGNIFIER_H
