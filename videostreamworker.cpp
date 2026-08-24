#include "videostreamworker.h"
#include <QDebug>

VideoStreamWorker::VideoStreamWorker(const QString &source, QObject *parent)
    : QObject(parent), m_source(source), m_running(false) {}

VideoStreamWorker::~VideoStreamWorker() {
    stop();
}

void VideoStreamWorker::startStream() {
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
        return;
    }
    else qDebug() << "stream started";

    m_running = true;

    while (m_running) {
        for (int i = 0; i < 3; ++i) {
            if (!m_running) break;
            m_cap.grab();
        }

        cv::Mat frame;
        if (m_cap.retrieve(frame) && !frame.empty()) {
            cv::Mat rgbFrame;
            cv::cvtColor(frame, rgbFrame, cv::COLOR_BGR2RGB);
            QImage img(rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888);
            emit frameReady(frame.clone());
        } else {
            emit streamError("Stream disconnected or frame empty.");
            break;
        }

        QThread::msleep(30);
    }

    if (m_cap.isOpened()) {
        m_cap.release();
    }
}

void VideoStreamWorker::stop() {
    m_running = false;
}

// Thread-safe method invoked via signal/slot connection
void VideoStreamWorker::setResolution(int width, int height) {
    if (m_cap.isOpened() && width > 0 && height > 0) {
        m_cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        qDebug() << "VideoStreamWorker: Resolution requested to set ->" << width << "x" << height;
    }
}
