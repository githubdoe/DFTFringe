#include "videostreamworker.h"
#include <QDebug>
#include <QFileDialog>
#include <QSettings>
#include "spdlog/spdlog.h"

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

    spdlog::get("logger")->trace("Stream started successfully");

    m_running = true;

    // Launch the background capture loop in a dedicated std::thread
    m_captureThread = std::unique_ptr<std::thread>(new std::thread(&VideoStreamWorker::captureLoop, this));

    emit streamStarted();
}



void VideoStreamWorker::captureLoop() {
    int errorCount = 0;
    QSettings set;
    int maxErrorLimit = set.value("liveViewConnectFrames",15).toInt(); // Number of failed attempts before giving up

    while (m_running) {
        cv::Mat frame;
        bool success = false;

        {
            QMutexLocker locker(&m_controlMutex);
            if (m_cap.isOpened()) {

                success = m_cap.read(frame) && !frame.empty();

            }
        }

        if (success) {
            // Successful read: reset error counter and update live frame
            errorCount = 0;
            QMutexLocker locker(&m_frameMutex);
            m_latestFrame = frame.clone();

            cv::Scalar meanValue = cv::mean(frame);
            double averagePixel = meanValue[0];
            //qDebug() << "avg: " << averagePixel;
            if (averagePixel > 0)
                continue;

            // error! - we need to reconnect
            spdlog::get("logger")->error("resetting connection because frame is blank");
            {
                QMutexLocker locker2(&m_controlMutex);
                m_cap.release();
                bool isInt = false;
                int camId = m_source.toInt(&isInt);
                m_source.toInt(&isInt);
                if (isInt) {
                    #if defined(_WIN32) || defined(_WIN64)
                    m_cap.open(camId, cv::CAP_DSHOW);
                    #else
                    m_cap.open(camId);
                    #endif
                } else {
                    m_cap.open(m_source.toStdString());
                }
            }
            if (m_cap.isOpened()) {
                setResolution(savedResolutionWidth, savedResolutionHeight);
            }

        } else {
            errorCount++;

            spdlog::get("logger")->trace("errorCount {}", errorCount);
            // Create a status/error image canvas (e.g., 800x600 dark background)
            cv::Mat errorFrame(600, 800, CV_8UC3, cv::Scalar(30, 30, 30));

            std::string msg = "Connecting / Waiting for Stream...";
            std::string countMsg = "Attempt: " + std::to_string(errorCount) + " of " + std::to_string(maxErrorLimit);

            int baseline = 0;
            double fontScale = 0.8;
            int thickness = 2;

            // Draw primary warning message (Centered, Orange/Yellow)
            cv::Size textSize = cv::getTextSize(msg, cv::FONT_HERSHEY_SIMPLEX, fontScale, thickness, &baseline);
            cv::Point textOrg((errorFrame.cols - textSize.width) / 2, (errorFrame.rows / 2) - 20);
            cv::putText(errorFrame, msg, textOrg, cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0, 165, 255), thickness);

            // Draw error count below it (Centered, White)
            double countScale = 0.6;
            cv::Size countSize = cv::getTextSize(countMsg, cv::FONT_HERSHEY_SIMPLEX, countScale, 1, &baseline);
            cv::Point countOrg((errorFrame.cols - countSize.width) / 2, (errorFrame.rows / 2) + 25);
            cv::putText(errorFrame, countMsg, countOrg, cv::FONT_HERSHEY_SIMPLEX, countScale, cv::Scalar(255, 255, 255), 1);

            // Publish the status frame so the client UI can show it
            {
                QMutexLocker locker(&m_frameMutex);
                m_latestFrame = errorFrame;
            }

            // If we've exceeded the limit, give up and close the stream
            if (errorCount >= maxErrorLimit) {
                if (m_running) {
                    emit streamError("Stream connection failed after multiple attempts.");
                    m_running = false;
                }
                break;
            }

            // Sleep briefly between retry attempts so it doesn't spin at 100% CPU
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        spdlog::get("logger")->trace("Changing source to: {}", newSource.toStdString());

        m_source = newSource;
    }

    // Restart with the new source
    startStream();
}

