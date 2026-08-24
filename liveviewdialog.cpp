#include "liveviewdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCloseEvent>
#include <QTextEdit>
#include <QFormLayout>
#include <QSettings>
#include <QApplication>
#include <QPainter>
#include "videostreamworker.h"
// ==========================================
// LiveViewDialog Implementation
// ==========================================

LiveViewDialog::LiveViewDialog(const QString &defaultStreamUrl, QWidget *parent)
    : QDialog(parent),  tabWidget(nullptr) {

    setWindowTitle("DFTFringe - Live View");
    setAttribute(Qt::WA_DeleteOnClose, true);
    resize(850, 750);

    // 1. Build the UI components
    setupUI(defaultStreamUrl);

    QSettings settings;
    QString savedUrl = settings.value("LiveView/streamUrl", defaultStreamUrl).toString();



    // 2. Initialize video stream
    // In LiveViewDialog initialization (constructor):
    m_thread = new QThread(this);
    m_worker = new VideoStreamWorker(savedUrl); //
    m_worker->moveToThread(m_thread);

    // Connect thread lifecycle to worker cleanup
    connect(m_thread, &QThread::started, m_worker, &VideoStreamWorker::startStream);
    connect(m_thread, &QThread::finished, m_worker, &QObject::deleteLater);
    qRegisterMetaType<cv::Mat>("cv::Mat");
    connect(m_worker, &VideoStreamWorker::frameReady, this, [this](cv::Mat frame) {
            m_latestFrame = frame;
            renderCurrentFrame();
        }, Qt::QueuedConnection);

    connect(m_worker, &VideoStreamWorker::streamError, this, [this](const QString &msg) {
        if (imageLabel) {
            imageLabel->setText(msg);
        }
        emit streamDisconnected();
    });

    // Start the background thread
    m_thread->start();

}
LiveViewDialog::~LiveViewDialog() {
    if (m_worker) {
            m_worker->stop();
        }
        if (m_thread) {
            m_thread->quit();
            m_thread->wait(); // Wait safely for the background loop to exit
        }
}

void LiveViewDialog::closeEvent(QCloseEvent *event) {
    cap.release();
    QDialog::closeEvent(event);
}

