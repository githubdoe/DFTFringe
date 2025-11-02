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
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
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
#include "settings2.h"
#include "utils.h"
#include <QTextDocument>
#include <QtMath>
#include <opencv2/core/core_c.h>

double M2PI = M_PI * 2.;
SimulationsView *SimulationsView::m_Instance = 0;
class arcSecScaleDraw: public QwtScaleDraw
{
public:
    double m_mirrorDiam;
    double s1;
    arcSecScaleDraw(double mirrorDiameter): m_mirrorDiam(mirrorDiameter)
    {
        s1 = (1.22 * 550.e-6/m_mirrorDiam) * 57.3 * 3600;
        // From Telescope Optics Rutten & Van Vernrooij page 211.
        //s1 = 206265 * 5.5e-4 / m_mirrorDiam;
        qDebug() << "arcsec" << s1;
    }
    virtual QwtText label( double value ) const
    {

        if (value ==0)
            return QString("");


        return QString("%1").arg(s1 / value, 6, 'f', 2);
    }
};
SimulationsView::SimulationsView(QWidget *parent) :
    QWidget(parent),
     needs_drawing(false),needs_drawing_3D(true),m_arcSecScaleDraw(0),ui(new Ui::SimulationsView),m_wf(0)
{
    ui->setupUi(this);
    ui->PSF3DWidget->hide();
    initMTFPlot();

    //QList<double> ticks;
    //ticks << 5.<< 2.5  << 1. << .5 << .3;

    //QwtScaleDiv *scaleDivP = new QwtScaleDiv( 20, .3);
    //scaleDivP->setTicks(QwtScaleDiv::MajorTick, ticks);
    //ui->MTF->setAxisScaleDiv(QwtPlot::xBottom, *scaleDivP);

    m_PSF_3Dgraph = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(m_PSF_3Dgraph);
    QHBoxLayout *hLayout = new QHBoxLayout();
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    ui->PSF3DWidget->setLayout(hLayout);
    vLayout->setAlignment(Qt::AlignTop);
    ui->MakePB->setEnabled(false);

    QSettings set;
    m_psf_doLog = false;// set.value("psfDoLog", 1).toBool();
    ui->doLog->setChecked(m_psf_doLog);
    ui->FFTSizeSB->blockSignals(true);
    ui->FFTSizeSB->setValue(set.value("FFTSize", 1000).toInt());
    ui->FFTSizeSB->blockSignals(false);
    ui->centerMagnifySB->setValue(set.value("StarTestMagnify", 4).toDouble());
    ui->gammaSB->setValue(set.value("StarTestGamma", 2.).toDouble());
    connect(&m_guiTimer, SIGNAL(timeout()), this, SLOT(on_MakePB_clicked()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showContextMenu(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);
}

SimulationsView::~SimulationsView()
{
    delete ui;
}

void SimulationsView::initMTFPlot(){
    ui->MTF->detachItems(QwtPlotItem::Rtti_PlotItem);
    ui->MTF->setAxisTitle( QwtPlot::yLeft, "Percent Contrast" );


    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->attach( ui->MTF);
    m_arcSecScaleDraw  =  new arcSecScaleDraw(mirrorDlg::get_Instance()->diameter);
    ui->MTF->setAxisScaleDraw(ui->MTF->xBottom, m_arcSecScaleDraw);
    QwtPlotLegendItem *customLegend = new QwtPlotLegendItem();
    customLegend->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    customLegend->attach(ui->MTF);
    QwtPlotTextLabel *t = new QwtPlotTextLabel();
    QwtText title( "MTF" );
    title.setRenderFlags( Qt::AlignHCenter | Qt::AlignTop );
    t->setText(title);
    t->attach(ui->MTF);
}
void SimulationsView::setSurface(wavefront *wf){
    m_wf = wf;
    m_arcSecScaleDraw->s1 = (1.22 * 550.e-6/wf->diameter) * 57.3 * 3600;
    QString txt = QString("diameter %1 with max resolution of %2 arcsec").arg(wf->diameter, 6, 'f', 1).arg(m_arcSecScaleDraw->s1, 6, 'f', 2);
   ui->MTF->setAxisTitle(QwtPlot::xBottom,txt);
    if (wf == 0 ){
        needs_drawing = false;
        return;
    }

    ui->MakePB->setEnabled((wf != 0));
    needs_drawing = true;
    needs_drawing_3D = true;

    if (!isHidden())
        on_MakePB_clicked();
}
void SimulationsView::saveImage(QString fileName){
    QSettings settings;
    QString path = settings.value("lastPath","").toString();
    if (fileName == "")
        fileName = QFileDialog::getSaveFileName(0,
                                        "File name for image to be saved", path);
    if (!fileName.endsWith(".jpg"))
        fileName = fileName + ".jpg";
    QImage svImage = QImage(size(),QImage::Format_ARGB32 );
    QPainter p3(&svImage);
    render(&p3);
    svImage.save(fileName);
}

void SimulationsView::showContextMenu(const QPoint &pos)
{

// Handle global position
    QPoint globalPos = mapToGlobal(pos);
    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Save as image",  this, SLOT(saveImage()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
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

    // defocus is in waves on input

    std::vector<double> newZerns = m_Instance->m_wf->InputZerns;
    zernikeProcess &zp = *zernikeProcess::get_Instance();

    newZerns[3] -= defocus;

    // if use wavefront then apply defocus to wavefront.
    QSettings settings;
    bool GB_enabled = settings.value("GBlur", true).toBool();

    // save the user selected defocus enable to be restored after this.
    bool saved_defocus_enable = zernEnables[3];
    zernEnables[3] = false;
    cv::Mat nulled_surface = zp.null_unwrapped( *(m_Instance->m_wf), newZerns, zernEnables);
    zernEnables[3] = saved_defocus_enable;
    if (GB_enabled){
       double gbValue = settings.value("GBValue", 21).toInt();
       int blurRad = 2 *(m_Instance->m_wf)->m_outside.m_radius * gbValue * .01;

        blurRad &= 0xfffffffe;
        ++blurRad;


        qDebug() << "Blurr" << blurRad;
        cv::GaussianBlur( nulled_surface, nulled_surface , cv::Size( blurRad, blurRad ),0,0);
    }
    nulled_surface  *= M2PI * md->lambda/outputLambda;
    return nulled_surface;
}

// create star test using pupil_size which is usually smaller than the wavefront being sampled.
cv::Mat SimulationsView:: computeStarTest(cv::Mat surface, int pupil_size, double pad , bool returnComplex){
    alias = false;
    cv::Mat out;


    int nx = surface.size().width;//pupil_size;
    int ny = surface.size().height;

    cv::Mat tmp[] = {cv::Mat::zeros(cv::Size(nx,ny),CV_64FC1)
                    ,cv::Mat::zeros(cv::Size(nx,ny),CV_64FC1)};

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


    int padSize =  pupil_size/pad;
    dX = (m_wf->diameter/2.)/m_wf->m_outside.m_radius;

    cv::resize(tmp[0],tmp[0],cv::Size(padSize,padSize),cv::INTER_AREA);
    cv::resize(tmp[1],tmp[1],cv::Size(padSize,padSize),cv::INTER_AREA);


    cv::Mat in[] = {cv::Mat::zeros(cv::Size(pupil_size,pupil_size),CV_64FC1)
                    ,cv::Mat::zeros(cv::Size(pupil_size,pupil_size),CV_64FC1)};

    tmp[0].copyTo(in[0](cv::Rect(0,0,tmp[0].cols,tmp[0].cols)));
    tmp[1].copyTo(in[1](cv::Rect(0,0,tmp[0].cols,tmp[0].cols)));
    //showData("xxxxff", in[0].clone());
    cv::Mat complexIn;

    cv::merge(in,2,complexIn);
    dft(complexIn,out);
    shiftDFT(out);
    //cv::flip(out,out,0);      // needs work.
    cv::Mat planes[2];
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
    std::vector<double> sums;
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
    cvtColor(tmp,tmp, cv::COLOR_GRAY2RGB);
    cv::Mat t = tmp(cv::Rect(offset,offset,2 * ee, 2 * ee));
    cv::resize(t,t,cv::Size(size,size),0,0,cv::INTER_AREA);
    cv::flip(t,t,0);
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
                data.at<cv::Vec2d>(i,j)[0] *= -1.;
                data.at<cv::Vec2d>(i,j)[1]= -1.;

            }
        }
    }
}

