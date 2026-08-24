#include "custom3dinputhandler.h"

Custom3DInputHandler::Custom3DInputHandler(QAbstract3DGraph *graph)
    : Q3DInputHandler(graph), m_isRightDragging(false), m_graphRef(graph) {
    setZoomAtTargetEnabled(true);
}

void Custom3DInputHandler::mousePressEvent(QMouseEvent *event, const QPoint &mousePos) {
    if (event->button() == Qt::RightButton) {
        m_isRightDragging = true;
        m_lastMousePos = event->pos();
        event->accept();
        return; // Consume event so base handler doesn't interfere
    }

    // Map Left-Click to Right-Click for standard rotation in base handler
    Qt::MouseButton mappedButton = (event->button() == Qt::LeftButton) ? Qt::RightButton : event->button();
    Qt::MouseButtons mappedButtons = event->buttons();
    if (mappedButtons & Qt::LeftButton) {
        mappedButtons = (mappedButtons & ~Qt::LeftButton) | Qt::RightButton;
    }

    QMouseEvent customEvent(event->type(), event->localPos(), event->globalPos(),
                             mappedButton, mappedButtons, event->modifiers());
    Q3DInputHandler::mousePressEvent(&customEvent, mousePos);
}

void Custom3DInputHandler::mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos) {
    if (event->button() == Qt::RightButton) {
        m_isRightDragging = false;
        event->accept();
        return;
    }

    Qt::MouseButton mappedButton = (event->button() == Qt::LeftButton) ? Qt::RightButton : event->button();
    Qt::MouseButtons mappedButtons = event->buttons();
    if (mappedButtons & Qt::LeftButton) {
        mappedButtons = (mappedButtons & ~Qt::LeftButton) | Qt::RightButton;
    }

    QMouseEvent customEvent(event->type(), event->localPos(), event->globalPos(),
                             mappedButton, mappedButtons, event->modifiers());
    Q3DInputHandler::mouseReleaseEvent(&customEvent, mousePos);
}

void Custom3DInputHandler::mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos) {
    if (m_isRightDragging && m_graphRef) {
        QPoint delta = event->pos() - m_lastMousePos;
        m_lastMousePos = event->pos();

        // Adjust camera target to pan the view
        Q3DCamera *camera = m_graphRef->scene()->activeCamera();
        if (camera) {
            QVector3D target = camera->target();
            // Scale panning speed based on zoom distance or fixed factor
            float panScale = 0.003f;
            target.setX(target.x() - delta.x() * panScale);
            target.setZ(target.z() + delta.y() * panScale); // Invert Y delta for natural feel
            camera->setTarget(target);
        }
        event->accept();
        return;
    }

    Qt::MouseButtons mappedButtons = event->buttons();
    if (mappedButtons & Qt::LeftButton) {
        mappedButtons = (mappedButtons & ~Qt::LeftButton) | Qt::RightButton;
    }

    QMouseEvent customEvent(event->type(), event->localPos(), event->globalPos(),
                             event->button(), mappedButtons, event->modifiers());
    Q3DInputHandler::mouseMoveEvent(&customEvent, mousePos);
}