void LiveViewDialog::setupUI(const QString &defaultStreamUrl) {
    tabWidget = new QTabWidget(this);

    // ==========================================
    // TAB 1: Live Feed & Analysis Split View
    // ==========================================
    QWidget *feedTab = new QWidget(this);

    // --- Left Side: Live Image View (with scroll area) ---
    imageLabel = new LiveImageView(this);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setStyleSheet("background-color: black;");
    imageLabel->setAlignment(Qt::AlignCenter);

    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(false);
    scrollArea->setBackgroundRole(QPalette::Dark);

    // --- Right Side: Surface Result Display ---
    surfaceResultLabel = new QLabel(this);
    surfaceResultLabel->setStyleSheet("background-color: black; color: white;");
    surfaceResultLabel->setAlignment(Qt::AlignCenter);
    surfaceResultLabel->setMinimumSize(300, 300);
    surfaceResultLabel->setText("Surface Result\n(Inactive)");

    // Put them side by side in a splitter or horizontal layout
    QHBoxLayout *viewSplitLayout = new QHBoxLayout();
    viewSplitLayout->addWidget(scrollArea, 1);        // Live feed takes available stretch
    viewSplitLayout->addWidget(surfaceResultLabel, 1);  // Surface result shares the space

    // --- Controls Layout (unchanged) ---
    dftButton = new QPushButton("Switch to Live DFT Mode", this);
    dftButton->setCheckable(true);
    connect(dftButton, &QPushButton::clicked, this, &LiveViewDialog::toggleDftMode);

    resolutionCombo = new QComboBox(this);
    resolutionCombo->addItem("256 x 256 (Fast)", 256);
    resolutionCombo->addItem("512 x 512 (Balanced)", 512);
    resolutionCombo->addItem("1024 x 1024 (Detailed)", 1024);
    resolutionCombo->setCurrentIndex(1);
    resolutionCombo->setEnabled(false);

    zoomCombo = new QComboBox(this);
    zoomCombo->addItem("Fit to Window", -1.0);
    zoomCombo->addItem("50%", 0.5);
    zoomCombo->addItem("100%", 1.0);
    zoomCombo->addItem("150%", 1.5);
    zoomCombo->addItem("200%", 2.0);
    zoomCombo->addItem("300%", 3.0);
    zoomCombo->addItem("400%", 4.0);
    zoomCombo->setCurrentIndex(1);

    averageMode = new QComboBox(this);
    averageMode->addItem("Show current");
    averageMode->addItem("Compute Average show current");
    averageMode->addItem("Compute Average and show Average");

    connect(zoomCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LiveViewDialog::onZoomChanged);

    grabButton = new QPushButton("Grab Igram", this);
    grabButton->setStyleSheet("background-color: #2e7d32; color: white; font-weight: bold;");
    connect(grabButton, &QPushButton::clicked, this, &LiveViewDialog::onGrabClicked);

    startAnalysisBtn = new QPushButton("Start Loop", this);
    startAnalysisBtn->setStyleSheet("background-color: #1976d2; color: white; font-weight: bold;");
    pauseAnalyBtn = new QPushButton("Pause",this);
    pauseAnalyBtn->setStyleSheet("background-color: #f39c12; color: white; font-weight: bold;");
    stopAnalysisBtn = new QPushButton("Stop Loop", this);
    stopAnalysisBtn->setStyleSheet("background-color: #d32f2f; color: white; font-weight: bold;");

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(grabButton);
    controlLayout->addWidget(startAnalysisBtn);
    controlLayout->addWidget(pauseAnalyBtn);
    controlLayout->addWidget(stopAnalysisBtn);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(dftButton);
    controlLayout->addWidget(new QLabel("DFT Size:", this));
    controlLayout->addWidget(resolutionCombo);
    controlLayout->addSpacing(15);
    controlLayout->addWidget(new QLabel("Zoom:", this));
    controlLayout->addWidget(zoomCombo);

    // status area
    statusLeft = new QLabel(this);
    statusRight = new QLabel(this);
    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLayout->addWidget(statusLeft);
    statusLayout->addWidget(statusRight);

    QVBoxLayout *feedLayout = new QVBoxLayout(feedTab);
    feedLayout->addLayout(statusLayout);
    feedLayout->addLayout(viewSplitLayout, 1); // Add the side-by-side views
    feedLayout->addLayout(controlLayout);

    // ==========================================
    // TAB 2 & 3: Settings & Help
    // ==========================================
    QWidget *settingsTab = createSettingsTab(defaultStreamUrl);

    QWidget *helpTab = new QWidget(this);
    QVBoxLayout *helpLayout = new QVBoxLayout(helpTab);
    QTextEdit *helpText = new QTextEdit(this);
    helpText->setReadOnly(true);
    helpText->setHtml(
        "<h3>Automated Live Analysis Prerequisites</h3>"
        "<p>Before starting the automated analysis loop, ensure the following steps are completed:</p>"
        "<ol>"
        "  <li><b>Camera Alignment:</b> Position your interferogram so it is stable and clearly visible in the live feed window.</li>"
        "  <li><b>Initial Outline:</b> The application needs to know the boundary of the mirror. On your first frame, make sure the mirror edge/outline and central obstruction are properly defined.</li>"
        "  <li><b>Threshold & Settings:</b> Verify your analysis parameters in the main application preferences.</li>"
        "</ol>"
        "<p>Once configured, switch back to the <b>Live Feed</b> tab and click <b>Start Loop</b> to begin automated capture and processing.</p>"
    );
    helpLayout->addWidget(helpText);

    tabWidget->addTab(feedTab, "Live Feed");
    tabWidget->addTab(settingsTab, "Settings");
    tabWidget->addTab(helpTab, "Help & Instructions");
    // ==========================================
        // Top-Right Corner Controls (Live Feed Only)
        // ==========================================
        QWidget *cornerWidget = new QWidget(this);
        QHBoxLayout *cornerLayout = new QHBoxLayout(cornerWidget);
        cornerLayout->setContentsMargins(0, 0, 0, 0);

        QLabel *spinLabel = new QLabel("Delete if RMS >", this);
        maxRMS = new QDoubleSpinBox(this);
        maxRMS->setRange(0.0, 100.0);
        maxRMS->setValue(.4);
        maxRMS->setSingleStep(.05);

        cornerLayout->addWidget(averageMode);
        cornerLayout->addWidget(spinLabel);
        cornerLayout->addWidget(maxRMS);

        // Assign it to the top-right of the tab widget
        tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);



    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->addWidget(tabWidget);
    setLayout(mainLayout);
}
QWidget* LiveViewDialog::createSettingsTab(const QString &defaultStreamUrl) {
    QWidget *settingsTab = new QWidget(this);
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsTab);

    settingsLayout->addWidget(new QLabel("Select or Manage Recent Streams / Camera IDs:", this));

    urlListWidget = new QListWidget(this);
    urlListWidget->setMaximumHeight(120);

    QSettings settings;
    QStringList urlHistory = settings.value("LiveView/urlHistory").toStringList();
    if (urlHistory.isEmpty()) {
        urlHistory << "0" << "1" << "http://192.168.50.5:5000/video_feed";
    }
    urlListWidget->addItems(urlHistory);

    // Clicking a history list item instantly sets the line edit and triggers stream application
    connect(urlListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            urlLineEdit->setText(item->text().trimmed());
            onApplySettings(); // Instant apply
        }
    });

    QString currentUrl = settings.value("LiveView/streamUrl", urlHistory.first()).toString();
    QList<QListWidgetItem*> matches = urlListWidget->findItems(currentUrl, Qt::MatchExactly);
    if (!matches.isEmpty()) {
        urlListWidget->setCurrentItem(matches.first());
    } else if (urlListWidget->count() > 0) {
        urlListWidget->setCurrentRow(0);
    }

    urlLineEdit = new QLineEdit(this);
    urlLineEdit->setPlaceholderText("Or type a new URL / ID here...");
    urlLineEdit->setText(currentUrl);

    // Pressing Enter in the line edit triggers instant apply
    connect(urlLineEdit, &QLineEdit::editingFinished, this, &LiveViewDialog::onApplySettings);

    QPushButton *removeHistoryBtn = new QPushButton("Remove Selected from History", this);
    connect(removeHistoryBtn, &QPushButton::clicked, this, [this]() {
        QListWidgetItem *item = urlListWidget->currentItem();
        if (item && urlListWidget->count() > 1) {
            delete item;
        }
    });

    QHBoxLayout *listActionLayout = new QHBoxLayout();
    listActionLayout->addWidget(removeHistoryBtn);

    // --- Camera Resolution Selector ---
    QHBoxLayout *resLayout = new QHBoxLayout();
    resLayout->addWidget(new QLabel("Camera Resolution:", this));

    resolutionCombo = new QComboBox(this);
    resolutionCombo->addItem("Default (Auto)", QSize(0, 0));
    resolutionCombo->addItem("640 x 480 (VGA)", QSize(640, 480));
    resolutionCombo->addItem("1280 x 720 (HD)", QSize(1280, 720));
    resolutionCombo->addItem("1920 x 1080 (FHD)", QSize(1920, 1080));
    resolutionCombo->addItem("2560 x 1440 (QHD)", QSize(2560, 1440));
    resolutionCombo->addItem("3840 x 2160 (4K)", QSize(3840, 2160));

    QString savedResStr = settings.value("LiveView/resolution", "Default (Auto)").toString();
    int resIndex = resolutionCombo->findText(savedResStr);
    if (resIndex != -1) {
        resolutionCombo->setCurrentIndex(resIndex);
    }
    resLayout->addWidget(resolutionCombo);

    // Changing resolution combo instantly applies it
    connect(resolutionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int){
        onApplySettings();
    });
    // ------------------------------------


    deleteIgramAfter = new QCheckBox("Delete IGram after analysis");
    deleteIgramAfter->setChecked(settings.value("LiveView/deleteAfter", true).toBool());
    connect(deleteIgramAfter, &QCheckBox::toggled, this, [this](bool checked) {
        QSettings s;
        s.setValue("LiveView/deleteAfter", checked);
    });

    settingsLayout->addWidget(urlLineEdit);
    settingsLayout->addWidget(urlListWidget);
    settingsLayout->addLayout(listActionLayout);
    settingsLayout->addLayout(resLayout);
     settingsLayout->addWidget(deleteIgramAfter);
    settingsLayout->addStretch(); // No more apply button clutter!

    return settingsTab;
}

