#ifndef DFTAREA_H
#define DFTAREA_H

#include <QWidget>
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "igramarea.h"
#include "dfttools.h"
#include <QImage>
#include "vortexdebug.h"
#include <string>
using namespace cv;
extern void showData(const string& txt, cv::Mat mat, bool useLog = false);
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
public slots:
    void doDFT();
    void resizeIgram();
    void setFilter(double);
    void dftSizeChanged(const QString&);
    void dftSizeVal(int);
    void setChannel(const QString&);
    void dftCenterFilter(double v);
    void makeSurface();
    void newIgram(QImage);
signals:
    void setDftSizeVal(int);
    void selectDFTTab();
    void updateFilterSize(int);
    void newWavefront(cv::Mat, CircleOutline, CircleOutline, QString);
    void dftReady(QImage);
private:
    Ui::DFTArea *ui;
    cv::Mat input;
    cv::Mat m_mask;
    bool capture;
    CircleOutline m_outside;
    CircleOutline m_center;

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
};


#endif // DFTAREA_H
