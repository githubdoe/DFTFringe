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
#ifndef DFTAREA_H
#define DFTAREA_H

#include <QWidget>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "IgramArea.h"
#include "dfttools.h"
#include <QImage>
#include "vortexdebug.h"
#include <string>
using namespace cv;
extern void showData(const std::string& txt, cv::Mat mat, bool useLog = false);
extern QImage showMag(cv::Mat complexI, bool show = false, const char *title = "FFT", bool doLog = true, double gamma = 0);
extern void shiftDFT(cv::Mat &magI);


namespace Ui {
class DFTArea;
}

class DFTArea : public QWidget
{
    Q_OBJECT

public:
    explicit DFTArea(QWidget *parent = 0, IgramArea* ip = 0, DFTTools *tools = 0,
                     vortexDebug *vdbug = 0);
    ~DFTArea();
    static DFTArea *get_Instance(QWidget *parent = 0,IgramArea* ip = 0, DFTTools *tools = 0,
                                 vortexDebug *vdbug = 0);
    int m_size;
    cv::Mat fftmagI;
    cv::Mat magI;
    cv::Mat m_dft;
    DFTTools *tools;
    QString channel;
    QString dftSizeStr;
    QImage magIImage;
    double m_center_filter;
    cv::Mat vortex(QImage &img,
               double low);


    bool success;
public slots:
    void doDFT();
    void dftSizeChanged(const QString&);
    void dftSizeVal(int);
    void setChannel(const QString&);
    void dftCenterFilter(double v);
    void makeSurface();
    void newIgram(QImage);
    void gamma(int);
    void showResizedDlg();
signals:
    void setDftSizeVal(int);
    void selectDFTTab();
    void updateFilterSize(int);
    void newWavefront(cv::Mat, CircleOutline, CircleOutline, QString,
                      QVector<std::vector<cv::Point> >);
    void dftReady(QImage);
    void statusBarUpdate(QString);
private:
    static DFTArea *m_Instance;
    Ui::DFTArea *ui;
    cv::Mat input;
    cv::Mat m_mask;
    bool capture;
    CircleOutline m_outside;
    CircleOutline m_center;
    QVector<std::vector<cv::Point> > m_poly;
    double m_gamma;

    IgramArea *igramArea;
    void paintEvent(QPaintEvent *);
    cv::Mat grayComplexMatfromImage(QImage &img);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    double m_smooth;
    vortexDebug    *m_vortexDebugTool;
    int m;  // x border added to dft
    int n;  //y border added to dft
    double scale;
};


#endif // DFTAREA_H
