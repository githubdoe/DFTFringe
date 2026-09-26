#include "frameprocessworker.h"
#include <QElapsedTimer>
#include <QPainter>
#include <QDebug>
void FrameProcessorWorker::processFrame(const cv::Mat frame, LiveViewParams params) {
    if (frame.empty()) return;
qDebug() << "Processing loop start";
    QElapsedTimer timer;
    timer.start();

    // 1. Copy your existing renderCurrentFrame() logic here

    cv::Mat displayMat = frame;
    if (displayMat.channels() == 1) {
        cv::cvtColor(displayMat, displayMat, cv::COLOR_GRAY2BGR);
    }

    // 1. Determine ROI around the mirror outline + 10 pixel padding if outline exists
    cv::Rect cropRoi(0, 0, displayMat.cols, displayMat.rows);
    bool hasOutline = (params.mirrorOutlineRadius > 0) && !params.outlineChanging;

    if (hasOutline) {
        int pad = 10;
        int xMin = static_cast<int>(params.mirrorOutlineCenter.x() - params.mirrorOutlineRadius - pad);
        int yMin = static_cast<int>(params.mirrorOutlineCenter.y() - params.mirrorOutlineRadius - pad);
        int xMax = static_cast<int>(params.mirrorOutlineCenter.x() + params.mirrorOutlineRadius + pad);
        int yMax = static_cast<int>(params.mirrorOutlineCenter.y() + params.mirrorOutlineRadius + pad);

        cv::Rect desiredRoi(xMin, yMin, xMax - xMin, yMax - yMin);
        cropRoi = desiredRoi & cv::Rect(0, 0, displayMat.cols, displayMat.rows);

        if (cropRoi.width > 0 && cropRoi.height > 0) {
            displayMat = displayMat(cropRoi).clone();
        } else {
            cropRoi = cv::Rect(0, 0, displayMat.cols, displayMat.rows);
        }
    }
    double DFTscale = 1.;
    if (params.dftChecked) {
        // 2. Compute raw DFT
        cv::Mat dftRaw = computeLiveDFT(frame, params.dftSize, params.userMirrorRect);
        if (dftRaw.empty()) return;

        // 3. Convert to float and apply logarithmic scaling on the raw DFT directly
        cv::Mat dftFloat;
        dftRaw.convertTo(dftFloat, CV_32F);

        cv::Mat dftLog = dftFloat;

        // 4. Compute statistics on the true DFT data
        double minVal, maxVal;
        cv::minMaxLoc(dftLog, &minVal, &maxVal);

        cv::Scalar meanVal, stdDevVal;
        cv::meanStdDev(dftLog, meanVal, stdDevVal);

        // 5. Clip dynamic range based on statistics
        double floorVal = meanVal[0] + 2 * stdDevVal[0];

        int val = params.dftLowThreshold;
        if (val != -1) {
            floorVal = val;
        }

        double ceilVal = meanVal[0] + (maxVal - meanVal[0])/params.vividValue;

        cv::Mat dftClamped;
        cv::threshold(dftLog, dftClamped, floorVal, 0, cv::THRESH_TOZERO);

        cv::Mat dftShifted = dftClamped - floorVal;
        double range = ceilVal - floorVal;
        if (range < 1e-5) range = 1.0;

        cv::Mat dftNorm;
        dftShifted.convertTo(dftNorm, CV_8U, 255.0 / range);

        // 6. Apply Jet colormap to the square DFT
        cv::Mat dftColorSquare;
        cv::applyColorMap(dftNorm, dftColorSquare, cv::COLORMAP_JET);

        cv::Mat alphaMaskSquare;
        dftNorm.convertTo(alphaMaskSquare, CV_32F, 1.0 / 255.0);

        // 7. Resize and center to fit the cropped displayMat while keeping aspect ratio
        DFTscale = std::min(
                    static_cast<double>(displayMat.cols) / dftColorSquare.cols,
                    static_cast<double>(displayMat.rows) / dftColorSquare.rows
                    );

        int newWidth = static_cast<int>(dftColorSquare.cols * DFTscale);
        int newHeight = static_cast<int>(dftColorSquare.rows * DFTscale);

        int x = (displayMat.cols - newWidth) / 2;
        int y = (displayMat.rows - newHeight) / 2;
        cv::Rect roi(x, y, newWidth, newHeight);

        roi &= cv::Rect(0, 0, displayMat.cols, displayMat.rows);
        if (roi.width <= 0 || roi.height <= 0) return;

        cv::Mat resizedColor, resizedAlpha;
        cv::resize(dftColorSquare, resizedColor, roi.size(), 0, 0, cv::INTER_LINEAR);
        cv::resize(alphaMaskSquare, resizedAlpha, roi.size(), 0, 0, cv::INTER_LINEAR);

        // 8. Localized Alpha Blending
        cv::Mat scaledAlpha;
        cv::multiply(resizedAlpha, 2.7, scaledAlpha);
        cv::threshold(scaledAlpha, scaledAlpha, 1.0, 1.0, cv::THRESH_TRUNC);

        cv::Mat bgFloat, fgFloat;
        displayMat(roi).convertTo(bgFloat, CV_32FC3);
        resizedColor.convertTo(fgFloat, CV_32FC3);

        std::vector<cv::Mat> alphaChannels(3, scaledAlpha);
        cv::Mat alpha3C;
        cv::merge(alphaChannels, alpha3C);

        cv::Mat blended;
        cv::multiply(fgFloat, alpha3C, fgFloat);
        cv::multiply(bgFloat, cv::Scalar::all(1.0) - alpha3C, bgFloat);
        cv::add(bgFloat, fgFloat, blended);

        blended.convertTo(displayMat(roi), CV_8UC3);
    }




    //        imageSize->setText(QString("%1 x %2 frame:%3")
    //                           .arg(img.size().width())
    //                           .arg(img.size().height())
    //                           .arg(cnt++));


    // Calculate elapsed time (in milliseconds with fractional decimal places)

    QImage img;
    if (displayMat.type() == CV_8UC3) {
        cv::Mat rgb;
        cv::cvtColor(displayMat, rgb, cv::COLOR_BGR2RGB);
        img = QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else if (displayMat.type() == CV_8UC1) {
        img =  QImage(displayMat.data, displayMat.cols, displayMat.rows, displayMat.step, QImage::Format_Grayscale8).copy();
    }


    QPainter dftpainter(&img);

    if (!params.outlineChanging && params.dftChecked) {
        dftpainter.setBrush(QColor(0, 0, 100, 70));
        dftpainter.setPen(QPen(Qt::yellow, 2));
        int centerx = img.width() / 2;
        int centery = img.height() / 2;

        double effectiveMirrorRadius = params.mirrorOutlineRadius * params.imageDownScale;
        double binDftSpace = params.centerFilterRadius * (static_cast<double>(params.dftSize) / (effectiveMirrorRadius * 2.0));
        int bin = static_cast<int>(binDftSpace * DFTscale);

        dftpainter.drawEllipse(QPointF(centerx, centery), bin, bin);
    }

    if (params.mirrorOutlineRadius != 0) {
        dftpainter.setPen(QPen(Qt::green, 2));
        QPointF adjustedCenter(params.mirrorOutlineCenter.x() - cropRoi.x, params.mirrorOutlineCenter.y() - cropRoi.y);
        dftpainter.drawEllipse(adjustedCenter, params.mirrorOutlineRadius, params.mirrorOutlineRadius);
    }

    int targetWidth = static_cast<int>(img.width() * params.zoomFactor);
    int targetHeight = static_cast<int>(img.height() * params.zoomFactor);

    QPixmap finalPixmap = QPixmap::fromImage(img).scaled(targetWidth, targetHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    double elapsedMs = static_cast<double>(timer.nsecsElapsed()) / 1000000.0;

    // 3. Emit the finished product back to the GUI thread
    emit frameProcessed(finalPixmap, elapsedMs, img.size(), m_scale, DFTscale);
}

cv::Mat FrameProcessorWorker::computeLiveDFT(const cv::Mat &inputFrame, int targetSize, const QRect &roi) {
    cv::Mat gray;
    if (inputFrame.channels() == 3) {
        cv::cvtColor(inputFrame, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = inputFrame;
    }

    cv::Mat workingArea;
    if (roi.isValid() && roi.width() > 10 && roi.height() > 10) {
        QRect clamped = roi.intersected(QRect(0, 0, gray.cols, gray.rows));
        if (!clamped.isEmpty()) {
            cv::Rect cvRoi(clamped.x(), clamped.y(), clamped.width(), clamped.height());
            workingArea = gray(cvRoi).clone();

            cv::Point center(workingArea.cols / 2, workingArea.rows / 2);
            int radius = std::min(center.x, center.y);
            cv::Mat mask = cv::Mat::zeros(workingArea.size(), CV_8UC1);
            cv::circle(mask, center, radius, cv::Scalar(255), -1);
            workingArea.setTo(0, mask == 0);
        } else {
            workingArea = gray;
        }
    } else {
        workingArea = gray;
    }

    // 1. Calculate uniform scale, ensuring we ONLY scale down (cap scale at 1.0)
    int maxDim = std::max(workingArea.cols, workingArea.rows);
    m_scale = static_cast<double>(targetSize) / maxDim;
    if (m_scale > 1.0) {
        m_scale = 1.0;
    }

    int newW = std::round(workingArea.cols * m_scale);
    int newH = std::round(workingArea.rows * m_scale);

    // Safety fallback if dimensions are somehow invalid
    if (newW <= 0) newW = 1;
    if (newH <= 0) newH = 1;

    cv::Mat resized;
    cv::resize(workingArea, resized, cv::Size(newW, newH), 0, 0, cv::INTER_AREA);

    // 2. Create target canvas and center the resized image with strict bounds protection
    cv::Mat padded = cv::Mat::zeros(targetSize, targetSize, workingArea.type());

    int xOffset = std::max(0, (targetSize - newW) / 2);
    int yOffset = std::max(0, (targetSize - newH) / 2);

    int copyW = std::min(newW, targetSize - xOffset);
    int copyH = std::min(newH, targetSize - yOffset);

    if (copyW > 0 && copyH > 0 && xOffset >= 0 && yOffset >= 0 &&
            (xOffset + copyW) <= targetSize && (yOffset + copyH) <= targetSize) {
        cv::Rect targetRoi(xOffset, yOffset, copyW, copyH);
        cv::Rect sourceRoi(0, 0, copyW, copyH);
        resized(sourceRoi).copyTo(padded(targetRoi));
    }

    cv::Mat floatImg;
    padded.convertTo(floatImg, CV_32F);

    cv::Mat complexImg;
    cv::dft(floatImg, complexImg, CV_DXT_FORWARD | cv::DFT_COMPLEX_OUTPUT);

    std::vector<cv::Mat> planes;
    cv::split(complexImg, planes);
    cv::Mat mag;
    cv::magnitude(planes[0], planes[1], mag);

    mag += cv::Scalar::all(1);
    cv::log(mag, mag);

    mag = mag(cv::Rect(0, 0, mag.cols & -2, mag.rows & -2));
    int cx = mag.cols / 2;
    int cy = mag.rows / 2;

    cv::Mat q0(mag, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(mag, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(mag, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(mag, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);

    cv::normalize(mag, mag, 0, 255, cv::NORM_MINMAX, CV_8UC1);
    return mag;
}


