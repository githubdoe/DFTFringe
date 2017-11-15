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
#include <qmath.h>
#include <QDebug>
#include <qmath.h>

CircleOutline::CircleOutline(QPointF center, double rad){
    center.rx() = round(2 * center.x())/2.;
    center.ry() = round( 2 * center.y())/2.;
    m_p1 = QPointF(center.x() - (rad), center.y());
    m_p2 = QPointF(center.x()+rad, center.y());
    m_center = center;
    m_radius = round(2 *rad)/2.;

}


CircleOutline::CircleOutline(QPointF p1, QPointF p2)
{
    p1.rx() = round( 2 * p1.x())/2.;
    p1.ry() = round( 2 * p1.y())/2.;
    p2.rx() = round(2 * p2.x())/2.;
    p2.ry() = round(2 * p2.y())/2.;
    m_p1 = gPlus(p1);
    m_p2 = gPlus(p2);

    m_center.rx() = (double)(p1.x()+p2.x())/2.;
    m_center.ry() = (double)(p1.y()+p2.y())/2.;
    m_radius = fabs(p1.x() - p2.x())/2.;

}
CircleOutline::CircleOutline()
{   m_center = QPointF(-1,-1);
    m_radius = 0;
}

CircleOutline::~CircleOutline(){}
bool CircleOutline::isInside(double x, double y, int offset)
{
    double x1,y1;
    x1 = (x+.5) - m_center.x();
    y1 = (y+.5) - m_center.y();
    return (sqrt(x1 * x1 + y1 * y1) <= (m_radius-offset));
}

void fillCircle(cv::Mat &m, double cx, double cy, double rad, void* color){
    //Size size = m.size();
    size_t step = m.step;
    int pix_size = (int)m.elemSize();
    uchar* ptr = m.ptr();

#define ICV_PUT_POINT( ptr, x )     \
    memcpy( ptr + (x)*pix_size, color, pix_size );

    for (int y = 0; y < m.rows; ++y){
        uchar *tptr0 = ptr + y * step;
        for (int x = 0; x < m.cols; ++x){
            double dx = (double)(x  - cx)/(rad);
            double dy = (double)(y  - cy)/(rad);
            if (sqrt(dx * dx + dy * dy) <= 1.){
                ICV_PUT_POINT(tptr0, x);
            }

        }
    }


}

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
    m_radius = round(m_radius * 2)/2.;
    //m_radius = (int)m_radius;
    m_p1.m_p *= factor;
    m_p2.m_p *= factor;
    m_center.rx() = round(2 * m_center.x() * factor)/2.;
    m_center.ry() = round(2 * m_center.y() * factor)/2.;
}
bool CircleOutline::isValid(){
    return m_radius > 0;
}
void CircleOutline::draw(QPainter& dc, double scale, double scale2){
    if (m_radius == 0)
        return;
    //m_p1.draw(dc, scale);
    //m_p2.draw(dc, scale);
    if (scale2 < 0)
        scale2 = scale;

    dc.drawEllipse(m_center * scale,m_radius * scale,m_radius* scale2);
    dc.drawLine((m_center.x() -m_radius - 5)* scale, m_center.y()* scale, scale *( m_center.x() + m_radius + 5), scale * m_center.y());
    dc.drawLine(scale * m_center.x(), m_center.y() - m_radius * scale2 - 5, scale * m_center.x(), m_center.y() + m_radius * scale2 + 5);
}

QVector<QPointF> CircleOutline::makeCircleofPoints(int cnt){
    QVector<QPointF> points;
    double step = (M_PI * 2.)/cnt;
    for (int i = 0; i < cnt; ++i)
    {
        double theta = i * step;
        int x,y;
        x = m_radius * cos(theta) + m_center.x();
        y = m_radius * sin(theta) + m_center.y();
        points << QPointF(x,y);
    }
    return points;
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



