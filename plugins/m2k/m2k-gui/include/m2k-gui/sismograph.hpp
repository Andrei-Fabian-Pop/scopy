/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see http://www.github.com/analogdevicesinc/scopy).
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SISMOGRAPH_HPP
#define SISMOGRAPH_HPP

#include "scopy-m2k-gui_export.h"
#include "autoScaler.hpp"
#include "customqwtscaledraw.hpp"

#include <QMap>
#include <QVector>
#include <QWidget>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

namespace scopy {
class SCOPY_M2K_GUI_EXPORT Sismograph : public QwtPlot
{
	Q_OBJECT

	Q_PROPERTY(int numSamples READ getNumSamples WRITE setNumSamples)

	Q_PROPERTY(double sampleRate READ getSampleRate WRITE setSampleRate)

	Q_PROPERTY(PlotDirection plotDirection READ getPlotDirection WRITE setPlotDirection)

public:
	explicit Sismograph(QWidget *parent = nullptr);
	~Sismograph();

	enum PlotDirection : bool
	{
		LEFT_TO_RIGHT = false,
		RIGHT_TO_LEFT = true
	};

	int getNumSamples() const;
	void setNumSamples(int num);

	Sismograph::PlotDirection getPlotDirection() const;
	void setPlotDirection(PlotDirection plotDirection);

	double getSampleRate() const;
	void setSampleRate(double rate);

	void setUnitOfMeasure(QString unitOfMeasureName, QString unitOfMeasureSymbol);
	void setPlotAxisXTitle(const QString &title);

	bool getAutoscale() const;
	void setAutoscale(bool newAutoscale);
	void addScale(double x1, double x2, int maxMajorSteps, int maxMinorSteps, double stepSize = 0.0);

public Q_SLOTS:
	void plot(double sample);
	void reset();
	void setColor(const QColor &color);
	void updateScale(const QwtScaleDiv);
	void updateYScale(double max, double min);
	void setLineWidth(qreal width);
	void setLineStyle(Qt::PenStyle lineStyle);
	void setHistoryDuration(double time);

private:
	QwtPlotCurve curve;
	unsigned int numSamples;
	double sampleRate;
	double interval;
	AutoScaler *scaler;
	double m_currentScale;
	QString m_unitOfMeasureName;
	QString m_unitOfMeasureSymbol;
	CustomQwtScaleDraw *scaleLabel;
	double m_currentMaxValue;

	QVector<double> ydata;
	QVector<double> xdata;

	void updateScale();
	double findMaxInFifo();
	bool autoscale;
	enum PlotDirection plotDirection;

Q_SIGNALS:
	void dataChanged(std::vector<double> data);
};
} // namespace scopy

#endif /* SISMOGRAPH_HPP */
