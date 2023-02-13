#ifndef SWIOTRUNTIME_HPP
#define SWIOTRUNTIME_HPP
#include <filter.hpp>
#include <tool_launcher.hpp>
#include "gui/customcolqgridlayout.hpp"
#include "swiot/swiotad.hpp"
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
public:
	SwiotRuntime(struct iio_context *ctx, Filter *filt,
		      ToolMenuItem *toolMenuItem,
		      QJSEngine *engine, ToolLauncher *parent);
	~SwiotRuntime();

	adiscope::gui::ToolView* getToolView();
	void initAdToolView();
private:
	SwiotAd* m_swiotRuntimeAd;
	adiscope::gui::ToolView* m_toolView;
	QMap<QString, struct iio_device*> m_iioDevices;

	//	//Runtime A
	const QVector<QString> channel_function{"digital_input", "voltage_out", "current_out", "voltage_in",
							"diagnostic", "diagnostic", "diagnostic", "diagnostic"};
	//	//Runtime B
	//	const QVector<QString> channel_function{"current_in_ext", "current_in_loop", "resistance", "digital_input",
	//	"diagnostic", "diagnostic", "diagnostic", "diagnostic"};
	//	//Runtime C
	//	const QVector<QString> channel_function{"digital_input_loop", "current_in_ext_hart", "current_in_ext_hart", "disable",
	//	"diagnostic", "diagnostic", "diagnostic", "diagnostic"};
	//	//Runtime D
	//	const QVector<QString> channel_function{"current_out", "disable", "disable", "disable",
	//	"diagnostic", "diagnostic", "diagnostic", "diagnostic"};//digital_out not for sure
};
}


#endif // SWIOTRUNTIME_HPP