void LiveViewDialog::initStream(const QString &url) {

    if (cap.isOpened()) {
        cap.release();
    }

    bool isInt = false;
    int camIndex = url.toInt(&isInt);

    if (isInt) {
        #if defined(_WIN32) || defined(_WIN64)
          cap.open(camIndex, cv::CAP_DSHOW);
        #else
          cap.open(camIndex);
        #endif
    } else {
        cap.open(url.toStdString());
    }

    if (cap.isOpened()) {

        qDebug() << "frame size" <<   cap.get(cv::CAP_PROP_FRAME_WIDTH) << cap.get(cv::CAP_PROP_FRAME_HEIGHT);


    } else {
        imageLabel->setText("Failed to open stream: " + url);
        imageLabel->adjustSize();
    }
}

void LiveViewDialog::onApplySettings() {
    QString newUrl = urlLineEdit->text().trimmed();
    if (newUrl.isEmpty()) return;

    QSettings settings;
    settings.setValue("LiveView/streamUrl", newUrl);
    settings.setValue("LiveView/resolution", resolutionCombo->currentText());

    // Save/update history list if needed...


    if (cap.isOpened()) {
        cap.release();
    }

    // Reopen stream (handle integer IDs vs HTTP streams)
    bool isInt = false;
    int camId = newUrl.toInt(&isInt);

    if (isInt) {
#if defined(_WIN32) || defined(_WIN64)
        cap.open(camId, cv::CAP_DSHOW);
#else
        cap.open(camId);
#endif
    } else {
        cap.open(newUrl.toStdString());
    }

    // Apply selected resolution if valid
    QSize selectedRes = resolutionCombo->currentData().toSize();
    if (cap.isOpened() && selectedRes.width() > 0 && selectedRes.height() > 0) {
        cap.set(cv::CAP_PROP_FRAME_WIDTH, selectedRes.width());
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, selectedRes.height());
    }

    if (cap.isOpened()) {
        // Or your preferred interval
    } else {
        if (imageLabel) imageLabel->setText("Failed to open stream.");
    }
}
void LiveViewDialog::onGrabClicked() {
    if (!m_latestFrame.empty()) {
        emit igramCaptured(); // Emit a safe deep copy of the frame
    }
}
void LiveViewDialog::toggleDftMode() {
    m_dftModeEnabled = dftButton->isChecked();
    resolutionCombo->setEnabled(m_dftModeEnabled);
    imageLabel->setDftModeActive(m_dftModeEnabled);
    if (m_dftModeEnabled) {
        dftButton->setText("Switch to Spatial Live View");
    } else {
        dftButton->setText("Switch to Live DFT Mode");
    }
    renderCurrentFrame();
}

