#include "swiotadvmenu.hpp"
#include "gui/customSwitch.hpp"
#include "qdebug.h"

using namespace adiscope::gui;


SwiotAdvMenu::SwiotAdvMenu(QWidget *parent)
	: QWidget(parent),
	  m_widget(parent)
{}

SwiotAdvMenu::~SwiotAdvMenu(){
}

void SwiotAdvMenu::addMenuLayout(QHBoxLayout *layout){
	m_menuLayers.push_back(layout);
}

void SwiotAdvMenu::setAttrValues(QMap<QString, QStringList> values){
	m_attrValues=values;
}


QMap<QString, QStringList> SwiotAdvMenu::getAttrValues(){
	return m_attrValues;
}

QVector<QHBoxLayout *> SwiotAdvMenu::getMenuLayers(){
	return m_menuLayers;
}

double SwiotAdvMenu::convertFromRaw(int rawValue){
	double value=0.0;
	if(m_attrValues.contains("offset") && m_attrValues.contains("scale")){
		double offset = m_attrValues["offset"][0].toDouble();
		double scale = m_attrValues["scale"][0].toDouble();
		value = (rawValue + offset) * scale;
	}
	return value;
}

CurrentInLoopMenu::CurrentInLoopMenu(QWidget* parent):SwiotAdvMenu(parent)
{
}

CurrentInLoopMenu::~CurrentInLoopMenu()
{}

void CurrentInLoopMenu::init(){
	//dac code
	QHBoxLayout *dacCodeLayout = new QHBoxLayout();

	dacCodeSpinButton = new PositionSpinButton({
					{"value", 1E0}
					}, "DAC Code", 0,
					8191,
					true, false, m_widget);
	dacCodeLayout->addWidget(dacCodeSpinButton);

	addMenuLayout(dacCodeLayout);

	//dac label
	QHBoxLayout *dacLabelLayout = new QHBoxLayout();

	dacLabel = new QLabel("mA", m_widget);
	dacLabelLayout->addWidget(dacLabel);
	dacLabelLayout->setAlignment(Qt::AlignRight);

	addMenuLayout(dacLabelLayout);
}

void CurrentInLoopMenu::connectSignalsToSlots(){
	connect(dacCodeSpinButton, &PositionSpinButton::valueChanged, this, &CurrentInLoopMenu::dacCodeChanged);

}

void CurrentInLoopMenu::dacCodeChanged(double value){
//	QString attrName("raw");
//	m_attrValues[attrName].clear();
//	m_attrValues[attrName].push_back(QString::number(value));

	double val = convertFromRaw(value);
	dacLabel->clear();
	dacLabel->setText(QString::number(val)+" mA");

//	Q_EMIT attrValuesChanged(attrName);
}

DigitalInMenu::DigitalInMenu(QWidget* parent):SwiotAdvMenu(parent)
{
}

DigitalInMenu::~DigitalInMenu()
{}

void DigitalInMenu::init(){
	//Threshold Mode
	QHBoxLayout *thresholdLayout = new QHBoxLayout();

	thresholdOptions = new QComboBox(m_widget);
	thresholdOptions->addItem(QString("Set between GND and 16V"));
	thresholdOptions->addItem(QString("Set between GND and AVDD"));
	thresholdOptions->setCurrentIndex(0);

	thresholdLayout->addWidget(new QLabel("Threshold Mode ",m_widget));
	thresholdLayout->addWidget(thresholdOptions);

	addMenuLayout(thresholdLayout);

	//Comparator Threshold
	QHBoxLayout *comparatorLayout = new QHBoxLayout();

	comparatorThresholdSpinButton = new PositionSpinButton({
							    {"V",1E0}},
							    "Comparator Threshold",0.0,16,
							    true, false, m_widget);

	comparatorLayout->addWidget(comparatorThresholdSpinButton);

	addMenuLayout(comparatorLayout);

	connectSignalsToSlots();
}

void DigitalInMenu::connectSignalsToSlots(){
	connect(thresholdOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DigitalInMenu::thresholdOptionIndexChanged);
}

void DigitalInMenu::thresholdOptionIndexChanged(){
	auto value = thresholdOptions->currentIndex();
	if (value == 0) {
		comparatorThresholdSpinButton->setMaxValue(16);
	}
	if (value == 1) {
		comparatorThresholdSpinButton->setMaxValue(31);
	}

}

DigitalInLoopMenu::DigitalInLoopMenu(QWidget* parent):SwiotAdvMenu(parent)
{
}

DigitalInLoopMenu::~DigitalInLoopMenu()
{}

