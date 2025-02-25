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

#include "grtimesinkcomponent.h"
#include <gr-util/grsignalpath.h>
#include <QLoggingCategory>
#include "iioutil/iiopingtask.h"
#include <pluginbase/preferences.h>

Q_LOGGING_CATEGORY(CAT_GRTIMESINKCOMPONENT, "GRTimeSinkComponent")
using namespace scopy;
using namespace scopy::adc;
using namespace scopy::grutil;

GRTimeSinkComponent::GRTimeSinkComponent(QString name, GRTopBlockNode *t, QObject *parent)
	: QObject(parent)
{
	m_node = t;
	m_sync = m_node->sync();
	m_top = t->src();
	m_name = name;
	m_singleShot = false;
	m_syncMode = false;
	m_armed = false;
	init();
	m_sync->addInstrument(this);
}

GRTimeSinkComponent::~GRTimeSinkComponent() { m_sync->removeInstrument(this); }

void GRTimeSinkComponent::connectSignalPaths()
{
	QList<GRSignalPath *> sigpaths;

	// for through grdevices - get sampleRate;
	std::unique_lock lock(refillMutex);
	for(auto &sigpath : m_top->signalPaths()) {
		qDebug(CAT_GRTIMESINKCOMPONENT) << "Trying " << sigpath->name();
		if(!sigpath->enabled())
			continue;
		if(!sigpath->name().startsWith(m_name))
			continue;
		sigpaths.append(sigpath);
		qDebug(CAT_GRTIMESINKCOMPONENT) << "Appended " << sigpath->name();
	}

	time_sink = time_sink_f::make(m_samplingInfo.plotSize, m_samplingInfo.bufferSize, m_samplingInfo.sampleRate,
				      m_name.toStdString(), sigpaths.count());
	time_sink->setRollingMode(m_samplingInfo.rollingMode);
	time_sink->setSingleShot(m_singleShot);

	int index = 0;
	time_channel_map.clear();

	for(GRChannel *gr : qAsConst(m_channels)) {
		GRSignalPath *sigPath = gr->sigpath();
		if(sigPath->enabled()) {
			// connect end of signal path to time_sink input
			m_top->connect(sigPath->getGrEndPoint(), 0, time_sink, index);
			// map signal path to time_sink input
			time_channel_map.insert(sigPath->name(), index);

			index++;
		}
	}
}

void GRTimeSinkComponent::tearDownSignalPaths()
{
	setData(true);
	qInfo() << "TEARING DOWN";
}

size_t GRTimeSinkComponent::updateData()
{
	std::unique_lock lock(refillMutex);
	if(!time_sink)
		return false;
	uint64_t new_samples = time_sink->updateData();
	return new_samples;
}

bool GRTimeSinkComponent::finished() { return (time_sink) ? time_sink->finishedAcquisition() : false; }

void GRTimeSinkComponent::setData(bool copy)
{
	int index = 0;

	for(GRChannel *gr : qAsConst(m_channels)) {
		int index = time_channel_map.value(gr->sigpath()->name(), -1);
		if(index == -1)
			continue;

		const float *xdata = time_sink->time().data();
		const float *ydata = time_sink->data()[index].data();
		const size_t size = time_sink->data()[index].size();

		gr->onNewData(xdata, ydata, size, copy);
	}
}

void GRTimeSinkComponent::setSingleShot(bool b)
{
	m_singleShot = b;
	if(time_sink) {
		time_sink->setSingleShot(m_singleShot);
	}
}

SamplingInfo GRTimeSinkComponent::samplingInfo() { return m_samplingInfo; }

void GRTimeSinkComponent::setSamplingInfo(SamplingInfo p)
{
	m_samplingInfo = p;
	m_top->setVLen(m_samplingInfo.bufferSize);
	if(time_sink) {
		time_sink->setRollingMode(m_samplingInfo.rollingMode);
		if(m_armed)
			Q_EMIT requestRebuild();
	}
}

void GRTimeSinkComponent::onArm()
{
	connect(this, &GRTimeSinkComponent::requestRebuild, m_top, &GRTopBlock::rebuild, Qt::QueuedConnection);
	connect(m_top, SIGNAL(builtSignalPaths()), this, SLOT(connectSignalPaths()));
	connect(m_top, SIGNAL(teardownSignalPaths()), this, SLOT(tearDownSignalPaths()));
	connect(m_top, SIGNAL(started()), this, SIGNAL(ready()));
	connect(m_top, SIGNAL(aboutToStop()), this, SIGNAL(finish()));
	Q_EMIT arm();
	m_armed = true;
}

void GRTimeSinkComponent::onDisarm()
{
	m_armed = false;
	Q_EMIT disarm();
	disconnect(this, &GRTimeSinkComponent::requestRebuild, m_top, &GRTopBlock::rebuild);
	disconnect(m_top, SIGNAL(builtSignalPaths()), this, SLOT(connectSignalPaths()));
	disconnect(m_top, SIGNAL(teardownSignalPaths()), this, SLOT(tearDownSignalPaths()));
	disconnect(m_top, SIGNAL(started()), this, SIGNAL(ready()));
	disconnect(m_top, SIGNAL(aboutToStop()), this, SIGNAL(finish()));
}

void GRTimeSinkComponent::init()
{
	m_samplingInfo.sampleRate = 1;
	m_samplingInfo.bufferSize = 32;
	m_samplingInfo.plotSize = 32;
}

void GRTimeSinkComponent::deinit() { qDebug(CAT_GRTIMESINKCOMPONENT) << "Deinit"; }

bool GRTimeSinkComponent::start()
{
	bool ok;
	double timeout = Preferences::get("adc_acquisition_timeout").toDouble(&ok);
	if(!ok)
		timeout = 1000;

	iio_context_set_timeout(m_node->ctx(), 1000);
	if(!IIOPingTask::pingCtx(m_node->ctx()))
		return false;
	iio_context_set_timeout(m_node->ctx(), timeout);
	m_sync->setBufferSize(this, m_samplingInfo.bufferSize);
	m_sync->setSingleShot(this, m_singleShot);
	m_top->setVLen(m_samplingInfo.bufferSize);
	m_sync->arm(this);
	m_top->build();
	m_top->start();
	return true;
}

void GRTimeSinkComponent::stop()
{
	m_top->stop();
	m_top->teardown();
	m_sync->disarm(this);
}

bool GRTimeSinkComponent::syncMode() { return m_syncMode; }

void GRTimeSinkComponent::setSyncMode(bool b) { m_syncMode = b; }

void GRTimeSinkComponent::setSyncController(SyncController *s) { m_sync = s; }

void GRTimeSinkComponent::addChannel(GRChannel *ch) { m_channels.append(ch); }

void GRTimeSinkComponent::removeChannel(GRChannel *ch) { m_channels.removeAll(ch); }

void GRTimeSinkComponent::setSyncSingleShot(bool b) { Q_EMIT requestSingleShot(b); }

void GRTimeSinkComponent::setSyncBufferSize(uint32_t val) { Q_EMIT requestBufferSize(val); }

const QString &GRTimeSinkComponent::name() const { return m_name; }
