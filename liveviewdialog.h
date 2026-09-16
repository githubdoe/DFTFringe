
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
#include "liveviewhistory.h"
#include <QSplitter>

enum class CameraProperty {
    Brightness,
    Contrast,
    Exposure, // Shutter speed
    Gain
};
Q_DECLARE_METATYPE(CameraProperty)
class ResizableScrollArea : public QScrollArea {
    Q_OBJECT
public:
    explicit ResizableScrollArea(QWidget *parent = nullptr) : QScrollArea(parent) {}

    std::function<void()> onResized;

protected:
    void resizeEvent(QResizeEvent *event) override {
        QScrollArea::resizeEvent(event);
        if (onResized) {
            onResized(); // Triggers your fit-to-window or resize logic
        }
    }
};

class LiveViewDialog : public QDialog {
    Q_OBJECT
public:
    explicit LiveViewDialog( QWidget *parent = nullptr);
    ~LiveViewDialog();
    QImage getFrame() {return matToQImage(m_latestFrame.clone());};
protected:
    void closeEvent(QCloseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private slots:

    void onResolutionChanged(int index);
    void onZoomChanged(int index);
    void onMirrorDefined(QPointF center, double radius);
    void setFitToWindowZoom();
    void onYellowRadiusChanged(double radius);
    void onRequestZoomChange(double newZoom);
    void onApplySettings();
    void onDFTSizeChanged(int index);
    void restartStream();
public slots:
    void setOutsidecircle(QPointF center, double radius);
    void onGrabClicked();
    //void setCenterFilter(double radius, double scale, int DFTSize);
    void setCenterFilter(double freqBin) ;
signals:
    void igramCaptured();
    void streamDisconnected();
    void requestChangeSource(QString newSource);
    void requestSetResolution(int width, int height);
    void stopLiveLoopRequested();
    void requestFrame();
    void requestCameraSetting(CameraProperty prop , int val);
    void outline(QPointF center, double rad);
    void blueCircle(QPointF center, double rad);
public:
    QPushButton *startAnalysisBtn;
    QPushButton *stopAnalysisBtn;
    QPushButton *pauseAnalyBtn;


    QCheckBox *deleteIgramAfter;
    QDoubleSpinBox *maxRMS;
    QCheckBox *averageMode;
    QLabel *imageSize;
    QLabel *statusLeft;
    QLabel *statusRight;
    bool m_tmpShowLive = false;
    QCheckBox *autoRMSatStarup;
    QDoubleSpinBox *RMSMargin;
    QCheckBox *deleteIntermidiateWaveFront = nullptr;
    QCheckBox *showHistory;
    bool FirstWaveFrontSeen = false;
    bool saveAverage = false;
    QPushButton *saveAverageBtn;
    bool loopRunning = false;
    bool m_stopRequested = false;// used to signal the dialog is trying to close.
    bool FirsdtWaveFrontSeen = false;
    LiveImageView *imageLabel;
    liveViewHistory *history;
    bool m_showBestFit = false;
private:
    VideoStreamWorker *m_worker;
    QThread *m_thread;

    ResizableScrollArea *scrollArea;
    QCheckBox *dftCheckBox;
    QPushButton *grabButton;
    QListWidget *urlListWidget;
    QLineEdit *urlLineEdit;
    QTabWidget *tabWidget;
    QDoubleSpinBox *vivid;  // makes the DFT Vivid;
    QSplitter *leftSplitter;
    QSpinBox *DFTLowThreshold;

    int m_centerFilterRadius = 0;
    double m_RMSMargin = 1.;
    double m_filterPercent = 0;
    QTimer *m_rmsTimer;

    QComboBox *resolutionCombo;
    QComboBox *dftresolutionCombo;
    QComboBox *zoomCombo;
    cv::VideoCapture cap;

    int m_dftSize = 1024;
    double m_zoomFactor = 1.0;
    QImage m_latestImage;
    cv::Mat m_latestFrame;
    QRect m_userMirrorRect;

    QImage matToQImage(const cv::Mat &mat);
    cv::Mat computeLiveDFT(const cv::Mat &inputFrame, int targetSize, const QRect &roi);
    void renderCurrentFrame();
    QRect getMirrorRect();
    void setupUI(const QString &defaultStreamUrl);
    void initSettingsDialog(const QString &defaultStreamUrl);
    QDialog *m_settingsDlg;

    double m_DFTscale = 1.;
    bool m_hasActiveCircle = false;
    bool m_fitToWindow = true;
private:
    QPointF m_mirrorOutlineCenter;
    double m_mirrorOutlineRadius = 0.;
};
