#include "liveimageview.h"
#include <QCursor>
#include <cmath>
#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QShowEvent>
LiveImageView::LiveImageView(QWidget *parent) : QLabel(parent) {
    setFocusPolicy(Qt::StrongFocus);
}

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
        if (m_hasYellowCircle) {

            m_state = InteractionState::ResizingYellowRadius;
            setCursor(Qt::BlankCursor);
            double dx = clickImg.x() - m_yellowCenter.x();
            double dy = clickImg.y() - m_yellowCenter.y();
            m_yellowRadius = std::hypot(dx, dy);

            emit yellowRadiusChanged(m_yellowRadius);
         return;
        }
    }

    if (event->button() == Qt::LeftButton) {
        if (m_helpOverlay)
            m_helpOverlay->hide();
        double distToGreen = m_hasCircle ? std::hypot(clickImg.x() - m_nativeCenter.x(), clickImg.y() - m_nativeCenter.y()) : 99999.0;
        if ( m_hasCircle && distToGreen <= m_nativeRadius) {
            m_state = InteractionState::DraggingGreenCenter;
            m_dragOffsetImg = clickImg - m_nativeCenter.toPoint();
            setCursor(Qt::ClosedHandCursor);
            event->accept();

            emit outlineChanging(true);
            return;

        }
    }
}

void LiveImageView::mouseMoveEvent(QMouseEvent *event) {
    QPoint currentPoint = mapToImageCoordinates(event->pos());

    if (m_state == InteractionState::ResizingYellowRadius) {
        double dx = currentPoint.x() - m_yellowCenter.x();
        double dy = currentPoint.y() - m_yellowCenter.y();
        m_yellowRadius = std::hypot(dx, dy);
        qDebug() << "Rad" << m_yellowRadius;
        emit yellowRadiusChanged(m_yellowRadius);
    }

    else if (m_state == InteractionState::DraggingGreenCenter) {
        m_nativeCenter = currentPoint - m_dragOffsetImg;
        emit mirrorDefined(m_nativeCenter, m_nativeRadius);
    }

}

void LiveImageView::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {

        if (m_state == InteractionState::DraggingGreenCenter) {

        }

        m_state = InteractionState::None;
        setCursor(Qt::ArrowCursor);
        event->accept();
    }
}

void LiveImageView::keyPressEvent(QKeyEvent *event) {
    // Determine step size (e.g., hold Shift for a larger step, say 10 pixels)
    double step = (event->modifiers() & Qt::ShiftModifier) ? 10.0 : 1.0;

    bool handled = true;
    switch (event->key()) {
    case Qt::Key_Left:
        m_nativeCenter.rx() -= step;
        break;
    case Qt::Key_Right:
        m_nativeCenter.rx() += step;
        break;
    case Qt::Key_Up:
        m_nativeCenter.ry() -= step;
        break;
    case Qt::Key_Down:
        m_nativeCenter.ry() += step;
        break;
    case Qt::Key_Shift:
        if (!event->isAutoRepeat()) {
            emit shiftStateChanged(true);
        }
    case Qt::Key_Control:
        if (m_helpOverlay && m_helpOverlay->isVisible()) {
            m_helpOverlay->hide();
        }else if (m_helpOverlay){
            m_helpOverlay->show();
        }


        break;
        handled = false; // Let default handling run if needed
        break;
    default:
        handled = false;
        break;
    }

    if (handled) {
        update(); // Redraw the label to show the new point position
        emit mirrorDefined(m_nativeCenter, m_nativeRadius); // Notify listeners
        event->accept();
    } else {
        QLabel::keyPressEvent(event);
    }
}

void LiveImageView::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Shift) {
        if (!event->isAutoRepeat()) {
            emit shiftStateChanged(false);
            emit mirrorDefined(m_nativeCenter, m_nativeRadius);
            emit outlineChanging(false);

        }
    }
    QWidget::keyReleaseEvent(event);
}

void LiveImageView::wheelEvent(QWheelEvent *event) {
    QPoint imgPos = mapToImageCoordinates(event->position().toPoint());
    int numDegres = event->angleDelta().y() / 8;
    int numSteps = -numDegres / 15;

    if (numSteps == 0) return;

    // Check Yellow Circle Hover
    if (m_hasYellowCircle) {
        double scaleFactor = 1.0 + (numSteps * 0.05);
        m_yellowRadius = std::max(5.0, m_yellowRadius * scaleFactor);
        emit yellowRadiusChanged(m_yellowRadius);
        event->accept();
        return;
    }

}


#include <QTimer>
#include <QLabel>
#include <QShowEvent>

void LiveImageView::showEvent(QShowEvent *event) {
    QLabel::showEvent(event);

    // If you only want this to happen the *first* time it's opened:
    if (m_hasShownHelp) return;
    m_hasShownHelp = true;

    // 1. Create the overlay lazily as a single QLabel if it doesn't exist yet
    if (!m_helpOverlay) {
        QLabel *helpLbl = new QLabel(this);
        helpLbl->setAttribute(Qt::WA_TransparentForMouseEvents); // Let clicks pass through

        // Single stylesheet handles background, text color, rounded corners, and larger padding/font
                helpLbl->setStyleSheet(
                    "background-color: rgba(0, 100, 100, 70);" // Slightly more opaque
                    "color: #ffffff;"
                    "border-radius: 8px;"
                    "padding: 16px;"                         // More padding for breathing room
                    "font-size: 20px;"                       // Increased base font size
                );

                // Use HTML formatting with a larger title
                helpLbl->setText(
                    "• <b>Left Click + Drag:</b> Adjust mirror outline green circle<br>"
                    "• <b>Right Click</b> Set filter diameter.<br>"
                    "• <b>Mouse Wheel:</b> Increase\\decrease filter<br>"
                    "• <b>Arrow Keys:</b> Nudge green outline<br>"
                    "• <b>Hold Shift:</b> Toggle fullscreen view"
                    "<br><br><b>ctrl</b> Toggle to display\\hide this"
                );



        m_helpOverlay = helpLbl;
    }

    // 2. Position and size it appropriately (auto-sizes to content, but we can set a fixed width/height)
    m_helpOverlay->adjustSize();
    m_helpOverlay->move(10, 10); // Top-left corner with a 10px margin
    m_helpOverlay->show();
    m_helpOverlay->raise();


}
