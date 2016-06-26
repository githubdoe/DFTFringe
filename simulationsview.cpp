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
#include "simulationsview.h"
#include "ui_simulationsview.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "mirrordlg.h"
#include "zernikeprocess.h"
#include "dftarea.h"
#include <math.h>
#include <qwt_plot.h>
#include <qwt_plot_legenditem.h>
#include <qwt_legend.h>
#include <qwt_plot_textlabel.h>
#include "mirrordlg.h"
#include <qwt_scale_draw.h>
#include <QSettings>
double M2PI = M_PI * 2.;
SimulationsView *SimulationsView::m_Instance = 0;
class arcSecScaleDraw: public QwtScaleDraw
{
public:
    double m_mirrorDiam;
    double s1;
    arcSecScaleDraw(double mirrorDiameter): m_mirrorDiam(mirrorDiameter)
    {
        // From Telescope Optics Rutten & Van Vernrooij page 211.
        s1 = 206265 * 5.5e-4 / m_mirrorDiam;
    }
    virtual QwtText label( double value ) const
    {
        if (value ==0)
            return QString("");
        return QString().sprintf("%6.2lf",s1 / value);
    }
};
SimulationsView::SimulationsView(QWidget *parent) :
    QWidget(parent),
     needs_drawing(false),ui(new Ui::SimulationsView),m_wf(0)
{
    ui->setupUi(this);
    ui->MTF->setAxisTitle( QwtPlot::yLeft, "Percent Contrast" );
    ui->MTF->setAxisTitle(QwtPlot::xBottom,"Resolution arcseconds");

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->attach( ui->MTF);

    QwtPlotLegendItem *customLegend = new QwtPlotLegendItem();
    customLegend->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    customLegend->attach(ui->MTF);
    QwtPlotTextLabel *t = new QwtPlotTextLabel();
    QwtText title( "MTF" );
    title.setRenderFlags( Qt::AlignHCenter | Qt::AlignTop );
    t->setText(title);
    t->attach(ui->MTF);
    ui->MTF->setAxisScaleDraw(ui->MTF->xBottom, new arcSecScaleDraw(mirrorDlg::get_Instance()->diameter));

    ui->MakePB->setEnabled(false);
    QSettings set;
    ui->FFTSizeSB->blockSignals(true);
    ui->FFTSizeSB->setValue(set.value("FFTSize", 1000).toInt());
    ui->FFTSizeSB->blockSignals(false);
    connect(&m_guiTimer, SIGNAL(timeout()), this, SLOT(on_MakePB_clicked()));
}

SimulationsView::~SimulationsView()
{
    delete ui;
}

void SimulationsView::setSurface(wavefront *wf){
    m_wf = wf;

    if (wf == 0 ){
        needs_drawing = false;
        return;
    }

    ui->MakePB->setEnabled((wf != 0));
    needs_drawing = true;

    if (!isHidden())
        on_MakePB_clicked();
}

SimulationsView *SimulationsView::getInstance(QWidget *parent){
    if (m_Instance == 0)
    {
        m_Instance = new SimulationsView(parent);
    }
    return m_Instance;
}



cv::Mat SimulationsView::nulledSurface(double defocus){
    cv::Mat out;

    mirrorDlg *md = mirrorDlg::get_Instance();


    // defocus is in mm on input

    std::vector<double> newZerns = m_Instance->m_wf->InputZerns;
    zernikeProcess &zp = *zernikeProcess::get_Instance();

    newZerns[3] -= defocus;

    // if use wavefront then apply defocus to wavefront.
    QSettings settings;
    bool GB_enabled = settings.value("GBlur", true).toBool();
    double gbValue = settings.value("GBValue", 21).toInt();
    cv::Mat nulled_surface = zp.null_unwrapped( *(m_Instance->m_wf), newZerns, zernEnables);
    if (GB_enabled){
        cv::GaussianBlur( nulled_surface, nulled_surface , cv::Size( gbValue, gbValue ),0,0);
    }
    nulled_surface  *= M2PI * md->lambda/550.;
    return nulled_surface;
}

