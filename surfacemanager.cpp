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
#include "surfacemanager.h"
#include <limits>
#include <cmath>
#include <QWidget>
#include <opencv/cv.h>
#include <QtWidgets/QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "zernikeprocess.h"
#include "zernikedlg.h"
#include <QMessageBox>
#include <QTextStream>
#include <QHash>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_renderer.h>
#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_scale_engine.h>
#include <algorithm>
#include <vector>
#include "zernikeprocess.h"
#include <QTimer>
#include <qprinter.h>
#include "rotationdlg.h"
#include <qwt_scale_draw.h>
#include "zernikes.h"
#include <qwt_abstract_scale.h>
#include <qwt_plot_histogram.h>
#include "savewavedlg.h"
#include "simulationsview.h"
#include "standastigwizard.h"
#include "subtractwavefronatsdlg.h"
#include <QTextDocument>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <QTabWidget>
#include <QPrintDialog>
#include <QSplitter>
#include "settingsgeneral2.h"
#include "foucaultview.h"
#include "myutils.h"
#include "contourview.h"
#include "circleutils.h"
#include "circle.h"
#include "utils.h"
#include "wavefrontfilterdlg.h"
#include "reportdlg.h"
#include "Circleoutline.h"
#include <math.h>
#include "transformwavefrontdlg.h"
#include "psi_dlg.h"
#include "opencv2/opencv.hpp"
#include "reportpage2.h"
#include "oglrendered.h"
cv::Mat theMask;
cv::Mat deb;
double outputLambda;
double bilinear(cv::Mat mat, cv::Mat mask, double x, double y)
{

    int w = mat.rows;
    int h = mat.cols;

    // bilinear interpolation to compute  pixel from its
    // four neighbors
    if (x < 0)
        return 0;
    if (y < 0)
        return 0;
    if (x >= w)
        return 0.;
    if (y >= h)
        return 0.;

    int fx = floor(x);
    int fy = floor(y);

    double a = x - fx;
    double b = y - fy;
    try {
        double f00 = mat.at<double>(fy,fx);
        // if not interpolation needed then return the anchor point
        if (a == 0.0 && b == 0.0)
            return f00;

        if (fy >= h-1)
        {
                return 0.;
        }

        if (fx >= w-1)
        {
            return 0.;
        }
        bool f00inside = mask.at<uchar>(fy,fx);
        double f01 = mat.at<double>(fy+1, fx);//[fx + (fy + 1) * w];
        bool f01inside = mask.at<uchar>(fy+1,fx);
        double f11 = mat.at<double>(fy+1,fx+1);//[fx + 1 + (fy+1) * w];
        bool f11inside = mask.at<uchar>(fy+1,fx+1);
        double 	f10 = mat.at<double>(fy,fx+1);//[ndx_src + 1];
        bool f10inside = mask.at<uchar>(fy,fx+1);

        // full bilinear
        if (f00inside && f01inside && f10inside && f11inside)
            return f00 + a *( f10 - f00) + b * (f01 - f00) +
                    a * b * (f00 + f11 - f10 - f01);
        {
            // not all four corners are inside boundary  find which two are and to linear.
            if (f00inside && f10inside){
                return f00 +  a * (f10-f00);
            }
            if (f00inside && f01inside){
                return f00 + b * (f01 - f00);
            }
            if (f01inside && f11inside){
                return f01 + b * (f11 - f01);
            }
            if (f11inside && f01inside){
                return f11 + a * (f11 - f10);
            }

            // Only one inside
            if(f00inside) return f00;
            if (f01inside) return f01;
            if (f10inside) return f10;
            if (f11inside) return f11;

            // none were inside boundary so go look in the other direction.
            double sum = 0.;
            int cnt = 0;
            int u = 1;
            for (int del = 1; del <= u; ++del){
                if (fy+del > h) continue;
                if (fy -del < 0) continue;
                if (fx +del > w) continue;
                if (fx - del < 0) continue;

                if (mask.at<uchar>(fy+del,fx) != 0){
                    sum += mat.at<double>(fy+del, fx);
                    ++cnt;
                }
                if (mask.at<uchar>(fy-del,fx)!= 0){
                    sum += mat.at<double>(fy-del, fx);
                    ++cnt;
                }
                if (mask.at<uchar>(fy,fx+del)!= 0){
                    sum += mat.at<double>(fy,fx+del);
                    ++cnt;
                }
                if (mask.at<uchar>(fy,fx-del)!= 0){
                    sum += mat.at<double>(fy,fx-del);
                    ++cnt;
                }
                if (mask.at<uchar>(fy+del,fx+del)!= 0){
                    sum += mat.at<double>(fy+del, fx+del);
                    ++cnt;
                }
                if (mask.at<uchar>(fy-del,fx-del)!= 0){
                    sum += mat.at<double>(fy-del, fx-del);
                    ++cnt;
                }
            }
            if (cnt > 0){
                sum /= cnt;
                return sum;
            }
            return 0.;

        }

    }
    catch (...)  // would like to catch just access violations here but this was quick.
    {
        qDebug() << "except";
        return 0.0;
    }
}
void expandBorder(wavefront *wf){
    double cx, cy;
    cx = wf->m_outside.m_center.rx();
    cy = wf->m_outside.m_center.ry();
    double rad = wf->m_outside.m_radius-2;
    double r2 = rad * rad;
    double rc2 = wf->m_inside.m_radius * wf->m_inside.m_radius;
    cv::Mat centerMask;
    if (wf->m_inside.m_radius){
        centerMask = cv::Mat::ones(wf->data.size(), CV_8UC1);
        uchar zero = 0;
        fillCircle(centerMask, wf->m_inside.m_center.x(), wf->m_inside.m_center.y(),
                   wf->m_inside.m_radius, &zero);
    }
    for (int y = 0; y < wf->data.rows; ++y){
        double y1 = (double)(y - cy);
        double v = y1/rad;
        for (int x = 0; x < wf->data.cols; ++x){
            double x1 = (double)(x - cx);
            double u = x1/rad;
            double rho = sqrt(u * u + v * v);
            if (rho > 1.){

                double D = (x1 * x1 + y1 * y1);
                double alpha = r2/D;
                double xr = alpha * x1 + cx;
                double yr = alpha * y1 + cy;

                wf->nulledData.at<double>(y,x) = bilinear(wf->nulledData,wf->mask, xr,yr);
            }
            if (wf->m_inside.m_radius && centerMask.at<uchar>(y,x) == 0){
                double x1 = (double)(x - wf->m_inside.m_center.x());
                double y1 = (double)(y - wf->m_inside.m_center.y());
                double u = x1 /( wf->m_inside.m_radius+2);
                double v = y1 / (wf->m_inside.m_radius+2);
                double rho = sqrt(u * u + v * v);
                if (rho > .1 && rho <= 1.){
                    double D = (x1 * x1 + y1 * y1);
                    double alpha = rc2/D;
                    double xr = alpha * x1 + wf->m_inside.m_center.x();
                    double yr = alpha * y1 + wf->m_inside.m_center.y();
                    wf->nulledData.at<double>(y,x) = bilinear(wf->nulledData,centerMask, xr,yr);
                }
            }

        }
    }
}

class wftNameScaleDraw: public QwtScaleDraw
{
public:
    QVector<wavefront*> names;
    int currentNdx;
    wftNameScaleDraw(QVector<wavefront*> nameList, int nx)
    {
        names = nameList;
        currentNdx = nx;
        setTickLength( QwtScaleDiv::MajorTick, 10 );
        setTickLength( QwtScaleDiv::MinorTick, 1 );
        setTickLength( QwtScaleDiv::MediumTick, 5 );

        setLabelRotation( -45.0 );
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        setSpacing( 5 );
    }
    virtual QwtText label( double value ) const
    {
        if (value >= names.size())
            return QString("");
        QString n = names[int(((int)value + currentNdx) % names.size())]->name;
        QStringList l = n.split("/");
        if (l.size() > 1)
            n = l[l.size()-2] + "/"+ l[l.size()-1];
        return n;
    }
};

class StrehlScaleDraw: public QwtScaleDraw
{
public:
    StrehlScaleDraw()
    {

    }
    virtual QwtText label( double value ) const
    {
        double st =(2. *M_PI * value);
        st *= st;
        const double  e = 2.7182818285;
        double strl = pow(e,-st);
        return QString().sprintf("%6.3lf",strl);
    }
};

class ZernScaleDraw: public QwtScaleDraw
{
public:
    QVector<QString> m_names;
    ZernScaleDraw(QVector<QString> names)

    {
        m_names = names;
        setTickLength( QwtScaleDiv::MajorTick, 10 );
        setTickLength( QwtScaleDiv::MinorTick, 1 );
        setTickLength( QwtScaleDiv::MediumTick, 5 );

        setLabelRotation( -60.0 );
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        setSpacing( 5 );
    }

    virtual QwtText label( double value ) const
    {
        if (value == 2.){
            QwtText x( QString("wave front RMS"));
            x.setColor(Qt::red);

            return x;
        }
        if (value < 4)
            return QString("");
        if (value > m_names.size() +3)
            return QString("");
        QString str = m_names[int(value-4)];
        QwtText t(str);
        if (value == 6.)
            t.setColor(Qt::red);
        return t;

    }
};
// --- PROCESS ---
// Start processing data.
void SurfaceManager::generateSurfacefromWavefront(int wavefrontNdx) {

    wavefront *wf = m_wavefronts[wavefrontNdx];
    generateSurfacefromWavefront(wf);

    surfaceGenFinished( wavefrontNdx);
}

void SurfaceManager::generateSurfacefromWavefront(wavefront * wf){
    zernikeProcess &zp = *zernikeProcess::get_Instance();
    if (wf->dirtyZerns){
        if (mirrorDlg::get_Instance()->isEllipse()){
            wf->nulledData = wf->data.clone();
            if (m_GB_enabled){

                // compute blur radius
                int gaussianRad = 2 * wf->m_outside.m_radius * m_gbValue * .01;
                gaussianRad &= 0xfffffffe;

                ++gaussianRad;
                    cv::GaussianBlur( wf->nulledData.clone(), wf->workData,
                                      cv::Size( gaussianRad, gaussianRad ),0,0,BORDER_REFLECT);
            }
            else {
                wf->workData = wf->data.clone();
            }
            wf->InputZerns = std::vector<double>(zp.m_norms.size(), 0);
            wf->dirtyZerns = false;


            return;
        }

        //compute zernike values

        mirrorDlg *md = mirrorDlg::get_Instance();
        zp.unwrap_to_zernikes(*wf);
        // check for swapped conic value
        if (!m_ignoreInverse && (md->cc != 0.0) && md->cc * wf->InputZerns[8] < 0.){
            bool reverse = false;
            if (m_askAboutReverse){
                if (QMessageBox::Yes == QMessageBox::question(0,"should invert?","Wavefront seems inverted.  Do you want to invert it?"))
                {
                   reverse = true;
                    m_askAboutReverse = false;
                }else
                {
                    reverse = false;
                }
            }
            else {
                reverse = true;
            }
            if (reverse){
                wf->data *= -1;
            }
            zp.unwrap_to_zernikes(*wf);
        }
        ((MainWindow*)parent())-> zernTablemodel->setValues(wf->InputZerns, !wf->useSANull);
        ((MainWindow*)parent())-> zernTablemodel->update();
        // fill in void from obstruction of igram.
        if ( wf->regions.size() > 0){
            zp.fillVoid(*wf);
            makeMask(wf, false);
        }
        // null out desired terms.
        //cv::Mat tiltremoved = zp.null_unwrapped(*wf, wf->InputZerns, zernEnables, 0,3);
        //wf->data = tiltremoved;
        //zp.unwrap_to_zernikes(*wf);
        wf->nulledData = zp.null_unwrapped(*wf, wf->InputZerns, zernEnables,0,Z_TERMS   );
        wf->dirtyZerns = false;
    }

    wf->workData = wf->nulledData.clone();


    if (m_GB_enabled){
            expandBorder(wf);
            // compute blur radius
            int gaussianRad = 2 * wf->m_outside.m_radius * m_gbValue * .01;

            gaussianRad &= 0xfffffffe;
            ++gaussianRad;
            cv::GaussianBlur( wf->nulledData.clone(), wf->workData,
                              cv::Size( gaussianRad, gaussianRad ),0,0,BORDER_REFLECT);
    }

    wf->nulledData.release();
}
cv::Mat SurfaceManager::computeWaveFrontFromZernikes(int wx, int wy, std::vector<double> &zerns, QVector<int> zernsToUse){
    double rad = getCurrent()->m_outside.m_radius;
    double xcen = (wx-1)/2, ycen = (wy-1)/2;

    cv::Mat result = cv::Mat::zeros(wy,wx, numType);

    double rho;

    std::vector<bool> &en = zernEnables;
    mirrorDlg *md = mirrorDlg::get_Instance();
    zernikePolar &zpolar = *zernikePolar::get_Instance();
    for (int i = 0; i <  wx; ++i)
    {
        double x1 = (double)(i - (xcen)) / rad;
        for (int j = 0; j < wy; ++j)
        {
            double y1 = (double)(j - (ycen )) /rad;
            rho = sqrt(x1 * x1 + y1 * y1);

            if (rho <= 1.)
            {
                double S1 = 0;
                double theta = atan2(y1,x1);
                zpolar.init(rho,theta);
                for (int ii = 0; ii < zernsToUse.size(); ++ii) {
                    int z = zernsToUse[ii];

                    if ( z == 3 && m_surfaceTools->m_useDefocus){
                        S1 += m_surfaceTools->m_defocus * zpolar.zernike(z,rho,theta);
                    }
                    else {
                        if (en[z]){
                            if (z == 8 && md->doNull)
                                S1 +=    md->z8 * zpolar.zernike(z,rho, theta);

                            S1 += zerns[z] * zpolar.zernike(z,rho, theta);
                        }
                    }
                }
                result.at<double>(j,i) = S1;
            }
            else
            {
                result.at<double>(j,i) = 0 ;
            }
        }
    }
    //cv::imshow("zernbased", result);
    //cv::waitKey(1);
    return result;
}
SurfaceManager *SurfaceManager::m_instance = 0;
SurfaceManager *SurfaceManager::get_instance(QObject *parent, surfaceAnalysisTools *tools,
                                             ProfilePlot *profilePlot, contourView *contourPlot,
                                             SurfaceGraph *glPlot, metricsDisplay *mets){
    if (m_instance == 0){
        m_instance = new SurfaceManager(parent, tools, profilePlot, contourPlot, glPlot, mets);
    }
    return m_instance;
}

