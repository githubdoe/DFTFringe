#include "videostreamworker.h"
#include <QDebug>
#include <QFileDialog>

VideoStreamWorker::VideoStreamWorker(const QString &source, QObject *parent)
    : QObject(parent), m_source(source) {}

VideoStreamWorker::~VideoStreamWorker() {
    stop();
}

void VideoStreamWorker::startStream() {
    QMutexLocker locker(&m_controlMutex);

    if (m_running) return;

    bool isInt = false;
    int camId = m_source.toInt(&isInt);
    qputenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "stimeout;2000000;");

    if (m_cap.isOpened()) {
        m_cap.release();
    }

    if (isInt) {
#if defined(_WIN32) || defined(_WIN64)
        m_cap.open(camId, cv::CAP_DSHOW);
#else
        m_cap.open(camId);
#endif
    } else {
        m_cap.open(m_source.toStdString());
    }

    if (!m_cap.isOpened()) {
        emit streamError("Failed to open stream source: " + m_source);
        m_running = false;
        return;
    }

    qDebug() << "Stream started successfully";
    m_running = true;

    // Launch the background capture loop in a dedicated std::thread
    m_captureThread = std::unique_ptr<std::thread>(new std::thread(&VideoStreamWorker::captureLoop, this));

    emit streamStarted();
}

void VideoStreamWorker::captureLoop() {
    while (m_running) {
        cv::Mat frame;

        // Read from camera (blocking call, but safe because m_cap is owned here)
        if (!m_cap.isOpened() || !m_cap.read(frame) || frame.empty()) {
            if (m_running) {
                emit streamError("Stream error or connection lost.");
                m_running = false;
            }
            break;
        }

        // Quickly update the latest frame buffer
        {
            QMutexLocker locker(&m_frameMutex);
            m_latestFrame = frame.clone();
        }
    }
}

void VideoStreamWorker::fetchNextFrame() {
    if (!m_running) return;

    // Optional debug test patterns can remain here if desired,
    // but normal operation just grabs the latest captured frame:
    cv::Mat frameToSend;
    {
        QMutexLocker locker(&m_frameMutex);
        if (m_latestFrame.empty()) return;
        frameToSend = m_latestFrame.clone();
    }

    emit frameReady(frameToSend);
}

void VideoStreamWorker::stop() {
    m_running = false;

    // Wait for the background capture thread to finish cleanly
    if (m_captureThread && m_captureThread->joinable()) {
        m_captureThread->join();
    }

    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened()) {
        m_cap.release();
    }
}

void VideoStreamWorker::changeSource(QString newSource) {
    // Stop current stream and join thread safely
    stop();

    {
        QMutexLocker locker(&m_controlMutex);
        qDebug() << "Changing source to:" << newSource;
        m_source = newSource;
    }

    // Restart with the new source
    startStream();
}

void VideoStreamWorker::setResolution(int width, int height) {
    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() && width > 0 && height > 0) {
        m_cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        qDebug() << "VideoStreamWorker: Resolution requested to set ->" << width << "x" << height;
    }
}
