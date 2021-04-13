#ifndef TOOLLAUNCHER_HPP
#define TOOLLAUNCHER_HPP

#include <QMainWindow>

#include <scopy/core/context_enumerator.hpp>
#include <scopy/gui/connect_dialog.hpp>
#include <scopy/gui/scopy_about_dialog.hpp>
#include <scopy/gui/tool_menu.hpp>
#include <scopy/gui/user_notes.hpp>

#include <scopy/core/plugin_interface.hpp>
#include <scopy/core/tool_interface.hpp>

namespace Ui {
class ToolLauncher;
}

namespace scopy {
namespace gui {

class ToolLauncher : public QMainWindow
{
	Q_OBJECT
public:
	explicit ToolLauncher(QWidget* parent = nullptr);

private:
	void swapMenu(QWidget* menu);

	void setTestLbl(const QString& text);

	void setupUi();

private Q_SLOTS:
	void onBtnHomeClicked();
	void detectedUris(const QStringList& uris);

private:
	Ui::ToolLauncher* m_ui;

	ToolMenu* m_toolMenu;

	ScopyAboutDialog* m_about;

	QWidget* m_current;
	QSettings* m_settings;
	Preferences* m_prefPanel;
	UserNotes* m_notesPanel;

	scopy::core::ContextEnumerator* m_contextEnumerator;
	std::vector<scopy::core::PluginInterface*> m_plugins;
	std::vector<scopy::core::ToolInterface*> m_tools;
};
} // namespace gui
} // namespace scopy

#endif // TOOLLAUNCHER_HPP
