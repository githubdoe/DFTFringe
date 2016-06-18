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
#ifndef CIRCLEOUTLINE_H
#define CIRCLEOUTLINE_H

#ifndef CIRCLE_H
#define CIRCLE_H

#include "boundary.h"
#include "gplus.h"
#include <QVector>
#include <QPointF>

class CircleOutline: public boundary
{
    public:
        CircleOutline();
        CircleOutline(QPointF center, double rad);
        CircleOutline(QPointF p1, QPointF p2);
        virtual ~CircleOutline();
        void draw(QPainter& painter, double scale, double scale2 = -1.);
        bool isInside(QPointF& p , int offset = 0);
        void enlarge(int del);
        void translate(QPointF del);
        void scale(double factor);
        QVector<QPointF> makeCircleofPoints(int cnt);
        bool isValid();
        QPointF m_center;
        double m_radius;
        gPlus m_p1;
        gPlus m_p2;
    protected:

    private:
};

#endif // CIRCLE_H


#endif // CIRCLEOUTLINE_H
