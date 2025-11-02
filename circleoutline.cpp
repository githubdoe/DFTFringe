/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License
**
** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/
#include "Circleoutline.h"
#include <QDebug>
#include <qmath.h>

/**
 * @brief Fills a circle in a cv::Mat image using double coordinates and radius.
 * Does same as cv::circle but uses double type coordinated and radius.
 * @param m Reference to the image matrix.
 * @param cx X coordinate of the center.
 * @param cy Y coordinate of the center.
 * @param rad Radius of the circle.
 * @param color Pointer to the color value to use.
 */
void fillCircle(cv::Mat &m, double cx, double cy, double rad, void *color) {
    size_t step = m.step;
    int pix_size = (int)m.elemSize();
    uchar *ptr = m.ptr();

#define ICV_PUT_POINT(ptr, x) memcpy(ptr + (x) * pix_size, color, pix_size);

    for (int y = 0; y < m.rows; ++y) {
        uchar *tptr0 = ptr + y * step;
        for (int x = 0; x < m.cols; ++x) {
            double dx = (double)(x - cx) / (rad);
            double dy = (double)(y - cy) / (rad);
            if (sqrt(dx * dx + dy * dy) <= 1.)
                ICV_PUT_POINT(tptr0, x);
        }
    }
#undef ICV_PUT_POINT
}

/**
 * @brief Default constructor. Initializes an invalid circle.
 */
CircleOutline::CircleOutline() {
    m_center = QPointF(-1, -1);
    m_radius = 0;
}

/**
 * @brief Constructs a circle from a center point and radius.
 * @param center Center of the circle.
 * @param rad Radius of the circle.
 */
CircleOutline::CircleOutline(QPointF center, double rad) {
    center.rx() = round(2 * center.x()) / 2.;
    center.ry() = round(2 * center.y()) / 2.;
    m_p1 = QPointF(center.x() - rad, center.y());
    m_p2 = QPointF(center.x() + rad, center.y());
    m_center = center;
    m_radius = round(2 * rad) / 2.;
}

/**
 * @brief Constructs a circle from two points on the diameter.
 * @param p1 First point on the diameter.
 * @param p2 Second point on the diameter.
 */
CircleOutline::CircleOutline(QPointF p1, QPointF p2) {
    p1.rx() = round(2 * p1.x()) / 2.;
    p1.ry() = round(2 * p1.y()) / 2.;
    p2.rx() = round(2 * p2.x()) / 2.;
    p2.ry() = round(2 * p2.y()) / 2.;
    m_p1 = p1;
    m_p2 = p2;

    m_center.rx() = (p1.x() + p2.x()) / 2.;
    m_center.ry() = (p1.y() + p2.y()) / 2.;
    m_radius = fabs(p1.x() - p2.x()) / 2.;
}

/**
 * @brief Constructs a circle from a QJsonObject.
 * @param obj JSON object containing center and radius.
 */
CircleOutline::CircleOutline(const QJsonObject &obj)
    : CircleOutline(
          QPointF(obj["center_x"].toDouble(), obj["center_y"].toDouble()),
          obj["radius_x"].toDouble()) {}

/**
 * @brief Translates the circle by a given offset.
 * @param del Offset to translate by.
 */
void CircleOutline::translate(QPointF del) {
    m_center += del;
    m_p1 += del;
    m_p2 += del;
}

/**
 * @brief Scales the circle by a given factor.
 * @param factor Scale factor.
 */
void CircleOutline::scale(double factor) {
    m_radius *= factor;
    m_radius = round(m_radius * 2) / 2.;
    m_p1 *= factor;
    m_p2 *= factor;
    m_center.rx() = round(2 * m_center.x() * factor) / 2.;
    m_center.ry() = round(2 * m_center.y() * factor) / 2.;
}

/**
 * @brief Checks if the circle is valid (radius > 0).
 * @return True if valid, false otherwise.
 */
bool CircleOutline::isValid() { return m_radius > 0; }

/**
 * @brief Draws the circle using a QPainter.
 * @param dc QPainter to draw with.
 * @param scale Scale factor for drawing.
 * @param scale2 Optional second scale factor for y-axis.
 */
void CircleOutline::draw(QPainter &dc, double scale, double scale2) {
    if (m_radius == 0)
        return;
    if (scale2 < 0)
        scale2 = scale;

    dc.drawEllipse(m_center * scale, m_radius * scale, m_radius * scale2);
    dc.setPen(QPen(Qt::black, 3, Qt::DotLine));
    dc.drawEllipse(m_center * scale, m_radius * scale, m_radius * scale2);
    dc.drawLine((m_center.x() - m_radius - 5) * scale, m_center.y() * scale,
                scale * (m_center.x() + m_radius + 5), scale * m_center.y());
    dc.drawLine(scale * m_center.x(), m_center.y() - m_radius * scale2 - 5,
                scale * m_center.x(), m_center.y() + m_radius * scale2 + 5);
}

/**
 * @brief Generates points along the circumference of the circle.
 * @param cnt Number of points to generate.
 * @return QVector of points on the circle.
 */
QVector<QPointF> CircleOutline::makeCircleofPoints(int cnt) {
    QVector<QPointF> points;
    double step = (M_PI * 2.) / cnt;

    for (int i = 0; i < cnt; ++i) {
        double theta = i * step;
        int x, y;
        x = m_radius * cos(theta) + m_center.x();
        y = m_radius * sin(theta) + m_center.y();
        points << QPointF(x, y);
    }
    return points;
}

/**
 * @brief Enlarges the circle by a given delta.
 * @param del Amount to enlarge the radius.
 */
void CircleOutline::enlarge(int del) {
    m_radius += del;
    if (m_p1.rx() > m_p2.rx()) {
        QPointF tmp = m_p1;
        m_p1 = m_p2;
        m_p2 = tmp;
    }
    m_p1.rx() -= del;
    m_p2.rx() += del;
}

/**
 * @brief Serializes the circle to a QJsonObject.
 * @param obj JSON object to write to.
 */
void CircleOutline::toJson(QJsonObject &obj) {
    obj["center_x"] = m_center.x();
    obj["center_y"] = m_center.y();
    // in case some day in the future we allow elliptical
    // outlines the data format is ready to go
    obj["radius_x"] = m_radius;
    obj["radius_y"] = m_radius;
}
