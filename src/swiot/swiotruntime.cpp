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
  Tool(ctx, toolMenuItem, nullptr, "SwiotRuntime",parent),
  m_iio_dev(nullptr)
{
	run_button=nullptr;
	m_iio_dev = iio_context_find_device(ctx, device_name);
	m_channels = createChannels();
	if(m_iio_dev!=nullptr){


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

		m_toolView = adiscope::gui::ToolViewBuilder(recipe,m_monitorChannelManager,parent).build();

		m_customColGrid = new CustomColQGridLayout(100,this);
		m_toolView->addFixedCentralWidget(m_customColGrid,0,0,0,0);
		setCentralWidget(getToolView());

		initMonitorToolView();
	}




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

}

void SwiotRuntime::initMonitorToolView()
{
//	//Runtime A
	const QVector<QString> channel_function{"digital_input", "voltage_out", "current_out", "voltage_in"};
//	//Runtime B
//	const QVector<QString> channel_function{"current_in_ext", "current_in_loop", "resistance", "digital_input"};
	//Runtime C
//	const QVector<QString> channel_function{"digital_input_loop", "current_in_ext_hart", "current_in_ext_hart", "disable"};
//	//Runtime D
//	const QVector<QString> channel_function{"current_out", "disable", "disable", "disable"};//digital_out not for sure
	int chId = 1;
	bool first = true;
	setUpdatesEnabled(false);

	for (int i=0;i<channel_function.size();i++) {

		if(channel_function[i].compare("disable")!=0){
			std::vector<ChannelWidget*> channelList;

			adiscope::gui::SwiotGenericMenu *menu = new adiscope::gui::SwiotGenericMenu(this);
			std::string deviceName = iio_device_get_name(m_iio_dev);
			menu->init((QString::fromStdString(deviceName + "-CH") + QString::number(chId)) + QString::fromStdString(": "), channel_function[i], new QColor("green"));

			struct iio_channel* iio_chnl = getIioChannel(m_iio_dev, i);
			SwiotAdModel* swiotModel = new SwiotAdModel(iio_chnl);
			SwiotController* controller = new SwiotController(menu, swiotModel);

			controller->addMenuAttrValues();

			if(controller){
				m_controllers.push_back(controller);
			}

			ChannelWidget *mainCh_widget =
					m_toolView->buildNewChannel(m_monitorChannelManager, menu, false, chId, false, false, QColor("green"),
								    QString::fromStdString("channel "), channel_function[i]);
			if (first) {
				mainCh_widget->menuButton()->click();
				first = false;
			}
			m_toolView->buildChannelGroup(m_monitorChannelManager, mainCh_widget,channelList);

			controller->createConnections();
		}
		chId++;
	}
	setUpdatesEnabled(true);
}

QMap<QString, struct iio_channel*> SwiotRuntime::createChannels(){
	QMap<QString, struct iio_channel*> map;

	if(m_iio_dev!=nullptr){
		int chnlsNumber = iio_device_get_channels_count(m_iio_dev);
		for(int i=0;i<chnlsNumber;i++){
			struct iio_channel* iio_chnl = iio_device_get_channel(m_iio_dev, i);
			std::string chnlId = iio_channel_get_id(iio_chnl);
			map.insert(QString::fromStdString(chnlId), iio_chnl);
		}
	}
	return map;
}

struct iio_channel* SwiotRuntime::getIioChannel(struct iio_device* iio_dev, int chnlIdx){

	QString voltage("voltage" +QString::number(chnlIdx));
	QString current("current" +QString::number(chnlIdx));
	QString resistence("resistence" +QString::number(chnlIdx));
	bool voltageOutput=false;
	int chnlNoPerChannel = 0;
	struct iio_channel* iio_chnl = nullptr;

	if(m_channels.contains(voltage)){
		iio_chnl = m_channels[voltage];
		voltageOutput = iio_channel_is_output(iio_chnl);
		chnlNoPerChannel++;
	}
	if(m_channels.contains(current)){
		iio_chnl = m_channels[current];
		chnlNoPerChannel++;
	}
	if(m_channels.contains(resistence)){
		iio_chnl = m_channels[resistence];
	}
	if(chnlNoPerChannel==2){
		if(voltageOutput){
			iio_chnl = m_channels[voltage];
		}
	}
	return iio_chnl;
}

adiscope::gui::ToolView* SwiotRuntime::getToolView()
{
	return m_toolView;
}



