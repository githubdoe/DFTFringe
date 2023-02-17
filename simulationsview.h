/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#ifndef SIMULATIONSVIEW_H
#define SIMULATIONSVIEW_H

#include <QWidget>
#include "wavefront.h"
#include <opencv2/opencv.hpp>
#include <QTimer>
#include <QtDataVisualization/Q3DSurface>
class arcSecScaleDraw;
using namespace QtDataVisualization;
namespace Ui {
class SimulationsView;
}

class SimulationsView : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationsView(QWidget *parent = 0);
    ~SimulationsView();
    static SimulationsView *getInstance(QWidget *parent);
    void setSurface(wavefront *wf);
    cv::Mat  computeStarTest(cv::Mat surface, int pupil_size, double pad, bool returnComplex = false);
    void computeMPF();
    void compute();
    bool needs_drawing;
    bool needs_drawing_3D;
    cv::Mat nulledSurface(double defocus);
    cv::Mat m_PSF;
private:
    bool alias;
    QTimer m_guiTimer;
    cv::Mat m_psf;
    arcSecScaleDraw *m_arcSecScaleDraw;
    Q3DSurface *m_PSF_3Dgraph;
    double dX;  // pupil sample size based on mirror size in wave front.
    double dF;  // psf sample size;

    void mtf(cv::Mat magPsf, QString txt, QColor color);
    void initMTFPlot();

    void make3DPsf(cv::Mat surface);
public slots:
        void on_MakePB_clicked();
private slots:

    void on_defocusSB_valueChanged(double);

    void on_gammaSB_valueChanged(double value);

    void on_centerMagnifySB_valueChanged(double value);

    void on_FFTSizeSB_valueChanged(int val);

    void showContextMenu(const QPoint &pos);

    void saveImage(QString fn = "");

    void on_film_clicked();

    void on_show3D_clicked(bool checked);

    void on_doLog_clicked(bool checked);

private:
    Ui::SimulationsView *ui;
    static SimulationsView* m_Instance;
    bool m_Computed;
    bool m_psf_doLog;
    wavefront *m_wf;
};
// class to save value on construction and then restore old value on destruction
template<class T> class save_restore
{
    T* m_pVal;
    T m_Val;
public:
    save_restore(T* pval, T new_val) : m_pVal(pval), m_Val(*pval)
    {
        *m_pVal = new_val;
    };
    save_restore(T* pval) :m_pVal(pval),m_Val(*pval){};
    void setVal(T val) {m_Val = val;};

    ~save_restore() { *m_pVal = m_Val;};

};
#endif // SIMULATIONSVIEW_H
