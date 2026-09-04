#include "LiveImageView.h"
#include <QPainter>
#include <QPen>
#include <QCursor>
#include <cmath>
#include <QDebug>

LiveImageView::LiveImageView(QWidget *parent) : QLabel(parent) {}

void LiveImageView::setZoomFactor(double zoom) {
    m_zoomFactor = zoom;
    update();
}

QPoint LiveImageView::mapToImageCoordinates(const QPoint &widgetPos) const {
    if (m_zoomFactor <= 0.0) return widgetPos;
    int imgX = static_cast<int>(widgetPos.x() / m_zoomFactor);
    int imgY = static_cast<int>(widgetPos.y() / m_zoomFactor);
    return QPoint(imgX, imgY);
}

void LiveImageView::mousePressEvent(QMouseEvent *event) {
    QPoint clickImg = mapToImageCoordinates(event->pos());

    if (event->button() == Qt::RightButton) {
        m_hasCircle = false;
        emit mirrorDefined(QRect());
        update();
        return;
    }

    // Shift-click: Move existing circle if clicking inside the radius
    if ((event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) && m_hasCircle) {
        double distToCenter = std::sqrt(std::pow(clickImg.x() - m_nativeCenter.x(), 2) +
                                        std::pow(clickImg.y() - m_nativeCenter.y(), 2));

        if (distToCenter <= m_nativeRadius) {
            m_state = InteractionState::DraggingCenter;
            m_dragOffsetImg = clickImg - m_nativeCenter.toPoint();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
    }

    // Standard Left-click: Start defining a new circle from the center out or edge-to-edge
    if (event->button() == Qt::LeftButton) {
        m_nativeCenter = clickImg;
        m_nativeRadius = 0.0;
        m_hasCircle = false;
        m_state = InteractionState::DrawingRadius;
        setCursor(Qt::CrossCursor);
        update();
        event->accept();
    }
}

void LiveImageView::mouseMoveEvent(QMouseEvent *event) {
    QPoint currentImg = mapToImageCoordinates(event->pos());

    if (m_state == InteractionState::DrawingRadius) {
        // Calculate radius dynamically as mouse drags outward from m_nativeCenter
        double dx = currentImg.x() - m_nativeCenter.x();
        double dy = currentImg.y() - m_nativeCenter.y();
        m_nativeRadius = std::sqrt(dx * dx + dy * dy);
        update();
    }
    else if (m_state == InteractionState::DraggingCenter) {
        QPoint newCenter = currentImg - m_dragOffsetImg;
        m_nativeCenter = newCenter;
        update();
    }
    else {
        if (m_hasCircle) {
            double distToCenter = std::sqrt(std::pow(currentImg.x() - m_nativeCenter.x(), 2) +
                                            std::pow(currentImg.y() - m_nativeCenter.y(), 2));

            if (distToCenter <= m_nativeRadius) {
                setCursor(Qt::OpenHandCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
    }
}

QRect LiveImageView::getMirrorRect() const {
    int x = static_cast<int>(m_nativeCenter.x() - m_nativeRadius);
    int y = static_cast<int>(m_nativeCenter.y() - m_nativeRadius);
    int width = static_cast<int>(2 * m_nativeRadius);
    int height = static_cast<int>(2 * m_nativeRadius);
    return QRect(x, y, width, height);
}

void LiveImageView::setOutsideCircle(QPointF center, double radius) {
    m_nativeCenter = center;
    m_nativeRadius = radius;
    m_hasCircle = true;
    emit mirrorDefined(getMirrorRect());
    update();
}

void LiveImageView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        if (m_state == InteractionState::DrawingRadius) {
            if (m_nativeRadius > 5.0) {
                m_hasCircle = true;
                emit mirrorDefined(getMirrorRect());
            } else {
                m_hasCircle = false;
                emit mirrorDefined(QRect());
            }
        }

        m_state = InteractionState::None;
        setCursor(Qt::ArrowCursor);
        update();
        event->accept();
    }
}

void LiveImageView::wheelEvent(QWheelEvent *event) {
    QPoint imgPos = mapToImageCoordinates(event->position().toPoint());
    int numDegrees = event->angleDelta().y() / 8;
    int numSteps = numDegrees / 15;

    if (numSteps == 0) return;

    bool insideCircle = false;
    if (m_hasCircle) {
        double dist = std::sqrt(std::pow(imgPos.x() - m_nativeCenter.x(), 2) +
                                std::pow(imgPos.y() - m_nativeCenter.y(), 2));
        if (dist <= m_nativeRadius) {
            insideCircle = true;
        }
    }

    if (insideCircle && m_hasCircle) {
        // Scale radius via scroll wheel steps
        double scaleFactor = 1.0 + (numSteps * 0.05);
        m_nativeRadius = std::max(10.0, m_nativeRadius * scaleFactor);

        update();
        emit mirrorDefined(getMirrorRect());
        event->accept();
    } else {
        double zoomFactorChange = (numSteps > 0) ? 1.15 : 1.0 / 1.15;
        double newZoom = std::max(0.2, std::min(m_zoomFactor * zoomFactorChange, 8.0));
        emit requestZoomChange(newZoom);
        event->accept();
    }
}

void LiveImageView::paintEvent(QPaintEvent *event) {
    QLabel::paintEvent(event); // Render underlying pixmap

    if (m_state != InteractionState::None || m_hasCircle) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // Scale native center and radius precisely once by the current zoom factor
        QPointF centerScaled(m_nativeCenter.x() * m_zoomFactor, m_nativeCenter.y() * m_zoomFactor);
        double radiusScaled = m_nativeRadius * m_zoomFactor;

        QPen pen(Qt::green, 2, Qt::DashLine);
        painter.setPen(pen);
        painter.drawEllipse(centerScaled, radiusScaled, radiusScaled);
    }
}

void LiveImageView::setFilterPercentage(double p, int dftSize) {
    QPointF dftCenter(dftSize / 2.0, dftSize / 2.0);
    double baseRadius = dftSize / 2.0;
    double targetRadius = baseRadius * p;

    m_nativeCenter = dftCenter;
    m_nativeRadius = targetRadius;
    update();
}
