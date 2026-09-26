#pragma once
#pragma once
#include <QObject>
#include <QPixmap>
#include <QPointF>
#include <opencv2/opencv.hpp>

struct LiveViewParams {
    bool dftChecked;
    int dftLowThreshold;
    double vividValue;
    double zoomFactor;
    bool outlineChanging;
    double mirrorOutlineRadius;
    QPointF mirrorOutlineCenter;
    int dftSize;
    double imageDownScale;
    double centerFilterRadius;
    QRect userMirrorRect;
};
// Register it so Qt can pass it across threads safely
Q_DECLARE_METATYPE(LiveViewParams)
class FrameProcessorWorker : public QObject {
    Q_OBJECT
private:
    double m_scale;
public:
    FrameProcessorWorker(QObject *parent = nullptr) : QObject(parent) {};

    cv::Mat computeLiveDFT(const cv::Mat &inputFrame, int targetSize, const QRect &roi);


public slots:
void processFrame(const cv::Mat frame, LiveViewParams params);

signals:
    void frameProcessed(const QPixmap &pixmap, double elapsedMs, QSize originalSized, double m_scale, double DFTScale);

private:
    // Local cached copies of parameters to avoid race conditions with GUI widgets
    bool m_dftEnabled = false;
    double m_mirrorOutlineRadius = 0;
    QPointF m_mirrorOutlineCenter;
    bool m_outlineChanging = false;
    int m_dftSize = 512;
    double m_zoomFactor = 1.0;
};