void LiveViewDialog::onZoomChanged(int index) {
    double data = zoomCombo->itemData(index).toDouble();
    if (data < 0) {
        setFitToWindowZoom();
    } else {
        m_zoomFactor = data;
        imageLabel->setZoomFactor(m_zoomFactor);
        renderCurrentFrame();
    }
}

void LiveViewDialog::setFitToWindowZoom() {
    if (m_latestFrame.empty()) return;

    QSize viewportSize = scrollArea->viewport()->size();
    int imgW = m_dftModeEnabled ? m_dftSize : m_latestFrame.cols;
    int imgH = m_dftModeEnabled ? m_dftSize : m_latestFrame.rows;

    if (imgW <= 0 || imgH <= 0) return;

    double scaleX = static_cast<double>(viewportSize.width()) / imgW;
    double scaleY = static_cast<double>(viewportSize.height()) / imgH;

    m_zoomFactor = std::min(scaleX, scaleY);
    // Clamp to a reasonable minimum
    m_zoomFactor = std::max(0.1, m_zoomFactor);

    imageLabel->setZoomFactor(m_zoomFactor);
    renderCurrentFrame();
}
void LiveViewDialog::onResolutionChanged(int index) {
    m_dftSize = resolutionCombo->itemData(index).toInt();
}



