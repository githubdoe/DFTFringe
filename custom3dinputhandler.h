#ifndef CUSTOM3DINPUTHANDLER_H
#define CUSTOM3DINPUTHANDLER_H

#include <QtDataVisualization/Q3DInputHandler>
#include <QtDataVisualization/QAbstract3DGraph>
#include <QtDataVisualization/Q3DCamera>
#include <QMouseEvent>

using namespace QtDataVisualization;

class Custom3DInputHandler : public Q3DInputHandler {
    Q_OBJECT
public:
    Custom3DInputHandler(QAbstract3DGraph *graph = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event, const QPoint &mousePos) override;
    void mouseReleaseEvent(QMouseEvent *event, const QPoint &mousePos) override;
    void mouseMoveEvent(QMouseEvent *event, const QPoint &mousePos) override;

private:
    bool m_isRightDragging;
    QPoint m_lastMousePos;
    QAbstract3DGraph *m_graphRef;
};

#endif // CUSTOM3DINPUTHANDLER_H
