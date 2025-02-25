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

#include "m2ktool.hpp"
#include "utils.h"

#include <QMimeData>
#include <iostream>
#include <style.h>

using namespace scopy;
using namespace scopy::m2k;

M2kTool::M2kTool(ToolMenuEntry *tme, ApiObject *api, const QString &name, QWidget *parent)
	: QWidget(parent)
	, api(api)
	, name(name)
	, saveOnExit(true)
	, isDetached(false)
	, m_running(false)
	, window(nullptr)
	, tme(tme)
{
	tme->setEnabled(true);
	p = Preferences::GetInstance();
	connect(p, &Preferences::preferenceChanged, this, &M2kTool::readPreferences);
	M2kTool::readPreferences();
	Style::GetInstance()->setM2KStylesheet(this);
}

M2kTool::~M2kTool()
{
	disconnect(p, &Preferences::preferenceChanged, this, &M2kTool::readPreferences);

	tme->setRunning(false);
	tme->setEnabled(false);
}

const QString &M2kTool::getName() { return name; }

void M2kTool::setName(const QString &name) { this->name = name; }

void M2kTool::settingsLoaded() {}

ApiObject *M2kTool::getApi() { return api; }

void M2kTool::readPreferences() { saveOnExit = p->get("general_save_on_exit").toBool(); }

ToolMenuEntry *M2kTool::getTme() const { return tme; }

void M2kTool::run() {}
void M2kTool::stop() {}
void M2kTool::single() {}
bool M2kTool::isRunning() { return m_running; }
