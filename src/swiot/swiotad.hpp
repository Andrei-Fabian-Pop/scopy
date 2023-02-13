#ifndef SWIOTAD_HPP
#define SWIOTAD_HPP

#include "gui/customcolqgridlayout.hpp"
#include "gui/tool_view.hpp"
#include "qwidget.h"
#include "swiot/swiotcontroller.hpp"
#include "tool_launcher.hpp"
#include <QVector>


extern "C"{
	struct iio_device;
	struct iio_channel;
}

namespace adiscope {

namespace gui {
class GenericMenu;
class ChannelManager;
}

class SwiotAd: public QWidget
{
	Q_OBJECT
public:
	explicit SwiotAd(QWidget* parent=nullptr, ToolLauncher* toolLauncher = nullptr, struct iio_device* iioDev = nullptr);
	~SwiotAd();

	void initMonitorToolView();
	void setChannelsFunction(QVector<QString> chnlsFunction);
	adiscope::gui::ToolView* getToolView();
private:
	struct iio_device* m_iioDev;

	adiscope::gui::ChannelManager* m_monitorChannelManager;
	adiscope::gui::ToolView* m_toolView;
	CustomColQGridLayout* m_customColGrid;
	QWidget* m_widget;

	QVector<SwiotController*> m_controllers;
	QVector<QString> m_chnlsFunction;
	QMap<QString, struct iio_channel*> m_channels;

	QMap<QString, struct iio_channel*> createChannels();
	struct iio_channel* getIioChannel(int chnlIdx);
};
}
#endif // SWIOTAD_HPP
