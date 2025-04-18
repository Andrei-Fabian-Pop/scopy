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

#include "registermapvalues.hpp"

#include "readwrite/iregisterreadstrategy.hpp"
#include "readwrite/iregisterwritestrategy.hpp"
#include "regreadwrite.hpp"

#include <src/readwrite/fileregisterreadstrategy.hpp>
#include <src/readwrite/fileregisterwritestrategy.hpp>
#include <src/readwrite/iioregisterwritestrategy.hpp>

#include <QFile>
#include <QTextStream>
#include <QDebug>

#include <pluginbase/statusbarmanager.h>

using namespace scopy;
using namespace regmap;

RegisterMapValues::RegisterMapValues(QObject *parent)
	: QObject{parent}
{
	registerReadValues = new QMap<uint32_t, uint32_t>();

	m_readConnection =
		QObject::connect(this, &RegisterMapValues::requestRead, this, &RegisterMapValues::getValueOfRegister);
	writeConnection = QObject::connect(this, &RegisterMapValues::requestWrite, this, &RegisterMapValues::readDone);
}

RegisterMapValues::~RegisterMapValues() { delete registerReadValues; }

QMap<uint32_t, uint32_t> *RegisterMapValues::getRegisterReadValues() const { return registerReadValues; }

bool RegisterMapValues::hasValue(uint32_t address) { return registerReadValues->contains(address); }

void RegisterMapValues::readDone(uint32_t address, uint32_t value)
{
	registerReadValues->insert(address, value);
	Q_EMIT registerValueChanged(address, value);
}

uint32_t RegisterMapValues::getValueOfRegister(uint32_t address) { return registerReadValues->value(address); }

void RegisterMapValues::setReadStrategy(IRegisterReadStrategy *readStrategy)
{
	this->readStrategy = readStrategy;
	QObject::disconnect(m_readConnection);
	QObject::connect(this, &RegisterMapValues::requestRead, readStrategy, &IRegisterReadStrategy::read);
	QObject::connect(readStrategy, &IRegisterReadStrategy::readDone, this, &RegisterMapValues::readDone);
}

void RegisterMapValues::setWriteStrategy(IRegisterWriteStrategy *writeStrategy)
{
	this->writeStrategy = writeStrategy;
	QObject::disconnect(writeConnection);
	QObject::connect(this, &RegisterMapValues::requestWrite, writeStrategy, &IRegisterWriteStrategy::write);
	if(readStrategy) {
		QObject::connect(writeStrategy, &IRegisterWriteStrategy::writeSuccess, readStrategy,
				 &IRegisterReadStrategy::read);
	}
}

void RegisterMapValues::registerDump(QString path)
{
	QMap<uint32_t, uint32_t>::iterator mapIterator;

	QFile file(path);
	if(!file.isOpen()) {
		if(!file.open(QIODevice::WriteOnly)) {
			StatusBarManager::pushMessage("Can't open file!", 3000);
		} else {
			QTextStream out(&file);

			for(mapIterator = registerReadValues->begin(); mapIterator != registerReadValues->end();
			    mapIterator++) {
				out << QString::number(mapIterator.key(), 16) << ","
				    << QString::number(mapIterator.value(), 16) << endl;
			}
		}

		file.close();
	} else {
		qDebug() << "File already opened! ";
	}
}

IRegisterReadStrategy *RegisterMapValues::getReadStrategy() const { return readStrategy; }

IRegisterWriteStrategy *RegisterMapValues::getWriteStrategy() const { return writeStrategy; }
