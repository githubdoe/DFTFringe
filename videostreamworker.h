#pragma once

#include <QObject>
#include <QThread>
#include <QImage>
#include <QString>
#include <atomic>
#include <opencv2/opencv.hpp>

class VideoStreamWorker : public QObject {
    Q_OBJECT
public:
    explicit VideoStreamWorker(const QString &source, QObject *parent = nullptr);
    ~VideoStreamWorker() override;

public slots:
    void startStream();
    void stop();
    void setResolution(int width, int height); // <-- New slot

signals:
    void frameReady(cv::Mat);
    void streamError(const QString &errorMsg);

private:
    QString m_source;
    cv::VideoCapture m_cap;
    std::atomic<bool> m_running;
};