// create star test using pupil_size which is usually smaller than the wavefront being sampled.
cv::Mat SimulationsView::computeStarTest(cv::Mat surface, int pupil_size, double pad , bool returnComplex){
    alias = false;
    cv::Mat out;

    int nx = surface.size().width;//pupil_size;
    int ny = surface.size().height;

    cv::Mat tmp[] = {cv::Mat::zeros(Size(nx,ny),CV_64F)
                    ,cv::Mat::zeros(Size(nx,ny),CV_64F)};

    for (int y = 0; y < ny; ++y){
        for (int x = 0; x < nx; ++x){
            tmp[1].at<double>(y,x) =  cos(surface.at<double>(y,x));
            tmp[0].at<double>(y,x) = -sin(surface.at<double>(y,x));

        }
    }

    // apply the mask
    cv::Mat tmp2;
    tmp[0].copyTo(tmp2, m_wf->workMask);
    tmp[0] = tmp2.clone();
    tmp[1].copyTo(tmp2, m_wf->workMask);
    tmp[1] = tmp2.clone();
    //pupil_size += 1;
    // now reduce the wavefront with pad to fit into the fft size;
    // new padSize is fft_size/pad;

    int padSize = pupil_size/pad;

    cv::resize(tmp[0],tmp[0],cv::Size(padSize,padSize),cv::INTER_AREA);
    cv::resize(tmp[1],tmp[1],cv::Size(padSize,padSize),cv::INTER_AREA);

    cv::Mat in[] = {cv::Mat::zeros(Size(pupil_size,pupil_size),CV_64F)
                    ,cv::Mat::zeros(Size(pupil_size,pupil_size),CV_64F)};

    tmp[0].copyTo(in[0](cv::Rect(0,0,tmp[0].cols,tmp[0].cols)));
    tmp[1].copyTo(in[1](cv::Rect(0,0,tmp[0].cols,tmp[0].cols)));
    //showData("xxxxff", in[0].clone());
    cv::Mat complexIn;

    cv::merge(in,2,complexIn);
    dft(complexIn,out);
    shiftDFT(out);
    Mat planes[2];
    split(out, planes);
    magnitude(planes[0], planes[1], planes[0]);


    // check for aliasing
    // compute edge
    double edge_avg = 0.;
    double center_avg = 0.;
    int half = pupil_size /2;
    int last = pupil_size * .3;

    for (int i = 0; i < last; ++i)
    {
        edge_avg += planes[0].at<double>(half,i);
        center_avg += planes[0].at<double>(half, i+half);

    }

    double ddd = center_avg/edge_avg;

    if (ddd < 2)
    {
        alias = true;
/*
        AfxMessageBox(L"Warning, computed PSF was too large for the selected size of the simulation.\n"
                        L"Select larger simulation size from the Configuration Menu\n"
                        L"and try again.\n\n"
                        L"Note: PSF is also used to compute Foucault, Ronchi, and MTF\n"
                        L" Computeing MTF may cause this message 3 times\n"
                        L"Sometime this message is caused by the errors on the surface and so the simulatin may still be usable.\n"
                        L"The error usually shows up as a series of light and dark horizontal bands.");

        //throw FFT_ERROR();
        */
    }

    if (returnComplex)
        return out;
    return (planes[0]);

}

int ee95percent(cv::Mat_<double> data){

    int center = (data.cols-1)/2;
    double sum = 0;
    vector<double> sums;
    for (int i = 1; i < center; ++i) {
        sum += data(center,i+center -1);
        sum += data(center,center-i);
        sums.push_back(sum);

    }
    int i;
    for (i = sums.size()-1; i > 0; --i){
        if (sums[i] <= sums.back() * .97)
            break;
    }
    return i;
}
cv::Mat fitStarTest(cv::Mat img, int size, double gamma){
    int offset = (img.cols - 600)/2;
    int ee = ee95percent(img);
    offset = img.cols/2 - ee;
    cv::Mat tmp;
    cv::pow(img,gamma,img);
    //showData("fff", img.clone(), false);
    normalize(img, img,0,255,CV_MINMAX);
    img.convertTo(tmp,CV_8U);
    cvtColor(tmp,tmp, CV_GRAY2RGB);
    cv::Mat t = tmp(cv::Rect(offset,offset,2 * ee, 2 * ee));
    cv::resize(t,t,cv::Size(size,size),0,0,cv::INTER_AREA);
    return t;
}
void etoxplusy(cv::Mat data)
{
    for (int i = 0; i < data.rows; ++i)
    {
        for (int j = 0; j < data.cols; ++j)
        {
            if (((j + i) & 0x01) == 1)
            {
                data.at<Vec2d>(i,j)[0] *= -1.;
                data.at<Vec2d>(i,j)[1]= -1.;

            }
        }
    }
}
void SimulationsView::mtf(cv::Mat star, QString txt, QColor color){
    cv::Mat middle = star.col(star.cols/2);
    pow(middle,2,middle);
    cv::Mat planes[] = {Mat_<double>(middle),
                        cv::Mat::zeros(Size(middle.cols,middle.rows),CV_64F)};
    cv::Mat mtfIn, mtfOut, mtfMag;

    cv::merge(planes,2,mtfIn);
    //etoxplusy(mtfIn);
    cv::dft(mtfIn, mtfOut);
    split(mtfOut,planes);

    cv::magnitude(planes[0],planes[1], mtfMag);

    cv::normalize(mtfMag,mtfMag, 0,100,cv::NORM_MINMAX);
    int nx = (mtfMag.rows)/2;
    QwtPlotCurve *curve1 = new QwtPlotCurve(txt);
    curve1->setPen(QPen(color));
    QPolygonF points1;


    for (int x = 0; x < nx; ++x){

        points1 << QPointF((double)(x)/nx, mtfMag.at<double>(x,0));
    }
    curve1->setSamples(points1);
    curve1->attach(ui->MTF);
}

