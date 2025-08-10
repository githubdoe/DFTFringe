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


#include "boundary.h"
#include "gplus.h"
#include <QVector>
#include <QPointF>
#include <opencv2/opencv.hpp>
#include <QJsonObject>
void fillCircle(cv::Mat &m, double cx, double cy, double rad, void *color);
class CircleOutline final: public boundary
{
    public:
        CircleOutline();
        CircleOutline(QPointF center, double rad);
        CircleOutline(QPointF p1, QPointF p2);
        CircleOutline(const QJsonObject &obj);
        void toJson(QJsonObject &obj);

        ~CircleOutline();
        void draw(QPainter& painter, double scale, double scale2 = -1.);
        bool isInside(QPointF& p , int offset = 0);
        bool isInside(double x, double y, int offset=0);
        void enlarge(int del);
        void translate(QPointF del);
        void scale(double factor);
        QVector<QPointF> makeCircleofPoints(int cnt);
        bool isValid();
        QPointF m_center;
        double m_radius;
        gPlus m_p1;
        gPlus m_p2;
};

class ellipseOutline final: public boundary
{
public:
    ellipseOutline();
    ellipseOutline(QPointF center, double minorAxis, double majorAxis);
    ellipseOutline(QPointF left, QPointF right, double ecc);
    void draw(QPainter& painter, double scale, double scale2 = -1.);
    bool isInside(QPointF& p , int offset = 0);
    void enlarge(int del);
    void translate(QPointF del);
    void scale(double factor);


    bool isValid();
    QPointF m_center;
    double m_minor;
    double m_major;
    double m_left;
    double m_right;
    double e;
    gPlus m_p1;
    gPlus m_p2;
};

class rectangleOutline final: public boundary
{
public:
    rectangleOutline();
    rectangleOutline(QPointF upperLeft, QPointF lowerRight);
    void draw(QPainter& painter, double scale, double scale2 = -1.);
    bool isInside(QPointF& p , int offset = 0);
    void enlarge(int del);
    void translate(QPointF del);
    void scale(double factor);
    bool isValid();
    QPointF m_upperLeft;
    QPointF m_lowerRight;
    gPlus m_p1;
    gPlus m_p2;
};
QDataStream & operator<<(QDataStream & stream, const ellipseOutline &outline);
QDataStream & operator<<(QDataStream & stream, const CircleOutline &outline);
QDataStream & operator<<(QDataStream & stream, const rectangleOutline &outline);


#endif // CIRCLEOUTLINE_H
