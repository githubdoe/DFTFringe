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
class LiveViewDialog : public QDialog {
    Q_OBJECT
public:
    explicit LiveViewDialog(const QString &streamUrl, QWidget *parent = nullptr);
    ~LiveViewDialog();
    QImage getFrame() {return matToQImage(m_latestFrame.clone());};
protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void updateFrame();
    void toggleDftMode();
    void onResolutionChanged(int index);
    void onZoomChanged(int index);
    void onMirrorDefined(const QRect &rect);
    void setFitToWindowZoom();
    void onGrabClicked();
    void onApplySettings();

signals:
    void igramCaptured();
    void streamDisconnected();
public:
    QPushButton *startAnalysisBtn;
    QPushButton *stopAnalysisBtn;
    QLabel *surfaceResultLabel;
    QCheckBox *showAverage;
    QCheckBox *deleteIgramAfter;
    void setCenterFilterRadius(double percent) {imageLabel->m_centerPercent = percent;};

private:

    LiveImageView *imageLabel;
    QScrollArea *scrollArea;
    QPushButton *dftButton;
    QPushButton *grabButton;
    QListWidget *urlListWidget;
    QLineEdit *urlLineEdit;
    QTabWidget *tabWidget;
    double m_centerPercent;


    QComboBox *resolutionCombo;
    QComboBox *zoomCombo;
    QTimer *timer;
    cv::VideoCapture cap;

    bool m_dftModeEnabled = false;
    int m_dftSize = 512;
    double m_zoomFactor = 1.0;

    cv::Mat m_latestFrame;
    QRect m_userMirrorRect;

    QImage matToQImage(const cv::Mat &mat);
    cv::Mat computeLiveDFT(const cv::Mat &inputFrame, int targetSize, const QRect &roi);
    void renderCurrentFrame();
    void initStream(const QString &url);
    void setupUI(const QString &defaultStreamUrl);
    QWidget* createSettingsTab(const QString &defaultStreamUrl);
};
