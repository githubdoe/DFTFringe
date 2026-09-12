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
#include <QSplitter>
#include <QDialogButtonBox>
#include <QMessageBox>
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
        resize(600, 600);
    }
    QString savedUrl = settings.value("LiveView/streamUrl", 0).toString();
    initSettingsDialog(savedUrl);


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
    // Pull the next frame only after rendering finishes
    }, Qt::QueuedConnection);

    connect(m_worker, &VideoStreamWorker::streamError, this, [this](const QString &msg) {
        if (imageLabel) {
            statusLeft->setWordWrap(true);
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
        statusLeft->setText("Stream Connected");
        emit requestFrame();
        // Clear the status message after 5 seconds (5000 milliseconds)
        QTimer::singleShot(5000, this, [this]() {
            // Only clear if it still says "Stream Connected" so we don't overwrite a newer message
            if (statusLeft->text() == "Stream Connected") {
                statusLeft->clear();
            }
        });
    }, Qt::QueuedConnection);

    // Kick off the thread start safely after checking user preferences
        QTimer::singleShot(50, this, [this, savedUrl]() {
            QSettings settings;
            bool skipPrompt = settings.value("LiveView/skipUSBStartupPrompt", false).toBool();
            bool autoConnect = settings.value("LiveView/autoConnectUSB", true).toBool();

            // If it targets default USB device "0" and the user hasn't suppressed prompts
            if (!skipPrompt) {
                QMessageBox msgBox(this);
                msgBox.setWindowTitle("USB Camera Connection");
                msgBox.setText(QString("Attempt to connect to device %1 on startup?").arg(savedUrl));
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);

                QCheckBox dontAskBox("Never ask me again", &msgBox);
                msgBox.setCheckBox(&dontAskBox);

                int ret = msgBox.exec();
                if (dontAskBox.isChecked()) {
                    settings.setValue("LiveView/skipUSBStartupPrompt", true);
                }
                if (ret == QMessageBox::Yes) {
                    settings.setValue("LiveView/autoConnectUSB", true);
                    autoConnect = true;
                } else {
                    settings.setValue("LiveView/autoConnectUSB", false);
                    autoConnect = false;
                }
            }

            // Only start the thread if allowed
            if ((savedUrl != "0") || autoConnect) {
                if (m_thread && !m_thread->isRunning()) {
                    m_thread->start();
                }
            } else {
                statusLeft->setText("<span style='color: black; background-color: yellow'>USB connection skipped. Open settings to connect.</span>");
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
    setWindowTitle("DFTFringe - Live View");
    QSettings set;
    // Root layout for the dialog (Vertical)
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(8, 8, 8, 8);
    rootLayout->setSpacing(6);

    // ==========================================
    // TOP HEADER BAR (RMS & Average Mode)
    // ==========================================
    QWidget *headerWidget = new QWidget(this);
    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    averageMode = new QCheckBox("Compute and show average when loop is running", this);
    averageMode->setChecked(set.value("liveViewAverageMode", false).toBool());
    connect(averageMode, &QCheckBox::stateChanged, this,[](int val){
        QSettings set;
        set.setValue("liveViewAverageMode",val);
    });

    maxRMS = new QDoubleSpinBox(this);
    maxRMS->setRange(0.0, 100.0);
    maxRMS->setValue(.4);
    maxRMS->setSingleStep(.05);

    headerLayout->addWidget(averageMode);

    headerLayout->addWidget(new QLabel("Delete if RMS >", this));
    headerLayout->addWidget(maxRMS);
    headerLayout->addSpacing(100);

    rootLayout->addWidget(headerWidget);

    // ==========================================
    // CENTER SPLITTER (Video Feed vs Sidebar)
    // ==========================================
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);

    // --- Left Side: Live Image View ---
    QWidget *leftContainer = new QWidget(this);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    imageSize = new QLabel(this);
    statusRight = new QLabel(this);
    statusLeft = new QLabel(this);
    QHBoxLayout *statusLayout = new QHBoxLayout();
    statusLayout->addWidget(imageSize,0);
    statusLayout->addWidget(statusLeft,10);
    statusLeft->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    statusLayout->addWidget(statusRight,0);
    statusLayout->addStretch(1);
    leftLayout->addLayout(statusLayout);
    imageLabel = new LiveImageView(this);
    imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imageLabel->setStyleSheet("background-color: black;");
    imageLabel->setAlignment(Qt::AlignCenter);



    scrollArea = new QScrollArea(this);
    scrollArea->setWidget(imageLabel);
    scrollArea->setWidgetResizable(false);
    scrollArea->setBackgroundRole(QPalette::Dark);

    leftLayout->addWidget(scrollArea, 1);

    // --- Right Side: Scrollable Control Sidebar ---
    QScrollArea *sidebarScrollArea = new QScrollArea(this);
    sidebarScrollArea->setWidgetResizable(true);
    sidebarScrollArea->setMinimumWidth(260);
    sidebarScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget *sidebarContent = new QWidget(sidebarScrollArea);
    QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebarContent);
    sidebarLayout->setContentsMargins(6, 6, 6, 6);

    // Zoom Control
    sidebarLayout->addWidget(new QLabel("Zoom:", this));
    zoomCombo = new QComboBox(this);
    zoomCombo->addItem("Fit to Window", -1.0);
    zoomCombo->addItem("50%", 0.5);
    zoomCombo->addItem("100%", 1.0);
    zoomCombo->addItem("150%", 1.5);
    zoomCombo->addItem("200%", 2.0);
    zoomCombo->addItem("300%", 3.0);
    zoomCombo->addItem("400%", 4.0);
    zoomCombo->setCurrentIndex(set.value("liveViewZoom",1).toInt());
    onZoomChanged(zoomCombo->currentIndex());
    connect(zoomCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LiveViewDialog::onZoomChanged);
    sidebarLayout->addWidget(zoomCombo);

    // DFT Resolution Combo
    sidebarLayout->addWidget(new QLabel("DFT Size:", this));
    dftresolutionCombo = new QComboBox(this);
    dftresolutionCombo->addItem("256 x 256", 256);
    dftresolutionCombo->addItem("512 x 512", 512);
    dftresolutionCombo->addItem("1024 x 1024", 1024);

    dftresolutionCombo->setCurrentIndex(set.value("liveViewDftSize", 1).toInt());
    onDFTSizeChanged(dftresolutionCombo->currentIndex());
    connect(dftresolutionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &LiveViewDialog::onDFTSizeChanged);
    sidebarLayout->addWidget(dftresolutionCombo);
    QGroupBox *DFTGroup = new QGroupBox("DFTColor", this);
    QVBoxLayout *dftLayout = new QVBoxLayout(DFTGroup);
    // DFT Floor  values below this lever are transparent.
    dftLayout->addWidget(new QLabel("DFT Transparancy:", this));
    DFTLowThreshold = new QSpinBox(this);
    DFTLowThreshold->setRange(-1, 255);
    DFTLowThreshold->setSpecialValueText("Auto");

    DFTLowThreshold->setValue(set.value("liveViewDFTLow", -1).toInt());
    connect(DFTLowThreshold, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int val) {
        QSettings s;
        s.setValue("liveViewDFTLow", val);
    });
    dftLayout->addWidget(DFTLowThreshold);
    dftLayout->addLayout(dftLayout);
    // DFT Contrast
    dftLayout->addWidget(new QLabel("DFT Contrast:", this));
    vivid = new QDoubleSpinBox(this);
    vivid->setValue(2.1);
    vivid->setSingleStep(.05);
    dftLayout->addWidget(vivid);
    sidebarLayout->addWidget(DFTGroup);
    // Camera Hardware Sliders (Brightness / Exposure)
    sidebarLayout->addWidget(new QLabel("Brightness:", this));
    QSlider *brightnessSlider = new QSlider(Qt::Horizontal, this);
    brightnessSlider->setRange(-64, 64);
    brightnessSlider->setValue(0);
    connect(brightnessSlider, &QSlider::valueChanged, this, [this](int val) {
        emit requestCameraSetting(CameraProperty::Brightness, val);
    });
    sidebarLayout->addWidget(brightnessSlider);

    sidebarLayout->addWidget(new QLabel("Exposure / Shutter:", this));
    QSlider *exposureSlider = new QSlider(Qt::Horizontal, this);
    exposureSlider->setRange(-13, 0);
    exposureSlider->setValue(-6);
    connect(exposureSlider, &QSlider::valueChanged, this, [this](int val) {
        emit requestCameraSetting(CameraProperty::Exposure, val);
    });
    sidebarLayout->addWidget(exposureSlider);

    sidebarLayout->addStretch(1);

    // Popup Settings & Help Buttons
    QPushButton *settingsBtn = new QPushButton("Settings...", this);
    QPushButton *helpBtn = new QPushButton("Help & Instructions...", this);

    connect(settingsBtn, &QPushButton::clicked, this, [this, defaultStreamUrl]() {
        m_settingsDlg->exec(); // Just show the persistent dialog
    });

    connect(helpBtn, &QPushButton::clicked, this, [this]() {
        QDialog helpDlg(this);
        helpDlg.setWindowTitle("Live View Help");
        QVBoxLayout *dlgLayout = new QVBoxLayout(&helpDlg);
        QTextEdit *helpText = new QTextEdit(&helpDlg);
        helpText->setReadOnly(true);
        helpText->setHtml(
            "<h3>Settings</h3>"
            "<p>Settings tab lets you select the source of the video.  Use 0,1,or 2 for USB attached cameras.</p>"
            "<p>A selection for a URL stream might look like this:  http://192.168.50.5:5000/video_feed</p>"


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
            "<p>The average is not saved until you pause and press the \"Save Average\" button or the Stop button.  You can select any of the saved wave fronts and average them youself as usual.</p>"

           "<h3>Max RMS</h3>"
                    "<p>If an analyzed wave front's RMS value is larger than the Max RMS value it will"
                    " not be added to the average and it will not be added to the list of wave fronts."
                    "Also the surface display will switch to the live view instead of the average view"
                    "until the RMS value is below the max</p>"

                    );
        dlgLayout->addWidget(helpText);
        helpDlg.resize(800, 600);
        helpDlg.exec();
    });

    sidebarLayout->addWidget(settingsBtn);
    sidebarLayout->addWidget(helpBtn);

    sidebarContent->setLayout(sidebarLayout);
    sidebarScrollArea->setWidget(sidebarContent);

    mainSplitter->addWidget(leftContainer);
    mainSplitter->addWidget(sidebarScrollArea);
    mainSplitter->setStretchFactor(0, 4);
    mainSplitter->setStretchFactor(1, 1);

    rootLayout->addWidget(mainSplitter, 1);

    // ==========================================
    // BOTTOM ACTION BAR
    // ==========================================
    QHBoxLayout *bottomControlLayout = new QHBoxLayout();

    grabButton = new QPushButton("Grab Igram", this);
    grabButton->setStyleSheet("background-color: #2e7d32; color: white; font-weight: bold;");
    connect(grabButton, &QPushButton::clicked, this, &LiveViewDialog::onGrabClicked);

    startAnalysisBtn = new QPushButton("Start Loop", this);
    startAnalysisBtn->setStyleSheet("background-color: #1976d2; color: white; font-weight: bold;");

    pauseAnalyBtn = new QPushButton("Pause", this);
    pauseAnalyBtn->setStyleSheet("background-color: #f39c12; color: white; font-weight: bold;");
    pauseAnalyBtn->hide();

    stopAnalysisBtn = new QPushButton("Stop Loop", this);
    stopAnalysisBtn->setStyleSheet("background-color: #d32f2f; color: white; font-weight: bold;");
    stopAnalysisBtn->hide();

    saveAverageBtn = new QPushButton("Save average", this);
    saveAverageBtn->setStyleSheet("background-color: #f39c12; color: white; font-weight: bold;");
    saveAverageBtn->hide();
    connect(saveAverageBtn, &QPushButton::clicked, this, [this](){
        this->saveAverage = true;
    });

    dftCheckBox = new QCheckBox("Show DFT", this);
    dftCheckBox->setChecked(false);
    connect(dftCheckBox, &QCheckBox::toggled, this, &LiveViewDialog::renderCurrentFrame);

    bottomControlLayout->addWidget(grabButton);
    bottomControlLayout->addWidget(startAnalysisBtn);
    bottomControlLayout->addWidget(pauseAnalyBtn);
    bottomControlLayout->addWidget(stopAnalysisBtn);
    bottomControlLayout->addWidget(saveAverageBtn);
    bottomControlLayout->addSpacing(20);
    bottomControlLayout->addWidget(dftCheckBox);
    bottomControlLayout->addStretch(1);

    rootLayout->addLayout(bottomControlLayout);
    setLayout(rootLayout);
}
void LiveViewDialog::initSettingsDialog(const QString &defaultStreamUrl) {
    m_settingsDlg = new QDialog(this);
    m_settingsDlg->setWindowTitle("Live View Settings");
    QVBoxLayout *settingsLayout = new QVBoxLayout(m_settingsDlg);

    settingsLayout->addWidget(new QLabel("Select or Manage Recent Streams / Camera IDs:", m_settingsDlg));

    urlListWidget = new QListWidget(m_settingsDlg);
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

    urlLineEdit = new QLineEdit(m_settingsDlg);
    urlLineEdit->setPlaceholderText("Or type a new URL / ID here...");
    urlLineEdit->setText(currentUrl);

    connect(urlLineEdit, &QLineEdit::editingFinished, this, &LiveViewDialog::restartStream);
    connect(urlListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem *item) {
        statusLeft->setText("<span style='color: black  ;background-color: yellow'>Connecting to camera/stream... Please wait.</span>");

        if (m_thread && !m_thread->isRunning()) {
            m_thread->start();
        }
        if (item) {
            urlLineEdit->setText(item->text().trimmed());
            onApplySettings();
        }
    });

    QGroupBox *rmsGroup = new QGroupBox("RMS settings", m_settingsDlg);
    QVBoxLayout *rmsLayout = new QVBoxLayout(rmsGroup);


    autoRMSatStarup = new QCheckBox("Compute Max rms as a percentage of first analysis", rmsGroup);
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

    QHBoxLayout *resLayout = new QHBoxLayout();
    resLayout->addWidget(new QLabel("Camera Resolution:", m_settingsDlg));

    resolutionCombo = new QComboBox(m_settingsDlg);
    resolutionCombo->addItem("Default (Auto)", QSize(0, 0));
    resolutionCombo->addItem("640 x 480 (VGA)", QSize(640, 480));
    resolutionCombo->addItem("1280 x 720 (HD)", QSize(1280, 720));
    resolutionCombo->addItem("1280 x 960 (4:3)", QSize( 1280, 960));
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

    connect(resolutionCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int){
        onApplySettings();
    });

    deleteIgramAfter = new QCheckBox("Delete IGram after analysis", m_settingsDlg);
    deleteIgramAfter->setChecked(settings.value("LiveView/deleteAfter", true).toBool());
    connect(deleteIgramAfter, &QCheckBox::toggled, this, [this](bool checked) {
        QSettings s;
        s.setValue("LiveView/deleteAfter", checked);
    });


        deleteIntermidiateWaveFront = new QCheckBox("Do not add Wave fronts to list except for averages", m_settingsDlg);
        deleteIntermidiateWaveFront->setChecked(settings.value("LiveView/deleteIntermittent", false).toBool());
        connect(deleteIntermidiateWaveFront, &QCheckBox::toggled, this, [this](bool checked) {
            QSettings s;
            s.setValue("LiveView/deleteIntermittent", checked);
        });


    settingsLayout->addWidget(urlLineEdit);
    settingsLayout->addWidget(urlListWidget);
    settingsLayout->addWidget(rmsGroup);
    settingsLayout->addLayout(resLayout);
    settingsLayout->addWidget(deleteIgramAfter);
    settingsLayout->addWidget(deleteIntermidiateWaveFront);
    settingsLayout->addStretch();

    QDialogButtonBox *btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, m_settingsDlg);
    connect(btnBox, &QDialogButtonBox::accepted, m_settingsDlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, m_settingsDlg, &QDialog::reject);
    settingsLayout->addWidget(btnBox);

    m_settingsDlg->resize(550, 450);
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