void DigitalInLoopMenu::init(){
	//Threshold Mode
	QHBoxLayout *thresholdLayout = new QHBoxLayout();

	thresholdOptions = new QComboBox(m_widget);
	thresholdOptions->addItem(QString("Set between GND and 16V"));
	thresholdOptions->addItem(QString("Set between GND and AVDD"));
	thresholdOptions->setCurrentIndex(0);

	thresholdLayout->addWidget(new QLabel("Threshold Mode ",m_widget),1);
	thresholdLayout->addWidget(thresholdOptions,1);

	addMenuLayout(thresholdLayout);

	//Comparator Threshold
	QHBoxLayout *comparatorLayout = new QHBoxLayout();

	comparatorThresholdSpinButton = new PositionSpinButton({
							    {"V",1E0}},
							    "Comparator Threshold",0.0,16,
							    true, false, m_widget);

	comparatorLayout->addWidget(comparatorThresholdSpinButton);

	addMenuLayout(comparatorLayout);

	//dac code
	QHBoxLayout *dacCodeLayout = new QHBoxLayout();

	dacCodeSpinButton = new PositionSpinButton({
					{"value", 1E0}
					}, "DAC Code", 0,
					8191,
					true, false, m_widget);

	dacCodeLayout->addWidget(dacCodeSpinButton);

	addMenuLayout(dacCodeLayout);

	//dac label
	QHBoxLayout *dacLabelLayout = new QHBoxLayout();

	dacLabel = new QLabel("mA", m_widget);
	dacLabelLayout->addWidget(dacLabel);
	dacLabelLayout->setAlignment(Qt::AlignRight);

	addMenuLayout(dacLabelLayout);

	connectSignalsToSlots();
}

void DigitalInLoopMenu::connectSignalsToSlots(){
	connect(thresholdOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DigitalInLoopMenu::thresholdOptionIndexChanged);
	connect(dacCodeSpinButton, &PositionSpinButton::valueChanged, this, &DigitalInLoopMenu::dacCodeChanged);
}

void DigitalInLoopMenu::dacCodeChanged(double value){
//	QString attrName("raw");
//	m_attrValues[attrName].clear();
//	m_attrValues[attrName].push_back(QString::number(value));

	double val = convertFromRaw(value);
	dacLabel->clear();
	dacLabel->setText(QString::number(val)+" mA");

//	Q_EMIT attrValuesChanged(attrName);
}

void DigitalInLoopMenu::thresholdOptionIndexChanged(){
	auto value = thresholdOptions->currentIndex();
	if (value == 0) {
		comparatorThresholdSpinButton->setMaxValue(16);
	}
	if (value == 1) {
		comparatorThresholdSpinButton->setMaxValue(31);
	}

}

VoltageOutMenu::VoltageOutMenu(QWidget* parent):SwiotAdvMenu(parent)
{
}

VoltageOutMenu::~VoltageOutMenu()
{}

void VoltageOutMenu::init(){
	//dac code
	QHBoxLayout *dacCodeLayout = new QHBoxLayout();

	dacCodeSpinButton = new PositionSpinButton({
					{"value", 1E0}
					}, "DAC Code", 0,
					8191,
					true, false, m_widget);
	dacCodeLayout->addWidget(dacCodeSpinButton);

	addMenuLayout(dacCodeLayout);

	//dac label
	QHBoxLayout *dacLabelLayout = new QHBoxLayout();

	dacLabel = new QLabel("V", m_widget);
	dacLabelLayout->addWidget(dacLabel);
	dacLabelLayout->setAlignment(Qt::AlignRight);
	addMenuLayout(dacLabelLayout);

	//slew
	QHBoxLayout *slewLayout = new QHBoxLayout();

	slewOptions = new QComboBox(m_widget);
	slewOptions->addItem(QString("Disable"));
	slewOptions->addItem(QString("Enable"));
	slewOptions->setCurrentIndex(0);

	slewLayout->addWidget(new QLabel("Slew",m_widget),1);
	slewLayout->addWidget(slewOptions,1);

	addMenuLayout(slewLayout);

	//slew step
	QHBoxLayout *slewStepLayout = new QHBoxLayout();

	slewStepOptions = new QComboBox(m_widget);

	setAvailableOptions(slewStepOptions, "slew_step_available");
	slewStepOptions->setCurrentIndex(0);

	slewStepLayout->addWidget(new QLabel("Slew Step Size",m_widget),1);
	slewStepLayout->addWidget(slewStepOptions,1);

	addMenuLayout(slewStepLayout);

	//slew rate
	QHBoxLayout *slewRateLayout = new QHBoxLayout();

	slewRateOptions = new QComboBox(m_widget);

	setAvailableOptions(slewRateOptions, "slew_rate_available");
	slewRateOptions->setCurrentIndex(0);

	slewRateLayout->addWidget(new QLabel("Slew Rate",m_widget),1);
	slewRateLayout->addWidget(slewRateOptions,1);

	addMenuLayout(slewRateLayout);

	connectSignalsToSlots();
}

