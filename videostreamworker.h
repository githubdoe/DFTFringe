#pragma once

#include <QObject>
#include <QMutex>
#include <QString>
#include <opencv2/opencv.hpp>
#include <thread>
#include <atomic>
#include <QSize>

class VideoStreamWorker : public QObject {
    Q_OBJECT
public:
    explicit VideoStreamWorker(const QString &source, QObject *parent = nullptr);
    ~VideoStreamWorker();
    struct CameraConfiguration {
        QSize resolution;
        double exposure;
        double auto_exposure;
        double brightness;
        double gain;
    };
    bool getCameraConfiguration(CameraConfiguration &config);
    bool getCameraGainRange(bool &bGain, double &min, double &max, double &gain);
    void setCameraGain(bool bGain, double gain);
    bool getCameraExposureRange(double &min, double &max, double &exposure);
    void getAutoExpSetting(bool &bFeatureAvailable, bool &bAuto);
    void setAutoExpSetting(bool bAuto, bool bLockMutex = true);

public slots:
    void startStream();
    void fetchNextFrame();
    void stop();
    void changeSource(QString newSource);
    void setResolution(int width, int height);
    void dumpAll();
    void requestCameraSetting(QString prop , double val);


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

    bool m_gain_mode=true; // this is set to false if gain is unavailable and we will be using brightness settings in lieu of gain
    int savedResolutionWidth = 640;
    int savedResolutionHeight = 480;

};