void VideoStreamWorker::setResolution(int width, int height) {

    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() && width > 0 && height > 0) {
        bool isInt = false;
        m_source.toInt(&isInt);

        // Only apply resolution changes to local USB cameras, not URL streams
        if (isInt) {
            m_cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
            m_cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
            spdlog::get("logger")->trace("VideoStreamWorker: Resolution requested to set -> {} x {}", width, height);
            savedResolutionWidth = width;
            savedResolutionHeight = height;
        }
    }
}

void VideoStreamWorker::requestCameraSetting(QString prop , double val) {
    spdlog::get("logger")->trace("setting camera property");

    QMutexLocker locker(&m_controlMutex);
    bool isInt = false;
    m_source.toInt(&isInt);
    if (isInt == false)
        return;

    if (prop == "Exposure") {
        setAutoExpSetting(false, false);
        m_cap.set(cv::CAP_PROP_EXPOSURE, val);
    }

    if (prop == "Gain") {
        setAutoExpSetting(false, false);
        int prop_id = cv::CAP_PROP_GAIN;
        if (m_gain_mode == false)
            prop_id = cv::CAP_PROP_BRIGHTNESS;
        m_cap.set(prop_id, val);
    }
}

void VideoStreamWorker::dumpAll() {
    // send all camera properties to debug stream
    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened()) {
        bool isInt = false;
        m_source.toInt(&isInt);

        // Only apply resolution changes to local USB cameras, not URL streams
        if (isInt) {
            struct PropertyMap {
                cv::VideoCaptureProperties id;
                const char* name;
            };
            const std::vector<PropertyMap> properties = {
                { cv::CAP_PROP_POS_MSEC,      "CAP_PROP_POS_MSEC" },
                { cv::CAP_PROP_POS_FRAMES,    "CAP_PROP_POS_FRAMES" },
                { cv::CAP_PROP_POS_AVI_RATIO, "CAP_PROP_POS_AVI_RATIO" },
                { cv::CAP_PROP_FRAME_WIDTH,   "CAP_PROP_FRAME_WIDTH" },
                { cv::CAP_PROP_FRAME_HEIGHT,  "CAP_PROP_FRAME_HEIGHT" },
                { cv::CAP_PROP_FPS,           "CAP_PROP_FPS" },
                { cv::CAP_PROP_FOURCC,        "CAP_PROP_FOURCC" },
                { cv::CAP_PROP_FRAME_COUNT,   "CAP_PROP_FRAME_COUNT" },
                { cv::CAP_PROP_FORMAT,        "CAP_PROP_FORMAT" },
                { cv::CAP_PROP_MODE,          "CAP_PROP_MODE" },
                { cv::CAP_PROP_BRIGHTNESS,    "CAP_PROP_BRIGHTNESS" },
                { cv::CAP_PROP_CONTRAST,      "CAP_PROP_CONTRAST" },
                { cv::CAP_PROP_SATURATION,    "CAP_PROP_SATURATION" },
                { cv::CAP_PROP_HUE,           "CAP_PROP_HUE" },
                { cv::CAP_PROP_GAIN,          "CAP_PROP_GAIN" },
                { cv::CAP_PROP_EXPOSURE,      "CAP_PROP_EXPOSURE" },
                { cv::CAP_PROP_CONVERT_RGB,   "CAP_PROP_CONVERT_RGB" },
                { cv::CAP_PROP_WHITE_BALANCE_BLUE_U, "CAP_PROP_WHITE_BALANCE_BLUE_U" },
                { cv::CAP_PROP_RECTIFICATION, "CAP_PROP_RECTIFICATION" },
                { cv::CAP_PROP_MONOCHROME,    "CAP_PROP_MONOCHROME" },
                { cv::CAP_PROP_SHARPNESS,     "CAP_PROP_SHARPNESS" },
                { cv::CAP_PROP_AUTO_EXPOSURE, "CAP_PROP_AUTO_EXPOSURE" },
                { cv::CAP_PROP_GAMMA,         "CAP_PROP_GAMMA" },
                { cv::CAP_PROP_TEMPERATURE,   "CAP_PROP_TEMPERATURE" },
                { cv::CAP_PROP_TRIGGER,       "CAP_PROP_TRIGGER" },
                { cv::CAP_PROP_TRIGGER_DELAY, "CAP_PROP_TRIGGER_DELAY" },
                { cv::CAP_PROP_WHITE_BALANCE_RED_V, "CAP_PROP_WHITE_BALANCE_RED_V" },
                { cv::CAP_PROP_ZOOM,          "CAP_PROP_ZOOM" },
                { cv::CAP_PROP_FOCUS,         "CAP_PROP_FOCUS" },
                { cv::CAP_PROP_GUID,          "CAP_PROP_GUID" },
                { cv::CAP_PROP_ISO_SPEED,     "CAP_PROP_ISO_SPEED" },
                { cv::CAP_PROP_BACKLIGHT,     "CAP_PROP_BACKLIGHT" },
                { cv::CAP_PROP_PAN,           "CAP_PROP_PAN" },
                { cv::CAP_PROP_TILT,          "CAP_PROP_TILT" },
                { cv::CAP_PROP_ROLL,          "CAP_PROP_ROLL" },
                { cv::CAP_PROP_IRIS,          "CAP_PROP_IRIS" },
                { cv::CAP_PROP_SETTINGS,      "CAP_PROP_SETTINGS" },
                { cv::CAP_PROP_BUFFERSIZE,    "CAP_PROP_BUFFERSIZE" },
                { cv::CAP_PROP_AUTOFOCUS,     "CAP_PROP_AUTOFOCUS" }
            };

            for (const auto& prop : properties) {
                double value = m_cap.get(prop.id);

                // OpenCV returns -1 or 0 for unsupported properties depending on the backend
                if (value == -1) {
                    qDebug().noquote() << QString("%1: NOT SUPPORTED").arg(prop.name, -30);
                } else {
                     qDebug().noquote() << QString("%1: %2").arg(prop.name, -30).arg(value);
                }
            }

        }
    }
}

