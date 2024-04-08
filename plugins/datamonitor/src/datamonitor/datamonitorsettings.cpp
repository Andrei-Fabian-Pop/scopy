#include "curvestylemenu.hpp"
#include "datamonitorsettings.hpp"
#include "plottimeaxiscontroller.hpp"

#include <QDateTimeEdit>
#include <QDebug>
#include <QScrollArea>
#include <QTimer>
#include <datamonitorstylehelper.hpp>
#include <datamonitorutils.hpp>
#include <menucollapsesection.h>
#include <menucontrolbutton.h>
#include <menusectionwidget.h>
#include <mousewheelwidgetguard.h>
#include <plotchannel.h>

using namespace scopy;
using namespace datamonitor;

Q_LOGGING_CATEGORY(CAT_DATAMONITOR_SETTINGS, "DataMonitorSettings")

DataMonitorSettings::DataMonitorSettings(MonitorPlot *m_plot, QWidget *parent)
	: m_plot(m_plot)
	, QWidget{parent}
{
	mainLayout = new QVBoxLayout(this);
	mainLayout->setMargin(0);
	mainLayout->setSpacing(10);
	setLayout(mainLayout);
}

DataMonitorSettings::~DataMonitorSettings() {}

void DataMonitorSettings::init(QString title, QColor color)
{
	header = new MenuHeaderWidget(title, QPen(color), this);
	mainLayout->addWidget(header);

	settingsBody = new QWidget(this);
	layout = new QVBoxLayout(this);
	layout->setMargin(0);
	layout->setSpacing(10);
	settingsBody->setLayout(layout);

	mainLayout->addLayout(layout);

	QScrollArea *scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(settingsBody);
	mainLayout->addWidget(scrollArea);

	MenuSectionWidget *togglePlotContainer = new MenuSectionWidget(this);

	plotSwitch = new MenuOnOffSwitch("Toggle Plot ", togglePlotContainer, false);
	plotSwitch->onOffswitch()->setChecked(true);

	togglePlotContainer->contentLayout()->addWidget(plotSwitch);
	layout->addWidget(togglePlotContainer);

	connect(plotSwitch->onOffswitch(), &QAbstractButton::toggled, this, &DataMonitorSettings::togglePlot);

	// XAxis settings
	PlotTimeAxisController *plotTimeAxisController = new PlotTimeAxisController(m_plot, this);
	layout->addWidget(plotTimeAxisController);

	// YAxis settings
	layout->addWidget(generateYAxisSettings(this));

	CurveStyleMenu *curveMenu = new CurveStyleMenu(this);

	connect(curveMenu, &CurveStyleMenu::curveThicknessChanged, m_plot, &MonitorPlot::changeCurveThickness);
	connect(curveMenu, &CurveStyleMenu::curveStyleIndexChanged, m_plot, &MonitorPlot::changeCurveStyle);

	layout->addWidget(curveMenu);

	MouseWheelWidgetGuard *mouseWheelWidgetGuard = new MouseWheelWidgetGuard(this);
	mouseWheelWidgetGuard->installEventRecursively(this);

	DataMonitorStyleHelper::DataMonitorSettingsStyle(this);
}

void DataMonitorSettings::plotToggle(bool toggled) { plotSwitch->onOffswitch()->setChecked(toggled); }

void DataMonitorSettings::addMonitorsList(QMap<QString, DataMonitorModel *> *monitorList)
{

	MenuSectionWidget *selectMonitorsWidgets = new MenuSectionWidget(this);

	layout->addWidget(selectMonitorsWidgets);

	selectMonitorsWidgets->contentLayout()->addWidget(new QLabel("Available monitors: "));

	monitorsGroup = new QButtonGroup(this);

	foreach(QString monitor, monitorList->keys()) {
		addMonitor(monitor, monitorList->value(monitor)->getColor());
	}

	QSpacerItem *spacer = new QSpacerItem(10, 10, QSizePolicy::Preferred, QSizePolicy::Expanding);
	layout->addItem(spacer);
}

void DataMonitorSettings::addMonitor(QString monitor, QColor monitorColor)
{
	auto dev = monitor.split(":")[0];

	if(!deviceMap.contains(dev)) {
		generateDeviceSection(dev);
	}

	MenuControlButton *monitorChannel = new MenuControlButton(deviceMap.value(dev));
	monitorChannel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	deviceMap.value(dev)->contentLayout()->addWidget(monitorChannel);
	monitorChannel->setName(monitor);
	monitorChannel->setCheckBoxStyle(MenuControlButton::CS_CIRCLE);
	monitorChannel->setOpenMenuChecksThis(true);
	monitorChannel->setColor(monitorColor);
	monitorChannel->button()->setVisible(false);
	monitorChannel->setCheckable(false);

	monitorsGroup->addButton(monitorChannel);

	// apply hover to the buttons based on the color they have
	monitorChannel->setStyleSheet(QString(":hover{ background-color: %1 ; }").arg(monitorColor.name()));

	connect(monitorChannel, &MenuControlButton::clicked, monitorChannel->checkBox(), &QCheckBox::toggle);

	connect(monitorChannel->checkBox(), &QCheckBox::toggled, this,
		[=, this](bool toggled) { Q_EMIT monitorToggled(toggled, monitor); });

	// when removing the monitor disable all active monitors
	connect(this, &DataMonitorSettings::removeMonitor, monitorChannel, [=, this]() {
		if(monitorChannel->checkBox()->isChecked()) {
			Q_EMIT monitorToggled(false, monitor);
		}
	});
}

