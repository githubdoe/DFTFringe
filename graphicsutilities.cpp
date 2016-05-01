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
    file.write((char*)&size,8);
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
    int r = std::memcmp(&size, buf, sizeof(int));
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