SurfaceManager::SurfaceManager(QObject *parent, surfaceAnalysisTools *tools,
                               ProfilePlot *profilePlot, contourView *contourView,
                               SurfaceGraph *glPlot, metricsDisplay *mets): QObject(parent),
    m_surfaceTools(tools),m_profilePlot(profilePlot), m_contourView(contourView),
    m_SurfaceGraph(glPlot), m_metrics(mets),
    m_gbValue(21),m_GB_enabled(false),m_currentNdx(-1),m_standAvg(0),insideOffset(0),
    outsideOffset(0),m_askAboutReverse(true),m_ignoreInverse(false), workToDo(0), m_wftStats(0)
{

    okToUpdateSurfacesOnGenerateComplete = true;
    m_simView = SimulationsView::getInstance(0);

    pd = new QProgressDialog();
    pd->reset();

    connect (this,SIGNAL(progress(int)), pd, SLOT(setValue(int)));

    m_profilePlot->setWavefronts(&m_wavefronts);
    // create a timer for surface change update to all non current wave fronts
    m_waveFrontTimer = new QTimer(this);
    m_toolsEnableTimer = new QTimer(this);
    // setup signal and slot
    connect(m_waveFrontTimer, SIGNAL(timeout()),this, SLOT(backGroundUpdate()));
    connect(m_toolsEnableTimer, SIGNAL(timeout()), this, SLOT(enableTools()));

    connect(m_surfaceTools, SIGNAL(waveFrontClicked(int)), this, SLOT(waveFrontClickedSlot(int)));
    connect(m_surfaceTools, SIGNAL(wavefrontDClicked(const QString &)), this, SLOT(wavefrontDClicked(const QString &)));
    connect(m_surfaceTools, SIGNAL(centerMaskValue(int)),this, SLOT(centerMaskValue(int)));
    connect(m_surfaceTools, SIGNAL(outsideMaskValue(int)),this, SLOT(outsideMaskValue(int)));
    connect(m_surfaceTools, SIGNAL(surfaceSmoothGBEnabled(bool)), this, SLOT(surfaceSmoothGBEnabled(bool)));
    connect(m_surfaceTools, SIGNAL(surfaceSmoothGBValue(double)), this, SLOT(surfaceSmoothGBValue(double)));
    connect(m_surfaceTools, SIGNAL(wftNameChanged(int,QString)), this, SLOT(wftNameChanged(int,QString)));
    connect(this, SIGNAL(nameChanged(QString, QString)), m_surfaceTools, SLOT(nameChanged(QString,QString)));
    connect(m_metrics, SIGNAL(recomputeZerns()), this, SLOT(computeZerns()));
    connect(m_surfaceTools, SIGNAL(defocusChanged()), this, SLOT(defocusChanged()));
    connect(m_surfaceTools, SIGNAL(defocusSetup()), this, SLOT(defocusSetup()));
    connect(this, SIGNAL(currentNdxChanged(int)), m_surfaceTools, SLOT(currentNdxChanged(int)));
    connect(this, SIGNAL(deleteWavefront(int)), m_surfaceTools, SLOT(deleteWaveFront(int)));
    connect(m_surfaceTools, SIGNAL(deleteTheseWaveFronts(QList<int>)), this, SLOT(deleteWaveFronts(QList<int>)));
    connect(m_surfaceTools, SIGNAL(average(QList<int>)),this, SLOT(average(QList<int>)));
    connect(m_surfaceTools, SIGNAL(doxform(QList<int>)),this, SLOT(transfrom(QList<int>)));
    connect(m_surfaceTools, SIGNAL(invert(QList<int>)),this,SLOT(invert(QList<int>)));
    connect(m_surfaceTools, SIGNAL(filterWavefronts(QList<int>)),this,SLOT(filter(QList<int>)));
    connect(this, SIGNAL(enableControls(bool)),m_surfaceTools, SLOT(enableControls(bool)));
    connect(mirrorDlg::get_Instance(),SIGNAL(recomputeZerns()), this, SLOT(computeZerns()));
    connect(mirrorDlg::get_Instance(),SIGNAL(obstructionChanged()), this, SLOT(ObstructionChanged()));
    QSettings settings;
    m_GB_enabled = settings.value("GBlur", true).toBool();
    if (!settings.contains("gaussianRadiusConverted")){
            settings.setValue("gaussianRadiusConverted", true);
            settings.setValue("GBValue", 20);
    }
    m_gbValue = settings.value("GBValue", 20).toInt();
    //useDemoWaveFront();
    connect(Settings2::getInstance()->m_general, SIGNAL(outputLambdaChanged(double)), this, SLOT(outputLambdaChanged(double)));
    outputLambda = settings.value("outputLambda", 550.).toDouble();
    //useDemoWaveFront();
    surfaceSmoothGBValue(m_gbValue);
    initWaveFrontLoad();
}

SurfaceManager::~SurfaceManager(){}
void SurfaceManager::outputLambdaChanged(double val){
    outputLambda = val;
    computeZerns();
}
void DrawPoly(cv::Mat &data, QVector<std::vector<cv::Point> > poly){
    for (int n = 0; n < poly.size(); ++n){
        cv::Point points[1][poly[n].size()];
        for (int i = 0; i < poly[n].size(); ++i){

            points[0][i] = cv::Point(poly[n][i].x, data.rows - poly[n][i].y);

        }
        for (int j = 0; j < poly[n].size()-1; ++j){
            cv::line(data, points[0][j], points[0][j+1], cv::Scalar(0));

        }
        const Point* ppt[1] = { points[0]};
        int npt[] = {(int) (poly[n].size()) };

        fillPoly( data, ppt, npt, 1, Scalar(0), 8 );

    }
}

void SurfaceManager::makeMask(int waveNdx, bool useCenterCircle){
    makeMask(m_wavefronts[waveNdx], useCenterCircle);
}


void SurfaceManager::makeMask(wavefront *wf, bool useInsideCircle){
    int width = wf->data.cols;
    int height = wf->data.rows;
    double xm,ym;
    xm = wf->m_outside.m_center.x();
    ym = wf->m_outside.m_center.y();
    double radm =wf->m_outside.m_radius + outsideOffset-2;
    double rado = wf->m_inside.m_radius + insideOffset;
    if (rado > 0)
        rado += (insideOffset + 1);

    double cx = wf->m_inside.m_center.x();
    double cy = wf->m_inside.m_center.y();
    cv::Mat mask = cv::Mat::zeros(height,width,CV_8U);
    mirrorDlg &md = *mirrorDlg::get_Instance();
    double rx = radm;
    double rx2 = rx * rx;
    double ry = rx * md.m_verticalAxis/md.diameter;
    double ry2 = ry * ry;
    if (!mirrorDlg::get_Instance()->isEllipse()){
        uchar v = 0x2ff;
        fillCircle(mask, xm,ym,radm, &v);
    }
    else {
        for (int y = 0; y < height; ++y){
            for (int x = 0; x < width; ++x){

                if (fabs(x -xm) > rx || fabs(y -ym) > ry)
                    continue;
                double v = pow(x - xm, 2)/rx2 + pow(y - ym, 2)/ry2;
                if (v <= 1)
                    mask.at<uchar>(y,x) = 255;
            }
        }
    }

    if (rado > 0) {
        uchar color = 0;
        fillCircle(mask, cx,cy,rado, &color);
    }

    // expand the region by 10%
    if (wf->regions.size() > 0 && useInsideCircle){
        for (int n = 0; n < wf->regions.size(); ++ n){
            int xavg = 0;
            int yavg = 0;
            for (int i = 0; i < wf->regions[n].size(); ++i){
                xavg += wf->regions[n][i].x;
                yavg += wf->regions[n][i].y;
            }
            xavg /= wf->regions[n].size();
            yavg /= wf->regions[n].size();
            // find the closest point to the center
            double shortest = 99999;
            int shortestndx;
            for (int i = 0; i < wf->regions[n].size(); ++i){
                int delx = wf->regions[n][i].x - xavg;
                int dely = wf->regions[n][i].y - yavg;
                double del = sqrt(delx * delx + dely * dely);
                if (del < shortest){
                    shortest = del;
                    shortestndx = i;
                }
            }
            double scale = 1.1 * (shortest+2)/shortest;
            for (int i = 0; i < wf->regions[n].size(); ++i){
                wf->regions[n][i].x = scale * (wf->regions[n][i].x - xavg) + xavg;
                wf->regions[n][i].y = scale * (wf->regions[n][i].y - yavg) + yavg;
            }
        }

        DrawPoly(mask, wf->regions);

    }
    wf->mask = mask.clone();
    wf->workMask = mask.clone();
    //int s = wf->workMask.size[0];
    //cv::circle(wf->workMask, Point(s/2,s), s/4, cv::Scalar(0,0,0), -1);
    //line(wf->workMask, Point(s/2, 0), Point(s/2,s),cv::Scalar(0,0,0), 10);
   // line(wf->workMask, Point(0, s/2), Point(s,s/2),cv::Scalar(0,0,0), 10);
    //line(wf->workMask, Point(0, 0), Point(s,s),cv::Scalar(0,0,0), 10);
    theMask = mask.clone();

    // add central obstruction
    double r = md.obs * (2. * radm)/md.diameter;
    r/= 2.;
    if (r > 0){

        cv::Mat m = wf->workMask;
        circle(m,Point((m.cols-1)/2,(m.cols-1)/2),r, Scalar(0),-1);
    }

    if (Settings2::showMask())
        showData("surface manager mask",mask);

}
void SurfaceManager::wftNameChanged(int ndx, QString name){
    m_wavefronts[ndx]->name = name;

}

void SurfaceManager::sendSurface(wavefront* wf){
    emit currentNdxChanged(m_currentNdx);
    computeMetrics(wf);

    m_contourView->setSurface(wf);
    m_profilePlot->setSurface(wf);
    m_SurfaceGraph->setSurface(wf);
    m_simView->setSurface(wf);
    foucaultView::get_Instance()->setSurface(wf);

    QFile fn(wf->name);
    QFileInfo fileInfo(fn.fileName());
    QString filename(fileInfo.fileName());
    ((MainWindow*)(parent()))->setWindowTitle(filename);
}
void SurfaceManager::ObstructionChanged(){
    if (m_wavefronts.size() > 0)
    outsideMaskValue(outsideOffset);// use this function to trigger the mask making and recomputing.
}

void SurfaceManager::centerMaskValue(int val){
    insideOffset = val;
    double mmPerPixel = getCurrent()->diameter/(2 *( m_wavefronts[m_currentNdx]->m_outside.m_radius-1));
    m_surfaceTools->m_centerMaskLabel->setText(QString().sprintf("%6.2lf mm",mmPerPixel* val));
    makeMask(m_currentNdx);
    wavefront *wf = m_wavefronts[m_currentNdx];
    wf->dirtyZerns = true;
    wf->wasSmoothed = false;
    //emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(500);

}

void SurfaceManager::outsideMaskValue(int val){
    outsideOffset = val;
    double mmPerPixel = m_wavefronts[m_currentNdx]->diameter/(2 * (m_wavefronts[m_currentNdx]->m_outside.m_radius));
    m_surfaceTools->m_edgeMaskLabel->setText(QString().sprintf("%6.2lf mm",mmPerPixel* val));
    makeMask(m_currentNdx);
    wavefront *wf = m_wavefronts[m_currentNdx];
    wf->dirtyZerns = true;
    wf->wasSmoothed = false;
    //emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(500);

}
void SurfaceManager::useDemoWaveFront(){
    int wx = 640;
    int wy = wx;
    double rad = (double)(wx-1)/2.;
    double xcen = rad,ycen = rad;
    double rho;
    mirrorDlg *md = mirrorDlg::get_Instance();
    cv::Mat result = cv::Mat::zeros(wx,wx, numType);
    zernikePolar &zpolar = *zernikePolar::get_Instance();
    for (int i = 0; i <  wx; ++i)
    {
        double x1 = (double)(i - (xcen)) / rad;
        for (int j = 0; j < wy; ++j)
        {
            double y1 = (double)(j - (ycen )) /rad;
            rho = sqrt(x1 * x1 + y1 * y1);
            double theta = atan2(y1,x1);
            zpolar.init(rho,theta);

            if (rho <= 1.)
            {
                double S1 = md->z8 * -.9 * zpolar.zernike(8,rho,theta) + .02* zpolar.zernike(9, rho,theta);

                result.at<double>(j,i) = S1;
            }
            else
            {
                result.at<double>(j,i) = 0 ;
            }
        }
    }




    createSurfaceFromPhaseMap(result,
                              CircleOutline(QPointF(xcen,ycen),rad),
                              CircleOutline(QPointF(0,0),0),
                              QString("Demo"));
}

void SurfaceManager::waveFrontClickedSlot(int ndx)
{

    m_currentNdx = ndx;
    QString msg = QString().sprintf(" %dx%d ",m_wavefronts[ndx]->data.cols, m_wavefronts[ndx]->data.rows);
    ((MainWindow*)parent())->statusBar()->showMessage(msg);
    sendSurface(m_wavefronts[ndx]);
}
void SurfaceManager::deleteWaveFronts(QList<int> list){

    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach(int ndx, list ){
        m_currentNdx = ndx;

        deleteCurrent();
    }
    QApplication::restoreOverrideCursor();
}

void SurfaceManager::wavefrontDClicked(const QString & name){
    for (int i = 0; i < m_wavefronts.size(); ++i){
        if (m_wavefronts[i]->name.endsWith(name)){
            m_currentNdx = i;
            sendSurface(m_wavefronts[i]);
            break;
        }
    }
}

void SurfaceManager::surfaceSmoothGBValue(double value){

    QSettings settings;
    settings.setValue("GBValue", (int)(value));
    m_gbValue = value;
    mirrorDlg *md = mirrorDlg::get_Instance();

    m_surfaceTools->setBlurText(QString().sprintf("%6.2lf mm", .01 * value * md->diameter));
    if (m_wavefronts.size() == 0)
        return;

    m_wavefronts[m_currentNdx]->GBSmoothingValue = 0;
    //emit generateSurfacefromWavefront(m_currentNdx, this);

    m_waveFrontTimer->start(500);

}
void SurfaceManager::surfaceSmoothGBEnabled(bool b){

    m_GB_enabled = b;

    QSettings settings;
    settings.setValue("GBlur", m_GB_enabled);
    double rad = 320.;

    if (m_wavefronts.size() != 0)
        rad = m_wavefronts[m_currentNdx]->m_outside.m_radius-1;

    mirrorDlg *md = ((MainWindow*)parent())->m_mirrorDlg;
    double mmPerPixel = md->diameter/(2 * rad);
    m_surfaceTools->setBlurText(QString().sprintf("%6.2lf mm",m_gbValue* mmPerPixel));
    if (m_wavefronts.size() == 0)
        return;
    //emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(500);
}