bool VideoStreamWorker::getCameraConfiguration(CameraConfiguration &config) {
    // meant to be called only from Ui thread - return false if config is invalid

    spdlog::get("logger")->trace("getCameraConfiguration");

    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() == false)
        return false;
    bool isInt = false;
    m_source.toInt(&isInt);
    if (isInt == false)
        return false;
    config.auto_exposure = m_cap.get(cv::CAP_PROP_AUTO_EXPOSURE);
    config.exposure = m_cap.get(cv::CAP_PROP_EXPOSURE);
    config.brightness = m_cap.get(cv::CAP_PROP_BRIGHTNESS);
    config.gain = m_cap.get(cv::CAP_PROP_GAIN);
    config.resolution.setHeight(m_cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    config.resolution.setWidth(m_cap.get(cv::CAP_PROP_FRAME_WIDTH));

    return true;
}

void findMinMaxBinarySearch(cv::VideoCapture &m_cap, int propid, double &min, double &max, double &val) {
    double high = 10000;
    double low = val;
    double guess;
    double actual;

    //
    // search for max
    //

    while(true) {
        if (high-low <= 1) {
            max = low;
            break;
        }

        guess = (high+low)/2;
        guess = std::round(guess);
        m_cap.set(propid, guess);
        actual = m_cap.get(propid);
        if (guess == actual)
            low = guess;
        else
            high = guess;

    }

    //
    // search for min
    //

    high = val;
    low = -10000;
    while(true) {
        if (high-low <= 1) {
            min = high;
            break;
        }

        guess = (high+low)/2;
        guess = std::round(guess);
        m_cap.set(propid, guess);
        actual = m_cap.get(propid);
        if (guess == actual)
            high = guess;
        else
            low = guess;
    }
    m_cap.set(propid, val);
}

