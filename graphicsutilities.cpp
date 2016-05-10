/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "graphicsutilities.h"
#include <iostream>
#include <fstream>
#include <cstring>

void writeCircle(std::ofstream& file, CircleOutline& circle){
    double x = circle.m_center.x();
    file.write((char*)&x,8);
    double y = circle.m_center.y();
    file.write((char*)&y,8);
    double rx = circle.m_radius;
    file.write((char*)&rx,8);
    file.write((char*)&rx,8);
    int size = 2;
    file.write((char*)&size,4);
    file.write((char*)&(circle.m_p1),8);
    file.write((char*)&(circle.m_p2),8);
}

CircleOutline readCircle(std::ifstream& file){
    char buf[32];
    file.read(buf,8*4);
    double *dp = (double*)buf;
    double x = *(dp++);
    double y = *(dp++);
    double rx = *(dp++);

    file.read(buf,4);
    int size = 0; //= *(reinterpret_cast<int *>(buf));
    // ignore the ellipse point section
    std::memcpy(&size, buf, sizeof(int));
    for (int i = 0; i < size; ++i) {
        file.read(buf,8);
    }
    CircleOutline c;
    c.m_center.rx() = x;
    c.m_center.ry() = y;
    c.m_radius = rx;
    c.m_p1.m_p.rx() = c.m_center.x() + c.m_radius-1;
    c.m_p2.m_p.rx() = c.m_center.x() - c.m_radius-1;
    c.m_p1.m_p.ry() = c.m_center.y();
    c.m_p2.m_p.ry() = c.m_center.y();
    return c;
}