void LiveViewDialog::onMirrorDefined(const QRect &rect) {
    m_userMirrorRect = rect;
}

void LiveViewDialog::updateFrame() {
    cv::Mat frame;

    // Fast-grab and discard any accumulated backlog frames so we get the freshest one.
    // (Depending on your stream framerate, grabbing 2 to 4 times clears out the queue)
    for (int i = 0; i < 5; ++i) {
        QApplication::processEvents();

        // SAFETY CHECK: If the dialog is closing or cap was released during processEvents, bail out immediately!
        if (!cap.isOpened()) {
            return;
        }
        cap.grab();
    }
    qDebug() << "updateFrame1";
    // Now retrieve the final, most up-to-date frame
    if (cap.retrieve(frame) && !frame.empty()) {
        m_latestFrame = frame.clone();
        renderCurrentFrame();
    } else {
        // Stream failed or disconnected
        m_latestFrame.release();
        if (imageLabel) {
            imageLabel->setText("Connection Lost / Stream Ended.");
            imageLabel->adjustSize();
        }
    }
}

void LiveViewDialog::renderCurrentFrame() {
    if (m_latestFrame.empty()) return;
    qDebug() << "render1";
    cv::Mat displayMat = m_latestFrame;

    if (m_dftModeEnabled) {
        displayMat = computeLiveDFT(m_latestFrame, m_dftSize, m_userMirrorRect);
    }
    // Display directly via OpenCV
    QImage img = matToQImage(displayMat);
    if (img.isNull()) return;

    int targetWidth = static_cast<int>(img.width() * m_zoomFactor);
    int targetHeight = static_cast<int>(img.height() * m_zoomFactor);
    QFont Font("Serif", 12);
    QPainter p2(&img);

    statusLeft->setText( QString("%1 x %2")
                .arg(img.size().width())
                .arg(img.size().height()));


    imageLabel->setPixmap(QPixmap::fromImage(img).scaled(targetWidth, targetHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->resize(targetWidth, targetHeight);
}

cv::Mat LiveViewDialog::computeLiveDFT(const cv::Mat &inputFrame, int targetSize, const QRect &roi) {
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

    cv::Mat resized;
    cv::resize(workingArea, resized, cv::Size(targetSize, targetSize), 0, 0, cv::INTER_AREA);

    cv::Mat floatImg;
    resized.convertTo(floatImg, CV_32F);

    cv::Mat complexImg;
    cv::dft(floatImg, complexImg, cv::DFT_COMPLEX_OUTPUT);

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

QImage LiveViewDialog::matToQImage(const cv::Mat &mat) {
    if (mat.type() == CV_8UC3) {
        cv::Mat rgb;
        cv::cvtColor(mat, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    } else if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8).copy();
    }
    return QImage();
}