bool VideoStreamWorker::getCameraGainRange(bool &bGain, double &min, double &max, double &gain) {
    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() == false)
        return false;
    bool isInt = false;
    m_source.toInt(&isInt);
    if (isInt == false)
        return false;
    spdlog::get("logger")->trace("getCameraGainRange");
    bGain = true;
    gain = m_cap.get(cv::CAP_PROP_GAIN);
    if (gain == -1) {
        bGain = false;
        gain = m_cap.get(cv::CAP_PROP_BRIGHTNESS);
    }
    m_gain_mode = gain;

    // now set to -10,000 and 10,000 to find min and max
    int propid = cv::CAP_PROP_GAIN;
    if (bGain == false)
        propid = cv::CAP_PROP_BRIGHTNESS;

    m_cap.set(propid,-10000.0);
    min = m_cap.get(propid);
    m_cap.set(propid,10000.0);
    max = m_cap.get(propid);
    m_cap.set(propid,gain);


    if (max != min) {
        return true;
    }

    // max is min.  We need to do a binary search.
    findMinMaxBinarySearch(m_cap, propid, min, max, gain);

    return true;
}

void VideoStreamWorker::setCameraGain(bool bGain, double gain) {
    spdlog::get("logger")->trace("setCameraGain");

    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() == false)
        return;
    bool isInt = false;
    m_source.toInt(&isInt);
    if (isInt == false)
        return;
    int propid = cv::CAP_PROP_GAIN;
    if (bGain == false)
        propid = cv::CAP_PROP_BRIGHTNESS;
    m_cap.set(propid,gain);
}


bool VideoStreamWorker::getCameraExposureRange(double &min, double &max, double &exposure) {
    spdlog::get("logger")->trace("getCameraExposureRange");
    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() == false)
        return false;
    bool isInt = false;
    m_source.toInt(&isInt);
    if (isInt == false)
        return false;

    exposure = m_cap.get(cv::CAP_PROP_EXPOSURE);

    m_cap.set(cv::CAP_PROP_EXPOSURE,-10000.0);
    min = m_cap.get(cv::CAP_PROP_EXPOSURE);
    m_cap.set(cv::CAP_PROP_EXPOSURE,10000.0);
    max = m_cap.get(cv::CAP_PROP_EXPOSURE);
    m_cap.set(cv::CAP_PROP_EXPOSURE,exposure);

    if (max != min) {
        return true;
    }

    // max is min.  We need to do a binary search.
    findMinMaxBinarySearch(m_cap, cv::CAP_PROP_EXPOSURE, min, max, exposure);

    return true;
}

void VideoStreamWorker::getAutoExpSetting(bool &bFeatureAvailable, bool &bAuto) {
    // .75 auto on, .25 auto off  alternately 3=auto on and 1 auto off; -1 means unavailable
    QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() == false)
        return;
    bool isInt = false;
    m_source.toInt(&isInt);
    if (isInt == false) {
        bFeatureAvailable=false;
        return;
    }
    double val = m_cap.get(cv::CAP_PROP_AUTO_EXPOSURE);
    if (val == -1) {
        bFeatureAvailable = false;
        return;
    }
    bFeatureAvailable = true;
    if (val == .25 || val == 1) {
        bAuto = false;
        return;
    }
    if (val == 0.75 || val == 3) {
        bAuto = true;
        return;
    }
    bFeatureAvailable = false;
    spdlog::get("logger")->error("strange auto exp value: {}", val);

}

void VideoStreamWorker::setAutoExpSetting(bool bAuto, bool bLockMutex) {
    if (bLockMutex)
        QMutexLocker locker(&m_controlMutex);
    if (m_cap.isOpened() == false)
        return;
    bool isInt = false;
    m_source.toInt(&isInt);
    if (isInt == false)
        return;
    double val = m_cap.get(cv::CAP_PROP_AUTO_EXPOSURE);
    if (bAuto && val == .25) {
        m_cap.set(cv::CAP_PROP_AUTO_EXPOSURE, .75);
        return;
    }
    if (bAuto && val == 1) {
        m_cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 3);
        return;
    }
    if (!bAuto && val == .75) {
        m_cap.set(cv::CAP_PROP_AUTO_EXPOSURE, .25);
        return;
    }
    if (!bAuto && val == 3) {
        m_cap.set(cv::CAP_PROP_AUTO_EXPOSURE, 1);
        return;
    }
    // if we get here, either we don't know what value to use or the feature is unavailable
}