void DataMonitorSettings::generateDeviceSection(QString device)
{
	MenuSectionWidget *devMontirosContainer = new MenuSectionWidget(this);

	MenuCollapseSection *devMonitorsSection =
		new MenuCollapseSection(device, MenuCollapseSection::MHCW_NONE, devMontirosContainer);
	devMontirosContainer->contentLayout()->addWidget(devMonitorsSection);
	layout->addWidget(devMontirosContainer);

	devMonitorsSection->header()->setChecked(false);

	deviceMap.insert(device, devMonitorsSection);
}

void DataMonitorSettings::updateTitle(QString title) { header->label()->setText(title); }

void DataMonitorSettings::plotYAxisMinValueUpdate(double value) { m_ymin->setValue(value); }

void DataMonitorSettings::plotYAxisMaxValueUpdate(double value) { m_ymax->setValue(value); }

QWidget *DataMonitorSettings::generateYAxisSettings(QWidget *parent)
{
	MenuSectionWidget *yaxisContainer = new MenuSectionWidget(parent);
	MenuCollapseSection *yAxisSection =
		new MenuCollapseSection("Y-AXIS", MenuCollapseSection::MHCW_NONE, yaxisContainer);

	yaxisContainer->contentLayout()->addWidget(yAxisSection);

	MenuOnOffSwitch *autoscale = new MenuOnOffSwitch(tr("AUTOSCALE"), yAxisSection, false);

	QTimer *m_autoScaleTimer = new QTimer(this);
	m_autoScaleTimer->setInterval(1000);

	yAxisSection->contentLayout()->addWidget(autoscale);

	// Y-MIN-MAX
	QWidget *yMinMax = new QWidget(yAxisSection);
	QHBoxLayout *yMinMaxLayout = new QHBoxLayout(yMinMax);
	yMinMaxLayout->setMargin(0);
	yMinMaxLayout->setSpacing(10);
	yMinMax->setLayout(yMinMaxLayout);

	// TODO replace UM with the unit of measure of main monitor

	m_ymin = new PositionSpinButton(
		{
			{"UM", 1e0},
			{"k", 1e3},
			{"M", 1e6},
			{"G", 1e9},
		},
		"YMin", (double)((long)(-1 << 31)), (double)((long)1 << 31), false, false, yMinMax);

	m_ymin->setValue(DataMonitorUtils::getAxisDefaultMinValue());

	m_ymax = new PositionSpinButton(
		{
			{"UM", 1e0},
			{"k", 1e3},
			{"M", 1e6},
			{"G", 1e9},
		},
		"YMax", (double)((long)(-1 << 31)), (double)((long)1 << 31), false, false, yMinMax);

	m_ymax->setValue(DataMonitorUtils::getAxisDefaultMaxValue());

	yMinMaxLayout->addWidget(m_ymin);
	yMinMaxLayout->addWidget(m_ymax);

	yAxisSection->contentLayout()->addWidget(yMinMax);

	connect(m_autoScaleTimer, &QTimer::timeout, this, [=, this]() {
		m_plot->plotYAxisAutoscale();
		plotYAxisMinValueUpdate(m_plot->getYAxisIntervalMin());
		plotYAxisMaxValueUpdate(m_plot->getYAxisIntervalMax());
	});

	connect(autoscale->onOffswitch(), &QAbstractButton::toggled, yMinMax, [=, this](bool toggled) {
		plotYAxisAutoscale(toggled);
		yMinMax->setEnabled(!toggled);
		if(toggled) {
			m_autoScaleTimer->start();
		} else {
			m_autoScaleTimer->stop();
		}
	});

	connect(m_ymin, &PositionSpinButton::valueChanged, this, [=, this](double value) {
		m_plot->updateYAxisIntervalMin(value);
		m_ymax->setMinValue(value);
	});
	connect(m_ymax, &PositionSpinButton::valueChanged, this, [=, this](double value) {
		m_plot->updateYAxisIntervalMax(value);
		m_ymin->setMaxValue(value);
	});

	return yaxisContainer;
}

bool DataMonitorSettings::eventFilter(QObject *watched, QEvent *event)
{
	if(event->type() == QEvent::Wheel) {
		return true;
	}

	return QWidget::eventFilter(watched, event);
}