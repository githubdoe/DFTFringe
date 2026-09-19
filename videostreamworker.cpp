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
    int consecutiveErrors = 0;
    const int maxStartupRetries = 10; // Allow a few empty frames on startup

    while (m_running) {
        cv::Mat frame;

        // Read from camera
        if (!m_cap.isOpened() || !m_cap.read(frame) || frame.empty()) {
            consecutiveErrors++;

            // If it's just starting up, webcams often send a few blank/empty frames.
            // Give it a moment instead of dying immediately.
            if (consecutiveErrors < maxStartupRetries) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            if (m_running) {
                emit streamError("Stream error or connection lost.");
                m_running = false;
            }
            break;
        }

        // Reset error count once we successfully get a real frame
        consecutiveErrors = 0;

        // Quickly update the latest frame buffer
        {
            QMutexLocker locker(&m_frameMutex);
            m_latestFrame = frame.clone();
        }
    }
}

void VideoStreamWorker::fetchNextFrame() {
    if (!m_running) return;
    static cv::Mat testpattern;
    if (false) {  // use a file for the test patttern.
        if (testpattern.empty()){
            // 1. Open a file dialog to select an image
                QString filePath = QFileDialog::getOpenFileName(
                    nullptr,
                    "Open Image",
                    "",
                    "Image Files (*.png *.jpg *.jpeg *.bmp)"
                );

                if (filePath.isEmpty()) {
                    return; // User canceled
                }
            testpattern = cv::imread(filePath.toStdString(), cv::IMREAD_COLOR);
        }
        emit frameReady(testpattern.clone());
        return;
    }
    if (false){// for noraml op make this false.  This creates a calibration target for debug.  THe first side lobe will be at bin 32.


        if (testpattern.empty()){   // that will be 32 cycles per mirror diameter.
        int innerWidth = 800;
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