void SurfaceManager::computeMetrics(wavefront *wf){
    mirrorDlg *md = mirrorDlg::get_Instance();
    cv::Scalar mean,std;
    cv::meanStdDev(wf->workData,mean,std,wf->workMask);
    wf->mean = mean.val[0] * md->lambda/outputLambda;
    wf->std = std.val[0]* md->lambda/outputLambda;

    double mmin;
    double mmax;

    minMaxIdx(wf->workData, &mmin,&mmax);

    wf->min = mmin * md->lambda/outputLambda;
    wf->max = mmax * md->lambda/outputLambda;


    ((MainWindow*)(parent()))->zernTablemodel->setValues(wf->InputZerns, !wf->useSANull);
    ((MainWindow*)parent())-> zernTablemodel->update();
    ((MainWindow*)(parent()))->updateMetrics(*wf);

}
void SurfaceManager::defocusSetup(){
    wavefront *wf = m_wavefronts[m_currentNdx];
    qDebug() << "wave" << wf->workData.rows;
    m_profilePlot->setDefocusWaveFront(wf->workData);
}

void SurfaceManager::defocusChanged(){

    double val = m_surfaceTools->m_defocus;
    //if (!m_surfaceTools->m_useDefocus)
        //val = 0.;

    wavefront *wf = m_wavefronts[m_currentNdx];
    wf->dirtyZerns = true;
    wf->wasSmoothed = false;
    bool old_ignore = m_ignoreInverse;
    m_ignoreInverse = true;
    generateSurfacefromWavefront(wf);
    m_ignoreInverse = old_ignore;
    m_profilePlot->setDefocusValue(val);
    m_waveFrontTimer->start(2000);

}

void SurfaceManager::computeZerns()
{
    QList<int> doThese =  m_surfaceTools->SelectedWaveFronts();
    workToDo = doThese.size();
    mirrorDlg &md = *mirrorDlg::get_Instance();
    foreach(int ndx , doThese){
        wavefront &wf = *m_wavefronts[ndx];
        wf.diameter = md.diameter;
        wf.roc = md.roc;
        wf.lambda = md.lambda;
    }

    m_waveFrontTimer->start(500);
}

void SurfaceManager::writeWavefront(QString fname, wavefront *wf, bool saveNulled){
        std::ofstream file((fname.toStdString().c_str()));

        if (!file.is_open()) {
            QMessageBox::warning(0, tr("Write wave front"),
                                 tr("Cannot write file %1: ")
                                 .arg(fname));
            return;
        }
        file << wf->data.cols << std::endl << wf->data.rows << std::endl;
        for (int row = wf->data.rows - 1; row >=0; --row){
            for (int col = 0; col < wf->data.cols ; ++col){
                if (saveNulled)
                    file << wf->workData(row,col) << std::endl;
                else {
                    file << wf->data(row,col) << std::endl;

                }
            }
        }
        int offsetOutside = 0;
        QSettings set;
        if (set.value("applyOffsets", false).toBool()){
            offsetOutside = outsideOffset;
        }

        file << "outside ellipse " <<
                       wf->m_outside.m_center.x()
             << " " << wf->m_outside.m_center.y()
             << " " << wf->m_outside.m_radius + offsetOutside
             << " "  << wf->m_outside.m_radius << std:: endl;


        if (wf->m_inside.m_radius > 0){
            file << "obstruction ellipse " << wf->m_inside.m_center.x()
             << " " << wf->m_inside.m_center.y()
             << " " << wf->m_inside.m_radius + ((set.value("applyOffsets", false).toBool()) ? insideOffset: 0)
             << " " << wf->m_inside.m_radius << std:: endl;
        }

        file << "DIAM " << wf->diameter << std::endl;
        file << "ROC " << wf->roc << std::endl;
        file << "Lambda " << wf->lambda << std::endl;
        mirrorDlg &md = *mirrorDlg::get_Instance();
        if (md.isEllipse()){
            file << "ellipse_vertical_axis " << md.m_verticalAxis;
        }
}

void SurfaceManager::SaveWavefronts(bool saveNulled){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QList<int> list = m_surfaceTools->SelectedWaveFronts();
    if (list.size() <= 1 && m_wavefronts.size() > 0) {
        QSettings settings;
        QString lastPath = settings.value("lastPath","").toString();
        QString fileName = m_wavefronts[m_currentNdx]->name;
        QFileInfo fileinfo(fileName);
        QString file = fileinfo.baseName();
        fileName = QFileDialog::getSaveFileName(0,
             tr("Write wave font file"), lastPath + "/" + file,
             tr("wft (*.wft)"));
        if (fileName.isEmpty()){
            QApplication::restoreOverrideCursor();
            return;
        }
        if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".wft"); }
        QString lastDir = QFileInfo(fileName).absoluteDir().path();
        settings.setValue("lastPath", lastDir);
        writeWavefront(fileName, m_wavefronts[m_currentNdx], saveNulled);

    }
    else if (list.size() > 1){
        QSettings settings;
        QString path = settings.value("lastPath").toString();
        QFile fn(path);
        QFileInfo info(fn.fileName());
        QString dd = info.dir().absolutePath();

        QString dir = QFileDialog::getExistingDirectory(0, tr("Open Directory")
                                                        ,path,
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if (dir == ""){
            QApplication::restoreOverrideCursor();
            return;
        }
        // find last file in dir

        QProgressDialog progress("Saving wavefront files", "Cancel", 0, list.size(), 0);
        progress.setWindowModality(Qt::WindowModal);
        progress.setValue(0);
        for (int i = 0; i < list.size(); ++i){
            progress.setValue(i+1);

            wavefront *wf = m_wavefronts[list[i]];
            progress.setLabelText(wf->name);

            QString fname = wf->name;
            QStringList fnparts = fname.split("/");
            if (fnparts.size() > 1)
                fname = fnparts[fnparts.size()-1];
            if (QFileInfo(fname).suffix().isEmpty()) { fname.append(".wft");}
            QString fullPath = dir + QDir::separator() + fname;
            QFileInfo info;
            // check if file exists and if yes: Is it really a file and no directory?
            if (info.exists(fullPath)) {
                if (QMessageBox::question(0,"file alread exists", fullPath +" already exists. Do you want to overwrite it?") ==
                        QMessageBox::No)
                    continue;

            }
            qApp->processEvents();
            writeWavefront(fullPath, wf, saveNulled);


        }
    }
    QApplication::restoreOverrideCursor();
}
void SurfaceManager::createSurfaceFromPhaseMap(cv::Mat phase, CircleOutline outside,
                                               CircleOutline center,
                                               QString name, QVector<std::vector<Point> > polyArea){

    wavefront *wf;

    int newrows = Settings2::getInstance()->m_general->wavefrontSize();
    if (Settings2::getInstance()->m_general->shouldDownsize() && ( phase.rows > newrows)){
        double scaleFactor = (double)newrows/double(phase.rows);
        cv::resize(phase,phase, cv::Size(newrows,newrows), 0, 0,INTER_AREA);
        outside.scale(scaleFactor);
        center.scale(scaleFactor);

        for (int n = 0; n < polyArea.size(); ++n){
            for (unsigned int i = 0; i < polyArea[n].size(); ++i){
                int x = round((polyArea[n][i].x) * scaleFactor);
                int y = round((polyArea[n][i].y) * scaleFactor);
                polyArea[n][i].x = x;
                polyArea[n][i].y = y;
            }
        }

    }

    if (m_wavefronts.size() >0 && (m_currentNdx == 0 &&  m_wavefronts[0]->name == "Demo")){
        qDebug() << "using demo";
        wf = m_wavefronts[0];
        emit nameChanged(wf->name, name);
        wf->name = name;
        wf->dirtyZerns = true;
    }
    else {
        wf = new wavefront();
        m_wavefronts << wf;

        wf->name = name;

        m_surfaceTools->addWaveFront(wf->name);
        m_currentNdx = m_wavefronts.size()-1;
    }
    wf->m_outside = outside;
    wf->m_inside = center;
    wf->data = phase;
    mirrorDlg *md = mirrorDlg::get_Instance();
    wf->diameter = md->diameter;
    wf->lambda = md->lambda;
    wf->roc = md->roc;
    wf->dirtyZerns = true;
    wf->wasSmoothed = false;
    wf->regions = polyArea;
    m_currentNdx = m_wavefronts.size()-1;

    makeMask(m_currentNdx);

    generateSurfacefromWavefront(m_currentNdx);
    loadComplete();
    m_surfaceTools->select(m_currentNdx);
    emit showTab(2);
}
wavefront *SurfaceManager::xxx(QString name, bool t){
    return readWaveFront(name, false);
}

wavefront * SurfaceManager::readWaveFront(QString fileName, bool mirrorParamsChanged){
    std::ifstream file(fileName.toStdString().c_str());
    if (!file) {
        QString b = "Can not read file " + fileName + " " +strerror(errno);
        QMessageBox::warning(NULL, tr("Read Wavefront File"),b);
        return 0;
    }
    wavefront *wf = new wavefront();
    double width;
    double height;
    file >> width;
    file >> height;
    cv::Mat data(height,width, numType,0.);

    for( size_t y = 0; y < height; y++ ) {
        for( size_t x = 0; x < width; x++ ) {
            file >> data.at<double>(height - y-1,x);
            //data.at<double>(height - y - 1, x) += dist(generator);
        }
    }

    std::string line;
    QString l;
    mirrorDlg *md = mirrorDlg::get_Instance();

    double xm = (width-1)/2.,ym = (height-1)/2.,
            radm = min(xm,ym)-2 ,
            roc = md->roc,
            lambda = md->lambda,
            diam = md->diameter;
    double xo = width/2., yo = height/2., rado = 0;

    std::string dummy;
    while (getline(file, line)) {
        l = QString::fromStdString(line);
        std::istringstream iss(line);
        if (l.startsWith("outside")) {
            QStringList sl = l.split(" ");
            xm = sl[2].toDouble();
            radm = sl[4].toDouble();
            ym = sl[3].toDouble();
            continue;
        }
        if (l.startsWith("DIAM")){
            iss >> dummy >> diam;
            continue;
        }
        if (l.startsWith("ROC")){
            iss >> dummy >> roc;
            continue;
        }
        if (l.startsWith("Lambda")){
            iss >> dummy >> lambda;
            continue;
        }
        if (l.startsWith("obstruction")){
            iss >> dummy >> dummy >> xo >> yo >> rado;
            continue;
        }
        if (l.startsWith("ellipse_vertical_axis")){
            md->m_outlineShape = ELLIPSE;
            iss >> dummy >> md->m_verticalAxis;
        }
        if (l.startsWith("nulled")){
            wf->useSANull = false;
        }
    }

    wf->m_outside = CircleOutline(QPointF(xm,ym), radm);
    if (rado == 0){
        xo = xm;
        yo = ym;
    }
    if (md->isEllipse()){
        wf->m_outside = CircleOutline(QPointF(xm,ym), xm -2);
    }
    wf->m_inside = CircleOutline(QPoint(xo,yo), rado);


    if (lambda != md->lambda){
        if (lambdResp == ASK){
            QString message("The interferogram wavelength (");
            message += QString().sprintf("%6.3lf", lambda) +
                    ") Of the wavefront does not match the config value of " + QString().sprintf("%6.3lf\n",md->lambda) +
                    "Do you want to make the config match?";


            int resp = QMessageBox(QMessageBox::Information,"config",message,QMessageBox::Yes|QMessageBox::No |
                                 QMessageBox::YesToAll | QMessageBox::NoToAll).exec();

            switch (resp){
                case QMessageBox::YesToAll:
                        lambdResp = YES;
                break;
                case QMessageBox::NoToAll:
                    lambdResp = NO;
                    break;
            }
        }

        if ( lambdResp == YES || messageResult == QMessageBox::Yes){
            md->newLambda(QString::number(lambda));
        }
        else {
            mirrorParamsChanged = true;
        }
    }

    if (roundl(diam * 10) != roundl(md->diameter* 10))
    {
        QString message("The mirror diameter (");
        message += QString().sprintf("%6.3lf",diam) +
                ") Of the wavefront does not match the config value of " + QString().sprintf("%6.3lf\n",md->diameter) +
                "Do you want to make the config match?";
        if (diamResp == ASK){
            int resp = QMessageBox(QMessageBox::Information,"config", message,QMessageBox::Yes|QMessageBox::No |
                                 QMessageBox::YesToAll | QMessageBox::NoToAll).exec();

            switch (resp){
                case QMessageBox::YesToAll:
                        diamResp = YES;
                break;
            case QMessageBox::NoToAll:
                diamResp = NO;
                break;
            }
        }
        if (diamResp == YES || messageResult == QMessageBox::Yes){
            emit diameterChanged(diam);
        }
        else {
            diam = md->diameter;
            mirrorParamsChanged = true;
        }

    }

    if (roundl(roc * 10.) != roundl(md->roc * 10.))
    {
        QString message("The mirror roc (");
        message += QString().sprintf("%6.3lf",roc) +
                ") Of the wavefront does not match the config value of " + QString().sprintf("%6.3lf\n",md->roc) +
                "Do you want to make the config match?";
        //qDebug() << message;
        if (rocResp == ASK){
            int resp = QMessageBox(QMessageBox::Information,"config",message,QMessageBox::Yes|QMessageBox::No |
                                 QMessageBox::YesToAll | QMessageBox::NoToAll).exec();

            switch (resp){
                case QMessageBox::YesToAll:
                        rocResp = YES;
                break;
            case QMessageBox::NoToAll:
                rocResp = NO;
                break;
            }
        }
        if (rocResp == YES || messageResult == QMessageBox::Yes){
            emit rocChanged(roc);

        }
        else {
            roc = md->roc;

        }
    }
    wf->diameter = diam;
    wf->data= data;
    wf->roc = roc;
    wf->lambda = lambda;
    wf->wasSmoothed = false;

    return wf;
}
void SurfaceManager::downSizeWf(wavefront *wf){
    int newcols, newrows;
    qDebug() << "downSize1" << wf->data.rows << wf->data.cols << wf->m_outside.m_center;
    newcols = newrows = Settings2::getInstance()->m_general->wavefrontSize();
    if (newcols < wf->data.cols){
        double xscale;
        xscale = (double)newcols/wf->data.cols;
        cv::Mat resized = wf->data.clone();
        cv::resize(wf->data, wf->data, Size(newrows, newcols));

        // change outside and inside boundaries
        wf->m_outside.scale(xscale);
        wf->m_inside.scale(xscale);

    }
    qDebug() << "downSize2" << wf->data.rows << wf->data.cols << wf->m_outside.m_center;
}

