#ifndef FOUCAULTVIEW_H
#define FOUCAULTVIEW_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QMessageBox>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "surfacemanager.h"

// Enum to specify which optical test to simulate
enum class OpticalTestType {
    Foucault,
    Ronchi
};

// Parameter container so this can be called without needing the UI pointers
struct OpticalTestSettings {
    double rocOffset = 0.0;
    double ronchiX = 1.0;
    double lpi = 100.0;
    double gamma = 1.0;
    double slitWidth = 0.001;
    bool useMM = true;
    bool movingSource = true;
    bool knifeOnLeft = false;
    bool clearCenter = false;

    // Internal state variables used by the original logic
    int heightMultiply = 1;
    int lateralOffset = 0;
    double outputLambda = 550.0;
};

namespace Ui {
class foucaultView;
}

class foucaultView : public QWidget
{
    Q_OBJECT

public:
    static foucaultView *get_Instance(SurfaceManager *sm = 0);
    foucaultView(const foucaultView&) = delete;
    foucaultView& operator=(const foucaultView&) = delete;
    ~foucaultView();

    void setSurface(wavefront * wf);
    bool saveOnlyFoucault();
    bool needsDrawing;

    // The Refactored Static Engine - Callable from other classes
    static QImage generateOpticalTestImage(OpticalTestType type, wavefront* wf, const OpticalTestSettings& settings, bool bAutoCollimate);

public slots:
    void on_makePb_clicked();
    QImage *render();

private slots:
    void showContextMenu(QPoint pos);
    void on_gammaSb_valueChanged(double arg1);
    void on_lpiSb_valueChanged(double arg1);
    void on_movingSourceRb_clicked(bool /*unused*/);
    void on_radioButton_2_clicked();
    void on_knifeOnLeftCb_clicked();
    void on_lpiSb_editingFinished();
    void on_rocOffsetSb_editingFinished();
    void on_slitWidthSb_editingFinished();
    void on_useMM_clicked(bool checked);
    void on_scanPb_clicked();
    void on_h1x_clicked();
    void on_h2x_clicked();
    void on_h4x_clicked();
    void on_rocOffsetSlider_valueChanged(int value);
    void on_clearCenterCb_clicked();
    void on_autoStepSize_clicked(bool checked);
    void on_rocStepSize_editingFinished();
    void on_lateralOffset_valueChanged(int arg1);
    void on_SaveImageCB_clicked(bool checked);
    void saveRonchiImage();
    void saveFoucaultImage();
    void on_overlayProfile_stateChanged(int arg1);
    void on_RonchiX_valueChanged(double arg1);
    void on_pushButton_clicked();
    void on_autocollimation_clicked(bool checked);
    void showSelectedRonchiImages();
    void generateBatchRonchiImage(const QList<wavefront*>& wavefrontList);

private:
    Ui::foucaultView *ui;
    SurfaceManager *m_sm;
    QString imageDir;
    QImage m_foucaultQimage;
    QTimer m_guiTimer;
    int heightMultiply;
    double m_sag;
    double m_temp_sag;
    wavefront *m_wf;
    int lateralOffset;

    explicit foucaultView(QWidget *parent = 0, SurfaceManager *sm = 0);
    cv::Mat compute_star_test(int pupil_size, double defocus, double pad, bool use_OPD);
    double getStep();
    void draw_ROC_Scale();
};

#endif // FOUCAULTVIEW_H
