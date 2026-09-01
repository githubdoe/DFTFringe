#include "custom3dinputhandler.h"
// required only with Qt5 build
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


            // Right vector: horizontal, unaffected by pitch
            QVector3D right(qCos(xRotRad), 0.0f, -qSin(xRotRad));

            // Forward vector: direction the camera is looking
            QVector3D forward(qSin(xRotRad) * qCos(yRotRad),
                              -qSin(yRotRad),
                              qCos(xRotRad) * qCos(yRotRad));

            // True camera-up = right × forward, always orthogonal to both
            QVector3D up = QVector3D::crossProduct(forward, right).normalized();

            float dxScreen = -delta.x() * panScale;
            float dyScreen =  delta.y() * panScale * 2;

            target += right * dxScreen + up * dyScreen;
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
