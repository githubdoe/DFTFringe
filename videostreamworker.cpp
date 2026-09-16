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
    qputenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "stimeout;2000000;");
    {
        QMutexLocker locker(&m_mutex);

        // Release any existing capture instance first
        if (m_cap.isOpened()) {
            m_cap.release();
        }

        if (isInt) {
#if defined(_WIN32) || defined(_WIN64)
            // Use DirectShow on Windows with a quick check
            m_cap.open(camId, cv::CAP_DSHOW);
#else
            m_cap.open(camId);
#endif
        } else {
            // For network URLs, open with standard backend
            m_cap.open(m_source.toStdString());
        }

        // If it fails to open immediately, emit error and abort cleanly without hanging
        if (!m_cap.isOpened()) {
            emit streamError("Failed to open stream source: " + m_source);
            m_running = false;
            return;
        }
    }

    qDebug() << "stream started successfully";
    m_running = true;
    emit streamStarted();
}
void VideoStreamWorker::fetchNextFrame() {
    if (!m_running) return;
    static cv::Mat testpattern;
    if (false){
        if (testpattern.empty()){
        int innerWidth = 1024;
        int stripeWidth = 16;
        int borderWidth = 10; // Extra room for the outline/crop boundary
        int totalSize = innerWidth + (borderWidth * 2);

        // Create a larger canvas filled with black (the border background)
        cv::Mat img(totalSize, totalSize, CV_8UC1, cv::Scalar(0));

        // Define the region for the 1024x1024 striped circle, offset by the border
        cv::Rect roiRect(borderWidth, borderWidth, innerWidth, innerWidth);
        cv::Mat innerRegion = img(roiRect);

        // Draw the 16-pixel stripes inside the inner region
        for (int x = 0; x < innerWidth; x += stripeWidth) {
            int currentWidth = std::min(stripeWidth, innerWidth - x);
            if ((x / stripeWidth) % 2 == 1) {
                cv::Mat stripeRoi = innerRegion(cv::Rect(x, 0, currentWidth, innerWidth));
                stripeRoi.setTo(cv::Scalar(255));
            }
        }

        // Apply a circular mask to the inner region so it matches the expected mirror outline
        cv::Mat circularMask(innerWidth, innerWidth, CV_8UC1, cv::Scalar(0));
        cv::Point center(innerWidth / 2.0, innerWidth / 2.0);
        double radius = innerWidth / 2.0;
        cv::circle(circularMask, center, radius, cv::Scalar(255), -1);

        cv::Mat maskedInner;
        innerRegion.copyTo(maskedInner, circularMask);
        maskedInner.copyTo(innerRegion); // Copy back into the bordered canvas
        testpattern = img.clone();
        }
            emit frameReady(testpattern.clone());
        return;
    }

    cv::Mat frame;
    {
        QMutexLocker locker(&m_mutex);
        if (!m_running || !m_cap.isOpened()) return;

        // 1. Fast-forward through the buffer, but check if the stream dies
//        for (int i = 0; i < 1; ++i) {
//            if (!m_cap.grab()) {
//                emit streamError("Stream connection lost.");
//                m_running = false;
//                return;
//            }
//        }

        // 2. Read the final live frame
        if (!m_cap.read(frame) || frame.empty()) {
            emit streamError("Stream error or connection lost.");
            m_running = false;
            return;
        }
    }

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
    bool success = false;

    {
        QMutexLocker locker(&m_mutex);
        m_running = false; // Temporarily halt fetching during switch

        if (m_cap.isOpened()) {
            m_cap.release();
        }

        qDebug() << "Changing source to:" << newSource;
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

        if (m_cap.isOpened()) {
            m_running = true;
            success = true;
        }
    } // Mutex lock is automatically released here

    // Emit signals OUTSIDE the mutex block to prevent deadlocks
    if (!success) {
        emit streamError("Failed to open stream source: " + newSource);
    } else {
        qDebug() << "Stream source changed successfully to:" << newSource;
        emit streamStarted();
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
