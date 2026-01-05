#include "profilecurve.h"
#include <QPainter>
#include "profilecurve.h"
#include <qpainter.h>
#include <qpainterpath.h>
#include <qwt_scale_map.h>

// creates a curve of the profile and will highlight segments where the slope is violated if the user has that feature enabled.
// uses the base class for most of the work.  Overrides the actual painting of the line.
ProfileCurve::ProfileCurve(const QString &title)
    : QwtPlotCurve(title)
    , m_showSlopeError(false)
    , m_slopeLimit(0.0)
    , m_highlightWidth(2)
{
    // Ensure we are using basic line style for the base profile
    setStyle(QwtPlotCurve::Lines);
}

void ProfileCurve::setSlopeSettings(bool show, double limit, int highlightWidth)
{
    m_showSlopeError = show;
    m_slopeLimit = limit;
    m_highlightWidth = highlightWidth;
}

#include <QPainterPath> // Ensure this is at the top of profilecurve.cpp

#include <qwt_scale_map.h>
#include <qpainter.h>

void ProfileCurve::drawLines(QPainter *painter,
                             const QwtScaleMap &xMap,
                             const QwtScaleMap &yMap,
                             const QRectF &canvasRect,
                             int from, int to) const
{
    Q_UNUSED(canvasRect); // drawLines is an override function, we shall not change args
    if (to <= from) return;

    painter->save();

    // 1. MANUALLY DRAW THE BASE BLACK LINE (Replacing Qwt's default)
    // This prevents Qwt from drawing its own spider lines.
    QPen basePen = pen();
    painter->setPen(basePen);

    for (int i = from; i < to; ++i) {
        QPointF p1 = sample(i);
        QPointF p2 = sample(i + 1);

        // If either point is NaN, skip this segment entirely (lifts the pen)
        if (std::isnan(p1.y()) || std::isnan(p2.y())) continue;

        // Draw the segment only between two valid points
        double x1 = xMap.transform(p1.x());
        double y1 = yMap.transform(p1.y());
        double x2 = xMap.transform(p2.x());
        double y2 = yMap.transform(p2.y());

        painter->drawLine(QPointF(x1, y1), QPointF(x2, y2));
    }

    // 2. DRAW THE ORANGE HIGHLIGHTS
    if (m_showSlopeError && m_slopeLimit > 0.0) {
        QPen orangePen(QColor("orange"), m_highlightWidth);
        orangePen.setCapStyle(Qt::RoundCap);
        painter->setPen(orangePen);

        for (int i = from; i < to; ++i) {
            QPointF p1 = sample(i);
            QPointF p2 = sample(i + 1);

            if (std::isnan(p1.y()) || std::isnan(p2.y())) continue;

            if (std::abs(p1.y() - p2.y()) > m_slopeLimit) {
                painter->drawLine(
                    QPointF(xMap.transform(p1.x()), yMap.transform(p1.y())),
                    QPointF(xMap.transform(p2.x()), yMap.transform(p2.y()))
                );
            }
        }
    }

    painter->restore();
}
