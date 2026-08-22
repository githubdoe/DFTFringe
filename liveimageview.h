#pragma once

#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QRect>

class LiveImageView : public QLabel {
    Q_OBJECT
public:
    explicit LiveImageView(QWidget *parent = nullptr);
    double m_centerPercent = .1;
    void setZoomFactor(double zoom);
    bool hasValidMirror() const { return m_hasCircle; }
    QRect getMirrorRect() const; // Returns rect in original image coordinates
    void setDftModeActive(bool active) { m_dftModeActive = active; update(); }
    void setFilterPercentage(double p, int dftSize);
signals:
    void mirrorDefined(const QRect &rect);
    void requestZoomChange(double zoomFactor);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    enum class InteractionState {
        None,
        DrawingRadius,
        DraggingCenter
    };

    InteractionState m_state = InteractionState::None;
    double m_zoomFactor = 1.0;
    bool m_hasCircle = false;
    bool m_dftModeActive = false;
    QPoint m_centerImg;      // Circle center in original image pixels
    QPoint m_edgeImg;        // Edge point for radius calculation
    QPoint m_dragOffsetImg;  // Offset when grabbing the center to move the circle
    QPoint m_edgeImgDft;     // DFT circle edge.
    QPoint mapToImageCoordinates(const QPoint &widgetPos) const;
};
