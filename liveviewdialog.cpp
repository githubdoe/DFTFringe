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
#include <QGroupBox>
// ==========================================
// LiveViewDialog Implementation
// ==========================================

LiveViewDialog::LiveViewDialog(QWidget *parent)
    : QDialog(parent), tabWidget(nullptr) {

    setWindowTitle("DFTFringe - Live View");
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint);
    QSettings settings;
    if (settings.contains("LiveViewDialog/geometry")) {
        restoreGeometry(settings.value("LiveViewDialog/geometry").toByteArray());
    } else {
        resize(800, 600);
    }

    QString savedUrl = settings.value("LiveView/streamUrl", 0).toString();

    setupUI(savedUrl);
    statusLeft->setText("<span style='color: black  ;background-color: yellow'>Connecting to camera/stream... Please wait.</span>");
    // Initialize video stream thread and worker
    m_thread = new QThread(this);
    m_worker = new VideoStreamWorker(savedUrl);
    m_worker->moveToThread(m_thread);

    connect(m_thread, &QThread::started, m_worker, &VideoStreamWorker::startStream);
    connect(m_thread, &QThread::finished, m_worker, &QObject::deleteLater);

    qRegisterMetaType<cv::Mat>("cv::Mat");

    // Single unified connection for frame handling and pulling the next frame
    connect(m_worker, &VideoStreamWorker::frameReady, this, [this](cv::Mat frame) {
        m_latestFrame = frame;
        renderCurrentFrame();
        emit requestFrame(); // Pull the next frame only after rendering finishes
    }, Qt::QueuedConnection);

    connect(m_worker, &VideoStreamWorker::streamError, this, [this](const QString &msg) {
        if (imageLabel) {
            imageLabel->setText(msg);
            statusLeft->setText(QString("<span style='color: white; background-color: red;'>%1</span>")
                                .arg(msg + " Go to settings to set the stream number. You may have to close and restart this dialog."));
            imageLabel->adjustSize();
        }
        emit streamDisconnected();
    });

    connect(this, &LiveViewDialog::requestChangeSource, m_worker, &VideoStreamWorker::changeSource, Qt::QueuedConnection);
    connect(this, &LiveViewDialog::requestSetResolution, m_worker, &VideoStreamWorker::setResolution, Qt::QueuedConnection);
    connect(this, &LiveViewDialog::requestFrame, m_worker, &VideoStreamWorker::fetchNextFrame, Qt::QueuedConnection);

    // Trigger the very first frame pull once the worker confirms the stream is open
    connect(m_worker, &VideoStreamWorker::streamStarted, this, [this]() {
        emit requestFrame();
    }, Qt::QueuedConnection);

    // Kick off the thread start safely after construction
    QTimer::singleShot(50, this, [this]() {
        if (m_thread && !m_thread->isRunning()) {
            m_thread->start();
        }
    });
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

            if (loopRunning) {
                // 1. Tell the main window/loop to stop
                m_stopRequested = true;

                // 2. Hide immediately so the UI feels responsive and closed to the user
                hide();

                // 3. Ignore the close event so the object isn't destroyed out from under the loop yet
                event->ignore();
            } else {
                // Safe to close normally
                loopRunning = false;
                QSettings set; // Or use existing app settings key
                set.setValue("LiveViewDialog/geometry", saveGeometry());
                event->accept();
            }



}

