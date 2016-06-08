#ifndef FOUCAULTVIEW_H
#define FOUCAULTVIEW_H

#include <QWidget>
#include "surfacemanager.h"
#include <QTimer>
namespace Ui {
class foucaultView;
}

class foucaultView : public QWidget
{
    Q_OBJECT

public:
    explicit foucaultView(QWidget *parent, SurfaceManager *sm);
    ~foucaultView();

private slots:
    void on_makePb_clicked();

    void on_gammaSb_valueChanged(double arg1);

    void on_lpiSb_valueChanged(double arg1);

    void on_movingSourceRb_clicked(bool checked);

    void on_radioButton_2_clicked();

    void on_knifeOnLeftCb_clicked();

    void on_lpiSb_editingFinished();

    void on_rocOffsetSb_editingFinished();

    void on_slitWidthSb_editingFinished();

    void on_lateralKnifeSb_editingFinished();

    void on_useMM_clicked(bool checked);

    void on_scanPb_clicked();

    void on_h1x_clicked();

    void on_h2x_clicked();

    void on_h4x_clicked();

private:
    Ui::foucaultView *ui;
    SurfaceManager *m_sm;
    QTimer m_guiTimer;
    cv::Mat compute_star_test(int pupil_size, double defocus, double pad, bool use_OPD);
    int heightMultiply;
};

#endif // FOUCAULTVIEW_H
