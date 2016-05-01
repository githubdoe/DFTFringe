#include "gPlus.h"

gPlus::gPlus(QPointF p)
{
    m_p = p;
}
gPlus::gPlus() {}
gPlus::~gPlus()
{
    //dtor
}
void gPlus::draw(QPainter& dc,double scale, int size){
    QPointF p = m_p;
    size *= 1./scale;
    QPointF p1((p.x()-size) * scale,  p.y() * scale);
    QPointF p2((p.x()+size) * scale,  p.y() * scale);
    dc.drawLine(p1,p2);
    dc.drawLine((p.x()) * scale, (p.y()-size)*scale, p.x()*scale, (p.y()+size)*scale);
}

