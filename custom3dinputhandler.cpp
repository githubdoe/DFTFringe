#include "custom3dinputhandler.h"
#include <QtMath>
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMouseEvent customEvent(event->type(), event->position(), event->globalPosition(),
                             mappedButton, mappedButtons, event->modifiers());
#else
    QMouseEvent customEvent(event->type(), event->localPos(), event->globalPos(),
                             mappedButton, mappedButtons, event->modifiers());
#endif
    Q3DInputHandler::mousePressEvent(&customEvent, mousePos);
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMouseEvent customEvent(event->type(), event->position(), event->globalPosition(),
                             mappedButton, mappedButtons, event->modifiers());
#else
    QMouseEvent customEvent(event->type(), event->localPos(), event->globalPos(),
                             mappedButton, mappedButtons, event->modifiers());
#endif
    Q3DInputHandler::mousePressEvent(&customEvent, mousePos);
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
            float panScale = 0.003f;

            // Extract both rotations in radians
            float xRotRad = qDegreesToRadians(camera->xRotation()); // Yaw (horizontal orbit)
            float yRotRad = qDegreesToRadians(camera->yRotation()); // Pitch (vertical tilt)

            float cosX = qCos(xRotRad);
            float sinX = qSin(xRotRad);
            float cosY = qCos(yRotRad);
            float sinY = qSin(yRotRad);

            // Invert Y delta (flipped back so dragging down moves the scene down)
            float invertedDeltaY = -delta.y();

            float dx = (-delta.x() * cosX - invertedDeltaY * sinX * cosY) * panScale;
            float dy = (invertedDeltaY * sinY) * panScale;
            float dz = (delta.x() * sinX - invertedDeltaY * cosX * cosY) * panScale;

            target.setX(target.x() + dx);
            target.setY(target.y() + dy);
            target.setZ(target.z() + dz);

            camera->setTarget(target);
        }
        event->accept();
        return;
    }

    Qt::MouseButtons mappedButtons = event->buttons();
    if (mappedButtons & Qt::LeftButton) {
        mappedButtons = (mappedButtons & ~Qt::LeftButton) | Qt::RightButton;
    }

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QMouseEvent customEvent(event->type(), event->position(), event->globalPosition(),
                             event->button(), mappedButtons, event->modifiers());
#else
    QMouseEvent customEvent(event->type(), event->localPos(), event->globalPos(),
                             event->button(), mappedButtons, event->modifiers());
#endif
    Q3DInputHandler::mouseMoveEvent(&customEvent, mousePos);
}
