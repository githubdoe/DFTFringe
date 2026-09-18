#pragma once

#include <QObject>
#include <QMutex>
#include <QString>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>

class VideoStreamWorker : public QObject {
    Q_OBJECT
public:
    explicit VideoStreamWorker(const QString &source, QObject *parent = nullptr);
    ~VideoStreamWorker();

public slots:
    void startStream();
    void fetchNextFrame();
    void stop();
    void changeSource(QString newSource);
    void setResolution(int width, int height);

signals:
    void streamStarted();
    void streamError(const QString &errorStr);
    void frameReady(const cv::Mat &frame);

private:
    void captureLoop();

    QString m_source;
    cv::VideoCapture m_cap;
    cv::Mat m_latestFrame;

    QMutex m_frameMutex;      // Protects m_latestFrame
    QMutex m_controlMutex;    // Protects m_cap and stream state changes

    std::atomic<bool> m_running{false};
    std::unique_ptr<std::thread> m_captureThread;
};