bool SurfaceManager::loadWavefront(const QString &fileName){

    emit enableControls(false);
    bool mirrorParamsChanged = false;
    std::ifstream file(fileName.toStdString().c_str());
    if (!file) {
        QString b = "Can not read file " + fileName + " " +strerror(errno);
        QMessageBox::warning(NULL, tr("Read Wavefront File"),b);
    }
    wavefront *wf;

    if (m_currentNdx == 0 &&  m_wavefronts[0]->name == "Demo"){
        deleteCurrent();
    }

        wf = readWaveFront(fileName, mirrorParamsChanged);
        m_wavefronts << wf;

        wf->name = fileName;

        m_surfaceTools->addWaveFront(wf->name);
        m_currentNdx = m_wavefronts.size()-1;
        m_surfaceTools->select(m_currentNdx);

    // if resize to smaller
    if (Settings2::getInstance()->m_general->shouldDownsize()){
        downSizeWf(wf);
    }
    makeMask(m_currentNdx);

    m_surface_finished = false;
    try {
        generateSurfacefromWavefront(m_currentNdx);
    }
    catch (int i){
        deleteCurrent();
        throw i;
    }

    return mirrorParamsChanged;
}
void SurfaceManager::deleteCurrent(){
    if (m_wavefronts.size() == 0)
        return;
    if (m_wavefronts.length()) {
        emit deleteWavefront(m_currentNdx);
        delete m_wavefronts[m_currentNdx];
        m_wavefronts.removeAt(m_currentNdx);

        if (m_currentNdx > 0)
            --m_currentNdx;
    }
    showmem("After delete");
    if (m_wavefronts.length() > 0) {

        sendSurface(m_wavefronts[m_currentNdx]);
    }
    else useDemoWaveFront();

    emit currentNdxChanged(m_currentNdx);
}

void SurfaceManager::processSmoothing(){
    if (m_wavefronts.size() == 0)
        return;

    wavefront *wf = m_wavefronts[m_currentNdx];
    if (m_GB_enabled){
        if (wf->wasSmoothed != m_GB_enabled || wf->GBSmoothingValue != m_gbValue) {
            // compute blur radius
            int gaussianRad = 2. * wf->m_outside.m_radius * m_gbValue * .01;
            gaussianRad &= 0xfffffffe;
            ++gaussianRad;
            cv::GaussianBlur( wf->nulledData.clone(), wf->workData,
                              cv::Size( gaussianRad, gaussianRad ),0,0,BORDER_REFLECT);
        }
    }
    else if (wf->wasSmoothed == true) {
        wf->workData = wf->nulledData.clone();
    }

    sendSurface(wf);
}

void SurfaceManager::next(){
    if (m_wavefronts.size() == 0)
        return;
    if (m_currentNdx < m_wavefronts.length()-1)
        ++m_currentNdx;
    else
        m_currentNdx = 0;
    sendSurface(m_wavefronts[m_currentNdx]);


}

void SurfaceManager::previous(){
    if (m_wavefronts.size() == 0)
        return;
    if (m_currentNdx > 0) {
        --m_currentNdx;
    }
    else
        m_currentNdx = m_wavefronts.length()-1;

    sendSurface(m_wavefronts[m_currentNdx]);
}
QVector<int> histo(const std::vector<double> data, int bins, double min, double max){
    QVector<int> h(bins, 0);
    double interval = (max - min)/bins;
    for (unsigned int i = 0; i < data.size(); ++i){
        double to = interval;
        for (int j = 0; j <  bins; ++j){
            if (data[i] < to) {
                ++h[j];
                break;
            }
            to += interval;
        }
    }
    return h;
}
#include "statsview.h"
void SurfaceManager::saveAllWaveFrontStats(){

    if (m_wavefronts.size() == 0)
        return;
        statsView * sv = new statsView(this);
        sv->show();
        return;
}
void SurfaceManager::enableTools(){

    m_toolsEnableTimer->stop();

    m_surfaceTools->setEnabled(true);


}

void SurfaceManager::surfaceGenFinished(int ndx) {
    if (workToDo > 0)
        emit progress(++workProgress);

    m_surface_finished = true;
    if (okToUpdateSurfacesOnGenerateComplete){
        loadComplete();
    }
    if (workProgress == workToDo)
        workToDo = 0;

}

void SurfaceManager::loadComplete(){
    m_toolsEnableTimer->start(1000);
    if (m_wavefronts.size() > 0){
        sendSurface(m_wavefronts[m_currentNdx]);
        computeMetrics(m_wavefronts[m_currentNdx]);
    }
}

// Update all surfaces since some control has changed.  Skip current surface it has already been done
void SurfaceManager::backGroundUpdate(){

    m_waveFrontTimer->stop();
    workProgress = 0;

    m_surfaceTools->setEnabled(false);
    QList<int> doThese =  m_surfaceTools->SelectedWaveFronts();
    workToDo = doThese.size();
    pd->setLabelText("Updating Selected Surfaces");
    pd->setRange(0,doThese.size());
    foreach (int i, doThese){
        m_wavefronts[i]->dirtyZerns = true;
        m_wavefronts[i]->wasSmoothed = false;
        makeMask(i);
        try {
            generateSurfacefromWavefront(i);
        }
        catch (int i) {
            break;
        }
    }
    m_ignoreInverse = false;
    loadComplete();
}



void SurfaceManager::average(QList<int> list){
    if (list.length() < 2){
            QMessageBox::warning(0,"Warning", "Select at least two wave fronts to be averaged.");
            return;
    }
    qDebug() << "average these"<< list;
    QList<wavefront *> wflist;
    for (int i = 0; i < list.size(); ++i){
        wflist.append(m_wavefronts[list[i]]);
    }
    average(wflist);
}


#include "ccswappeddlg.h"
void SurfaceManager::average(QList<wavefront *> wfList){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    // check that all the cc have the same sign
    bool sign = wfList[0]->InputZerns[8] < 0;
    bool someReversed = false;
    bool needsUpdate = false;
    foreach (wavefront *wf, wfList){
        if ((wf->InputZerns[8] < 0) !=  sign)
        {
            someReversed = true;
            break;
        }
    }
    if (someReversed){
        CCSwappedDlg dlg;
        if (dlg.exec()){
            foreach(wavefront *wf, wfList){
                if ((wf->InputZerns[8] < 0 && dlg.getSelection() == NEGATIVE) ||
                    (wf->InputZerns[8] > 0 && dlg.getSelection() == POSITIVE))
                {
                    wf->data *= -1;
                    wf->dirtyZerns = true;
                    wf->wasSmoothed = false;
                    m_surface_finished = false;
                    needsUpdate = true;
                    generateSurfacefromWavefront(wf);
                }
            }
        }
        else {
            QApplication::restoreOverrideCursor();
            return;
        }
    }

    // normalize the size to the most common size

    QHash<QString,QList<int>> sizes;
    for (int i = 0; i < wfList.size(); ++i){
        QString size;
        size.sprintf("%d %d",wfList[i]->data.rows, wfList[i]->data.cols);
        if (sizes.find(size)!= sizes.end())
        {
            sizes[size].append( i);
        }
        else {
            QList<int> l;
            l << i;
            sizes[size] = l;
        }
    }
    int max = 0;
    QString maxkey;
    qDebug() << "sizes" << sizes;

    foreach(QString v, sizes.keys()){
        int a = sizes[v].length();
        if (a > max) {
            max = a;
            maxkey = v;
            qDebug() << "max" << v << max;
        }
    }
    int rrows, rcols;

    QTextStream s(&maxkey);

    s >> rrows >> rcols;

qDebug() << "maxkey" << maxkey << rrows << rcols << sizes[maxkey];
    cv::Mat mask = wfList[sizes[maxkey][0]]->workMask.clone();
    if (mask.cols != rcols || mask.rows != rrows){
        cv::resize(mask,mask,Size(rrows,rcols));
    }

    cv::Mat sum = cv::Mat::zeros(rrows,rcols, m_wavefronts[m_currentNdx]->data.type());
    cv::Mat count = cv::Mat::zeros(rrows,rcols, CV_32S);
    cv::Mat resizedImage;

    for (int j = 0; j < wfList.size(); ++j){

        cv::Mat resizedMask = wfList[j]->workMask.clone();
        resizedImage = wfList[j]->data;
        if (resizedMask.cols != rcols || resizedMask.rows != rrows){
            cv::resize(wfList[j]->workMask,resizedMask,Size(rrows,rcols));
            cv::resize(wfList[j]->data, resizedImage, Size(rrows,rcols));
        }
        cv::bitwise_and(mask, resizedMask, mask);

        sum  += resizedImage;
    }

    cv::Mat masked;
    sum = sum/wfList.size();


    wavefront *wf = new wavefront();
    *wf = *wfList[sizes[maxkey][0]];
    wf->data = sum.clone();
    wf->mask = mask;
    wf->workMask = mask.clone();
    m_wavefronts << wf;
    wf->wasSmoothed = false;
    wf->name = "Average.wft";
    wf->dirtyZerns = true;
    m_surfaceTools->addWaveFront(wf->name);
    m_currentNdx = m_wavefronts.size()-1;
    wf->regions.clear();
    makeMask(wf);
    generateSurfacefromWavefront(m_currentNdx);

    if (needsUpdate)
        m_waveFrontTimer->start(1000);
    else
       loadComplete();
    m_surfaceTools->select(m_currentNdx);
    QApplication::restoreOverrideCursor();
}
#include "averagewavefrontfilesdlg.h"
void SurfaceManager::averageComplete(wavefront *wf){
    wf->workMask = wf->mask.clone();
    m_wavefronts << wf;
    wf->wasSmoothed = false;
    wf->name = "Average.wft";
    wf->dirtyZerns = true;
    m_surfaceTools->addWaveFront(wf->name);
    m_currentNdx = m_wavefronts.size()-1;
    //makeMask(m_currentNdx);
    m_surface_finished = false;
    generateSurfacefromWavefront(m_currentNdx);
    m_surfaceTools->select(m_currentNdx);
}

void SurfaceManager::averageWavefrontFiles(QStringList files){
    averageWaveFrontFilesDlg dlg(files, this);
    connect(&dlg, SIGNAL(averageComplete(wavefront*)), this, SLOT(averageComplete(wavefront *)));
    if (dlg.exec()){

    }

}

cv::Mat rotate(wavefront * wf, double ang){
    double rad = ang * M_PI/180.;

    double sina = sin(rad);
    if (fabs(sina) < .00001) sina = 0.;

    double cosa = cos(rad);
    if (fabs(cosa) < .00001)
        cosa = 0.;
    double xcen = wf->m_outside.m_center.x();
    double ycen = wf->m_outside.m_center.y();

    cv::Mat rotated = cv::Mat::zeros(wf->data.size(), wf->data.type());

    for (int y = 0; y < wf->data.rows; ++y)
    {

        for (int x = 0; x < wf->data.cols; ++x)
        {
            double sx = (double)x  - xcen;
            double sy = (double)y  - ycen;
            double x1 = sx * cosa - sy * sina + xcen;
            double y1 = sx * sina + sy * cosa + ycen;
            if (wf->mask.at<uchar>(y,x) != 0)
            {
                double v = bilinear(wf->data,wf->mask, x1,y1);
                if (v == 0.){
                    qDebug() <<"(v == 0)" << x1 << y1 << x << y << wf->data.at<double>(y1,x1);
                }
                rotated.at<double>(y,x) = v;
            }
        }
    }
    return rotated;
}

void SurfaceManager::rotateThese(double angle, QList<int> list){
    workToDo = list.size();
    workProgress = 0;
    pd->setLabelText("Rotating Wavefronts");
    pd->setRange(0, list.size());
    for (int i = 0; i < list.size(); ++i) {
        wavefront *oldWf = m_wavefronts[list[i]];
        QString newName;
        QStringList l = oldWf->name.split('.');
        newName.sprintf("%s_%s%05.1lf",l[0].toStdString().c_str(), (angle >= 0) ? "CW":"CCW", fabs(angle) );
        newName += ".wft";
        wavefront *wf = new wavefront();
        *wf = *m_wavefronts[list[0]];
        //emit nameChanged(wf->name, newName);

        wf->name = newName;
        cv::Mat tmp;
        m_wavefronts << wf;
        m_surfaceTools->addWaveFront(wf->name);
        m_currentNdx = m_wavefronts.size()-1;
        m_surfaceTools->select(m_currentNdx);
        //wf->mask = cv::Mat::zeros(wf->data.size(), CV_8UC1);
        uchar ones = 0xff;
        //fillCircle(wf->mask, wf->m_outside.m_center.x(), wf->m_outside.m_center.y(),
                   //wf->m_outside.m_radius, &ones);

        double rad = -angle * M_PI/180.;
//        cv::Mat t = cv::Mat::zeros(wf->data.size(), CV_8UC1);
//        double xsum = 0;
//        double ysum = 0;
//        int cnt = 0;
//        for (int y = 0; y < t.rows; ++y){
//            for (int x = 0; x < t.cols; ++x){
//                if (wf->data.at<double>(y,x) == 0.){
//                    t.at<uchar>(y,x) = 255;
//                    xsum += x;
//                    ysum += y;
//                    ++cnt;
//                }
//            }
//        }
//        qDebug() << "center issue"<< xsum/cnt << ysum/cnt;
//        cv::imshow("ttt", t);
//        cv::waitKey(1);
        cv::Mat rotated = rotate(wf, angle);

        wf->data = rotated.clone();

        double sina = sin(rad);
        double cosa = cos(rad);

        double sx = wf->m_inside.m_center.x() - wf->m_outside.m_center.x();
        double sy = wf->m_inside.m_center.y() - wf->m_outside.m_center.y();

        wf->m_inside.m_center.rx() = sx * cosa - sy * sina + wf->m_outside.m_center.x();
        wf->m_inside.m_center.ry() = sx * sina + sy * cosa + wf->m_outside.m_center.y();

        makeMask(m_currentNdx);
        wf->dirtyZerns = true;
        wf->wasSmoothed = false;
        m_surface_finished = false;
        generateSurfacefromWavefront(m_currentNdx);
    }
    loadComplete();
}

void SurfaceManager::subtract(wavefront *wf1, wavefront *wf2, bool use_null){

    int size1 = wf1->data.rows * wf1->data.cols;
    int size2 = wf2->data.rows * wf2->data.cols;
    cv::Mat resize = wf2->data.clone();
    cv::Mat mask = wf2->mask.clone();
    if (size1 != size2) {
        cv::resize(resize,resize, cv::Size(wf1->data.cols, wf1->data.rows));
        cv::resize(mask, mask, cv::Size(wf1->data.cols, wf1->data.rows));
    }

    cv::bitwise_and(mask, wf1->mask, mask);
    cv::Mat masked;
    cv::Mat result = wf1->data - resize;

    //result.copyTo(masked, mask);
    wavefront *resultwf = new wavefront;
    *resultwf = *wf1;
    resultwf->data = result.clone();
    resultwf->mask = mask.clone();
    resultwf->workMask = mask.clone();
    m_wavefronts << resultwf;
    m_currentNdx = m_wavefronts.size() -1;

    QStringList n1 = wf1->name.split("/");
    QStringList n2 = wf2->name.split("/");
    resultwf->name = n1[n1.size()-1] + "-" + n2[n2.size()-1];
    m_surfaceTools->addWaveFront(resultwf->name);
    resultwf->dirtyZerns = true;
    resultwf->wasSmoothed = false;

    m_surface_finished = false;
    if (!use_null){
        resultwf->useSANull = false;
    }
    generateSurfacefromWavefront(m_currentNdx);

    loadComplete();
    m_surfaceTools->select(m_currentNdx);
}

