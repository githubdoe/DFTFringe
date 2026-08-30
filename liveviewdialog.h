#pragma once

#include <QDialog>
#include <QScrollArea>
#include <QTimer>
#include <QPushButton>
#include <QComboBox>
#include <opencv2/opencv.hpp>
#include "liveimageview.h"
#include <QLineEdit>
#include <QListWidget>
#include <QCheckBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include "videostreamworker.h"
#include <QThread>
#include <QCheckBox>

class LiveViewDialog : public QDialog {
    Q_OBJECT
public:
    explicit LiveViewDialog( QWidget *parent = nullptr);
    ~LiveViewDialog();
    QImage getFrame() {return matToQImage(m_latestFrame.clone());};
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void toggleDftMode();
    void onResolutionChanged(int index);
    void onZoomChanged(int index);
    void onMirrorDefined(const QRect &rect);
    void setFitToWindowZoom();
    void onGrabClicked();
    void onApplySettings();
    void onDFTSizeChanged(int index);

signals:
    void igramCaptured();
    void streamDisconnected();
    void requestChangeSource(QString newSource);
    void requestSetResolution(int width, int height);
    void stopLiveLoopRequested();
    void requestFrame();
public:
    QPushButton *startAnalysisBtn;
    QPushButton *stopAnalysisBtn;
    QPushButton *pauseAnalyBtn;
    QLabel *surfaceResultLabel;

    QCheckBox *deleteIgramAfter;
    void setCenterFilterRadius(double percent) {imageLabel->m_centerPercent = percent;};
    QDoubleSpinBox *maxRMS;
    QComboBox *averageMode;
    QLabel *statusLeft;
    QLabel *statusRight;
    bool m_tmpShowLive = false;
    QCheckBox *autoRMSatStarup;
    QDoubleSpinBox *RMSMargin;
    bool FirstWaveFrontSeen = false;
    bool saveAverage = false;
    QPushButton *saveAverageBtn;
    bool loopRunning = false;
    bool m_stopRequested = false;// used to signal the dialog is trying to close.
private:
    VideoStreamWorker *m_worker;
    QThread *m_thread;
    LiveImageView *imageLabel;
    QScrollArea *scrollArea;
    QPushButton *dftButton;
    QPushButton *grabButton;
    QListWidget *urlListWidget;
    QLineEdit *urlLineEdit;
    QTabWidget *tabWidget;
    QDoubleSpinBox *vivid;  // makes the DFT Vivid;
    QSpinBox *DFTLowThreshold;

    double m_centerPercent;
    double m_RMSMargin = 1.;

    QTimer *m_rmsTimer;

    QComboBox *resolutionCombo;
    QComboBox *dftresolutionCombo;
    QComboBox *zoomCombo;
    cv::VideoCapture cap;

    bool m_dftModeEnabled = false;
    int m_dftSize = 1024;
    double m_zoomFactor = 1.0;
    QImage m_latestImage;
    cv::Mat m_latestFrame;
    QRect m_userMirrorRect;

    QImage matToQImage(const cv::Mat &mat);
    cv::Mat computeLiveDFT(const cv::Mat &inputFrame, int targetSize, const QRect &roi);
    void renderCurrentFrame();
    void setupUI(const QString &defaultStreamUrl);
    QWidget* createSettingsTab(const QString &defaultStreamUrl);
};
