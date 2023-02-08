#ifndef SWIOTRUNTIME_HPP
#define SWIOTRUNTIME_HPP
#include <filter.hpp>
#include <tool_launcher.hpp>
#include "gui/customcolqgridlayout.hpp"
#include "swiot/swiotcontroller.hpp"
#include <tool.hpp>
#include "gui/tool_view_builder.hpp"
#include "swiot/swiotmodel.hpp"

namespace adiscope {

namespace gui {
class GenericMenu;
class ChannelManager;
}

class SwiotRuntime : public Tool
{
	Ui::NewInstrument* ui;
public:
	SwiotRuntime(struct iio_context *ctx, Filter *filt,
		      ToolMenuItem *toolMenuItem,
		      QJSEngine *engine, ToolLauncher *parent);
	~SwiotRuntime();

	adiscope::gui::ToolView* getToolView();
	struct iio_channel* getIioChannel(struct iio_device* iio_dev, int chnlIdx);
	void initMonitorToolView();

private:
	QMap<QString, struct iio_channel*> createChannels();

	adiscope::gui::ChannelManager* m_monitorChannelManager;
	adiscope::gui::ToolView* m_toolView;
	CustomColQGridLayout* m_customColGrid;

	QVector<SwiotController*> m_controllers;
	QMap<QString, struct iio_channel*> m_channels;

	const char device_name[20] = "ad74413r";
	struct iio_device *m_iio_dev;


private Q_SLOTS:
	void buttonHandler();

};
}


#endif // SWIOTRUNTIME_HPP
