/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#ifndef GRAPHICSUTILITIES_H
#define GRAPHICSUTILITIES_H

#include <QtCore>
#include "circleoutline.h"
#include <iostream>
#include <fstream>

void drawPlus(QPointF p, QPainter& dc);

CircleOutline readCircle(std::ifstream& file);
void writeCircle(std::ofstream& file, CircleOutline &circle);
#endif // GRAPHICSUTILITIES_H
