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

#include <QJsonObject>
#include <QPainter>
#include <QPointF>
#include <QVector>
#include <opencv2/opencv.hpp>

void fillCircle(cv::Mat &m, double cx, double cy, double rad, void *color);

class CircleOutline {
  public:
    CircleOutline();
    CircleOutline(QPointF center, double rad);
    CircleOutline(QPointF p1, QPointF p2);
    CircleOutline(const QJsonObject &obj);
    ~CircleOutline() = default;

    void translate(QPointF del);
    void scale(double factor);
    bool isValid();
    void draw(QPainter &painter, double scale, double scale2 = -1.);
    QVector<QPointF> makeCircleofPoints(int cnt);
    void enlarge(int del);
    void toJson(QJsonObject &obj);

    QPointF m_center;
    double m_radius;
    QPointF m_p1;
    QPointF m_p2;
};

#endif // CIRCLEOUTLINE_H
