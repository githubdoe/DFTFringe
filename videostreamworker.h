#pragma once

#include <QObject>
#include <QThread>
#include <QImage>
#include <QString>
#include <atomic>
#include <opencv2/opencv.hpp>
#include <QMutex>

class VideoStreamWorker : public QObject {
    Q_OBJECT
public:
    explicit VideoStreamWorker(const QString &source, QObject *parent = nullptr);
    ~VideoStreamWorker() override;

public slots:
    void startStream();
    void stop();
    void changeSource(QString newSource);
    void setResolution(int width, int height);

private slots:
    void processNextFrame(); // Handles one grab/retrieve cycle
signals:
    void frameReady(cv::Mat);
    void streamError(const QString &errorMsg);
private:
    cv::VideoCapture m_cap;
    QString m_source;
    bool m_running = false;
    QMutex m_mutex;
};