void SimulationsView::mtf(const cv::Mat &star, const QString &txt, QColor color){
    cv::Mat planes[2];
    cv::Mat mtfIn, mtfOut, mtfMag;
    split(star,planes);
    cv::magnitude(planes[0],planes[1], planes[0]);
    cv::pow(planes[0],2,planes[0]);
    planes[1] = cv::Scalar(0.);
    cv::Mat in;
    merge(planes,2,in);
    shiftDFT(in);

    cv::dft(in, mtfOut);
    shiftDFT(mtfOut);

    split(mtfOut,planes);

    cv::magnitude(planes[0],planes[1], mtfMag);

    cv::normalize(mtfMag,mtfMag, 0,100,cv::NORM_MINMAX);
    int nx = (mtfMag.rows);
    int half = nx/2;
    QwtPlotCurve *curve1 = new QwtPlotCurve(txt);
    curve1->setPen(QPen(QBrush(color),5));
    QPolygonF points1;


    for (int x = 1; x < half; ++x){

        double percentRes =  (double(x)/half);
        points1 << QPointF(percentRes, mtfMag.at<double>(x+half,half));
    }
    curve1->setSamples(points1);
    curve1->attach(ui->MTF);
}
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

void SimulationsView::makeFrame(double defocus, startestMovieDlg * dlg){


    double fftSize = ui->FFTSizeSB->value();
    double gamma = ui->gammaSB->value();

        QApplication::processEvents();

        cv::Mat inside = computeStarTest(nulledSurface(defocus), fftSize, 3);
        QScreen *screen = QGuiApplication::primaryScreen();
        int size = screen->availableSize().height()/2;
        cv::Mat t = fitStarTest(inside, size,gamma);
        cv::putText(t,QString("%1 waves").arg(2 * defocus, 5, 'f', 2).toStdString(),cv::Point(50,60),1,3,cv::Scalar(255, 255,255),3);


        QImage outdisplay((uchar*)t.data, t.cols, t.rows, t.step, QImage::Format_RGB888);


        QApplication::processEvents();

         dlg->setImage(outdisplay);

}
void SimulationsView::on_film_clicked()
{
    ui->film->setChecked(false);
    m_movieDlg = new startestMovieDlg(this);

    m_movieDlg->exec();

    delete m_movieDlg;
}

