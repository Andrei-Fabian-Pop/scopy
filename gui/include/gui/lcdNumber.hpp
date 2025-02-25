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

#ifndef LCD_NUMBER_HPP
#define LCD_NUMBER_HPP

#include "scopy-gui_export.h"

#include <QLCDNumber>

namespace scopy {
class SCOPY_GUI_EXPORT LcdNumber : public QLCDNumber
{
	Q_OBJECT

public:
	explicit LcdNumber(QWidget *parent = 0, unsigned precision = 3);
	~LcdNumber() {}

public Q_SLOTS:
	void display(double num);
	void setPrecision(unsigned precision);
	unsigned getPrecision();

private:
	unsigned precision;
};
} // namespace scopy

#endif /* LCD_NUMBER_HPP */