void VoltageOutMenu::connectSignalsToSlots(){
	connect(dacCodeSpinButton, &PositionSpinButton::valueChanged, this, &VoltageOutMenu::dacCodeChanged);
	connect(slewStepOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VoltageOutMenu::slewStepIndexChanged);
	connect(slewRateOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VoltageOutMenu::slewRateIndexChanged);
	connect(slewOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &VoltageOutMenu::slewIndexChanged);
}

void VoltageOutMenu::setAvailableOptions(QComboBox *list, QString attrName){
	QStringList availableValues = m_attrValues[attrName];
	QString unitMeasure = availableValues.takeLast();

	qDebug()<<unitMeasure;
	for(const auto& slewValue : availableValues ){
		list->addItem(slewValue + unitMeasure);
	}
}

void VoltageOutMenu::dacCodeChanged(double value){
	QString attrName("raw");
	m_attrValues[attrName].clear();
	m_attrValues[attrName].push_back(QString::number((int)value));

	double val = convertFromRaw(value);
	dacLabel->clear();
	dacLabel->setText(QString::number(val)+" V");

	Q_EMIT attrValuesChanged(attrName);
}

void VoltageOutMenu::slewStepIndexChanged(int idx){
	QString attrName = "slew_step";
	const auto& slewStep = m_attrValues["slew_step_available"][idx];

	m_attrValues[attrName].clear();
	m_attrValues[attrName].push_back(slewStep);

	Q_EMIT attrValuesChanged(attrName);
}

void VoltageOutMenu::slewRateIndexChanged(int idx){
	QString attrName = "slew_rate";
	const auto& slewStep = m_attrValues["slew_rate_available"][idx];

	m_attrValues[attrName].clear();
	m_attrValues[attrName].push_back(slewStep);

	Q_EMIT attrValuesChanged(attrName);
}

void VoltageOutMenu::slewIndexChanged(int idx){
	QString attrName = "slew_en";

	m_attrValues[attrName].clear();
	if(idx==0){
		m_attrValues[attrName].push_back(QString("0"));
	}
	else{
		m_attrValues[attrName].push_back(QString("1"));
	}

	Q_EMIT attrValuesChanged(attrName);
}



CurrentOutMenu::CurrentOutMenu(QWidget* parent):SwiotAdvMenu(parent)
{
}

CurrentOutMenu::~CurrentOutMenu()
{}

void CurrentOutMenu::init(){
	//dac code
	QHBoxLayout *dacCodeLayout = new QHBoxLayout();

	dacCodeSpinButton = new PositionSpinButton({
					{"value", 1E0}
					}, "DAC Code", 0,
					8196,
					true, false, m_widget);
	dacCodeLayout->addWidget(dacCodeSpinButton);

	addMenuLayout(dacCodeLayout);

	//dac label
	QHBoxLayout *dacLabelLayout = new QHBoxLayout();

	dacLabel = new QLabel("mA", m_widget);
	dacLabelLayout->addWidget(dacLabel);
	dacLabelLayout->setAlignment(Qt::AlignRight);
	addMenuLayout(dacLabelLayout);

	//slew
	QHBoxLayout *slewLayout = new QHBoxLayout();

	slewOptions = new QComboBox(m_widget);
	slewOptions->addItem(QString("Disable"));
	slewOptions->addItem(QString("Enable"));
	slewOptions->setCurrentIndex(0);

	slewLayout->addWidget(new QLabel("Slew",m_widget),1);
	slewLayout->addWidget(slewOptions,1);

	addMenuLayout(slewLayout);

	//slew step
	QHBoxLayout *slewStepLayout = new QHBoxLayout();

	slewStepOptions = new QComboBox(m_widget);

	setAvailableOptions(slewStepOptions, "slew_step_available");
	slewStepOptions->setCurrentIndex(0);

	slewStepLayout->addWidget(new QLabel("Slew Step Size",m_widget),1);
	slewStepLayout->addWidget(slewStepOptions,1);

	addMenuLayout(slewStepLayout);

	//slew rate
	QHBoxLayout *slewRateLayout = new QHBoxLayout();

	slewRateOptions = new QComboBox(m_widget);

	setAvailableOptions(slewRateOptions, "slew_rate_available");
	slewRateOptions->setCurrentIndex(0);

	slewRateLayout->addWidget(new QLabel("Slew Rate",m_widget),1);
	slewRateLayout->addWidget(slewRateOptions,1);

	addMenuLayout(slewRateLayout);

	connectSignalsToSlots();
}