void LiveViewDialog::setupUI(const QString &defaultStreamUrl) {
    tabWidget = new QTabWidget(this);
    QSettings set;
    // ==========================================
    // TAB 1: Live Feed & Analysis Split View
    // ==========================================
    QWidget *feedTab = new QWidget(this);

    // --- Left Side: Live Image View (with scroll area) ---
    imageLabel = new LiveImageView(this);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setStyleSheet("background-color: black;");
    imageLabel->setAlignment(Qt::AlignCenter);


    connect(imageLabel, &LiveImageView::mirrorDefined, this,[this](const QRect r){
        this->m_userMirrorRect = r;

    });

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
    dftButton = new QPushButton("Show DFT", this);
    dftButton->setCheckable(true);
    connect(dftButton, &QPushButton::clicked, this, &LiveViewDialog::toggleDftMode);

    dftresolutionCombo = new QComboBox(this);
    dftresolutionCombo->addItem("256 x 256 (Fast)", 256);
    dftresolutionCombo->addItem("512 x 512 (Balanced)", 512);
    dftresolutionCombo->addItem("1024 x 1024 (Detailed)", 1024);

    dftresolutionCombo->setCurrentIndex(set.value("liveViewDftSize", 1).toInt());
    connect(dftresolutionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LiveViewDialog::onDFTSizeChanged);

    zoomCombo = new QComboBox(this);
    zoomCombo->addItem("Fit to Window", -1.0);
    zoomCombo->addItem("50%", 0.5);
    zoomCombo->addItem("100%", 1.0);
    zoomCombo->addItem("150%", 1.5);
    zoomCombo->addItem("200%", 2.0);
    zoomCombo->addItem("300%", 3.0);
    zoomCombo->addItem("400%", 4.0);
    zoomCombo->setCurrentIndex(1);

    vivid = new QDoubleSpinBox(this);
    vivid->setValue(2.1);
    vivid->setSingleStep(.05);

    DFTLowThreshold = new QSpinBox(this);
    DFTLowThreshold->setRange(-1, 255);

    DFTLowThreshold->setSpecialValueText("Auto");
    DFTLowThreshold->setValue(set.value("liveViewDFTLow", -1).toInt());
    connect(DFTLowThreshold, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val) {
        QSettings set;
        set.setValue("liveViewDFTLow", val);
    });
    averageMode = new QComboBox(this);
    averageMode->addItem("Show current");
    averageMode->addItem("Compute and show Average");
    averageMode->setStyleSheet("QComboBox { background-color: #FFBF00; color: black; }");
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

    saveAverageBtn = new QPushButton("Save average",this);
    saveAverageBtn->setStyleSheet("background-color: #f39c12; color: white; font-weight: bold;");
    saveAverageBtn->hide();
    connect(saveAverageBtn, &QPushButton::clicked, this, [this](){
        this->saveAverage = true;
    } );

    QHBoxLayout *controlLayout = new QHBoxLayout();
    controlLayout->addWidget(grabButton);
    controlLayout->addWidget(startAnalysisBtn);
    controlLayout->addWidget(pauseAnalyBtn);
    controlLayout->addWidget(stopAnalysisBtn);
    controlLayout->addWidget(saveAverageBtn);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(dftButton);
    controlLayout->addWidget(new QLabel("DFT Size:", this));
    controlLayout->addWidget(dftresolutionCombo);
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
        "<h3>Settings</h3>"
        "<p>The settings tab lets you select the source of the video.  Use 0,1,or 2 for USB attached cameras.</p>"
        "<p>A selection for a URL stream might look like this:  http://192.168.50.5:5000/video_feed</p>"
        "<p>You can also set the camera resolution if it can accept it.</p>"

        "<h3>Auto RMS setup</h3>"
        "<p>Enable the checkbox if you want the Max RMS value to be set to value of the first analyzed wave front time a percentage."
           " This will happen the firsts time you \"Start\" the analysis.  From then on the Max value will not be modified by the program. "
           " You can still modify it yourself however.</p>"

        "<h3>Automated Live Analysis Prerequisites</h3>"
        "<p>Before starting the automated analysis loop, ensure the following steps are completed:</p>"
        "<ol>"
        "  <li><b>Use the Grab button</b> To import the igram into DFTFringe and outtline it as usualal. Then Press Done.</li>"
        "  <li><b>Set the blue circle as usual</b> Then press the compute surface button as usual.</li>"
        "  <li><b>Max RMS value</b> You can set the Max RMS value where values higher than that will not be used in the analysis. </li>"
        "</ol>"

        "<p><b>Start</b>Once configured, switch back to the Live Feed< tab and click <b>Start Loop</b> to begin automated capture and processing.</p>"

        "<p>If a wave front's RMS is equal or below the max RMS value it will be saved in the wave front list."
                " If averaging is turned on it will be added to the average as well.</p> "
       "<p>Once the looping has started you might want to pause it to adjust some settings without reseting the averaging.</P>"
        "<P>The Start button always resets the averaging if it was selected to be done.</p>"
        "<p>The Stop button always stops the current looping and any averaging happening.</p>"
        "<p>The average is not saved until you pause and press the \"Save Average\" button.  You can select any of the saved wave fronts and average them youself as usual.</p>"

       "<h3>Max RMS</h3>"
                "<p>If an analyzed wave front's RMS value is larger than the Max RMS value it will"
                " not be added to the average and it will not be added to the list of wave fronts."
                "Also the surface display will switch to the live view instead of the average view"
                "until the RMS value is below the max</p>"

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
        controlLayout->addSpacing(100);
        cornerLayout->addWidget(new QLabel("DFT Floor:"));
        cornerLayout->addWidget(DFTLowThreshold);

        cornerLayout->addWidget(new QLabel("DFT contrast:"));
        cornerLayout->addWidget(vivid);
        controlLayout->addSpacing(10);

        cornerLayout->addWidget(spinLabel);
        cornerLayout->addWidget(maxRMS);

        // Assign it to the top-right of the tab widget
        tabWidget->setCornerWidget(cornerWidget, Qt::TopRightCorner);



    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->addWidget(tabWidget);
    stopAnalysisBtn->hide();
    pauseAnalyBtn->hide();
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
    connect(urlLineEdit, &QLineEdit::editingFinished, this, &LiveViewDialog::restartStream);
    connect(urlListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        if (item) {
            urlLineEdit->setText(item->text().trimmed());
            onApplySettings(); // Applies immediately upon list selection
        }
    });
    QGroupBox * rmsGroup = new QGroupBox("RMS settings");
    QVBoxLayout *rmsLayout = new QVBoxLayout(rmsGroup);

    QHBoxLayout *listActionLayout = new QHBoxLayout();
    autoRMSatStarup = new QCheckBox("Compute Max rms as a percentage of first analysis");
    connect(autoRMSatStarup, &QCheckBox::toggled, this, [](bool checked) {
        QSettings settings;
        settings.setValue("liveViewAutoRMSCheckBox", checked);
    });
    autoRMSatStarup->setChecked(settings.value("liveViewAutoRMSCheckBox", false).toBool());
    rmsLayout->addWidget(autoRMSatStarup);

    // make RMS margin settings;
    QHBoxLayout *percentLayout = new QHBoxLayout();
    percentLayout->addWidget(new QLabel("Percent above RMS of first analysis:"));

    RMSMargin = new QDoubleSpinBox();
    connect(RMSMargin, QOverload<double>::of (&QDoubleSpinBox::valueChanged), this, [](double val){
        QSettings settings;
        settings.setValue("liveViewAutoRMSValue", val);
    });
    RMSMargin->setSingleStep(.25);
    RMSMargin->setValue(settings.value("liveViewAutoRMSValue", 1.5).toDouble());
    percentLayout->addWidget(RMSMargin);
    percentLayout->addStretch((1));
    rmsLayout->addLayout(percentLayout);




    // --- Camera Resolution Selector ---
    QHBoxLayout *resLayout = new QHBoxLayout();

    resLayout->addWidget(new QLabel("Camera Resolution:"));

    resolutionCombo = new QComboBox();
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
    resLayout->addStretch(1);

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
    settingsLayout->addWidget(rmsGroup);
    settingsLayout->addLayout(resLayout);
     settingsLayout->addWidget(deleteIgramAfter);
    settingsLayout->addStretch(); // No more apply button clutter!

    return settingsTab;
}

