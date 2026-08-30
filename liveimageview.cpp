#include "LiveImageView.h"
#include <QPainter>
#include <QPen>
#include <QCursor>
#include <cmath>

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
    if (event->button() == Qt::RightButton){
        // delete circle
        m_hasCircle = false;
        emit mirrorDefined(QRect());  // emit an invalid circle
        return;
    }
    // Shift-click Move existing circle if clicking inside the radius
    if ((event->button() == Qt::LeftButton && (event->modifiers() & Qt::ShiftModifier)) && m_hasCircle) {
        // Calculate current center and radius from the two defining edge/diameter points
        QPoint center = (m_centerImg + m_edgeImg) / 2;
        double dx = m_edgeImg.x() - m_centerImg.x();
        double dy = m_edgeImg.y() - m_centerImg.y();
        double radius = std::sqrt(dx * dx + dy * dy) / 2.0;

        double distToCenter = std::sqrt(std::pow(clickImg.x() - center.x(), 2) + std::pow(clickImg.y() - center.y(), 2));

        if (distToCenter <= radius) {
            m_state = InteractionState::DraggingCenter;
            // Store offset of click relative to the center
            m_dragOffsetImg = clickImg - center;
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
    }

    // Standard Left-click: Start defining a new circle edge-to-edge
    if (event->button() == Qt::LeftButton) {

        m_centerImg = clickImg; // First edge point
        if (m_dftModeActive)
            m_edgeImgDft = clickImg;
        else
            m_edgeImg = clickImg;   // Second edge point (tracks with mouse move)
        m_hasCircle = false;
        m_state = InteractionState::DrawingRadius; // Re-using state name for edge-to-edge drag
        setCursor(Qt::CrossCursor);
        update();
        event->accept();
    }
}

void LiveImageView::mouseMoveEvent(QMouseEvent *event) {
    QPoint currentImg = mapToImageCoordinates(event->pos());

    if (m_state == InteractionState::DrawingRadius) {
        // The second point is now the other side of the diameter
        m_edgeImg = currentImg;
        update();
    }
    else if (m_state == InteractionState::DraggingCenter) {
        // Move both points together maintaining the diameter vector
        QPoint currentCenter = (m_centerImg + m_edgeImg) / 2;
        QPoint radiusVector = m_edgeImg - currentCenter;

        QPoint newCenter = currentImg - m_dragOffsetImg;
        m_centerImg = newCenter - radiusVector;
        m_edgeImg = newCenter + radiusVector;
        update();
    }
    else {
        if (m_hasCircle) {
            QPoint center = (m_centerImg + m_edgeImg) / 2;
            double dx = m_edgeImg.x() - m_centerImg.x();
            double dy = m_edgeImg.y() - m_centerImg.y();
            double radius = std::sqrt(dx * dx + dy * dy) / 2.0;

            double distToCenter = std::sqrt(std::pow(currentImg.x() - center.x(), 2) + std::pow(currentImg.y() - center.y(), 2));

            if (distToCenter <= radius) {
                setCursor(Qt::OpenHandCursor);
            } else {
                setCursor(Qt::ArrowCursor);
            }
        }
    }
}

QRect LiveImageView::getMirrorRect() const {
    // Center is the midpoint between the two clicked edge points
    QPoint center = (m_centerImg + m_edgeImg) / 2;
    double dx = m_edgeImg.x() - m_centerImg.x();
    double dy = m_edgeImg.y() - m_centerImg.y();
    int radius = static_cast<int>(std::sqrt(dx * dx + dy * dy) / 2.0);

    int x = center.x() - radius;
    int y = center.y() - radius;
    int width = 2 * radius;
    int height = 2 * radius;
    return QRect(x, y, width, height);
}

