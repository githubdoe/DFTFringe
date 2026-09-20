#include "liveimageview.h"
#include <QCursor>
#include <cmath>
#include <QDebug>
LiveImageView::LiveImageView(QWidget *parent) : QLabel(parent) {}

void LiveImageView::setZoomFactor(double zoom) {
    m_zoomFactor = zoom;
    update();
}

void LiveImageView::setGreenCircle(const QPointF &center, double radius) {
    m_nativeCenter = center;
    m_nativeRadius = radius;
    m_hasCircle = true;

}

void LiveImageView::setYellowCircle(const QPointF &center, double radius) {
    m_yellowCenter = center;
    m_yellowRadius = radius;
    m_hasYellowCircle = true;

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
        emit mirrorDefined(m_nativeCenter, m_nativeRadius);
        return;
    }

    if (event->button() == Qt::LeftButton) {
        // 1. Check if clicking inside the yellow circle (radius-only resizing)
        if (m_hasYellowCircle) {
            double distToYellow = std::hypot(clickImg.x() - m_yellowCenter.x(), clickImg.y() - m_yellowCenter.y());
            if (distToYellow <= m_yellowRadius) {
                m_state = InteractionState::ResizingYellowRadius;
                setCursor(Qt::SizeFDiagCursor);
                event->accept();
                return;
            }
        }

        // 2. Shift-click or clicking inside green circle to drag its center
        double distToGreen = m_hasCircle ? std::hypot(clickImg.x() - m_nativeCenter.x(), clickImg.y() - m_nativeCenter.y()) : 99999.0;
        if ((event->modifiers() & Qt::ShiftModifier) && m_hasCircle && distToGreen <= m_nativeRadius) {
            m_state = InteractionState::DraggingGreenCenter;
            m_dragOffsetImg = clickImg - m_nativeCenter.toPoint();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            emit outlineChanging(true);
            return;
        }

        // 3. Default: Start drawing a new green circle edge-to-edge
        m_nativeCenter = clickImg;
        m_firstEdgePoint = clickImg;
        m_nativeRadius = 0.0;
        m_hasCircle = false;
        m_state = InteractionState::DrawingGreenRadius;
        setCursor(Qt::CrossCursor);
        event->accept();
        emit outlineChanging(true);
    }
}

void LiveImageView::mouseMoveEvent(QMouseEvent *event) {
    QPoint currentPoint = mapToImageCoordinates(event->pos());
qDebug() << "move";
    if (m_state == InteractionState::ResizingYellowRadius) {
        double dx = currentPoint.x() - m_yellowCenter.x();
        double dy = currentPoint.y() - m_yellowCenter.y();
        m_yellowRadius = std::hypot(dx, dy);
        emit yellowRadiusChanged(m_yellowRadius);
    }
    else if (m_state == InteractionState::DrawingGreenRadius) {
        double dx = currentPoint.x() - m_firstEdgePoint.x();
        double dy = currentPoint.y() - m_firstEdgePoint.y();
        m_nativeRadius = std::hypot(dx, dy) / 2.0;
        m_nativeCenter.setX((currentPoint.x() + m_firstEdgePoint.x()) / 2);
        m_nativeCenter.setY((currentPoint.y() + m_firstEdgePoint.y()) / 2);
        emit mirrorDefined(m_nativeCenter, m_nativeRadius);
    }
    else if (m_state == InteractionState::DraggingGreenCenter) {
        m_nativeCenter = currentPoint - m_dragOffsetImg;
        emit mirrorDefined(m_nativeCenter, m_nativeRadius);
    }
    else {
        // Cursor feedback for hovering
        bool overYellow = m_hasYellowCircle && (std::hypot(currentPoint.x() - m_yellowCenter.x(), currentPoint.y() - m_yellowCenter.y()) <= m_yellowRadius);
        bool overGreen = m_hasCircle && (std::hypot(currentPoint.x() - m_nativeCenter.x(), currentPoint.y() - m_nativeCenter.y()) <= m_nativeRadius);

        if (overYellow) {
            setCursor(Qt::SizeFDiagCursor);
        } else if (overGreen) {
            setCursor(Qt::OpenHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void LiveImageView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        if (m_state == InteractionState::ResizingYellowRadius) {
            emit yellowRadiusChanged(m_yellowRadius);
        }
        else if (m_state == InteractionState::DraggingGreenCenter) {
            emit mirrorDefined(m_nativeCenter, m_nativeRadius);
            qDebug() << "release1";
            emit outlineChanging(false);
        }
        else if (m_state == InteractionState::DrawingGreenRadius) {
            if (m_nativeRadius > 5.0) {
                m_hasCircle = true;
            } else {
                m_hasCircle = false;
            }
            emit mirrorDefined(m_nativeCenter, m_nativeRadius);
            qDebug() << "release2";
            emit outlineChanging(false);
        }

        m_state = InteractionState::None;
        setCursor(Qt::ArrowCursor);
        event->accept();
    }
}

void LiveImageView::wheelEvent(QWheelEvent *event) {
    QPoint imgPos = mapToImageCoordinates(event->position().toPoint());
    int numDegrees = event->angleDelta().y() / 8;
    int numSteps = numDegrees / 15;

    if (numSteps == 0) return;

    // 1. Check Yellow Circle Hover
    if (m_hasYellowCircle && (std::hypot(imgPos.x() - m_yellowCenter.x(), imgPos.y() - m_yellowCenter.y()) <= m_yellowRadius)) {
        double scaleFactor = 1.0 + (numSteps * 0.05);
        m_yellowRadius = std::max(5.0, m_yellowRadius * scaleFactor);
        emit yellowRadiusChanged(m_yellowRadius);
        event->accept();
        return;
    }

    // 2. Check Green Circle Hover
    if (m_hasCircle && (std::hypot(imgPos.x() - m_nativeCenter.x(), imgPos.y() - m_nativeCenter.y()) <= m_nativeRadius)) {
        double scaleFactor = 1.0 + (numSteps * 0.05);
        m_nativeRadius = std::max(10.0, m_nativeRadius * scaleFactor);
        emit mirrorDefined(m_nativeCenter, m_nativeRadius);
        event->accept();
        return;
    }

    // 3. Fallback: Window Zoom / Resize Request to Parent
    double zoomFactorChange = (numSteps > 0) ? 1.15 : 1.0 / 1.15;
    double newZoom = std::max(0.2, std::min(m_zoomFactor * zoomFactorChange, 8.0));
    emit requestZoomChange(newZoom);
    event->accept();
}