int offset = 0;
int stalkWidth;
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;
cv::Mat make_obstructionMask(cv::Mat mask){
    //return;
    cv::Mat out = mask.clone();
    int s = mask.size[0];
    //for (int y = 0; y < s; y += s/10){
    //cv::circle(out, Point(s/2 ,s/4), s/20, cv::Scalar(0,0,0), -1);
    //}
    //line(out, Point(s/2-offset, s/2), Point(s - offset,s/2),cv::Scalar(0,0,0), stalkWidth);
    cv::rectangle(out, Point(s/2-s/20, 0), Point(s/2+s/20, s/2),
                  cv::Scalar(0,0,0), -1);
    ///line(out, Point(0, 0), Point(s,s),cv::Scalar(0,0,0), stalkWidth);
    return out;
}


//3D psf
void SimulationsView::make3DPsf(const cv::Mat &surface){

    int nx = surface.size[0];
    int start = nx/2 - nx/4;
    cv::Mat data = surface.clone()(cv::Rect(start,start,nx/2,nx/2));


    if (m_psf_doLog){
        data += cv::Scalar::all(1); // switch to logarithmic scale
        cv::log(data, data);
    }
    double xmin,xmax;
    cv::minMaxLoc(data, &xmin,&xmax);

    data/=xmax;


    QList<QSurface3DSeries *> list = m_PSF_3Dgraph->seriesList();
    foreach(QSurface3DSeries* s, list){
        m_PSF_3Dgraph->removeSeries(s);
    }

    QSurfaceDataProxy *m_sqrtSinProxy = new QSurfaceDataProxy();
    QSurface3DSeries *m_sqrtSinSeries = new QSurface3DSeries(m_sqrtSinProxy);

    //draw surface
    {

        int sampleCountX = data.size[1];
        int sampleCountZ = data.size[0];
        int width = sampleCountX;
        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        QSurfaceDataArray *dataArray2 = new QSurfaceDataArray;
        dataArray->reserve(sampleCountZ);
        dataArray2->reserve(sampleCountZ);
        for (int i = 0 ; i < sampleCountZ ; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
            QSurfaceDataRow *backRow = new QSurfaceDataRow(sampleCountX);
            float z = -(sampleCountZ/2. - i);
            int index = 0;
            for (int j = 0; j < sampleCountX; j++) {
                float x = -(sampleCountX/2. -j);

                float y = data.at<double>(i,j);
                (*newRow)[index].setPosition(QVector3D(x, y, z));
                (*backRow)[index++].setPosition(QVector3D(x, y, width/2));
            }
            *dataArray << newRow;
            *dataArray2 << backRow;
        }

        QSurfaceDataProxy *m_sqrtSinProxy2 = new QSurfaceDataProxy();
        QSurface3DSeries *m_sqrtSinSeries2 = new QSurface3DSeries(m_sqrtSinProxy2);


        m_sqrtSinProxy->resetArray(dataArray);
        m_sqrtSinSeries->setDrawMode(QSurface3DSeries::DrawSurface);
        m_sqrtSinSeries->setFlatShadingEnabled(false);

        m_sqrtSinProxy2->resetArray(dataArray2);
        m_sqrtSinSeries2->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
        m_sqrtSinSeries2->setFlatShadingEnabled(false);

        QLinearGradient gr;
        gr.setColorAt(0.0, Qt::darkGray);
        gr.setColorAt(0.01,Qt::cyan);
        gr.setColorAt(0.33, Qt::blue);
        gr.setColorAt(0.67, Qt::lightGray);
        gr.setColorAt(1.0, Qt::red);
    m_PSF_3Dgraph->activeTheme()->setType(Q3DTheme::Theme(3));



        m_PSF_3Dgraph->axisX()->setRange(-width/2, width/2);
        m_PSF_3Dgraph->axisZ()->setRange(-width/2, width/2);

        m_PSF_3Dgraph->addSeries(m_sqrtSinSeries);
        m_PSF_3Dgraph->addSeries(m_sqrtSinSeries2);
        m_PSF_3Dgraph->seriesList().at(0)->setBaseGradient(gr);
        m_PSF_3Dgraph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    }
}