void LiveViewDialog::restartStream(){
    onApplySettings();

}

void LiveViewDialog::onApplySettings() {
    QString newUrl = urlLineEdit->text().trimmed();
    if (newUrl.isEmpty()) return;

    QSettings settings;
    settings.setValue("LiveView/streamUrl", newUrl);
    settings.setValue("LiveView/resolution", resolutionCombo->currentText());

    // Tell the worker thread to switch sources safely

    if (m_worker) {

            qDebug() << "apply emitting requestChangeSource:" << newUrl;
                    emit requestChangeSource(newUrl);
    }

    // Apply selected resolution if valid
    QSize selectedRes = resolutionCombo->currentData().toSize();
    if (m_worker && selectedRes.width() > 0 && selectedRes.height() > 0) {
        QMetaObject::invokeMethod(m_worker, "setResolution", Qt::QueuedConnection,
                                  Q_ARG(int, selectedRes.width()),
                                  Q_ARG(int, selectedRes.height()));
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

void LiveViewDialog::onDFTSizeChanged(int index){
    int data = dftresolutionCombo->itemData(index).toDouble();
    QSettings set;
    set.setValue("liveViewDFTSize",index );
    m_dftSize = data;
}

void LiveViewDialog::onZoomChanged(int index) {
    double data = zoomCombo->itemData(index).toDouble();
    QSettings set;
    set.setValue("liveViewZoom", data);
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



cv::Mat computeFringeModulation(const cv::Mat& src, int kernelSize) {
    cv::Mat gray, floatImg;

    // Ensure single-channel grayscale input
    if (src.channels() == 3) {
        cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = src;
    }

    // Convert to 32-bit float for division and precision
    gray.convertTo(floatImg, CV_32F);

    // Define local neighborhood structuring element (must match fringe frequency scale)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(kernelSize, kernelSize));

    cv::Mat iMax, iMin;
    cv::dilate(floatImg, iMax, kernel); // Local maximum intensity
    cv::erode(floatImg, iMin, kernel);  // Local minimum intensity

    // Modulation formula: (I_max - I_min) / (I_max + I_min)
    cv::Mat numerator, denominator, modulation;
    cv::subtract(iMax, iMin, numerator);
    cv::add(iMax, iMin, denominator);

    // Prevent division by zero in dark background regions outside the aperture
    denominator += 1e-5f;

    cv::divide(numerator, denominator, modulation);

    return modulation;
}

void LiveViewDialog::renderCurrentFrame() {
    if (m_latestFrame.empty()) return;

    cv::Mat displayMat = m_latestFrame.clone();
    if (displayMat.channels() == 1) {
        cv::cvtColor(displayMat, displayMat, cv::COLOR_GRAY2BGR);
    }
    //cv::Mat modulatation = computeFringeModulation(displayMat, 5);
    //cv::imshow("mod", modulatation);
    //fcv::waitKey(100);

    if (m_dftModeEnabled) {
        // 1. Compute raw DFT
        cv::Mat dftRaw = computeLiveDFT(m_latestFrame, m_dftSize, m_userMirrorRect);
        if (dftRaw.empty()) return;

        // 2. Convert to float and apply logarithmic scaling on the raw DFT directly
        cv::Mat dftFloat;
        dftRaw.convertTo(dftFloat, CV_32F);

        cv::Mat dftLog = dftFloat;
        //cv::log(dftFloat + 1.0, dftLog);

        // 3. Compute statistics on the true DFT data (ignoring empty display padding)
        double minVal, maxVal;
        cv::minMaxLoc(dftLog, &minVal, &maxVal);

        cv::Scalar meanVal, stdDevVal;
        cv::meanStdDev(dftLog, meanVal, stdDevVal);

        // 4. Clip dynamic range based on statistics


        double floorVal = meanVal[0] + 2 * stdDevVal[0];
        int val = DFTLowThreshold->value();
        if (val != -1) {
            floorVal = val;
        }

        double ceilVal = meanVal[0] + (maxVal - meanVal[0])/vivid->value();

        cv::Mat dftClamped;
        cv::threshold(dftLog, dftClamped, floorVal, 0, cv::THRESH_TOZERO);

        cv::Mat dftShifted = dftClamped - floorVal;
        double range = ceilVal - floorVal;
        if (range < 1e-5) range = 1.0;

        cv::Mat dftNorm;
        dftShifted.convertTo(dftNorm, CV_8U, 255.0 / range);

        // 5. Apply Jet colormap to the square DFT
        cv::Mat dftColorSquare;
        cv::applyColorMap(dftNorm, dftColorSquare, cv::COLORMAP_JET);

        // Also create a normalized mask for alpha blending
        cv::Mat alphaMaskSquare;
        dftNorm.convertTo(alphaMaskSquare, CV_32F, 1.0 / 255.0);

        // 6. Now resize the color and alpha maps to match the full display frame size
                cv::Mat dftColor, alphaMask;
                cv::resize(dftColorSquare, dftColor, displayMat.size(), 0, 0, cv::INTER_LINEAR);
                cv::resize(alphaMaskSquare, alphaMask, displayMat.size(), 0, 0, cv::INTER_LINEAR);

                // 7. Safe and fast pixel loop using the fully-resized maps
                for (int y = 0; y < displayMat.rows; ++y) {
                    for (int x = 0; x < displayMat.cols; ++x) {
                        float a = alphaMask.at<float>(y, x);
                        if (a < 0.05) continue; // Skip background noise

                        double alpha = std::min(1.0, a * 1.4); // Punch up saturation on peaks

                        cv::Vec3b &bgPixel = displayMat.at<cv::Vec3b>(y, x);
                        cv::Vec3b fgPixel = dftColor.at<cv::Vec3b>(y, x);

                        bgPixel[0] = cv::saturate_cast<uchar>(bgPixel[0] * (1.0 - alpha) + fgPixel[0] * alpha);
                        bgPixel[1] = cv::saturate_cast<uchar>(bgPixel[1] * (1.0 - alpha) + fgPixel[1] * alpha);
                        bgPixel[2] = cv::saturate_cast<uchar>(bgPixel[2] * (1.0 - alpha) + fgPixel[2] * alpha);
                    }
                }
    }

    // Display via OpenCV conversion
    QImage img = matToQImage(displayMat);
    if (img.isNull()) return;

    int targetWidth = static_cast<int>(img.width() * m_zoomFactor);
    int targetHeight = static_cast<int>(img.height() * m_zoomFactor);

    statusLeft->setText(QString("%1 x %2")
                .arg(img.size().width())
                .arg(img.size().height()));

    imageLabel->setPixmap(QPixmap::fromImage(img).scaled(targetWidth, targetHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    imageLabel->resize(targetWidth, targetHeight);
    emit requestFrame();
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

    // Preserve aspect ratio and calculate scale
    double scale = static_cast<double>(targetSize) / std::max(workingArea.cols, workingArea.rows);
    int newW = std::round(workingArea.cols * scale);
    int newH = std::round(workingArea.rows * scale);

    cv::Mat resized;
    cv::resize(workingArea, resized, cv::Size(newW, newH), 0, 0, cv::INTER_AREA);

    // Create target canvas and center the resized image with padding (0 since mask already zeroes background)
    cv::Mat padded = cv::Mat::zeros(targetSize, targetSize, workingArea.type());
    int xOffset = (targetSize - newW) / 2;
    int yOffset = (targetSize - newH) / 2;
    resized.copyTo(padded(cv::Rect(xOffset, yOffset, newW, newH)));

    cv::Mat floatImg;
    padded.convertTo(floatImg, CV_32F);

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
