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

#include "measurementcontroller.h"

#include "gui/widgets/measurementlabel.h"
#include "measure.h"
#include "measurementselector.h"

#include <QLoggingCategory>

#include <gui/stylehelper.h>
#include <gui/widgets/menucollapsesection.h>
#include <gui/widgets/menucombo.h>
#include <gui/widgets/menulineedit.h>
#include <gui/widgets/menuonoffswitch.h>
#include <gui/widgets/menusectionwidget.h>

Q_LOGGING_CATEGORY(CAT_MEASUREMENT_CONTROLLER, "MeasurementController");

namespace scopy::adc {

MeasurementController::MeasurementController(QPen pen, MeasureModel *msr, QObject *parent)
	: QObject(parent)
	, m_measure(msr)
	, m_pen(pen)
{
	connect(m_measure, &MeasureModel::newMeasurementsAvailable, this, [=]() {
		for(auto lbl : qAsConst(m_measureLabels)) {
			lbl->setValue(m_measure->measurement(lbl->name())->value());
		}
		for(auto lbl : qAsConst(m_statsLabels)) {
			auto stat = m_measure->measurement(lbl->name())->stat();
			lbl->setValue(stat.average(), stat.min(), stat.max());
		}
	});
}

MeasurementController::~MeasurementController() {}

StatsLabel *MeasurementController::findStatsLabel(QString name)
{
	for(auto &lbl : m_statsLabels) {
		if(lbl->name() == name) {
			return lbl;
		}
	}
	return nullptr;
}

MeasurementLabel *MeasurementController::findMeasurementLabel(QString name)
{
	for(auto &lbl : m_measureLabels) {
		if(lbl->name() == name) {
			return lbl;
		}
	}
	return nullptr;
}

QWidget *MeasurementController::enableMeasurement(QString name)
{
	for(int i = 0; i < m_availableMeasurements.count(); i++) {
		auto meas = m_availableMeasurements[i];
		if(meas.name == name) {

			if(findMeasurementLabel(name) != nullptr) {
				qWarning() << "Measurement label already added";
				return nullptr;
			}
			m_measure->measurement(name)->setEnabled(true);
			MeasurementLabel *lbl = new MeasurementLabel();
			lbl->setIdx(i);
			lbl->setName(meas.name);
			lbl->setUnit(meas.unit);
			lbl->setColor(m_pen.color());
			if(meas.formatter == "time") {
				auto tfp = new TimePrefixFormatter(lbl);
				tfp->setTwoDecimalMode(false);
				lbl->setMeasurementValueFormatter(tfp);
			} else if(meas.formatter == "metric") {
				auto mfp = new MetricPrefixFormatter(lbl);
				mfp->setTwoDecimalMode(false);
				lbl->setMeasurementValueFormatter(mfp);
			}

			m_measureLabels.append(lbl);
			qInfo() << "Measurement " << name << "added";
			Q_EMIT measurementEnabled(lbl);
			return lbl;
		}
	}
	return nullptr;
}

void MeasurementController::disableMeasurement(QString name)
{
	auto lbl = findMeasurementLabel(name);
	m_measure->measurement(name)->setEnabled(false);
	Q_EMIT measurementDisabled(lbl);

	if(lbl == nullptr) {
		qWarning() << "Measurement label was not added to the panel";
		return;
	}
	m_measureLabels.removeAll(lbl);
	delete lbl;
	qInfo() << "Measurement " << name << "removed";
}

QWidget *MeasurementController::enableStats(QString name)
{
	for(int i = 0; i < m_availableMeasurements.count(); i++) {
		auto meas = m_availableMeasurements[i];
		if(meas.name == name) {

			if(findStatsLabel(name) != nullptr) {
				qWarning() << "Stat label already added";
				return nullptr;
			}

			m_measure->measurement(name)->clearStat();
			m_measure->measurement(name)->setStatEnabled(true);
			StatsLabel *lbl = new StatsLabel();
			lbl->setIdx(i);
			lbl->setName(meas.name);
			lbl->setUnit(meas.unit);
			lbl->setColor(m_pen.color());
			if(meas.formatter == "time") {
				lbl->setMeasurementValueFormatter(new TimePrefixFormatter(lbl));
			} else if(meas.formatter == "metric") {
				lbl->setMeasurementValueFormatter(new MetricPrefixFormatter(lbl));
			}

			m_statsLabels.append(lbl);
			qInfo() << "Stat " << name << "added";
			Q_EMIT statsEnabled(lbl);
			return lbl;
		}
	}
	return nullptr;
}

void MeasurementController::disableStats(QString name)
{
	auto lbl = findStatsLabel(name);
	m_measure->measurement(name)->setStatEnabled(false);
	Q_EMIT statsDisabled(lbl);

	if(lbl == nullptr) {
		qWarning() << "Stat label was not added to the panel";
		return;
	}
	m_statsLabels.removeAll(lbl);
	delete lbl;
	qInfo() << "Stat " << name << "removed";
}

void MeasurementController::addMeasurement(MeasurementInfo v) { m_availableMeasurements.push_back(v); }

QList<MeasurementInfo> MeasurementController::availableMeasurements() const { return m_availableMeasurements; }

TimeChannelMeasurementController::TimeChannelMeasurementController(TimeMeasureModel *msr, QPen m_pen, QObject *parent)
	: MeasurementController(m_pen, msr, parent)
{

	addMeasurement({"Period", ":/gui/icons/measurements/period.svg", "", "time", "Horizontal"});
	addMeasurement({"Frequency", ":/gui/icons/measurements/frequency.svg", "Hz", "metric", "Horizontal"});
	addMeasurement({"Min", ":/gui/icons/measurements/min.svg", "", "metric", "Vertical"});
	addMeasurement({"Max", ":/gui/icons/measurements/max.svg", "", "metric", "Vertical"});
	addMeasurement({"Peak-peak", ":/gui/icons/measurements/peak_to_peak.svg", "", "metric", "Vertical"});
	addMeasurement({"Cycle Mean", ":/gui/icons/measurements/cycle_mean.svg", "", "metric", "Vertical"});
	addMeasurement({"RMS", ":/gui/icons/measurements/rms.svg", "", "metric", "Vertical"});
	addMeasurement({"Cycle RMS", ":/gui/icons/measurements/cycle_rms.svg", "", "metric", "Vertical"});
	addMeasurement({"AC RMS", ":/gui/icons/measurements/rms.svg", "", "metric", "Vertical"});
	addMeasurement({"Area", ":/gui/icons/measurements/area.svg", "Vs", "metric", "Vertical"});
	addMeasurement({"Cycle Area", ":/gui/icons/measurements/cycle_area.svg", "Vs", "metric", "Vertical"});
	addMeasurement({"Low", ":/gui/icons/measurements/low.svg", "", "metric", "Vertical"});
	addMeasurement({"High", ":/gui/icons/measurements/high.svg", "", "metric", "Vertical"});
	addMeasurement({"Amplitude", ":/gui/icons/measurements/amplitude.svg", "", "metric", "Vertical"});
	addMeasurement({"Middle", ":/gui/icons/measurements/middle.svg", "", "metric", "Vertical"});
	addMeasurement({"+Over", ":/gui/icons/measurements/p_overshoot.svg", "%", "metric", "Vertical"});
	addMeasurement({"-Over", ":/gui/icons/measurements/n_overshoot.svg", "%", "metric", "Vertical"});
	addMeasurement({"Rise", ":/gui/icons/measurements/rise_time.svg", "s", "metric", "Horizontal"});
	addMeasurement({"Fall", ":/gui/icons/measurements/fall_time.svg", "s", "metric", "Horizontal"});
	addMeasurement({"+Width", ":/gui/icons/measurements/p_width.svg", "s", "metric", "Horizontal"});
	addMeasurement({"-Width", ":/gui/icons/measurements/n_width.svg", "s", "metric", "Horizontal"});
	addMeasurement({"+Duty", ":/gui/icons/measurements/p_duty.svg", "%", "metric", "Horizontal"});
	addMeasurement({"-Duty", ":/gui/icons/measurements/n_duty.svg", "%", "metric", "Horizontal"});
}

TimeMeasureManager::TimeMeasureManager(QObject *parent) {}

TimeMeasureManager::~TimeMeasureManager() {}

void TimeMeasureManager::initMeasure(QPen m_pen)
{
	m_measureModel = new TimeMeasureModel(nullptr, 0, this);
	m_measureController = new TimeChannelMeasurementController(m_measureModel, m_pen, this);

	connect(m_measureController, &TimeChannelMeasurementController::measurementEnabled, this,
		&TimeMeasureManager::enableMeasurement);
	connect(m_measureController, &TimeChannelMeasurementController::measurementDisabled, this,
		&TimeMeasureManager::disableMeasurement);
	connect(m_measureController, &TimeChannelMeasurementController::statsEnabled, this,
		&TimeMeasureManager::enableStat);
	connect(m_measureController, &TimeChannelMeasurementController::statsDisabled, this,
		&TimeMeasureManager::disableStat);
}

QWidget *TimeMeasureManager::createMeasurementMenu(QWidget *parent)
{

	QWidget *w = new QWidget(parent);
	QVBoxLayout *lay = new QVBoxLayout(w);
	w->setLayout(lay);
	lay->setSpacing(6);
	lay->setMargin(0);

	QWidget *hMeasure = createMeasurementMenuSection("HORIZONTAL", w);
	QWidget *vMeasure = createMeasurementMenuSection("VERTICAL", w);
	lay->addWidget(hMeasure);
	lay->addWidget(vMeasure);
	return w;
}

MeasurementController *TimeMeasureManager::getController() { return m_measureController; }

MeasureModel *TimeMeasureManager::getModel() { return m_measureModel; }

QWidget *TimeMeasureManager::createMeasurementMenuSection(QString category, QWidget *parent)
{

	auto m_measureController = getController();
	MenuSectionCollapseWidget *measureSection =
		new MenuSectionCollapseWidget("MEASUREMENT " + category, MenuCollapseSection::MHCW_ARROW,
					      MenuCollapseSection::MHW_BASEWIDGET, parent);
	QWidget *headerWidget = new QWidget(measureSection);
	QHBoxLayout *headerLay = new QHBoxLayout(headerWidget);
	headerLay->setMargin(0);

	headerWidget->setLayout(headerLay);
	QLabel *lblName = new QLabel("NAME");
	QLabel *lblMeasure = new QLabel("MEASURE");
	QLabel *lblStat = new QLabel("STAT");
	headerLay->addWidget(lblName);
	headerLay->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding));
	headerLay->addWidget(lblMeasure);
	headerLay->addWidget(lblStat);
	measureSection->contentLayout()->addWidget(headerWidget);

	QWidget *scrollWidget = new QWidget(measureSection);
	measureSection->contentLayout()->addWidget(scrollWidget);
	QScrollArea *measureScroll = new QScrollArea(scrollWidget);
	MeasurementSelector *measureSelector = new MeasurementSelector();
	measureSection->contentLayout()->addWidget(measureScroll);
	measureScroll->setWidget(measureSelector);
	measureScroll->setWidgetResizable(true);

	measureScroll->setFixedHeight(150);

	for(auto &meas : m_measureController->availableMeasurements()) {
		if(meas.type.toUpper() == category.toUpper()) {
			measureSelector->addMeasurement(meas.name, meas.icon);
			connect(measureSelector->measurement(meas.name)->measureCheckbox(), &QCheckBox::toggled,
				[=](bool b) {
					if(b)
						m_measureController->enableMeasurement(meas.name);
					else
						m_measureController->disableMeasurement(meas.name);
				});

			connect(measureSelector->measurement(meas.name)->statsCheckbox(), &QCheckBox::toggled,
				[=](bool b) {
					if(b)
						m_measureController->enableStats(meas.name);
					else
						m_measureController->disableStats(meas.name);
				});
		}
	}
	measureSection->setCollapsed(true);

	connect(this, &MeasureManagerInterface::toggleAllMeasurement, measureSelector,
		&MeasurementSelector::toggleAllMeasurement);
	connect(this, &MeasureManagerInterface::toggleAllStats, measureSelector, &MeasurementSelector::toggleAllStats);

	return measureSection;
}

/*

static const std::map<int, QString> icons_spect = {
	{M2kMeasure::NOISE_FLOOR, ":/gui/icons/measurements/period.svg"},
	{M2kMeasure::SINAD, ":/gui/icons/measurements/frequency.svg"},
	{M2kMeasure::SNR, ":/gui/icons/measurements/frequency.svg"},
	{M2kMeasure::THD, ":/gui/icons/measurements/frequency.svg"},
	{M2kMeasure::THDN, ":/gui/icons/measurements/frequency.svg"},
	{M2kMeasure::SFDR, ":/gui/icons/measurements/frequency.svg"},
	};
*/

} // namespace scopy::adc

#include "moc_measurementcontroller.cpp"