cv::Mat zoomMat(cv::Mat &mat, double zoom){
    int half = mat.size[0]/2;
    int w = half/zoom;
    cv::Mat x = mat.clone()(cv::Rect(half-w,half-w,2 * w, 2 * w));

    return x;

}
void SimulationsView::on_MakePB_clicked()
{
    m_guiTimer.stop();
    ui->MakePB->setChecked(false);
    /**************************  special test code for PSF  */



    if (m_wf == 0)
        return;
    if (mirrorDlg::get_Instance()->isEllipse()){
        QMessageBox::warning(0,"warning","Star test simulation is not suppported for flat surfaces");
        return;
    }

    metricsDisplay *metrics = metricsDisplay::get_instance();
    QTextDocument doc;
    doc.setHtml(metrics->mStrehl->text());
    QString strehl = doc.toPlainText();
    doc.setHtml(metrics->mCC->text());
    QString bestFit = doc.toPlainText();
    QFileInfo fileInfo(m_wf->name);

    QString fileName = fileInfo.fileName();
    QString caption = QString("%1   Diameter: %2 ROC: %3 Best Fit CC: %4 Strehl: %5").arg(
                                        fileName).arg(
                                        m_wf->diameter, 6, 'f', 1).arg(
                                        m_wf->roc, 6, 'f', 1).arg(
                                        bestFit).arg(  // clazy:exclude=qstring-arg
                                        strehl);
    ui->caption->setText(caption);
    bool wasAliased = false;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    needs_drawing = false;
    double defocus = ui->defocusSB->value()/2;
    double gamma = ui->gammaSB->value();
    cv::Mat savedMask = m_wf->workMask.clone();
    double fftSize = ui->FFTSizeSB->value();
    //inside focus star test
    int wid = width() * .9/3.;
    cv::Mat theObstruction = m_wf->workMask;

    if (0) {  // when true creates an obstruction pattern defined by make_obstructionMask
        stalkWidth = m_wf->m_outside.m_radius * .1;
        theObstruction = make_obstructionMask(m_wf->workMask);
        m_wf->workMask = theObstruction.clone();
        showData(QString("%1").arg(m_wf->diameter, 0, 'f').toStdString().c_str(), theObstruction.clone(), false);

    }

    cv::Mat inside = computeStarTest(nulledSurface(-defocus), fftSize, 3);
    cv::Mat t = fitStarTest(inside, wid,gamma);
    cv::putText(t,QString("-%1 waves inside").arg(2 * defocus, 5, 'f', 1).toStdString(),cv::Point(50,30),1,1,cv::Scalar(255, 255,255));
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
    ui->inside->setPixmap(QPixmap::fromImage(indisplay.copy()));

    // outside focus star test
    cv::Mat outside = computeStarTest(nulledSurface(defocus),fftSize,3);
    t = fitStarTest(outside,wid ,gamma);
    cv::putText(t,QString("%1waves outside").arg(2 * defocus, 5, 'f', 1).toStdString(),cv::Point(50,30),1,1,cv::Scalar(255, 255,255));
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
    ui->outside->setPixmap(QPixmap::fromImage(outdisplay).copy());
    if (wasAliased){
        QMessageBox::warning(NULL, tr("Warning"),
                "Computed star test (in red) was too large for the selected size of the simulation.\n"
                                       "Select larger FFT Size or smaller Magnification\n"
                                       "and try again.\n\n"
                                       "Sometime this message is caused by the errors on the surface and so the simulatin may still be usable.\n"
                                       "The error usually shows up as a series of light and dark horizontal bands or dots.");
    }

    cv::Mat focused = computeStarTest(nulledSurface(0), fftSize,  ui->centerMagnifySB->value());
    t = fitStarTest(zoomMat(focused,ui->centerMagnifySB->value()), wid ,gamma/2);
    QString focusedtext = QString("Focused magnified by %1x").arg(ui->centerMagnifySB->value());
    cv::putText(t,focusedtext.toStdString().c_str(),cv::Point(20,20),1,1,cv::Scalar(255, 255,255));
    QImage focusDisplay ((uchar*)t.data, t.cols, t.rows, t.step, QImage::Format_RGB888);
    ui->Focused->setPixmap(QPixmap::fromImage(focusDisplay.copy()));

    // make psf plot
    ui->psfView->clear();


    m_wf->workMask = theObstruction.clone();
   // showData("xxx", theObstruction.clone(), false);

    cv::Mat psf = computeStarTest(nulledSurface(0), fftSize, fftSize/100);
    cv::pow(psf,2,psf);
    psf = zoomMat(psf,2);
    make3DPsf(psf);
    ui->psfView->setData(psf, "Actual", QPen(QBrush(Qt::red),3),m_psf_doLog);


    // remove obstructions
    cv::Mat noObstruction = savedMask.clone();
    mirrorDlg *md = mirrorDlg::get_Instance();
    double r = md->obs * (2. * m_wf->m_outside.m_radius)/md->diameter;
    if (r > 0){

        circle(noObstruction,Point(noObstruction.cols/2,noObstruction.cols/2),r, Scalar(255),-1);
    }
    cv::Mat p = m_wf->workData.clone() * 0.;

    m_wf->workMask = noObstruction;
    cv::Mat perfectPSF = computeStarTest(p, fftSize, fftSize/100);
    cv::pow(perfectPSF,2,perfectPSF);
    perfectPSF = zoomMat(perfectPSF,2);
    ui->psfView->setData(perfectPSF, "Perfect", QPen(QBrush(Qt::black),3), m_psf_doLog);

    //make mtf plot
    //ui->MTF->detachItems( QwtPlotItem::Rtti_PlotScale);
    initMTFPlot();
    //m_arcSecScaleDraw  =  new arcSecScaleDraw(mirrorDlg::get_Instance()->diameter);

    //ui->MTF->setAxisScaleDraw(ui->MTF->xBottom, m_arcSecScaleDraw);

    m_wf->workMask = theObstruction.clone();
    focused = computeStarTest(nulledSurface(0), fftSize, 2.,true);
    mtf(focused, "Actual X axis", Qt::red);

    m_wf->workMask = noObstruction.clone();
    perfectPSF = computeStarTest(p, fftSize,2.,true);

    mtf(perfectPSF, "Perfect", Qt::black);
    cv::Mat R90;
    cv::transpose(focused,R90);
    cv::flip(R90,R90,0);
    mtf(R90, "Actual Y Axis", Qt::darkGreen);
    m_wf->workMask = savedMask.clone();
    ui->MTF->replot();
    ui->MTF->show();
    QApplication::restoreOverrideCursor();

}

