#ifndef STYLEHELPER_H
#define STYLEHELPER_H

#include <QMap>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QHBoxLayout>

#include <scopy-gui_export.h>
#include <utils.h>
#include <dynamicWidget.h>
#include <menu_anim.hpp>

#include <semiexclusivebuttongroup.h>
#include <QVBoxLayout>
#include <QComboBox>
#include <spinbox_a.hpp>
#include <smallOnOffSwitch.h>
#include <customSwitch.h>
#include "widgets/popupwidget.h"

namespace scopy {
class SCOPY_GUI_EXPORT StyleHelper : public QObject {
	Q_OBJECT
protected:
	StyleHelper(QObject *parent = nullptr);
	~StyleHelper();

public:
	// singleton
	StyleHelper(StyleHelper &other) = delete;
	void operator=(const StyleHelper &) = delete;
	static StyleHelper *GetInstance();
public:
	static void initColorMap();
	static QString getColor(QString id);
	static void SquareToggleButtonWithIcon(QPushButton* btn, QString objectName, bool checkable = false);
	static void BlueButton(QPushButton *btn, QString objectName);
	static void BlueSquareCheckbox(QCheckBox *chk, QString objectName  = "");
	static void CollapseCheckbox(QCheckBox *chk, QString objectName = "");
	static void ColoredCircleCheckbox(QCheckBox *chk, QColor color, QString objectName = "");
	static void ColoredSquareCheckbox(QCheckBox *chk, QColor color, QString objectName = "");
	static void MenuMediumLabel(QLabel * lbl, QString objectName = "");
	static void MenuControlButton(QPushButton *btn, QString objectName = "", bool checkable = true);
	static void MenuControlWidget(QWidget* w, QColor color, QString objectName = "");
	static void MenuLargeLabel(QLabel *lbl, QString objectName = "");
	static void MenuHeaderLine(QFrame *line, QPen pen, QString objectName = "");
	static void MenuHeaderWidget(QWidget *w, QString objectName = "");
	static void MenuComboBox(QComboBox *cb, QString objectName = "");
	static void MenuSmallLabel(QLabel *lbl, QString objectName = "");
	static void MenuComboWidget(QWidget *w, QString objectName = "");
	static void MenuSectionWidget(QWidget *w, QString objectName = "");
	static void MenuSpinBox(SpinBoxA *w, QString objectName = "");
	static void MenuSpinComboBox(QComboBox *w, QString objectName);
	static void MenuOnOffSwitch(QWidget *w, QString objectName = "");
	static void MenuOnOffSwitchLabel(QLabel *w, QString objectName = "");
	static void MenuOnOffSwitchButton(SmallOnOffSwitch *w, QString objectName = "");
	static void MenuCollapseSection(QWidget *w, QString objectName = "");
	static void MenuCollapseHeaderLabel(QLabel *w, QString objectName = "");
	static void MenuComboLabel(QLabel *w, QString objectName = "");
	static void MenuHeaderLabel(QLabel *w, QString objectName = "");
	static void MenuControlLabel(QLabel *w, QString objectName = "");
	static void MenuBigSwitch(QWidget *w, QString objectName);
	static void MenuBigSwitchButton(CustomSwitch *w, QString objectName);
	static void MenuLineEdit(QLineEdit *m_edit, QString objectName);
	static void MenuLineEditWidget(QWidget *w, QString objectName);
	static void BlueGrayButton(QPushButton *btn, QString objectName);
	static void OverlayMenu(QWidget *w, QString objectName = "");
	static void BlueIconButton(QPushButton *w, QIcon icon, QString objectName = "");
	static void TabWidgetEastMenu(QTabWidget *w, QString objectName = "");
	static void BackgroundPage(QWidget *w, QString objectName = "");
	static void BackgroundWidget(QWidget *w, QString objectName = "");
	static void TabWidgetLabel(QLabel *w, QString objectName = "");
	static void TitleLabel(QLabel *w, QString objectName = "");
private:
	QMap<QString,QString> colorMap;
	static StyleHelper * pinstance_;
};
}

#endif // STYLEHELPER_H
