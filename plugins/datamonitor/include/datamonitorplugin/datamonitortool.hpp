#ifndef DATAMONITORTOOL_HPP
#define DATAMONITORTOOL_HPP

#include "dataacquisitionmanager.hpp"

#include "iio.h"
#include <QPushButton>
#include <QWidget>
#include <tooltemplate.h>
#include <toolbuttons.h>
#include <QMap>
#include <QScrollArea>
#include <QTimer>
#include <QLabel>

namespace scopy {

class CollapsableMenuControlButton;
class FlexGridLayout;

namespace datamonitor {

class DataMonitorModel;

class DataMonitorTool : public QWidget
{
	friend class DataMonitorStyleHelper;
	Q_OBJECT
public:
	explicit DataMonitorTool(iio_context *ctx, QWidget *parent = nullptr);

	RunBtn *getRunButton() const;

Q_SIGNALS:
	void setStartTime();

private:
	ToolTemplate *tool;
	QPushButton *openLastMenuBtn;
	GearBtn *settingsButton;
	InfoBtn *infoBtn;
	RunBtn *runBtn;
	QPushButton *clearBtn;
	AddBtn *addMonitorButton;

	FlexGridLayout *m_flexGridLayout;
	QScrollArea *m_scrollArea;
	void generateMonitor(DataMonitorModel *model, CollapsableMenuControlButton *channelManager);

	QButtonGroup *grp;

	DataAcquisitionManager *dataAcquisitionManager;

	iio_context *ctx;
	int activeMonitor = -1;
	bool first = true;

	QLabel *startTime;
	void resetStartTime();
};
} // namespace datamonitor
} // namespace scopy
#endif // DATAMONITORTOOL_HPP