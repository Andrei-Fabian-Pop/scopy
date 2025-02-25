/*
 * Copyright (c) 2019 Analog Devices Inc.
 *
 * This file is part of Scopy
 * (see http://www.github.com/analogdevicesinc/scopy).
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CUSTOM_SWITCH_HPP
#define CUSTOM_SWITCH_HPP

#include "scopy-gui_export.h"

#include <QLabel>
#include <QPushButton>

namespace scopy {
class SCOPY_GUI_EXPORT CustomSwitch : public QPushButton
{
	Q_OBJECT

public:
	explicit CustomSwitch(QWidget *parent = nullptr);
	explicit CustomSwitch(QString on, QString off, QWidget *parent = nullptr);
	~CustomSwitch();

	void setOnText(const QString text);
	void setOffText(const QString text);

	QSize sizeHint() const override;

public Q_SLOTS:
	void update();
	void onToggle(bool en);

protected:
	void init();
	void paintEvent(QPaintEvent *event) override;
	bool event(QEvent *) override;

private:
	QLabel *m_onLabel;
	QLabel *m_offLabel;
};
} // namespace scopy

#endif /* CUSTOM_SWITCH_HPP */