void SurfaceManager::subtractWavefronts(){
    QList<QString> list;
    for (int i = 0; i < m_wavefronts.size(); ++i){
        list.append(m_wavefronts[i]->name);
    }
    subtractWavefronatsDlg dlg(list);
    if (dlg.exec()){
        int ndx2 = dlg.getSelected();
        if (ndx2 == -1){
            QMessageBox::warning(0,"Warning", "No wavefront was selected.");
            return;
        }
        wavefront *wf1 = m_wavefronts[m_currentNdx];
        wavefront *wf2 = m_wavefronts[ndx2];
        subtract(wf1,wf2, dlg.use_null);
    }
}

void SurfaceManager::transfrom(QList<int> list){
    RotationDlg dlg(list);
    connect(&dlg, SIGNAL(rotateTheseSig(double, QList<int>)), this, SLOT(rotateThese( double, QList<int>)));
    dlg.exec();

}
void SurfaceManager::invert(QList<int> list){
    workToDo = list.size();
    workProgress = 0;
    pd->setLabelText("Inverting Wavefronts");
    pd->setRange(0, list.size());
    for (int i = 0; i < list.size(); ++i) {
        m_wavefronts[list[i]]->data *= -1;
        m_wavefronts[list[i]]->dirtyZerns = true;
        m_wavefronts[list[i]]->wasSmoothed = false;
        m_ignoreInverse = true;

    }
    m_waveFrontTimer->start(500);
}

void SurfaceManager::filter(QList<int> list){
    wavefrontFilterDlg dlg;
    dlg.setRemoveFileMode();
    connect(&dlg, SIGNAL(waveWasSelected(QString)),this, SLOT(wavefrontDClicked(QString)));

    for (int ndx= 0; ndx < m_wavefronts.size(); ++ndx) {
        wavefront *wf = m_wavefronts[ndx];
        QStringList paths = wf->name.split("/");
        QStringList tmp;
        if (paths.size() > 1)
            tmp = paths.mid(paths.size()-2,-1);
        else
            tmp = paths;

        dlg.addRMSValue(tmp.join("/"), QPointF(ndx,wf->std));
        dlg.addAstigValue(tmp.join("/"), QPointF(wf->InputZerns[4], wf->InputZerns[5]));
    }

    dlg.plot();
    if (dlg.exec()){
        for (int i = m_wavefronts.size()-1; i >= 0; --i){
            wavefront *wf = m_wavefronts[i];
            if (wf->std > dlg.rms()){
                if (dlg.shouldFilterFile()){
                    QFile file (wf->name);
                    file.remove();
                }
                if (dlg.shouldFilterWavefront()){
                    m_currentNdx = i;
                    deleteCurrent();
                }
            }
        }

    }
}

#include "wftexaminer.h"
wftExaminer *wex;
double wrapAngle(double angle){
    if (angle < -360){
        angle += 360;
    }
    if (angle >= 360)
        angle -= 360;
    return angle;
}
void SurfaceManager::inspectWavefront(){
    wex = new wftExaminer(m_wavefronts[m_currentNdx]);
    wex->show();
}


#include <sstream>

#include <QMap>

void plotlineThruAstigs(QwtPlot *plt, cv::Mat xastig, cv::Mat yastig, QList<rotationDef *>list){

  QList<rotationDef *> tmplist = list;
  QList<QPointF> values;
  for (int i = 0; i < list.size(); ++i){
      values << QPointF(xastig.at<double>(i,0), yastig.at<double>(i,0));
  }
    while (tmplist.size()){

        double angle = tmplist.front()->angle;
        double x1 = values.front().x();
        double y1 = values.front().y();
        for (int j = 1; j < tmplist.size(); ++j){
            if (fabs(angle - tmplist[j]->angle) == 90 || fabs(tmplist[j]->angle - angle) == 90){
                double x2 = values[j].x();
                double y2 = values[j].y();
                QwtPlotCurve *curve = new QwtPlotCurve();
                curve->setItemAttribute(QwtPlotItem::Legend, false);
                QPolygonF line;

                line << QPointF(x1,y1);
                line << QPointF(x2,y2);
                curve->setSamples(line);
                curve->attach(plt);
                tmplist.removeAt(j);
                values.removeAt(j);
                break;
            }
        }
        tmplist.removeFirst();
        values.removeFirst();
    }

}

// calculate stand astig for each input
// for each input rotate the average by the input angle and subtract it from the input
// plot the astig of each of the inputs which will be the stand only astig.
// on input list is the list of wavefront files and thier rotation angle.
//     inputs are the actual wavefronts at original rotations.
//     avgNdx is the index in the m_wavefronts list of the average with stand removed.
textres SurfaceManager::Phase2(QList<rotationDef *> list, QList<wavefront *> inputs, int avgNdx ){
    QTextEdit *editor = new QTextEdit;

    QTextDocument *doc = editor->document();
    textres results;
    results.Edit = editor;
    const int Width = 400 * .9;
    const int Height = 300 * .9;
    QImage contour(Width ,Height, QImage::Format_ARGB32 );

    QPrinter printer(QPrinter::HighResolution);
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( true );
    printer.setOutputFileName( "stand.pdf" );
    printer.setOutputFormat( QPrinter::PdfFormat );
    //printer.setResolution(85);
    printer.setPaperSize(QPrinter::A4);

    QPainter painterPDF( &printer );
    cv::Mat xastig = cv::Mat::zeros(list.size(), 1, numType);
    cv::Mat yastig = cv::Mat::zeros(list.size(), 1, numType);
    cv::Mat standxastig = cv::Mat::zeros(list.size(), 1, numType);
    cv::Mat standyastig = cv::Mat::zeros(list.size(), 1, numType);
    cv::Mat standastig = cv::Mat::zeros(list.size(), 1, numType);
    QVector<cv::Mat> standwfs;
    QVector<double> astigMag;
    editor->resize(printer.pageRect().size());
    doc->setPageSize(printer.pageRect().size());
    cv::Mat standavg = cv::Mat::zeros(inputs[0]->workData.size(), numType);
    cv::Mat standavgZernMat = cv::Mat::zeros(inputs[0]->workData.size(), numType);
    double mirrorXaverage = 0;
    double mirrorYAverage = 0;
    double maxXastig = 0;
    double maxYastig = 0;
    m_ignoreInverse = true;
    for (int i = 0; i < list.size(); ++i){
        // rotate the average to match the input
        QList<int> toRotate;
        toRotate.append(avgNdx);
        int ndx = m_wavefronts.size(); // index of result of rotation
        m_surface_finished = false;

        rotateThese(wrapAngle(-list[i]->angle),toRotate);  // use neg angle to rotate back to original input
        while(!m_surface_finished){qApp->processEvents();}
        m_surface_finished = false;
        subtract(inputs[i], m_wavefronts[ndx],false);
        ++ndx;      // now ndx point to the stand only wavefront
        while(!m_surface_finished){qApp->processEvents();}
        cv::Mat resized = m_wavefronts[ndx]->workData.clone();
        if (standavg.cols != m_wavefronts[ndx]->workData.cols || standavg.rows != m_wavefronts[ndx]->workData.rows){
            cv::resize(m_wavefronts[ndx]->workData, resized, Size(standavg.cols, standavg.rows));
        }
        standavg += resized;
        //create contour of astig
        double xa = standxastig.at<double>(i,0) = m_wavefronts[ndx]->InputZerns[4];

        double ya = standyastig.at<double>(i,0) = m_wavefronts[ndx]->InputZerns[5];

        double mag = sqrt(xa * xa + ya * ya);
        standastig.at<double>(i,0) = mag;
        astigMag.push_back(sqrt(mag));
        QVector<int> zernsToUse;
        zernsToUse << 4 << 5;
        for (int ii = 9; ii < Z_TERMS; ++ii)
            zernsToUse << ii;

        cv::Mat m = computeWaveFrontFromZernikes(inputs[0]->data.cols,inputs[0]->data.rows,
                m_wavefronts[ndx]->InputZerns, zernsToUse );
        standavgZernMat += m;
        standwfs << m;
        double mirrorX = inputs[i]->InputZerns[4];
        double mirrorY = inputs[i]->InputZerns[5];
        xastig.at<double>(i,0) = mirrorX;
        mirrorXaverage += mirrorX;
        maxXastig = max(maxXastig,mirrorX);
        mirrorYAverage += mirrorY;
        maxYastig = max(maxYastig, mirrorY);
        yastig.at<double>(i,0) = mirrorY;
        qDebug() << "Mirror astigs " << mirrorX << mirrorY;
    }

    Circle fittedcircle1;
    Circle fittedcircle2;
    Circle fittedcircle;
    if (xastig.rows > 2){
        CircleData d(xastig.rows, xastig.ptr<double>(0), yastig.ptr<double>(0));
        fittedcircle1 = CircleFitByHyper(d);
    }


        double xmean = 0;
        double ymean = 0;
        for (int i = 0; i < xastig.rows; ++i){
            xmean += xastig.at<double>(i,0);
            ymean += yastig.at<double>(i,0);
        }
        double avgRadius = 0;
        for (int i = 0; i < list.size(); ++i){
            double xdel = xastig.at<double>(i,0) - xmean/xastig.rows;
            double ydel = yastig.at<double>(i,0) - ymean/xastig.rows;

            avgRadius += sqrt(xdel * xdel + ydel * ydel);
        }
        fittedcircle2 = Circle(xmean/xastig.rows, ymean/xastig.rows, avgRadius/xastig.rows );
        fittedcircle = fittedcircle2;
qDebug() << "circle fit"<< avgRadius << fittedcircle1.r << fittedcircle2.r;
    fittedcircle = fittedcircle1;
    if (xastig.rows == 2 || fittedcircle1.r > 1.5 * fittedcircle2.r ){
        fittedcircle = fittedcircle2;
    }


    double smin,smax;
    cv::minMaxIdx(standavg,&smin, &smax);

    standavg /= list.size();

    cv::minMaxIdx(standavgZernMat,&smin, &smax);

    standavgZernMat = standavgZernMat/list.size();
    cv::minMaxIdx(standavgZernMat,&smin, &smax);

    cv::Scalar standMean,standStd;
    cv::Scalar standXMean,standXStd;
    cv::Scalar standYMean, standyStd;
    cv::meanStdDev(standastig,standMean,standStd);
    cv::meanStdDev(standxastig,standXMean,standXStd);
    cv::meanStdDev(standyastig,standYMean,standyStd);

    double ymin,standymin, ymax,standymax;
    double xmax,standxmax, xmin,standxmin;

    minMaxIdx(standxastig, &standxmin,&standxmax);
    minMaxIdx(standyastig, &standymin,&standymax);
    minMaxIdx(xastig, &xmin,&xmax);
    minMaxIdx(yastig, &ymin,&ymax);

    QwtPlot *pl1 = new QwtPlot();
    pl1->setCanvasBackground(QColor(240,240,240));
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(pl1);
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMajorPen(Qt::darkGray, 1);
    grid->setMinorPen(Qt::gray, 1,Qt::DotLine);

    QVector<QwtPlot *> astigPlots;

    QwtPlotRenderer renderer;
    //renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend);
    QPainter painter( &contour );

    QString html = "<html><head/><body><h1>Test Stand Astig Analysis</h1>"
            "<h3>Stand induced astig at each rotation position:</h3>";
    html.append( "<table  style='ds margin-top:0px; margin-bottom:0px; margin-left:10px; margin-right:10px;'"
                 " width='70%' cellspacing='1' cellpadding='1'>"

                 "<tr><b><td>rotation angle</td><td>X astig</td><td>Y astig</td><td> magnitude</td><td>astig angle Degrees</td></b></tr>");
    double maxX = -1000;
    double minX =1000.;
    double maxY = -1000;
    double minY = 1000.;
    for (int i = 0; i < list.size(); ++i){
        double xval = standxastig.at<double>(i,0);
        double yval = standyastig.at<double>(i,0);
        maxX = max(maxX, xval);
        minX = min(minX, xval);
        maxY = max(maxY, yval);
        minY = min(minY, yval);

        double mag = sqrt(pow(xval,2) + pow(yval,2));



        html.append("<tr><td><align = 'right'> " + QString().number(list[i]->angle,'f',3 ) + "</td>" );
        html.append("<td>" + QString().number(xval,'f',3) + "</td>"
                    "<td>" + QString().number(yval,'f',3) + "</td>"
                    "<td>" + QString().number(mag,'f',3) + "</td>"
                    "<td>" + QString().number((180./ M_PI) * atan2(yval,xval)/2.,'f',1) + "</td></right></tr>");

    }
    html.append("<tr></b><td>MEAN</b></td><td>" + QString().number(fittedcircle.a,'f',3) + "</td><td>" +
                QString().number(fittedcircle.b,'f',3) + "</td><td>" + QString().number(sqrt(fittedcircle.a * fittedcircle.a + fittedcircle.b * fittedcircle.b),'f',3) + "</td></tr>");
    html.append("<tr><b><td><b>STD</b></td><td><ALIGN ='right'>" + QString().number(standXStd[0],'f',3) + "</td><td>" +
                QString().number(standyStd[0],'f',3) + "</td><td>" + QString().number(standStd[0],'f',3) + "</td></tr>");
    html.append("</table><br><p>The Table above shows the computed stand astig at each rotation.<br>"
                "If the variation for the astig values is small then "
                "The stand removal was good.  Idealy the STD (standard deviation) should be"
                " less than .1 which means less than .1 wave pv on the surface of the mirror"
                "</p>");


    int cnt = 0;
    QString imagesHtml = "<table  style='ds margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;'"
                 " 'width='100%' cellspacing='2' cellpadding='0'>";

    mirrorXaverage /= list.size();
    mirrorYAverage /= list.size();
    QPolygonF mirrorAstigAtEachRotation;
    double mirrorAstigRadius =0;
    plotlineThruAstigs(pl1, xastig, yastig, list);
    for (int i = 0; i < list.size(); ++i){
        if (cnt++ == 0)
            imagesHtml.append("<tr>");
        // make plot of stand astig
        QwtPlotCurve *curve = new QwtPlotCurve(QString().number(list[i]->angle));

        QColor color(Qt::GlobalColor( 7 + i%13 ) );
        QPen pen(color);
        curve->setPen(pen);
        curve->attach(pl1);
        QwtPlotMarker *standPoint = new QwtPlotMarker();
        standPoint->setSymbol(new QwtSymbol(QwtSymbol::Cross, color,color, QSize(11,11)));
        standPoint->setValue(QPointF(standxastig.at<double>(i,0), standyastig.at<double>(i,0)));
        standPoint->attach(pl1);
        QwtPlotMarker *m = new QwtPlotMarker();
        m->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, color,color, QSize(7,7)));
        mirrorAstigAtEachRotation << QPointF(inputs[i]->InputZerns[4],inputs[i]->InputZerns[5]);
        double xdel = inputs[i]->InputZerns[4] - mirrorXaverage;
        double ydel = inputs[i]->InputZerns[5] - mirrorYAverage;
        double r = sqrt(xdel * xdel + ydel * ydel);
        mirrorAstigRadius += r;
        qDebug() << "radius" << r << inputs[i]->InputZerns[4] << inputs[i]->InputZerns[5];
        m->setYValue(inputs[i]->InputZerns[5]);
        m->setXValue(inputs[i]->InputZerns[4]);
        m->attach(pl1);

        // make contour plots of astig zernike terms of stand
        ContourPlot *cp = new ContourPlot();
        cp->m_zRangeMode = "Fractions of Wave";
        cp->contourWaveRangeChanged(inputs[0]->std* 3);
        wavefront * wf = new wavefront(*inputs[i]);


        wf->data = wf->workData = standwfs[i];
        cv::resize(inputs[i]->workMask, wf->mask, cv::Size(wf->data.cols, wf->data.rows));
        wf->workMask = wf->mask;

        cv::minMaxIdx(wf->data,&smin, &smax);

        cv::Scalar mean,std;
        cv::meanStdDev(wf->data,mean,std);
        wf->std = std[0];
        wf->useSANull = false;
        wf->min = smin;
        wf->max =  smax;
        wf->name = QString().sprintf("%06.2lf",list[i]->angle);

        cp->setSurface(wf);
        cp->resize(Width,Height);
        cp->replot();
        contour.fill( QColor( Qt::white ).rgb() );
        renderer.render( cp, &painter, QRect(0,0,Width,Height) );
        QString imageName = QString().sprintf("mydata://zern%s.png",wf->name.toStdString().c_str());
        imageName.replace("-","CCW");
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour));
        results.res.append (imageName);
        imagesHtml.append("<td><p> <img src='" +imageName + "' /></p></td>");
        if (cnt == 2){
            cnt = 0;
            imagesHtml.append("</tr>");
        }
    }

    mirrorAstigRadius /= list.size();
    imagesHtml.append("</table><p>");


    //display average of all stand zernwavefronts
    wavefront * wf2 = new wavefront(*inputs[0]);
    wf2->data = wf2->workData = standavgZernMat ;
    cv::resize(inputs[0]->mask,wf2->mask, cv::Size(wf2->data.cols, wf2->data.rows));
    wf2->workMask = wf2->mask;
    cv::Scalar mean,std;
    cv::meanStdDev(wf2->data,mean,std);
    wf2->std = std[0];
    double mmin;
    double mmax;

    minMaxIdx(wf2->data, &mmin,&mmax);
    wf2->min = mmin;
    wf2->max = mmax;
    //wf2->workMask = m_wavefronts[0]->workMask.clone();
    wf2->name = QString("Average Stand zernike based");
    ContourPlot *cp1 = new ContourPlot();
    //cp1->m_zRangeMode = "Min/Max";
    cp1->setSurface(wf2);
    cp1->resize(Width, Height);
    cp1->replot();
    QImage contour2(500, 500, QImage::Format_ARGB32 );
    contour2.fill( QColor( Qt::white ).rgb() );
    QPainter painter2( &contour2 );

    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend, false);
    renderer.render( cp1, &painter2, QRect(0,0,Width,Height) );
    QString imageName = "mydata://StandCotourZerns.png";
    doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour2));

    wf2->data = wf2->workData = standavg;
    wf2->useSANull = false;
    wf2->name = QString("Average Stand effects.");
    wf2->dirtyZerns = true;
    wf2->wasSmoothed = false;
    makeMask(wf2);
    generateSurfacefromWavefront(wf2);
    m_ignoreInverse = false;
    m_standAvg = wf2;
    cp1->setSurface(wf2);
    cp1->resize(Width,Height);
    cp1->replot();

    contour2.fill( QColor( Qt::white ).rgb() );
    renderer.render( cp1, &painter2, QRect(0,0,Width,Height) );
    cp1->m_zRangeMode = "Auto"; // restore contour plot to auto scaling.
    imageName = QString("mydata://StandCotourMat.png");
    doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour2));



    pl1->insertLegend( new QwtLegend() , QwtPlot::TopLegend);
    pl1->setAxisTitle( QwtPlot::yLeft, "Y astig" );
    pl1->setAxisTitle(QwtPlot::xBottom, "X astig");
    //  ...a vertical line at 0...
    QwtPlotMarker *muY = new QwtPlotMarker();
    muY->setLineStyle( QwtPlotMarker::VLine );
    muY->setLinePen( Qt::black, 0, Qt::DashDotLine );
    muY->setXValue( 0.);
    muY->attach( pl1);
    //  ...a horizontal line at 0...
    QwtPlotMarker *mux = new QwtPlotMarker();
    mux->setLineStyle( QwtPlotMarker::HLine );
    mux->setLinePen( Qt::black, 0, Qt::DashDotLine );

    mux->setYValue( 0.);
    mux->attach( pl1);
    // plot the mean
    QwtPlotMarker *meanMark = new QwtPlotMarker();
    meanMark->setSymbol(new QwtSymbol(QwtSymbol::Star1,QColor(255,255,255), QColor(0,0,0), QSize(15,15)));
    meanMark->setLabel(QString("stand Mean astig") );

    meanMark->setLabelAlignment( Qt::AlignTop );

    meanMark->attach(pl1);
    double mirrorXastig = 0;
    double mirrorYastig = 0;
    int count = mirrorAstigAtEachRotation.size();
    for (int i = 0; i < count; ++i){
        mirrorXastig += mirrorAstigAtEachRotation[i].rx();
        mirrorYastig += mirrorAstigAtEachRotation[i].ry();
    }
    mirrorXastig /= count;
    mirrorYastig /= count;

    //QwtPlotMarker *mirrorMeanMark = new QwtPlotMarker();
    //mirrorMeanMark->setValue(QPointF(bestFitAstig.a, bestFitAstig.b));
    //mirrorMeanMark->setSymbol(new QwtSymbol(QwtSymbol::Triangle,QColor(0,255,0,40), QColor(0,0,0), QSize(30,30)));
    //mirrorMeanMark->attach(pl1);

    QPolygonF stdCircle;
    double SE = standStd[0]/sqrt(standastig.rows);
    for (double rho = 0; rho <= 2 * M_PI; rho += M_PI/32.){
        stdCircle << QPointF(standXMean[0]+SE * cos(rho),standYMean[0] + SE * sin(rho));
    }

    QwtPlotCurve *curveStandStd = new QwtPlotCurve("Standard Error");
    curveStandStd->setPen(Qt::darkYellow,3,Qt::DotLine );
    curveStandStd->setSamples(stdCircle);
    //curveStandStd->attach(pl1);

    // draw average astig radius circule around points.
    meanMark->setValue(wf2->InputZerns[4], wf2->InputZerns[5]);
    QwtPlotCurve *curveAvgMirror = new QwtPlotCurve("Mirror Astig Circle");
    stdCircle.clear();
    for (double rho = 0; rho <= 2 * M_PI; rho += M_PI/32.){
        //stdCircle << QPointF(mirrorXastig+mirrorAstigRadius * cos(rho), mirrorYastig + mirrorAstigRadius * sin(rho));
        stdCircle << QPointF(fittedcircle.a+fittedcircle.r * cos(rho), fittedcircle.b + fittedcircle.r * sin(rho));

    }

    double maxv = max(fabs(mirrorYastig) +  fabs(mirrorAstigRadius),fabs(mirrorXastig ) + fabs(mirrorAstigRadius))  * 1.2;
    maxv = max(maxX, maxv);
    maxv = max(fabs(minX), maxv);
    maxv = max(maxv, maxY);
    maxv = max(fabs(minY), maxv);
    pl1->setAxisScale(QwtPlot::xBottom, -maxv, maxv);
    pl1->setAxisScale(QwtPlot::yLeft,   -maxv, maxv);
    QColor color(Qt::green);
    QPen pen(color,3);
    curveAvgMirror->setPen(pen);
    curveAvgMirror->setSamples(stdCircle);
    curveAvgMirror->attach(pl1);
    pl1->resize(500,500);
    pl1->replot();
    QwtPlotRenderer renderer3;
    QImage contour3(500, 500, QImage::Format_ARGB32 );
    contour3.fill( QColor( Qt::white ).rgb() );
    QPainter painter3( &contour3 );

    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend, false);
    renderer.render( pl1, &painter3, QRect(0,0,500,500) );
    imageName = QString("mydata://plot.png");
    doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour3));
    results.res.append (imageName);
    html.append("<p> <img src='" + imageName + "' /></p>");
    html.append("<p font-size:12pt> The plot above shows the astig of each input file plotted as colored dots."
                "The large green circle is the average distance all these points are from the center."
                "For it to be meaningful you should have pairs of rotations at both 0-90 and 45-135."
                "That is 90 deg pairs for some angles at 45 deg of 0 deg.<br>"
                " In a perfect world each dot would have be on the circle. "
                " If any dots are far from the circle look for a problem at that rotation angle.</p>"
               "<p font-size:12pt>It shows the stand only astig ploted as plus signs.<br>"
               "The mean value of test stand only astig is plotted as a large star. It and the center of the green circle should be the same.<br>"
                "There is a circle around the mean value that is the standard error of the mean. Its diameter represents the "
                "variabilty of the mean.</p>"
                "<p font-size:12pt>If the variation for the astig values is small then "
                "The stand removal was good.  Idealy the STD (standard deviation) should be"
                " less than .1 which means less than .1 wave pv on the surface of the mirror"
                "<br>The colored plus signs are what is calculated for test stand induced astig."
                "at each rotation angle.</p><br>"


                "<table><tr><td><img src='mydata://StandCotourZerns.png' /></td><td><img src='mydata://StandCotourMat.png' /></td></tr></table>"
                "<p font-size:12pt>The contours above are the average system induced forces derived from the average of all rotations.<br>"
                "The left contour is based on the zernike values and the contour on the right is based on the wavefront.</p>"
                "<p font-size:12pt>The contour plots below are of what is beleived to be test stand only induced errors at each rotation. "
                "Check that they are similar at each rotation.  If not then maybe "
                "stand (system) induced error is not same at each rotation then the stand removal is not reliable. "
                "However it is unlikely that they will all look exactly the same.</p>");


    html.append(imagesHtml);
    html.append("</body></html>");
    editor->setHtml(html);
    return results;
}


