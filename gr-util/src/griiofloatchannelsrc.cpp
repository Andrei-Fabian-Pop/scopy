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

#include "griiofloatchannelsrc.h"

#include "gnuradio/blocks/copy.h"
#include "gnuradio/blocks/int_to_float.h"
#include "gnuradio/blocks/short_to_float.h"
#include "grlog.h"
#include "grtopblock.h"

using namespace scopy::grutil;
GRIIOFloatChannelSrc::GRIIOFloatChannelSrc(GRIIODeviceSource *dev, QString channelName, QObject *parent)
	: GRIIOChannel(channelName, dev, parent)
{
	m_iioCh = iio_device_find_channel(dev->iioDev(), channelName.toStdString().c_str(), false);
	fmt = iio_channel_get_data_format(m_iioCh);

	m_sampleRateAttribute = findAttribute(
		{
			"sample_rate",
			"sampling_rate",
			"sample_frequency",
			"sampling_frequency",
		},
		m_iioCh);

	m_scaleAttribute = findAttribute(
		{
			"scale",
		},
		m_iioCh);

	iio_chan_type type = iio_channel_get_type(m_iioCh);
	m_unit = IIOUnitsManager::iioChannelTypes().value(type, {"Adimensional", ".", 1});
}

void GRIIOFloatChannelSrc::build_blks(GRTopBlock *top)
{
	m_top = top;
	qDebug(SCOPY_GR_UTIL) << "Building GRIIOFloatChannelSrc";
	m_dev->addChannel(this);
	switch(fmt->length) {
	case 16:
		x2f = gr::blocks::short_to_float::make();
		break;
	case 32:
		x2f = gr::blocks::int_to_float::make();
		break;
	default:
		qInfo(SCOPY_GR_UTIL) << "creating copy block of size " << fmt->length / 8;
		x2f = gr::blocks::copy::make(fmt->length / 8);
		break;
	}

	s2v = gr::blocks::stream_to_vector::make(sizeof(float), top->vlen());
	top->connect(x2f, 0, s2v, 0);
	end_blk = s2v;
	start_blk.append(x2f);
}

void GRIIOFloatChannelSrc::destroy_blks(GRTopBlock *top)
{
	m_dev->removeChannel(this);
	s2v = nullptr;
	x2f = nullptr;
	end_blk = nullptr;
	start_blk.clear();
}

bool GRIIOFloatChannelSrc::samplerateAttributeAvailable()
{
	if(m_sampleRateAttribute.isEmpty())
		return false;
	return true;
}

double GRIIOFloatChannelSrc::readSampleRate()
{
	char buffer[20];
	bool ok = false;
	double sr;
	if(!samplerateAttributeAvailable())
		return -1;

	iio_channel_attr_read(m_iioCh, m_sampleRateAttribute.toStdString().c_str(), buffer, 20);
	QString str(buffer);
	sr = str.toDouble(&ok);
	if(ok) {
		return sr;
	} else {
		return -1;
	}
}

bool GRIIOFloatChannelSrc::scaleAttributeAvailable()
{
	if(m_scaleAttribute.isEmpty())
		return false;
	return true;
}

double GRIIOFloatChannelSrc::readScale()
{
	char buffer[20];
	bool ok = false;
	double sc;
	if(!scaleAttributeAvailable())
		return -1;

	iio_channel_attr_read(m_iioCh, m_scaleAttribute.toStdString().c_str(), buffer, 20);
	QString str(buffer);
	sc = str.toDouble(&ok);
	if(ok) {
		return sc;
	} else {
		return -1;
	}
}

scopy::IIOUnit GRIIOFloatChannelSrc::unit() { return m_unit; }

const iio_data_format *GRIIOFloatChannelSrc::getFmt() const { return fmt; }

struct iio_channel *GRIIOFloatChannelSrc::channel() const { return m_iioCh; }

struct iio_device *GRIIOFloatChannelSrc::dev() const { return m_dev->iioDev(); }

struct iio_context *GRIIOFloatChannelSrc::ctx() const { return m_dev->ctx(); }

const QString &GRIIOFloatChannelSrc::scaleAttribute() const { return m_scaleAttribute; }