void SimulationsView::on_defocusSB_valueChanged(double){
    if (!Settings2::getInstance()->m_general->useStarTestMake())
        m_guiTimer.start(1000);

}

void SimulationsView::on_gammaSB_valueChanged(double value)
{
    QSettings set;
    set.setValue("StarTestGamma", value);
    if (!Settings2::getInstance()->m_general->useStarTestMake())
        m_guiTimer.start(1000);
}

void SimulationsView::on_centerMagnifySB_valueChanged(double value)
{
    QSettings set;
    set.setValue("StarTestMagnify", value );
    if (!Settings2::getInstance()->m_general->useStarTestMake())
        m_guiTimer.start(1000);
}

void SimulationsView::on_FFTSizeSB_valueChanged(int val)
{
   QSettings set;
   set.setValue("FFTSize", val);
    if (!Settings2::getInstance()->m_general->useStarTestMake())
        m_guiTimer.start(1000);
}


void SimulationsView::on_show3D_clicked(bool checked)
{
    if (checked)
    {

        ui->PSF3DWidget->show();
        ui->psfView->hide();
    }
    else {
        ui->PSF3DWidget->hide();
        ui->psfView->show();
    }
}


void SimulationsView::on_doLog_clicked(bool checked)
{
    m_psf_doLog = checked;
    QSettings set;
    set.setValue("psfDoLog", checked);
    on_MakePB_clicked();
}

