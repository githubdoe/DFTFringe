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
    file.write((char*)(&rx),8);
    file.write((char*)(&rx),8);

    int size = 20;
    file.write((char*)&size,4);
    QVector<QPointF> points = circle.makeCircleofPoints(size);
    for (int i = 0; i < size; ++i){
        double x,y;
        x = points[i].x();
        y = points[i].y();

        file.write((char*)&x,8);
        file.write((char*)&y,8);
    }
}

CircleOutline readCircle(std::ifstream &file, double x_offset, double y_offset){
    char buf[32 + 4];
    file.read(buf,8*4 + 4);
    if (!file){
        qDebug() << "failed to read.  Read only " << file.gcount();
        return     CircleOutline();
    }

    double *dp = (double*)buf;
    double x = *(dp++);
    double y = *(dp++);
    double rx = *(dp++);

    ++dp;
    int size = *(int *)dp; //= *(reinterpret_cast<int *>(buf));
    if (size > 20)
       return     CircleOutline();
    // ignore the ellipse point section
    for (int i = 0; i < size; ++i) {

        file.read(buf,8);
        file.read(buf,8);
    }
    CircleOutline c;
    c.m_center.rx() = x + x_offset;
    c.m_center.ry() = y + y_offset;
    c.m_radius = rx;
    c.m_p1.m_p.rx() = c.m_center.x() + c.m_radius;
    c.m_p2.m_p.rx() = c.m_center.x() - c.m_radius;
    c.m_p1.m_p.ry() = c.m_center.y();
    c.m_p2.m_p.ry() = c.m_center.y();
    return c;
}
