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
#include "Circleoutline.h"
#include "CircleOutline.h"
#include <Qmath.h>
#include <QDebug>
CircleOutline::CircleOutline(QPointF center, double rad){
    m_p1 = QPointF(center.x() - rad, center.y());
    m_p2 = QPointF(center.x()+rad, center.y());
    m_center = center;
    m_radius = rad;
}
double round(double v)
{
    return floor(v*(1./.5) + 0.5)/(1./.5);
}

CircleOutline::CircleOutline(QPointF p1, QPointF p2)
{

    m_p1 = gPlus(p1);
    m_p2 = gPlus(p2);

    m_center.rx() = (double)(p1.x()+p2.x())/2.d;
    m_center.ry() = (double)(p1.y()+p2.y())/2.d;

    QPointF start = p1;
    QPointF endpos = p2;
    if (start.x() > p2.x()) {
        int t = p2.x();
        endpos.rx() = start.x();
        start.rx() = t;
    }
    if (p2.y() < p1.y()){
        int t = p2.y();
        endpos.ry() = p1.y();
        start.ry() = t;
    }
    m_radius = (double)(endpos.x()- start.x())/2.d;
}
CircleOutline::CircleOutline()
{   m_center = QPointF(-1,-1);
    m_radius = 0;
}

CircleOutline::~CircleOutline(){}

bool CircleOutline::isInside(QPointF& p, int offset)
{
    double x,y;
    x = p.x() - m_center.x();
    y = p.y() - m_center.y();
    return (sqrt(x * x + y * y) <= (m_radius-offset));
}
void CircleOutline::translate(QPointF del){
    m_center += del;
    m_p1.m_p += del;
    m_p2.m_p += del;
}
void CircleOutline::scale(double factor){
    m_radius *= factor;
    m_p1.m_p *= factor;
    m_p2.m_p *= factor;
    m_center.rx() = m_center.x() * factor;
    m_center.ry() = m_center.y() * factor;
}
bool CircleOutline::isValid(){
    return m_radius > 0;
}
void CircleOutline::draw(QPainter& dc, double scale){
    if (m_radius == 0)
        return;
    //m_p1.draw(dc, scale);
    //m_p2.draw(dc, scale);

    dc.drawEllipse(m_center * scale,m_radius * scale,m_radius* scale);
    dc.drawLine((m_center.x() -m_radius)* scale, m_center.y()* scale,scale *( m_center.x() + m_radius),
            scale * m_center.y());
    dc.drawLine(scale * m_center.x(), scale* (m_center.y() - m_radius), scale * m_center.x(), scale * (m_center.y() + m_radius));
}



void CircleOutline::enlarge(int del) {
    //qDebug() << "enlarge "<<del;
    m_radius += del;
    if (m_p1.m_p.rx() > m_p2.m_p.rx()){
        gPlus tmp =m_p1;
        m_p1 = m_p2;
        m_p2 = tmp;
    }
    m_p1.m_p.rx()-= del;
    m_p2.m_p.rx()+= del;


}