void SurfaceManager::computeStandAstig(define_input *wizPage, QList<rotationDef *> list){
    // check for pairs
    QVector<rotationDef*> lookat = list.toVector();

    while (lookat.size()){
        for (int i = 0; i < lookat.size(); ++i){
            double angle1 = wrapAngle(lookat[i]->angle);

            double found = false;
            for (int j = i+1; j < lookat.size(); ++j){
                double angle2 = wrapAngle(lookat[j]->angle);

                if (angle2 == wrapAngle(angle1 -90) || angle2 == wrapAngle(angle1 + 90))
                {
                    found = true;
                    lookat.remove(j);
                    lookat.remove(i);
                    found = true;
                    break;
                }
            }

            if (!found){
                if (QMessageBox::Yes ==
                  QMessageBox::question(0, tr("Error"),
                                     QString().sprintf("No 90 deg pair for %s and angle %6.2lf",
                                                       lookat[i]->fname.toStdString().c_str(),
                                                       lookat[i]->angle))){
                        wizPage->runpb->setText("compute");
                        wizPage->runpb->setEnabled(true);
                        return;
                }
            lookat.remove(i);
            }
            break;
        }
    }
        QApplication::setOverrideCursor(Qt::WaitCursor);
    QPrinter printer(QPrinter::HighResolution);
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( true );
    printer.setOutputFileName( "stand.pdf" );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setResolution(85);
    printer.setPaperSize(QPrinter::A4);


    QTextEdit *editor = new QTextEdit;
    editor->resize(printer.pageRect().size());
    const int contourWidth = 2 * 340/3;
    const int contourHeight = 2 * 360/3;
    QImage contour = QImage(contourWidth ,contourHeight, QImage::Format_ARGB32 );


    QwtPlotRenderer renderer;
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend);
    ContourPlot *plot =new ContourPlot(0,0,true);//m_contourPlot;
    plot->m_minimal = true;
    QList<int> unrotatedNdxs;
    QList<int> rotatedNdxs;


    QString html = ("<html><head/><body><h1><center>Test Stand Astig Removal</center></h1>"
                    "<h2><center>" + AstigReportTitle);
           html.append("    <font color='grey'>" + QDate::currentDate().toString() +
                       " " +QTime::currentTime().toString()+"</font></center><h2>");
           html.append("<h3>Step 1. Counter rotate input files results:</h3>"
                       "Check that all the counter rotated images appear to be oriented the same way."
                       "If the stand astig is equal to or larger than the mirror astig they may not appear to be oriented the same way."
                       "But continue anyway and look for other features on the surface that rotated if possible."
            "<table border='1'style='ds margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;'"
            " 'width='100%' cellspacing='2' cellpadding='0'>");
    html.append("<tr><td><p align='center'><b>Unrotated inputs</b></p></td>");
    html.append("<td><p align='center'><b> Counter Rotated </b></p></td></tr>");


    QTextDocument *doc = editor->document();
    QList<QString> doc1Res;
    doc->setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
    QList<wavefront *> rotated;
    QList<wavefront *> inputs;
    editor->append("<tr>");
    int startingNdx = m_wavefronts.size() -1;

    for (int i = 0; i < list.size(); ++i){
        contour.fill( QColor( Qt::white ).rgb() );
        QPainter painter( &contour );
        int ndx = m_wavefronts.size();

        // get input file
        QStringList loadList;
        loadList << list[i]->fname;
        loadWavefront(list[i]->fname);
        wavefront * wf = m_wavefronts[m_currentNdx];
        inputs.append(wf);
        unrotatedNdxs.append(m_currentNdx);
        plot->setSurface(wf);
        plot->replot();
        renderer.render( plot, &painter, QRect(0,0,contourWidth,contourHeight) );

        QString imageName = QString().sprintf("mydata://%s.png",list[i]->fname.toStdString().c_str());
        QString angle = QString().sprintf("%6.2lf Deg",-list[i]->angle);
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour));
        doc1Res.append(imageName);
        html.append("<tr><td><p align='center'> <img src='" +imageName + "' /><br><b>" + angle + "</b></p></td>");

        // counter rotate it
        wizPage->runpb->setText(QString("Counter Rotating ") + list[i]->fname.right(list[i]->fname.size() - list[i]->fname.lastIndexOf("/")-1));
        QList<int> l;
        l.append(ndx);
        ndx = m_wavefronts.size();


        qDebug() << "Counter rotating " << list[i]->fname << " angle " << list[i]->angle;
        rotateThese(wrapAngle(list[i]->angle),l);
        rotated.append(m_wavefronts[ndx]);
        wf = m_wavefronts[ndx];

        loadComplete();
        plot->setSurface(wf);
        plot->replot();

        contour.fill( QColor( Qt::white ).rgb() );
        renderer.render( plot, &painter, QRect(0,0,contourWidth,contourHeight) );
        angle = QString().sprintf("%6.2lf Deg",-list[i]->angle);
        imageName = QString().sprintf("mydata://CR%s%s.png",list[i]->fname.toStdString().c_str(),angle.toStdString().c_str());

        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour));
        doc1Res.append(imageName);
        html.append("<td><p align='center'> <img src='" +imageName + "' /><br><b>Counter " + angle + "</b></p></td>");
        html.append("</tr>");
    }
    html.append("</table></body></html>");

    editor->setHtml(html);
    editor->setDocument(doc);

    //editor->show();

    // Now average all the rotated ones.
    QList<wavefront *> wlist;
    for (int i = 0; i < rotated.size(); ++i){
        wlist << rotated[i];
    }


    average(wlist);

    loadComplete();
    // now we have the average of all the counter rotated inputs.  That is the average with stand removed (hopefully.)
    m_standRemoved = m_wavefronts[m_currentNdx];
    emit nameChanged(m_wavefronts[m_currentNdx]->name, QString("AverageStandRemoved"));
    m_standRemoved->name = QString("AverageStandRemoved");
    int avgNdx = m_currentNdx;  // the index of the average

    QTextEdit *page2 = new QTextEdit;
    page2->resize(600,800);
    QTextDocument *doc2 = page2->document();
    QList<QString> doc2Res;
    html = ("<html><head/><body><h1>Test Stand Astig Removal</h1>"
            "<h3>Step 2. Averaged surface with stand induced terms removed:</h3>");
    ContourPlot *plotAvg =new ContourPlot(0,0,false);//m_contourPlot;

    plotAvg->setSurface(m_wavefronts[m_currentNdx]);
    contour = QImage(550,450, QImage::Format_ARGB32 );
    contour.fill( QColor( Qt::white ).rgb() );
    QPainter painter( &contour );

    renderer.render( plotAvg, &painter, QRect(0,0,550,450) );

    QString imageName = "mydata://AvgAstigremoved.png";
    doc2->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour));
    doc2Res.append(imageName);
    html.append("<p> <img src='" +imageName + "' /></p>");
    html.append("</body></html>");
    page2->setDocument(doc2);
    page2->setHtml(html);
    //page2->show();

    /**************************************************************************************************/
    // PHASE 2
    // calculate stand astig for each input
    // for each input rotate the average by the input angle and subtract it from the input
    // plot the astig of each of the inputs which will be the stand only astig.

    wizPage->runpb->setText("computing stand astigs");
    textres page3res = Phase2(list, inputs, avgNdx);
    QTabWidget *tabw = new QTabWidget();
    tabw->setTabShape(QTabWidget::Triangular);
    tabw->addTab(editor, "Page 1 input analysis");
    tabw->addTab(page2, "Page 2 Stand removed.");
    tabw->addTab(page3res.Edit, "Page 3 stand analysis");
    tabw->resize(800,600);
    tabw->show();

    // build pdf doc from the three textedits
    QTextDocument pdfDoc;
    foreach( QString res, page3res.res){
        pdfDoc.addResource(QTextDocument::ImageResource, res, page3res.Edit->document()->resource(QTextDocument::ImageResource,res));
    }
    pdfDoc.addResource(QTextDocument::ImageResource, QString("mydata://StandCotourZerns.png"),
                       page3res.Edit->document()->resource(QTextDocument::ImageResource,QString("mydata://StandCotourZerns.png") ));
    pdfDoc.addResource(QTextDocument::ImageResource, QString("mydata://StandCotourMat.png"),
                       page3res.Edit->document()->resource(QTextDocument::ImageResource,QString("mydata://StandCotourMat.png") ));
    foreach(QString res, doc1Res){
        pdfDoc.addResource(QTextDocument::ImageResource, res, editor->document()->resource(QTextDocument::ImageResource,res));
    }
    foreach(QString res, doc2Res){
        pdfDoc.addResource(QTextDocument::ImageResource, res, page2->document()->resource(QTextDocument::ImageResource,res));
    }

     printer.setOutputFileName(AstigReportPdfName);
     QTextCursor cursor(&pdfDoc);
     cursor.insertHtml(editor->toHtml());
     QTextBlockFormat blockFormat;
     blockFormat.setPageBreakPolicy(QTextFormat::PageBreak_AlwaysBefore);
     cursor.insertBlock(blockFormat);
     cursor.insertHtml(page2->toHtml());
     cursor.insertBlock(blockFormat);
     cursor.insertHtml(page3res.Edit->toHtml());
     pdfDoc.print(&printer);
     if (!wizPage->showWork->isChecked()){
         //Delete all work wavefronts except the overall average with stand removed.
         QList<int> deleteThese;
         for (int i = m_wavefronts.size() -1; i > startingNdx; --i){
             if (i != avgNdx)
                deleteThese.append(i);
         }
         deleteWaveFronts(deleteThese);
     }
     m_wavefronts << m_standAvg;
     m_surfaceTools->addWaveFront(m_standAvg->name);
     wizPage->runpb->setText("Compute");
     wizPage->runpb->setEnabled(true);

     m_surfaceTools->select(m_currentNdx);
     QApplication::restoreOverrideCursor();
}

