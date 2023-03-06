#ifndef TESTPLUGIN_H
#define TESTPLUGIN_H

#define SCOPY_PLUGIN_NAME TestPlugin
#define SCOPY_PLUGIN_PRIO 1

#include <QObject>
#include <pluginbase/plugin.h>
#include <pluginbase/pluginbase.h>
#include "scopytestplugin_export.h"
#include <QLineEdit>

namespace adiscope {

class SCOPYTESTPLUGIN_EXPORT TestPlugin : public QObject, public PluginBase
{
	Q_OBJECT
	SCOPY_PLUGIN;
public:
	friend class TestPlugin_API;
	void loadApi() override;
	void initPreferences() override;
	void initMetadata() override;
	bool compatible(QString m_param) override;
	bool loadPreferencesPage() override;
	bool loadPage() override;
	bool loadIcon() override;
	void loadToolList() override;
	bool loadExtraButtons() override;
	QString about() override;
	QString version() override;

	bool onConnect() override;
	bool onDisconnect() override;

public Q_SLOTS:
	void messageCallback(QString topic, QString message) override;

private:
	QWidget *tool;
	QLineEdit *edit;

};

class SCOPYTESTPLUGIN_EXPORT TestPlugin_API : public ApiObject {
	Q_OBJECT
public:
	explicit TestPlugin_API(TestPlugin *p) : ApiObject(p),p(p) {}
	~TestPlugin_API() {}
	TestPlugin* p;

	Q_PROPERTY(QString testText READ testText WRITE setTestText);
	QString testText() const;
	void setTestText(const QString &newTestText);
private:
	QString m_testText;
};

}


#endif // TESTPLUGIN_H