void LiveViewDialog::onDFTSizeChanged(int index){
    int data = dftresolutionCombo->itemData(index).toDouble();
    QSettings set;
    set.setValue("liveViewDFTSize",index );
    m_dftSize = data;
}

void LiveViewDialog::onZoomChanged(int index) {
    double data = zoomCombo->itemData(index).toDouble();
    QSettings set;
    set.setValue("liveViewZoom", index);
    if (data < 0) {
        setFitToWindowZoom();
    } else {
        m_zoomFactor = data;
        imageLabel->setZoomFactor(m_zoomFactor);

        // Re-apply scaled circle if we have one active
        if (m_hasActiveCircle) {

            imageLabel->setOutsideCircle(m_rawCircleCenter , m_rawCircleRadius);
        }

        renderCurrentFrame();
    }
}

void LiveViewDialog::setFitToWindowZoom() {
    if (m_latestFrame.empty()) return;

    QSize viewportSize = scrollArea->viewport()->size();
    int imgW =  m_latestFrame.cols;
    int imgH =  m_latestFrame.rows;

    if (imgW <= 0 || imgH <= 0) return;

    double scaleX = static_cast<double>(viewportSize.width()) / imgW;
    double scaleY = static_cast<double>(viewportSize.height()) / imgH;

    m_zoomFactor = std::min(scaleX, scaleY);
    // Clamp to a reasonable minimum
    m_zoomFactor = std::max(0.1, m_zoomFactor);
    // Re-apply scaled circle if we have one active
    if (m_hasActiveCircle) {

        imageLabel->setOutsideCircle(m_rawCircleCenter , m_rawCircleRadius );
    }
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

    if (dftCheckBox->isChecked()) {
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
        double floorVal = meanVal[0] + 2 * stdDevVal[0];// use this value if Auto is selected.

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

        // 6. Resize the color and alpha maps to match the full display frame size
        cv::Mat dftColor, alphaMask;
        cv::resize(dftColorSquare, dftColor, displayMat.size(), 0, 0, cv::INTER_LINEAR);
        cv::resize(alphaMaskSquare, alphaMask, displayMat.size(), 0, 0, cv::INTER_LINEAR);

        // 7. Vectorized blending (replaces the nested loops)
        // Scale alpha to punch up peaks, capped at 1.0
        cv::Mat scaledAlpha;
        cv::multiply(alphaMask, 1.4, scaledAlpha);
        cv::threshold(scaledAlpha, scaledAlpha, 1.0, 1.0, cv::THRESH_TRUNC); // Equivalent to std::min(1.0, a * 1.4)

        // Convert displayMat and dftColor to float for precise blending
        cv::Mat bgFloat, fgFloat;
        displayMat.convertTo(bgFloat, CV_32FC3);
        dftColor.convertTo(fgFloat, CV_32FC3);

        // Split alpha into 3 channels so it matches the 3-channel BGR matrices
        std::vector<cv::Mat> alphaChannels(3, scaledAlpha);
        cv::Mat alpha3C;
        cv::merge(alphaChannels, alpha3C);

        // Blending formula: bg * (1 - alpha) + fg * alpha -> bg + alpha * (fg - bg)
        cv::Mat blended;
        cv::multiply(fgFloat, alpha3C, fgFloat);
        cv::multiply(bgFloat, cv::Scalar::all(1.0) - alpha3C, bgFloat);
        cv::add(bgFloat, fgFloat, blended);

        // Convert back to 8-bit BGR and write directly back to displayMat
        blended.convertTo(displayMat, CV_8UC3);
    }

    // Display via OpenCV conversion
    QImage img = matToQImage(displayMat);
    if (dftCheckBox->isChecked()) {
        // draw center filter circle
        QPainter dftpainter(&img);

        dftpainter.setBrush(QColor(0,0,100,70));

        dftpainter.setPen(QPen(Qt::yellow, 2));

        int centerx = img.width()/2;
        int centery = img.height()/2;
        int rad = centerx * m_centerPercent;

        dftpainter.drawEllipse(QPointF(centerx, centery), rad,rad);
    }


    int targetWidth = static_cast<int>(img.width() * m_zoomFactor);
    int targetHeight = static_cast<int>(img.height() * m_zoomFactor);

    imageSize->setText(QString("%1 x %2")
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

void LiveViewDialog::setOutsidecircle(QPointF center, double radius) {
    m_rawCircleCenter = center;
    m_rawCircleRadius = radius;
    m_hasActiveCircle = true;

    imageLabel->setOutsideCircle(center, radius);
}

