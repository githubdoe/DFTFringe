#ifndef LIVEIMAGEVIEW_H
#define LIVEIMAGEVIEW_H

#include <QLabel>
#include <QPointF>
#include <QMouseEvent>
#include <QWheelEvent>

class LiveImageView : public QLabel {
    Q_OBJECT
public:
    explicit LiveImageView(QWidget *parent = nullptr);

    void setZoomFactor(double zoom);
    void setGreenCircle(const QPointF &center, double radius);
    void setYellowCircle(const QPointF &center, double radius);

signals:
    void mirrorDefined(QPointF center, double radius);
    void yellowRadiusChanged(double radius);
    void requestZoomChange(double newZoom);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    QPoint mapToImageCoordinates(const QPoint &widgetPos) const;

    enum class InteractionState {
        None,
        DrawingGreenRadius,
        DraggingGreenCenter,
        ResizingYellowRadius
    };

    InteractionState m_state = InteractionState::None;
    double m_zoomFactor = 1.0;

    // Green Circle Cache (Fully adjustable: center & radius)
    QPointF m_nativeCenter;
    QPointF m_firstEdgePoint;
    double m_nativeRadius = 0.0;
    bool m_hasCircle = false;

    // Yellow Circle Cache (Radius-only adjustment, fixed center from parent)
    QPointF m_yellowCenter;
    double m_yellowRadius = 0.0;
    bool m_hasYellowCircle = false;

    QPoint m_dragOffsetImg;
};

#endif // LIVEIMAGEVIEW_H
