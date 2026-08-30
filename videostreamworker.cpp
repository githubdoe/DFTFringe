#include "videostreamworker.h"
#include <QDebug>
#include <QTimer>
#include <QMutexLocker>

VideoStreamWorker::VideoStreamWorker(const QString &source, QObject *parent)
    : QObject(parent), m_source(source), m_running(false) {}

VideoStreamWorker::~VideoStreamWorker() {
    stop();
}

void VideoStreamWorker::startStream() {
    bool isInt = false;
    int camId = m_source.toInt(&isInt);

    {
        QMutexLocker locker(&m_mutex);
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
            return;
        }
    }

    qDebug() << "stream started";
    m_running = true;

}
void VideoStreamWorker::fetchNextFrame() {
    if (!m_running) return;

    cv::Mat frame;
    {
        QMutexLocker locker(&m_mutex);
        if (!m_running || !m_cap.isOpened()) return;

        // Grab and retrieve the latest frame from the buffer
        if (!m_cap.read(frame) || frame.empty()) {
            emit streamError("Stream disconnected or frame empty.");
            m_running = false;
            return;
        }
    }

    // Emit the frame back to the UI thread
    emit frameReady(frame.clone());
}



void VideoStreamWorker::stop() {
    m_running = false;
    QMutexLocker locker(&m_mutex);
    if (m_cap.isOpened()) {
        m_cap.release();
    }
}

void VideoStreamWorker::changeSource(QString newSource) {
    QMutexLocker locker(&m_mutex);
    if (m_cap.isOpened()) {
        m_cap.release();
    }

    qDebug() << "changing source" << newSource;
    m_source = newSource;
    bool isInt = false;
    int camId = m_source.toInt(&isInt);

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
    } else {
        qDebug() << "Stream source changed successfully to:" << m_source;
    }
}

void VideoStreamWorker::setResolution(int width, int height) {
    qDebug() << "set res";
    QMutexLocker locker(&m_mutex);
    if (m_cap.isOpened() && width > 0 && height > 0) {
        m_cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        qDebug() << "VideoStreamWorker: Resolution requested to set ->" << width << "x" << height;
    }
}
