/*
 * Copyright (c) 2023 Analog Devices Inc.
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
 */

#include "max14906/diosettingstab.h"
#include <gui/widgets/menucollapsesection.h>
#include <gui/widgets/menuheader.h>
#include <gui/widgets/menusectionwidget.h>
#include <style.h>

using namespace scopy::swiot;
using namespace scopy::gui;

DioSettingsTab::DioSettingsTab(QWidget *parent)
	: QWidget(parent)
{
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	QVBoxLayout *layout = new QVBoxLayout(this);
	setLayout(layout);
	layout->setSpacing(10);
	layout->setContentsMargins(0, 0, 0, 0);

	MenuHeaderWidget *header = new MenuHeaderWidget(
		"MAX14906", QPen(Style::getAttribute(json::theme::interactive_primary_idle)), this);
	MenuSectionWidget *plotSettingsContainer = new MenuSectionWidget(this);
	MenuCollapseSection *plotTimespanSection = new MenuCollapseSection("PLOT", MenuCollapseSection::MHCW_NONE,
									   MenuCollapseSection::MHW_BASEWIDGET, this);
	plotTimespanSection->setLayout(new QVBoxLayout());
	plotTimespanSection->contentLayout()->setSpacing(10);
	plotTimespanSection->contentLayout()->setMargin(0);

	QLabel *label = new QLabel(this);
	label->setText("Polling at 1 sample/second");
	Style::setStyle(label, style::properties::label::menuSmall);

	// timespan
	m_maxSpinButton = new MenuSpinbox(tr("Timespan"), 10, "s", 1, 300, true, false, this);
	m_maxSpinButton->setIncrementMode(MenuSpinbox::IS_FIXED);
	m_maxSpinButton->setScaleRange(1, 1);

	connect(m_maxSpinButton, &MenuSpinbox::valueChanged, this,
		[this]() { Q_EMIT timeValueChanged(m_maxSpinButton->value()); });

	plotTimespanSection->contentLayout()->addWidget(label);
	plotTimespanSection->contentLayout()->addWidget(m_maxSpinButton);

	plotSettingsContainer->contentLayout()->addWidget(plotTimespanSection);
	layout->addWidget(header);
	layout->addWidget(plotSettingsContainer);
	layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));
}

DioSettingsTab::~DioSettingsTab() {}

double DioSettingsTab::getTimeValue() const { return m_maxSpinButton->value(); }

#include "moc_diosettingstab.cpp"
