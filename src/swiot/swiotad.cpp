#include "swiotad.hpp"
#include "gui/tool_view_builder.hpp"
#include "swiotgenericmenu.hpp"
#include "swiotmodel.hpp"
#include <iio.h>

using namespace adiscope;

SwiotAd::SwiotAd(QWidget* parent, ToolLauncher *toolLauncher, struct iio_device* iioDev):
	m_iioDev(iioDev)
      ,m_widget(parent)
{
	m_channels = createChannels();
	if (m_iioDev != nullptr) {
		adiscope::gui::ToolViewRecipe recipe;
		recipe.helpBtnUrl = "";
		recipe.hasRunBtn = true;
		recipe.hasSingleBtn = true;
		recipe.hasPairSettingsBtn = true;
		recipe.hasPrintBtn = false;
		recipe.hasChannels = true;
		recipe.channelsPosition = adiscope::gui::ChannelsPositionEnum::VERTICAL;

		m_monitorChannelManager = new adiscope::gui::ChannelManager(recipe.channelsPosition);
		m_monitorChannelManager->setChannelIdVisible(false);
		m_monitorChannelManager->setToolStatus("Channels");

		m_toolView = adiscope::gui::ToolViewBuilder(recipe, m_monitorChannelManager, toolLauncher).build();
		m_customColGrid = new CustomColQGridLayout(100, parent);
		m_toolView->addFixedCentralWidget(m_customColGrid,0,0,0,0);
	}
}

SwiotAd::~SwiotAd()
{}

void SwiotAd::initMonitorToolView()
{
	int chId = 1;
	bool first = true;
	for (int i = 0; i < m_chnlsFunction.size(); i++) {

		if (m_chnlsFunction[i].compare("disable") != 0) {
			std::vector<ChannelWidget*> channelList;

			adiscope::gui::SwiotGenericMenu *menu = new adiscope::gui::SwiotGenericMenu(m_widget);
			std::string deviceName = iio_device_get_name(m_iioDev);
			menu->init((QString::fromStdString(deviceName + " - Channel ") + QString::number(chId)) + QString::fromStdString(": ")
				   , m_chnlsFunction[i], new QColor("green"));

			struct iio_channel* iioChnl = getIioChannel(i);
			SwiotAdModel* swiotModel = new SwiotAdModel(iioChnl);
			SwiotController* controller = new SwiotController(menu, swiotModel);

			controller->addMenuAttrValues();

			if (controller) {
				m_controllers.push_back(controller);
			}

			ChannelWidget *mainCh_widget =
					m_toolView->buildNewChannel(m_monitorChannelManager, menu, false, chId, false, false, QColor("green"),
								  QString::fromStdString("channel "), m_chnlsFunction[i] +" "+QString::number(chId));
			if (first) {
				mainCh_widget->menuButton()->click();
				first = false;
			}
			m_toolView->buildChannelGroup(m_monitorChannelManager, mainCh_widget,channelList);

			controller->createConnections();
		}
		chId++;
	}
}

QMap<QString, struct iio_channel*> SwiotAd::createChannels()
{
	QMap<QString, struct iio_channel*> map;

	if (m_iioDev != nullptr) {
		int chnlsNumber = iio_device_get_channels_count(m_iioDev);
		for (int i = 0; i < chnlsNumber; i++) {
			struct iio_channel* iioChnl = iio_device_get_channel(m_iioDev, i);
			std::string chnlId = iio_channel_get_id(iioChnl);
			map.insert(QString::fromStdString(chnlId), iioChnl);
		}
	}
	return map;
}

struct iio_channel* SwiotAd::getIioChannel(int chnlIdx)
{
	QString voltage("voltage" +QString::number(chnlIdx));
	QString current("current" +QString::number(chnlIdx));
	QString resistence("resistence" +QString::number(chnlIdx));
	bool voltageOutput=false;
	int chnlNoPerChannel = 0;
	struct iio_channel* iioChnl = nullptr;

	if (m_channels.contains(voltage)) {
		iioChnl = m_channels[voltage];
		voltageOutput = iio_channel_is_output(iioChnl);
		chnlNoPerChannel++;
	}
	if (m_channels.contains(current)) {
		iioChnl = m_channels[current];
		chnlNoPerChannel++;
	}
	if (m_channels.contains(resistence)) {
		iioChnl = m_channels[resistence];
	}
	if (chnlNoPerChannel == 2) {
		if(voltageOutput){
			iioChnl = m_channels[voltage];
		}
	}
	return iioChnl;
}

adiscope::gui::ToolView* SwiotAd::getToolView()
{
	return m_toolView;
}

void SwiotAd::setChannelsFunction(QVector<QString> chnlsFunction)
{
	m_chnlsFunction = chnlsFunction;
}