void CurrentOutMenu::connectSignalsToSlots(){
	connect(dacCodeSpinButton, &PositionSpinButton::valueChanged, this, &CurrentOutMenu::dacCodeChanged);
	connect(slewStepOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CurrentOutMenu::slewStepIndexChanged);
	connect(slewRateOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CurrentOutMenu::slewRateIndexChanged);
	connect(slewOptions, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CurrentOutMenu::slewIndexChanged);
}

void CurrentOutMenu::setAvailableOptions(QComboBox *list, QString attrName){
	QStringList availableValues = m_attrValues[attrName];
	QString unitMeasure = availableValues.takeLast();

	for(const auto& slewValue : availableValues ){
		list->addItem(slewValue + unitMeasure);
	}
}

void CurrentOutMenu::dacCodeChanged(double value){
	QString attrName("raw");
	m_attrValues[attrName].clear();
	m_attrValues[attrName].push_back(QString::number((int)value));

	double val = convertFromRaw(value);
	dacLabel->clear();
	dacLabel->setText(QString::number(val)+" mA");

	Q_EMIT attrValuesChanged(attrName);
}

void dacCodeChanged(){

}

void CurrentOutMenu::slewStepIndexChanged(int idx){
	QString attrName = "slew_step";
	const auto& slewStep = m_attrValues["slew_step_available"][idx];

	m_attrValues[attrName].clear();
	m_attrValues[attrName].push_back(slewStep);

	Q_EMIT attrValuesChanged(attrName);
}

void CurrentOutMenu::slewRateIndexChanged(int idx){
	QString attrName = "slew_rate";
	const auto& slewStep = m_attrValues["slew_rate_available"][idx];

	m_attrValues[attrName].clear();
	m_attrValues[attrName].push_back(slewStep);

	Q_EMIT attrValuesChanged(attrName);
}

void CurrentOutMenu::slewIndexChanged(int idx){
	QString attrName = "slew_en";

	m_attrValues[attrName].clear();
	if(idx==0){
		m_attrValues[attrName].push_back(QString("0"));
	}
	else{
		m_attrValues[attrName].push_back(QString("1"));
	}

	Q_EMIT attrValuesChanged(attrName);
}

DigitalOutMenu::DigitalOutMenu(QWidget* parent):SwiotAdvMenu(parent)
{
}

DigitalOutMenu::~DigitalOutMenu()
{}

void DigitalOutMenu::init(){
	//GPO Mode
	QHBoxLayout *gpoModeLayout = new QHBoxLayout();

	gpoModeOptions = new QComboBox(m_widget);
	gpoModeOptions->addItem(QString("100k pulldown resistor"));
	gpoModeOptions->addItem(QString("Set using GPO_DATA bit"));
	gpoModeOptions->addItem(QString("GPO_PAR_DATA"));
	gpoModeOptions->addItem(QString("Comparator Output"));
	gpoModeOptions->addItem(QString("High impedance state"));
	gpoModeOptions->setCurrentIndex(0);

	gpoModeLayout->addWidget(new QLabel("GPO Mode Config  ", m_widget),1);
	gpoModeLayout->addWidget(gpoModeOptions,1);

	addMenuLayout(gpoModeLayout);

	//GPO Data
	QHBoxLayout *gpoDataLayout = new QHBoxLayout();

	gpoDataSwitch = new CustomSwitch(m_widget);
	gpoDataSwitch->setChecked(true);

	gpoDataLayout->addWidget(new QLabel("GPO Data", m_widget));
	gpoDataLayout->addWidget(gpoDataSwitch);
	gpoDataLayout->setAlignment(gpoDataSwitch,Qt::AlignRight);

	addMenuLayout(gpoDataLayout);
}

void DigitalOutMenu::connectSignalsToSlots(){

}

WithoutAdvSettings::WithoutAdvSettings(QWidget* parent):SwiotAdvMenu(parent)
{}

WithoutAdvSettings::~WithoutAdvSettings()
{}

void WithoutAdvSettings::init(){
	QHBoxLayout *msgLayout = new QHBoxLayout();
	msgLayout->addWidget(new QLabel("No advanced settings available",m_widget));
	addMenuLayout(msgLayout);
}

void WithoutAdvSettings::connectSignalsToSlots(){
}