void LiveImageView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        if (m_state == InteractionState::DrawingRadius) {
            double dx = m_edgeImg.x() - m_centerImg.x();
            double dy = m_edgeImg.y() - m_centerImg.y();
            int radius = static_cast<int>(std::sqrt(dx * dx + dy * dy));
            if (radius > 5) {
                m_hasCircle = true;
                emit mirrorDefined(getMirrorRect());
            }
            else {
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
    int numSteps = numDegrees / 15; // Standard mouse wheel step

    if (numSteps == 0) return;

    // If we have a valid mirror, check if the mouse is inside the circle
    bool insideCircle = false;
    if (m_hasCircle) {
        QPoint center = (m_centerImg + m_edgeImg) / 2;
        double dx = m_edgeImg.x() - m_centerImg.x();
        double dy = m_edgeImg.y() - m_centerImg.y();
        double radius = std::sqrt(dx * dx + dy * dy) / 2.0;

        double dist = std::sqrt(std::pow(imgPos.x() - center.x(), 2) + std::pow(imgPos.y() - center.y(), 2));
        if (dist <= radius) {
            insideCircle = true;
        }
    }

    if (insideCircle && m_hasCircle) {
        // Adjust the mirror radius when scrolling inside the circle
        // We push/pull the edge point away from or closer to the center
        QPoint center = (m_centerImg + m_edgeImg) / 2;
        double dx = m_edgeImg.x() - center.x();
        double dy = m_edgeImg.y() - center.y();
        double currentRadius = std::sqrt(dx * dx + dy * dy);

        // Scale radius by a small factor per wheel step (e.g., 5% per step)
        double scaleFactor = 1.0 + (numSteps * 0.05);
        double newRadius = std::max(10.0, currentRadius * scaleFactor);

        // Preserve angle and update edge point
        double angle = std::atan2(dy, dx);
        if (m_dftModeActive){
            m_edgeImgDft.setX(center.x() + static_cast<int>(newRadius * std::cos(angle)));
            m_edgeImgDft.setY(center.y() + static_cast<int>(newRadius * std::sin(angle)));
        }
        else {
            m_edgeImg.setX(center.x() + static_cast<int>(newRadius * std::cos(angle)));
            m_edgeImg.setY(center.y() + static_cast<int>(newRadius * std::sin(angle)));
        }
        // Keep center fixed, update edge
        // m_centerImg remains opposite or we adjust center relative to edge.
        // With edge-to-edge drag, m_centerImg and m_edgeImg define the diameter endpoints.
        // Let's recalculate m_centerImg to keep the center invariant:
        QPoint offset(static_cast<int>(newRadius * std::cos(angle)), static_cast<int>(newRadius * std::sin(angle)));
        m_centerImg = center - offset;
        m_edgeImg = center + offset;

        update();
        emit mirrorDefined(getMirrorRect());
        event->accept();
    } else {
        // Scroll wheel outside the circle handles general image zooming
        // (Optional: require Ctrl or just let scrolling zoom if not over circle)
        double zoomFactorChange = (numSteps > 0) ? 1.15 : 1.0 / 1.15;
        //double newZoom = std::clamp(m_zoomFactor * zoomFactorChange, 0.2, 8.0);
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

        QPoint p1Scaled(static_cast<int>(m_centerImg.x() * m_zoomFactor),
                        static_cast<int>(m_centerImg.y() * m_zoomFactor));
        QPoint p2Scaled(static_cast<int>(m_edgeImg.x() * m_zoomFactor),
                        static_cast<int>(m_edgeImg.y() * m_zoomFactor));

        QPoint centerScaled = (p1Scaled + p2Scaled) / 2;
        double dx = p2Scaled.x() - p1Scaled.x();
        double dy = p2Scaled.y() - p1Scaled.y();
        int radiusScaled = static_cast<int>(std::sqrt(dx * dx + dy * dy) / 2.0);


            // Dashed green boundary
            QPen pen(Qt::green, 2, Qt::DashLine);
            painter.setPen(pen);

            painter.drawEllipse(centerScaled, radiusScaled, radiusScaled);

    }

}

void LiveImageView::setFilterPercentage(double p, int dftSize) {
    // 1. Center of the DFT image in native coordinates
    QPointF dftCenter(dftSize / 2.0, dftSize / 2.0);

    // 2. Reference radius and target radius in native coordinates
    double baseRadius = dftSize / 2.0;
    double targetRadius = baseRadius * p;

    // 3. Store the native edge point into a DFT-specific member variable (e.g., m_edgeImgDft)
    m_edgeImgDft = QPoint(dftCenter.x() + targetRadius, dftCenter.y());

    // 4. Trigger a redraw
    update();
}