void SurfaceManager::computeTestStandAstig(){
    standAstigWizard *wiz = new standAstigWizard(this);
    wiz->show();
}

void SurfaceManager::saveAllContours(){
    QSettings settings;
    QString lastPath = settings.value("lastPath","").toString();
    const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList filter;
    QString imageFilter( tr( "Images" ) );
    imageFilter += " (";
    for ( int i = 0; i < imageFormats.size(); i++ )
    {
        if ( i > 0 )
            imageFilter += " ";
        imageFilter += "*.";
        imageFilter += imageFormats[i];
    }
    imageFilter += ")";

    filter += imageFilter;

    QString fName = QFileDialog::getSaveFileName(0,
    tr("Save stats plot"), lastPath + "//allConturs.jpg",filter.join( ";;" ));
    if (fName.isEmpty())
        return;

    m_allContours.save( fName );
}

#include "showallcontoursdlg.h"
void SurfaceManager::showAllContours(){
    showAllContoursDlg dlg;
    if (!dlg.exec()) {
        return;
    }
    QRect rec = QApplication::desktop()->screenGeometry();
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ContourPlot *plot =new ContourPlot(0,0);//m_contourPlot;
    //plot->m_minimal = true;
    int cols = dlg.getColumns();
    int width = rec.width()/cols;
    int height = width * .82;
    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    QList<int> list = saTools->SelectedWaveFronts();

    int rows =  ceil((double)list.size()/cols);
    int columns = min(list.size(),int(ceil((double)list.size()/rows)));
    const QSizeF size(columns * (width + 10), rows * (height + 10));
    const QRect imageRect = QRect(0,0,size.width(),size.height());
    qDebug() << "save all" << imageRect;
    m_allContours = QImage( imageRect.size(), QImage::Format_ARGB32 );
    m_allContours.fill( QColor( Qt::white ).rgb() );
    QPainter painter( &m_allContours );
    QwtPlotRenderer renderer;
    if (plot->m_do_fill) {
        renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
        renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
    }
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend,false);
    renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales,false );
    for (int i = 0; i < list.size(); ++i)
    {
        wavefront * wf = m_wavefronts[list[i]];
        plot->setSurface(wf);
        plot->replot();
        int y_offset =  height * (i/columns) + 10;
        int x_offset = width * (i%columns) + 10;
        const QRectF topRect( x_offset, y_offset, width, height );
        renderer.render( plot, &painter, topRect );
    }
    painter.end();

    //image.save( "tmp.png" );
    QWidget *w = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;

    QScrollArea *scrollArea = new QScrollArea;

    QLabel *l = new QLabel();
    l->setPixmap(QPixmap::fromImage(m_allContours));
    l->setScaledContents( true );

    l->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    scrollArea->setWidget(l);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setAutoFillBackground(true);
    QPushButton *savePb = new QPushButton("Save as Image",w);

    connect(savePb, SIGNAL(pressed()), this, SLOT(saveAllContours()));
    layout->addWidget(savePb,0,Qt::AlignHCenter);
    layout->addWidget(scrollArea);
    w->setLayout(layout);
    w->setWindowTitle("Contours of all WaveFronts.");

    height = 2 * rec.height()/3;
    width = rec.width();
    w->resize(width,height);
    w->show();
    QApplication::restoreOverrideCursor();
}
void showImage(QImage img, QString title){
    QLabel *myLabel = new QLabel;
    myLabel->setPixmap(QPixmap::fromImage(img.scaled(1000,1000)));
    myLabel->setWindowTitle(title);
    myLabel->show();
}

