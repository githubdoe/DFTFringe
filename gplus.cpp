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
#include "gplus.h"

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