void SimulationsView::on_MakePB_clicked()
{
    m_guiTimer.stop();

    if (m_wf == 0)
        return;
    if (mirrorDlg::get_Instance()->isEllipse()){
        QMessageBox::warning(0,"warning","Star test simulation is not suppported for flat surfaces");
        return;
    }
    bool wasAliased = false;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    needs_drawing = false;
    double defocus = ui->defocusSB->value()/2;
    double gamma = ui->gammaSB->value();

    //inside focus star test
    cv::Mat inside = computeStarTest(nulledSurface(-defocus), ui->FFTSizeSB->value(), ui->centerMagnifySB->value());
    cv::Mat t = fitStarTest(inside,500,gamma);
    cv::putText(t,QString().sprintf("-%5.1lfmm inside",2 * defocus).toStdString(),cv::Point(50,30),1,1,cv::Scalar(255, 255,255));
    wasAliased |= alias;
    if (alias)
    {
        cv::Mat chans[3];
        split(t,chans);
        chans[1] *= 0;
        chans[2] *= 0;
        merge(chans,3,t);

    }
    QImage indisplay((uchar*)t.data, t.cols, t.rows, t.step, QImage::Format_RGB888);
    ui->inside->setPixmap(QPixmap::fromImage(indisplay));

    // outside focus star test
    cv::Mat outside = computeStarTest(nulledSurface(defocus),ui->FFTSizeSB->value(),ui->centerMagnifySB->value());
    t = fitStarTest(outside,500,gamma);
    cv::putText(t,QString().sprintf("%5.1lfmm outside",2 * defocus).toStdString(),cv::Point(50,30),1,1,cv::Scalar(255, 255,255));
    wasAliased |= alias;
    if (alias)
    {
        cv::Mat chans[3];
        split(t,chans);
        chans[2] *= 0;
        chans[1] *= 0;
        merge(chans,3,t);
    }
    QImage outdisplay((uchar*)t.data, t.cols, t.rows, t.step, QImage::Format_RGB888);
    ui->outside->setPixmap(QPixmap::fromImage(outdisplay));
    if (wasAliased){
        QMessageBox::warning(NULL, tr("Warning"),
                "Computed star test (in red) was too large for the selected size of the simulation.\n"
                                       "Select larger FFT Size or smaller Magnification\n"
                                       "and try again.\n\n"
                                       "Sometime this message is caused by the errors on the surface and so the simulatin may still be usable.\n"
                                       "The error usually shows up as a series of light and dark horizontal bands or dots.");
    }

    cv::Mat focused = computeStarTest(m_wf->workData, 600,  40);
    t = fitStarTest(focused, 200,gamma/2);
    cv::putText(t,QString().sprintf("Focused").toStdString(),cv::Point(20,20),1,1,cv::Scalar(255, 255,255));
    QImage focusDisplay ((uchar*)t.data, t.cols, t.rows, t.step, QImage::Format_RGB888);
        ui->Focused->setPixmap(QPixmap::fromImage(focusDisplay));

    // make psf plot
    ui->psfView->clear();
    cv::Mat psf = computeStarTest(nulledSurface(0), 600, 20);
    ui->psfView->setData(psf, "Actual", QPen(Qt::red));
    cv::Mat savedMask = m_wf->workMask.clone();
    // add central obstruction
    cv::Mat mask = m_wf->workMask.clone();
    cv::Mat noObstruction = m_wf->workMask.clone();
    mirrorDlg *md = mirrorDlg::get_Instance();
    double r = md->obs * (2. * m_wf->m_outside.m_radius)/md->diameter;
    if (r > 0){

        circle(noObstruction,Point(noObstruction.cols/2,noObstruction.cols/2),r, Scalar(255),-1);
    }

    cv::Mat p = m_wf->workData.clone() * 0.;
    m_wf->workMask = noObstruction;
    cv::Mat perfectPSF = computeStarTest(p, 600, 20);
    ui->psfView->setData(perfectPSF, "Perfect", QPen(Qt::black));

    //make mtf plot
    ui->MTF->detachItems( QwtPlotItem::Rtti_PlotCurve);
    m_wf->workMask = mask;
    focused = computeStarTest(nulledSurface(0), 512, 2);
    m_wf->workMask = noObstruction;
    perfectPSF = computeStarTest(p, 512,2);
    m_wf->workMask = mask.clone();
    mtf(focused, "actual", Qt::red);
    mtf(perfectPSF, "Perfect", Qt::black);

    ui->MTF->replot();
    ui->MTF->show();
    QApplication::restoreOverrideCursor();

}

void SimulationsView::on_defocusSB_valueChanged(double){
    m_guiTimer.start(500);

}

void SimulationsView::on_gammaSB_valueChanged(double)
{
    m_guiTimer.start(500);
}

void SimulationsView::on_centerMagnifySB_valueChanged(double)
{
    m_guiTimer.start(500);
}

void SimulationsView::on_FFTSizeSB_valueChanged(int val)
{
   QSettings set;
   set.setValue("FFTSize", val);

    m_guiTimer.start(500);
}