#include "pdfcalibrationdlg.h"
#include "ui_reportdlg.h"
void SurfaceManager::report(){


    if (m_wavefronts.size() == 0){
        QMessageBox::warning(0, tr(""),
                             tr("No wave front loaded to create a report for."));
        return;
    }


    // The actual PDF printer.
    QPrinter printer(QPrinter::QPrinter::HighResolution);


    //setup printer
    printer.setColorMode( QPrinter::Color );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setPageSize(QPageSize(QPageSize::A4));

    ReportDlg dlg(&printer);

    if (!dlg.exec())
        return;

    int finalWidth =  QGuiApplication::screens()[0]->geometry().width()/2.5;

    printer.setFullPage( true );
    QMargins marg = printer.pageLayout().marginsPixels(printer.resolution());

    printer.setOutputFileName( dlg.fileName );
    int width = printer.width();
    int height = printer.height();

    QRect printer_rect = printer.pageLayout().paintRectPixels(printer.resolution());
    qDebug() << "printer rect" << printer_rect << printer.resolution() << width << height;
    QPainter PDFPainter;//(&printer);  //Paints stuff to pdf printer
    QTextEdit *editor = new QTextEdit;
    //editor->resize(width/2,height/2);


    mirrorDlg *md = mirrorDlg::get_Instance();
    wavefront *wf = m_wavefronts[m_currentNdx];

    metricsDisplay *metrics = metricsDisplay::get_instance();
    QString title("<html><body><table width = '100%'><tr><td></td><td><h1><center>Interferometry Report for " +
                  dlg.title + "</center></td><td>"
                  + QDate::currentDate().toString() +
                  " " +QTime::currentTime().toString()+"<br>DFTFringe Version:"+APP_VERSION+"</td></tr></table>");

    QString Diameter = (md->isEllipse()) ? " Horizontal Axis: " : " Diameter: " +QString().number(md->diameter,'f',1) ;
    QString ROC = (md->isEllipse()) ? "Vertical Axis: " + QString().number(md->m_verticalAxis) : "ROC: " +  QString().number(md->roc,'f',1);
    QString FNumber = (md->isEllipse()) ? "" : "Fnumber: " + QString().number(md->FNumber,'f',1);
    QString BFC = (md->isEllipse()) ? " Flat" : "Best Fit CC: " +metrics->mCC->text();
    QString html = "<p style=\'font-size: 2em'>"
            "<table border='1' width = '100%'><tr><td>" + Diameter + " mm</td><td>" + ROC + " mm</td>"
            "<td>" +FNumber+ "</td></tr>"
            "<tr><td> RMS: " + QString().number(wf->std,'f',3) +
                QString().sprintf(" waves at %6.1lf nm</td><td>Strehl: ",outputLambda) + metrics->mStrehl->text() +
            "</td><td>" + BFC + "</td></tr>"
            "<tr><td>" + ((md->isEllipse()) ? "":"Desired Conic: " + QString::number(md->cc)) + "</td><td>" +
            ((md->doNull) ? QString().sprintf("SANull: %6.4lf",md->z8 * md->cc) : "No software Null") + "</td>"
            "<td>Waves per fringe: " + QString::number(md->fringeSpacing) + "<br>Interferogram Wave length: "+ QString::number(md->lambda) + "nm</td></tr>"
            "</table></p>";

    // zerenike values
    QString zerns("<p>This is a flat so no zernike values are computed.</p>");
    if (!md->isEllipse()){
        zerns = "<p><br><table width='100%' border = '1'>"
                "<tr><th colspan = '2'><h2>Zernike Values at interferogram wavelength</h2></th></tr>"
                "<tr><td><table  border='1' width='40%'>";
        zerns.append("<tr><th>Term</th><td><table width = '100%'><tr><th>Wyant</th><th>RMS</th></tr></table></td></tr>");
        int half = Z_TERMS/2 +2;
        for (int i = 3; i < half; ++i){
            double val = wf->InputZerns[i];
            bool enabled = zernEnables[i];
            if ( i == 3 && m_surfaceTools->m_useDefocus){
                val = m_surfaceTools->m_defocus;
                enabled = true;
            }
            if ( i == 8 && md->doNull){
                val -= md->z8 * md->cc;
            }


            zerns.append("<tr><td>" + QString(zernsNames[i]) + "</td><td><table width = '100%'><tr><td>" + QString().sprintf("%6.3lf </td><td>%6.3lf</td></tr></table>",
                             val, computeRMS(i,val)) + "</td><td>" +
                         QString((enabled) ? QString("") : QString("Disabled")) + "</td></tr>");
            if (i == 5){
                double x = wf->InputZerns[4];
                double y = wf->InputZerns[5];
                double mag = sqrt(x * x + y * y);
                double angle = atan2(y,x)/2;

                zerns.append("<tr><td>astig Polar</td><td><table width = '100%'><tr><td>"
                             + QString().sprintf("%6.3lf </td><td>%6.3lf Deg.</td></tr></table>",
                                 mag, angle * (180.0 / M_PI)) + "</td><td>" +
                             QString((enabled) ? QString("") : QString("Disabled")) + "</td></tr>");
            }
        }


        zerns.append("</table></td>");

        zerns.append("<td><table border='1' width = '50%'>"
                     "<tr><th>Term</th><td><table width = '100%'><tr><th>Wyant</th><th>RMS</th></tr></table></tr>");

        for (int i = half; i < Z_TERMS; ++i){
            double val = wf->InputZerns[i];
            zerns.append("<tr><td>" + QString(zernsNames[i]) + "</td><td><table width = '100%'><tr><td>" + QString().sprintf("%6.3lf</td><td>%6.3lf</td></tr></table>"
                                                                                                                             ,val, computeRMS(i,val)) + "</td><td>" +
                         QString((zernEnables[i]) ? QString("") : QString("Disabled")) + "</td></tr>");
        }
        zerns.append("</table></td></tr></table></p>");

    }
    QString tail = "</body></html>";
    QTextDocument *doc = editor->document();

    oglRendered oglw;



//    QLabel *myLabel2 = new QLabel;
//    myLabel2->setPixmap(QPixmap::fromImage(surfaceandLegend.scaled(finalWidth,finalWidth,
//                                                                   Qt::KeepAspectRatio,
//                                                               Qt::SmoothTransformation)));
//    myLabel2->setWindowTitle("3d Surface and legend");
//    myLabel2->show();
    // add surface 3D graph
    QString imagesHtml;

    //get the profile plot image
   if (dlg.ui->showProfile->isChecked()){
       QImage profile = QImage(m_profilePlot->size(),QImage::Format_ARGB32);
       QPainter p2(&profile);
       ((MainWindow*)(parent()))->m_profilePlot->render(&p2);

         QString review("mydata://review.png");
         doc->addResource(QTextDocument::ImageResource,  QUrl(review),
              QVariant(profile.scaledToWidth(dlg.profileWidth * finalWidth , Qt::SmoothTransformation)));
             imagesHtml.append("<br>");
             imagesHtml.append("<img src='" + review + "'>");
   }

   // get the contour plot image
   if (dlg.ui->showContour->isChecked()){
       QSize stmp = m_contourView->size();
       QImage contourImg(stmp, QImage::Format_ARGB32);
       QPainter contourPainter(&contourImg);
       m_contourView->repaint();
       m_contourView->render(&contourPainter);

       QString contour("mydata://contour.png");
       doc->addResource(QTextDocument::ImageResource,  QUrl(contour),
            QVariant(contourImg.scaledToWidth(dlg.contourWidth * finalWidth, Qt::SmoothTransformation)));
       imagesHtml.append("<p ><br>&nbsp;</p>");
       imagesHtml.append("<img src='" + contour + "'>");
   }

    // 3D Surface
    if (dlg.ui->show3D->isChecked()){
        // assemble the surface metrics from the OG 3D view and the color map legend
        // put into the OGLW window that will provide the titles and placement.
        QImage SurfaceImage =m_SurfaceGraph->render(1000, 1000);
        QSize lsize = m_SurfaceGraph->m_legend->size();
        QImage legend(lsize, QImage::Format_ARGB32);
        m_SurfaceGraph->m_legend->render(&legend);

        oglw.getLegend()->setPixmap(QPixmap::fromImage(legend.scaledToHeight(900 )));
        oglw.getModel()->setPixmap(QPixmap::fromImage(SurfaceImage));
        oglw.show();    // show on stack to get metrics.
        QFontMetrics fm =    oglw.fontMetrics();
        QRect tw = fm.boundingRect("Waves 550 nm Waves");
        QRect lg = oglw.getLegend()->geometry();
        QRect sg = oglw.getModel()->geometry();

        int surfw = sg.width() + tw.width();
        int surfh = sg.height();
        QImage surfaceandLegend(surfw,surfh, QImage::Format_ARGB32);
        QPainter painterSurfaceandLegend(&surfaceandLegend);
        oglw.render(&painterSurfaceandLegend);
        QString surfPath("mydata://surface.png");
        doc->addResource(QTextDocument::ImageResource,  QUrl(surfPath),
                         QVariant(surfaceandLegend.scaledToWidth(dlg.surfaceWidth * finalWidth, Qt::SmoothTransformation)));
        imagesHtml.append("<p ><br>&nbsp;</p>");
        imagesHtml.append("<img src='" + surfPath + "'>");
    }
    int lastH = 0;
    if (dlg.ui->showStarTest->isChecked()){

        // add star test if not testing a
        if (!md->isEllipse()){
            SimulationsView *sv = SimulationsView::getInstance(0);
            sv->resize(width,width);
            sv->on_MakePB_clicked();
            QImage svImage = QImage(sv->size(),QImage::Format_ARGB32 );

            QPainter p3(&svImage);
            sv->render(&p3);
            QString svpng("mydata://sv.png");
            doc->addResource(QTextDocument::ImageResource,  QUrl(svpng),
                  QVariant(svImage.scaledToWidth(dlg.startestWidth * finalWidth,
                                                 Qt::SmoothTransformation)));
                imagesHtml.append("<p ><br>&nbsp;</p>");
                imagesHtml.append(" <img src='" +svpng + "'></p>");
        }
    }

    if (dlg.ui->showRonchi->isChecked())
    {

        foucaultView *fv = foucaultView::get_Instance(0);
        fv->on_makePb_clicked();
        QImage fvImage = QImage(fv->size(),QImage::Format_ARGB32 );
        QPainter p3(&fvImage);
        fv->render(&p3);

        QString fvpng("mydata://fv.png");
        doc->addResource(QTextDocument::ImageResource,  QUrl(fvpng),
                         QVariant(fvImage.scaledToWidth(dlg.ronchiWidth * finalWidth)));
        imagesHtml.append("<br>");
        imagesHtml.append(" <img src='" + fvpng + "'></p>");
    }

    // add igram
    QImage igram = ((MainWindow*)(parent()))->m_igramArea->igramDisplay;
    if (dlg.ui->showIgram->isChecked() && igram.width() > 0){

        QString sigram("mydata://igram.png");
        doc->addResource(QTextDocument::ImageResource,  QUrl(sigram),
                         QVariant(igram.scaledToWidth(dlg.igramWidth * finalWidth,
                                                      Qt::SmoothTransformation)));
        imagesHtml.append("<table  style=\"page-break-before:always\" border = \"1\"><tr><th>typical interferogram</th></tr> <tr><td> <img src='" +
                           sigram + "'></td></tr></table><br>");
    }
    if (dlg.ui->showHistogram->isChecked()){
        // add pixel stats window
        m_contourView->getPixelstats()->resize(width/3,4 * width/3);
        QImage pixStats = m_contourView->getPixstatsImage();
        QString pixStat("mydata://pixStat.png");
        doc->addResource(QTextDocument::ImageResource, QUrl(pixStat),
                         QVariant(pixStats.scaledToWidth(dlg.histoWidth * finalWidth,
                                                          Qt::SmoothTransformation)));
        imagesHtml.append("<table  style=\"page-break-before:always\" border = \"1\"><tr><th>Pixel Hidtogram and SLope error</th></tr> <tr><td> <img src = '" +
                           pixStat + "'></td></tr></table>");
    }
    editor->setHtml(title + html +zerns + imagesHtml +  tail);
    editor->print(&printer);
    if (dlg.ui->showPDF->isChecked()){
        QDesktopServices::openUrl(QUrl(dlg.fileName));
     }



}
#include "unwraperrorsview.h"
void SurfaceManager::showUnwrap(){
    if (m_wavefronts.size() >0){
        unwrapErrorsView dlg(*m_wavefronts[m_currentNdx]);
        dlg.exec();
    }
}
void SurfaceManager::initWaveFrontLoad(){
    diamResp = ASK;
    rocResp = ASK;
    lambdResp = ASK;
}
void SurfaceManager::transform(){
    if (m_wavefronts.length() == 0){
        QMessageBox::warning(0,"Warning","No wave fronts loaded.");
        return;
    }
    TransformWaveFrontDlg dlg;
    connect(&dlg, SIGNAL(flipLR()), this, SLOT(flipHorizontal()));
    connect(&dlg, SIGNAL(flipV()),this,   SLOT(flipVertical()));
    connect(&dlg, SIGNAL(resizeW(int)), this, SLOT(resizeW(int)));
    connect(&dlg, SIGNAL(changeWavelength(double)), this, SLOT(changeWavelength(double)));

    dlg.exec();
}

void SurfaceManager::memoryLow(){
    int resp = QMessageBox::warning(0,"low on memory", "Do you want to continue?");
        okToContinue = resp;
}
void SurfaceManager::resize( wavefront *wf, int size){
    wavefront *nwf = new wavefront();
    *nwf = *wf;
    nwf->dirtyZerns = true;
    cv::Mat newData;
    cv::Mat newMask;
    cv::resize(wf->data,newData, cv::Size(size,size));
    cv::resize(wf->mask, newMask, cv::Size(size,size));
    nwf->data = newData.clone();
    nwf->mask = newMask.clone();

    nwf->m_outside.m_center.rx() = size/2.;
    nwf->m_outside.m_center.ry() = size/2.;
    double scale = (double)size/wf->data.rows;
    nwf->m_outside.m_radius = wf->m_outside.m_radius * scale -1;
    m_wavefronts << nwf;
    nwf->wasSmoothed = false;
    nwf->name = wf->name + "_newsize";
    m_surfaceTools->addWaveFront(nwf->name);
    m_currentNdx = m_wavefronts.size()-1;
    makeMask(nwf);
    generateSurfacefromWavefront(m_currentNdx);

}
void SurfaceManager::changeWavelength( wavefront *wf, double wavelength){
    wavefront *nwf = new wavefront();
    *nwf = *wf;
    nwf->dirtyZerns = true;
    nwf->data = nwf->data * ( wf->lambda/wavelength);
    nwf->lambda = wavelength;
    m_wavefronts << nwf;
    nwf->wasSmoothed = false;
    nwf->name = wf->name + "_newWavelength";
    m_surfaceTools->addWaveFront(nwf->name);
    m_currentNdx = m_wavefronts.size()-1;
    makeMask(nwf);
    generateSurfacefromWavefront(m_currentNdx);
}
void SurfaceManager::flipHorizontal( wavefront *wf){
    cv::Mat newData;
    cv::Mat newMask;
    cv::flip(wf->data, newData, 1);
    cv::flip(wf->mask, newMask, 1);

    wavefront *nwf = new wavefront();
    *nwf = *wf;
    nwf->dirtyZerns = true;
    nwf->m_inside.m_center.rx() = wf->data.cols-1 - wf->m_inside.m_center.x();
    nwf->m_outside.m_center.rx() = wf->data.cols-1 - wf->m_outside.m_center.x();

    nwf->data = newData.clone();
    nwf->mask = newMask.clone();


    nwf->workMask = newMask.clone();
    m_wavefronts << nwf;
    nwf->wasSmoothed = false;
    nwf->name = wf->name + "_FlippedH";
    m_surfaceTools->addWaveFront(nwf->name);
    m_currentNdx = m_wavefronts.size()-1;
    makeMask(nwf);
    generateSurfacefromWavefront(m_currentNdx);
}
void SurfaceManager::flipVertical( wavefront *wf){
    cv::Mat newData;
    cv::Mat newMask;
    cv::flip(wf->data, newData, 0);
    cv::flip(wf->mask, newMask, 0);

    wavefront *nwf = new wavefront();
    *nwf = *wf;
    nwf->dirtyZerns = true;
    nwf->m_inside.m_center.rx() = wf->data.cols-1 - wf->m_inside.m_center.x();
    nwf->m_outside.m_center.rx() = wf->data.cols-1 - wf->m_outside.m_center.x();

    nwf->data = newData.clone();
    nwf->mask = newMask.clone();


    nwf->workMask = newMask.clone();
    m_wavefronts << nwf;
    nwf->wasSmoothed = false;
    nwf->name = wf->name + "_FlippedV";
    m_surfaceTools->addWaveFront(nwf->name);
    m_currentNdx = m_wavefronts.size()-1;
    makeMask(nwf);
    generateSurfacefromWavefront(m_currentNdx);
}
void SurfaceManager::resizeW(int size){
    QList<int> list = m_surfaceTools->SelectedWaveFronts();

    if (list.length() == 0){
        QMessageBox::warning(0, tr("Warning"),
                             tr("No wavefronts have been selected."));

        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach(int ndx, list){
        resize(m_wavefronts[ndx], size);
    }
    QApplication::restoreOverrideCursor();
}
void SurfaceManager::changeWavelength(double wavelength){
    QList<int> list = m_surfaceTools->SelectedWaveFronts();
    if (list.length() == 0){
        QMessageBox::warning(0, tr("Warning"),
                             tr("No wavefronts have been selected."));

        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach(int ndx, list){
        changeWavelength(m_wavefronts[ndx], wavelength);
    }
    QApplication::restoreOverrideCursor();
}
void SurfaceManager::flipVertical(){
    QList<int> list = m_surfaceTools->SelectedWaveFronts();
    if (list.length() == 0){
        QMessageBox::warning(0, tr("Warning"),
                             tr("No wavefronts have been selected."));

        return;

    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach (int ndx, list) {
        flipVertical(m_wavefronts[ndx]);
    }
    QApplication::restoreOverrideCursor();
}
void SurfaceManager::flipHorizontal(){
    QList<int> list = m_surfaceTools->SelectedWaveFronts();
    if (list.length() == 0){
        QMessageBox::warning(0, tr("Warning"),
                             tr("No wavefronts have been selected."));

        return;

    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    foreach (int ndx, list) {
        flipHorizontal(m_wavefronts[ndx]);
    }
    QApplication::restoreOverrideCursor();
}
bool QPointFLessThan(const QPointF &p1, const QPointF & p2){
    return p1.x() < p2.x();
}

void SurfaceManager::tiltAnalysis(){
    QSettings set;
    QString path = set.value("mirrorConfigFile").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString dd = info.dir().absolutePath();
    QString fName = QFileDialog::getOpenFileName(0,
        tr("Select a zerns.csv file"), dd + "//Zerns.csv");

    if (fName.isEmpty())
        return;
    QFile f(fName);
    QTextStream in(&f);
    QFileInfo info2(f);

    QList<QPointF> xvals;
    QList<QPointF> yvals;

    if (f.open(QIODevice::ReadOnly)) {
        QString line;
        do {
            line = in.readLine();
            if (line.size() == 0)
                continue;
            if (line[0] == '#')
                continue;
            QStringList data = line.split(',');
            if (!data[0].contains("/")){
                data[0] = info2.absoluteDir().dirName() + "/"+ data[0];
            }
            xvals << QPointF(data[2].toDouble(), data[5].toDouble());
            yvals << QPointF(data[3].toDouble(), data[6].toDouble());


        } while (!line.isNull());
    }



   cv::Mat xline;
   //cv::fitLine(xvals.toVector().toStdVector(), xline, CV_DIST_L2, 0,.01,.01);
   std::sort(xvals.begin(), xvals.end(), QPointFLessThan);
   std::sort(yvals.begin(), yvals.end(), QPointFLessThan);
   QwtPlot *pl1 = new QwtPlot();
   pl1->resize(800,600);
   pl1->insertLegend( new QwtLegend() , QwtPlot::TopLegend);
   pl1->setAxisTitle( QwtPlot::yLeft, "astig" );
   pl1->setAxisTitle(QwtPlot::xBottom, "tilt");
   pl1->setTitle(QString().sprintf("tilt vs Astig %d samples", xvals.length()));
   QwtPlotGrid *grid = new QwtPlotGrid();
   grid->attach(pl1);
   QwtPlotCurve *xTilt = new QwtPlotCurve("X");
   QwtPlotCurve *yTilt = new QwtPlotCurve("y");
   xTilt->setStyle(QwtPlotCurve::Dots);
   yTilt->setStyle(QwtPlotCurve::Dots);
   QwtPlotCurve *xTLine = new QwtPlotCurve("X fit");
   xTilt->setPen(Qt::red,4);
   yTilt->setPen(Qt::blue,4);
   xTilt->setSamples(xvals.toVector());
   yTilt->setSamples(yvals.toVector());
   xTilt->attach(pl1);
   yTilt->attach(pl1);
   pl1->show();
}

