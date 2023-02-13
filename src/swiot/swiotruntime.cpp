#include "swiotruntime.hpp"
#include "gui/generic_menu.hpp"
#include <QtConcurrent>
#include <QFuture>

#include "gui/customcolqgridlayout.hpp"
#include "swiot/swiotgenericmenu.hpp"
#include <string>

using namespace adiscope;

SwiotRuntime::SwiotRuntime(struct iio_context *ctx, Filter *filt,
			     ToolMenuItem *toolMenuItem,
			     QJSEngine *engine, ToolLauncher *parent):
	Tool(ctx, toolMenuItem, nullptr, "SwiotRuntime",parent)
{
	run_button=nullptr;

	int devicesNumber = iio_context_get_devices_count(ctx);

	for (int i = 0; i < devicesNumber; i++) {
		struct iio_device* iioDev = iio_context_get_device(ctx, i);
		if (iioDev) {
			QString deviceName = QString(iio_device_get_name(iioDev));
			m_iioDevices[deviceName] = iioDev;
		}
	}

	m_swiotRuntimeAd = new SwiotAd(this, parent, m_iioDevices["ad74413r"]);
	m_swiotRuntimeAd->setChannelsFunction(channel_function);

	initAdToolView();
}

SwiotRuntime::~SwiotRuntime()
{
	if (m_toolView->getRunBtn()->isChecked()) {
		m_toolView->getRunBtn()->setChecked(false);
	}
	if (m_toolView) {
		delete m_toolView;
		m_toolView = nullptr;
	}
	if (m_swiotRuntimeAd) {
		delete m_swiotRuntimeAd;
		m_swiotRuntimeAd = nullptr;
	}

}

void SwiotRuntime::initAdToolView()
{
	m_toolView = m_swiotRuntimeAd->getToolView();
	setCentralWidget(m_toolView);

	setUpdatesEnabled(false);
	m_swiotRuntimeAd->initMonitorToolView();
	setUpdatesEnabled(true);
}

adiscope::gui::ToolView* SwiotRuntime::getToolView()
{
	return m_toolView;
}



