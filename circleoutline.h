#ifndef CIRCLEOUTLINE_H
#define CIRCLEOUTLINE_H

#ifndef CIRCLE_H
#define CIRCLE_H

#include "boundary.h"
#include "gplus.h"
#include <QPointF>

class CircleOutline: public boundary
{
    public:
        CircleOutline();
        CircleOutline(QPointF center, double rad);
        CircleOutline(QPointF p1, QPointF p2);
        virtual ~CircleOutline();
        void draw(QPainter& painter, double scale);
        bool isInside(QPointF& p , int offset = 0);
        void enlarge(int del);
        void translate(QPointF del);
        void scale(double factor);
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
