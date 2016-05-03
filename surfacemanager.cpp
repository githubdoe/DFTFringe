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
QMutex mutex;
int inprocess = 0;
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

        QwtText t(QString(m_names[int(value-4)]));
        if (value == 6.)
            t.setColor(Qt::red);
        return t;

    }
};
// --- CONSTRUCTOR ---
surfaceGenerator::surfaceGenerator(SurfaceManager *sm) :
    m_sm(sm),
    m_zg(0)
{

}

// --- DECONSTRUCTOR ---
surfaceGenerator::~surfaceGenerator() {
    // free resources
    qDebug() << "deleting surface Generator";
}

// --- PROCESS ---
// Start processing data.
void surfaceGenerator::process(int wavefrontNdx, SurfaceManager *sm) {
    // allocate resources using new here
    qDebug() << "starting process" << wavefrontNdx;
    m_sm = sm;

    mutex.lock();
    ++inprocess;
    mutex.unlock();
    wavefront *wf = m_sm->m_wavefronts[wavefrontNdx];


    //compute zernike values
    if (wf->dirtyZerns){

        zernikeProcess &zp = *zernikeProcess::get_Instance();

        zp.unwrap_to_zernikes(*wf);

        ((MainWindow*)m_sm->parent())-> zernTablemodel->setValues(wf->InputZerns);

        // null out desired terms.
        wf->nulledData = zp.null_unwrapped(*wf, wf->InputZerns, zernEnables,0,Z_TERMS   );
        wf->dirtyZerns = false;

    }
    wf->workData = wf->nulledData.clone();
    if (m_sm->m_GB_enabled){
        if (wf->useGBSmoothing != m_sm->m_GB_enabled || wf->GBSmoothingValue != m_sm->m_gbValue) {

            cv::GaussianBlur( wf->nulledData.clone(), wf->workData, cv::Size( m_sm->m_gbValue, m_sm->m_gbValue ),0,0);
        }
        wf->useGBSmoothing = true;
        wf->GBSmoothingValue = m_sm->m_gbValue;
    }
    else if (wf->useGBSmoothing == true) {
        wf->workData = wf->nulledData.clone();
        wf->useGBSmoothing = false;
    }
    if (sm->useZernikeBase) {
        QVector<int> zernsToUse;
        for (int i = 3; i < Z_TERMS; ++i)
            zernsToUse << i;
        wf->workData = sm->computeWaveFrontFromZernikes(wf->data.cols, wf->InputZerns, zernsToUse);
        wf->useGBSmoothing = false;
    }
    wf->computed = true;
    QMutexLocker lock(&mutex);

    emit finished( wavefrontNdx);
}
cv::Mat SurfaceManager::computeWaveFrontFromZernikes(int wx, std::vector<double> &zerns, QVector<int> zernsToUse){
    int wy = wx;
    double rad = (double)(wx-1)/2.d;
    double xcen = rad,ycen = rad;

    cv::Mat result = cv::Mat::zeros(wx,wx, CV_64F);

    double rho;

    std::vector<bool> &en = zernEnables;
    mirrorDlg *md = mirrorDlg::get_Instance();
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
                for (int ii = 0; ii < zernsToUse.size(); ++ii) {
                    int z = zernsToUse[ii];

                    if ( z == 3 && m_surfaceTools->m_useDefocus){
                        S1 -= m_surfaceTools->m_defocus * Zernike(z,x1,y1);
                    }
                    else {
                        if (en[z]){
                            if (z == 8 && md->doNull)
                                S1 +=    md->z8 * Zernike(z,x1,y1);

                            S1 += zerns[z] * Zernike(z,x1,y1);
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

SurfaceManager::SurfaceManager(QObject *parent, surfaceAnalysisTools *tools,
                               ProfilePlot *profilePlot, ContourPlot *contourPlot,
                               GLWidget *glPlot, metricsDisplay *mets): QObject(parent),
    m_surfaceTools(tools),m_profilePlot(profilePlot), m_contourPlot(contourPlot),
    m_oglPlot(glPlot), m_metrics(mets),
    m_gbValue(21),m_GB_enabled(false),m_currentNdx(-1),insideOffset(0),
    outsideOffset(0),useZernikeBase(false),m_askAboutReverse(true), workToDo(0), m_wftStats(0)
{
    m_simView = SimulationsView::getInstance(0);
    pd = new QProgressDialog();
    connect (this,SIGNAL(progress(int)), pd, SLOT(setValue(int)));
    m_generatorThread = new QThread;
    surfaceGenerator *sg = new surfaceGenerator(this);
    connect(sg,SIGNAL(showMessage(QString)), parent, SLOT(showMessage(QString)));
    sg->moveToThread(m_generatorThread);
    m_profilePlot->setWavefronts(&m_wavefronts);
    // create a timer for surface change update to all non current wave fronts
    m_waveFrontTimer = new QTimer(this);
    m_toolsEnableTimer = new QTimer(this);
    // setup signal and slot
    connect(m_waveFrontTimer, SIGNAL(timeout()),this, SLOT(backGroundUpdate()));
    connect(m_toolsEnableTimer, SIGNAL(timeout()), this, SLOT(enableTools()));
    connect(m_generatorThread, SIGNAL(finished()), m_generatorThread, SLOT(deleteLater()));
    connect(this, SIGNAL(generateSurfacefromWavefront(int, SurfaceManager *)), sg, SLOT(process(int, SurfaceManager *)));
    connect(sg, SIGNAL(finished(int)), this, SLOT(surfaceGenFinished(int)));
    m_generatorThread->start();

    connect(m_surfaceTools, SIGNAL(waveFrontClicked(int)), this, SLOT(waveFrontClickedSlot(int)));
    connect(m_surfaceTools, SIGNAL(wavefrontDClicked(const QString &)), this, SLOT(wavefrontDClicked(const QString &)));
    connect(m_surfaceTools, SIGNAL(centerMaskValue(int)),this, SLOT(centerMaskValue(int)));
    connect(m_surfaceTools, SIGNAL(outsideMaskValue(int)),this, SLOT(outsideMaskValue(int)));
    connect(m_surfaceTools, SIGNAL(surfaceSmoothGBEnabled(bool)), this, SLOT(surfaceSmoothGBEnabled(bool)));
    connect(m_surfaceTools, SIGNAL(surfaceSmoothGBValue(int)), this, SLOT(surfaceSmoothGBValue(int)));
    connect(m_surfaceTools, SIGNAL(wftNameChanged(int,QString)), this, SLOT(wftNameChanged(int,QString)));
    connect(this, SIGNAL(nameChanged(QString, QString)), m_surfaceTools, SLOT(nameChanged(QString,QString)));
    connect(m_metrics, SIGNAL(recomputeZerns()), this, SLOT(computeZerns()));
    connect(m_surfaceTools, SIGNAL(defocusChanged()), this, SLOT(computeZerns()));
    connect(this, SIGNAL(currentNdxChanged(int)), m_surfaceTools, SLOT(currentNdxChanged(int)));
    connect(this, SIGNAL(deleteWavefront(int)), m_surfaceTools, SLOT(deleteWaveFront(int)));
    connect(m_surfaceTools, SIGNAL(deleteTheseWaveFronts(QList<int>)), this, SLOT(deleteWaveFronts(QList<int>)));
    connect(m_surfaceTools, SIGNAL(average(QList<int>)),this, SLOT(average(QList<int>)));
    connect(m_surfaceTools, SIGNAL(doxform(QList<int>)),this, SLOT(transfrom(QList<int>)));
    connect(m_surfaceTools, SIGNAL(surfaceBaseChanged(bool)), this, SLOT(surfaceBaseChanged(bool)));
    connect(m_surfaceTools, SIGNAL(invert(QList<int>)),this,SLOT(invert(QList<int>)));
    connect(this, SIGNAL(enableControls(bool)),m_surfaceTools, SLOT(enableControls(bool)));
    connect(mirrorDlg::get_Instance(),SIGNAL(recomputeZerns()), this, SLOT(computeZerns()));
    connect(mirrorDlg::get_Instance(),SIGNAL(obstructionChanged()), this, SLOT(ObstructionChanged()));
    m_statsDlg = new StatsDlg(0);
    QSettings settings;
    m_GB_enabled = settings.value("GBlur", true).toBool();
    m_gbValue = settings.value("GBValue", 21).toInt();
    //useDemoWaveFront();

}

SurfaceManager::~SurfaceManager(){}

void SurfaceManager::centerMaskValue(int val){
    insideOffset = val;
    double mmPerPixel = m_wavefronts[m_currentNdx]->diameter/(2 *( m_wavefronts[m_currentNdx]->m_outside.m_radius-1));
    m_waveFrontTimer->start(1000);
    m_surfaceTools->m_centerMaskLabel->setText(QString().sprintf("%6.2lf mm",mmPerPixel* val));
    makeMask(m_currentNdx);
    emit generateSurfacefromWavefront(m_currentNdx, this);

}

void SurfaceManager::makeMask(int waveNdx){
    int width = m_wavefronts[waveNdx]->data.cols;
    int height = m_wavefronts[waveNdx]->data.rows;
    double xm,ym;
    xm = m_wavefronts[waveNdx]->m_outside.m_center.x();
    ym = m_wavefronts[waveNdx]->m_outside.m_center.y();
    double radm = m_wavefronts[waveNdx]->m_outside.m_radius + outsideOffset;
    double rado = m_wavefronts[waveNdx]->m_inside.m_radius + insideOffset;
    double cx = m_wavefronts[waveNdx]->m_inside.m_center.x();
    double cy = m_wavefronts[waveNdx]->m_inside.m_center.y();
    cv::Mat mask = cv::Mat::zeros(height,width,CV_8U);
    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            double dx = (double)(x - xm)/(radm);
            double dy = (double)(y - ym)/(radm);
            if (sqrt(dx * dx + dy * dy) <= 1.)
                mask.at<uchar>(y,x) = 255;
        }
    }

    if (rado > 0) {
        for (int y = 0; y < height; ++y){
            for (int x = 0; x < width; ++x){
                double dx = (double)(x - (cx))/(rado);
                double dy = (double)(y - (cy))/(rado);
                if (sqrt(dx * dx + dy * dy) < 1.)
                    mask.at<uchar>(y,x) = 0;
            }
        }

    }
    m_wavefronts[waveNdx]->mask = mask;
    m_wavefronts[waveNdx]->workMask = mask.clone();

        // add central obstruction
        mirrorDlg *md = mirrorDlg::get_Instance();
        double r = md->obs * (2. * radm)/md->diameter;
        r/= 2.;
        if (r > 0){
            cv::Mat m = m_wavefronts[waveNdx]->workMask;
            circle(m,Point(m.cols/2,m.cols/2),r, Scalar(0),-1);
        }
}
void SurfaceManager::wftNameChanged(int ndx, QString name){
    m_wavefronts[ndx]->name = name;
    qDebug() << name;
}

void SurfaceManager::sendSurface(wavefront* wf){
    emit currentNdxChanged(m_currentNdx);
    computeMetrics(wf);

    m_contourPlot->setSurface(wf);
    m_profilePlot->setSurface(wf);
    m_oglPlot->setSurface(wf);
    m_simView->setSurface(wf);


    QFile fn(wf->name);
    QFileInfo fileInfo(fn.fileName());
    QString filename(fileInfo.fileName());
    ((MainWindow*)(parent()))->setWindowTitle(filename);
}
void SurfaceManager::ObstructionChanged(){
    if (m_wavefronts.size() > 0)
    outsideMaskValue(outsideOffset);// use this function to trigger the mask making and recomputing.
}

void SurfaceManager::outsideMaskValue(int val){
    outsideOffset = val;
    double mmPerPixel = m_wavefronts[m_currentNdx]->diameter/(2 * (m_wavefronts[m_currentNdx]->m_outside.m_radius));
    m_surfaceTools->m_edgeMaskLabel->setText(QString().sprintf("%6.2lf mm",mmPerPixel* val));
    makeMask(m_currentNdx);
    wavefront *wf = m_wavefronts[m_currentNdx];
    wf->dirtyZerns = true;
    wf->useGBSmoothing = false;
    emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(1000);

}
void SurfaceManager::useDemoWaveFront(){
    int wx = 640;
    int wy = wx;
    double rad = (double)(wx-1)/2.d;
    double xcen = rad,ycen = rad;
    double rho;
    mirrorDlg *md = mirrorDlg::get_Instance();
    cv::Mat result = cv::Mat::zeros(wx,wx, CV_64F);
    for (int i = 0; i <  wx; ++i)
    {
        double x1 = (double)(i - (xcen)) / rad;
        for (int j = 0; j < wy; ++j)
        {
            double y1 = (double)(j - (ycen )) /rad;
            rho = sqrt(x1 * x1 + y1 * y1);

            if (rho <= 1.)
            {
                double S1 = md->z8 * -.9 * Zernike(8,x1,y1) + .02* Zernike(9, x1, y1);

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
    if (inprocess != 0)
        return;
    m_currentNdx = ndx;
    sendSurface(m_wavefronts[ndx]);
}
void SurfaceManager::deleteWaveFronts(QList<int> list){
    if (inprocess != 0)
        return;
    foreach(int ndx, list ){
        m_currentNdx = ndx;
        qDebug() << "delete " << ndx;
        deleteCurrent();
    }
}

void SurfaceManager::wavefrontDClicked(const QString & name){
    QString end = name;
    end.replace("\u2026","");
    for (int i = 0; i < m_wavefronts.size(); ++i){
        if (m_wavefronts[i]->name.endsWith(end)){
            m_currentNdx = i;
            sendSurface(m_wavefronts[i]);
            break;
        }
    }
}

void SurfaceManager::surfaceSmoothGBValue(int value){
    if (inprocess != 0)
        return;

    if (value %2 == 0) ++value;  // make sure blur radius is always odd;
    m_gbValue = value;

    QSettings settings;
    settings.setValue("GBValue", m_gbValue);
    double rad = 320.;
    if (m_wavefronts.size() != 0)
        rad = m_wavefronts[m_currentNdx]->m_outside.m_radius-1;
    mirrorDlg *md = mirrorDlg::get_Instance();
    double mmPerPixel = md->diameter/(2 * rad);
    m_surfaceTools->setBlurText(QString().sprintf("%6.2lf mm",m_gbValue* mmPerPixel));
    if (m_wavefronts.size() == 0)
        return;
    emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(1000);
}
void SurfaceManager::surfaceSmoothGBEnabled(bool b){
    if (inprocess != 0)
        return;
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
    emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(1000);
}

void SurfaceManager::computeMetrics(wavefront *wf){
    mirrorDlg *md = mirrorDlg::get_Instance();
    cv::Scalar mean,std;

    cv::meanStdDev(wf->workData,mean,std,wf->workMask);
    wf->mean = mean.val[0] * md->lambda/550.;
    wf->std = std.val[0]* md->lambda/550.;

    double mmin;
    double mmax;
    minMaxIdx(wf->workData, &mmin,&mmax);
    wf->min = mmin * md->lambda/550.;
    wf->max = mmax * md->lambda/550.;


    ((MainWindow*)(parent()))->zernTablemodel->setValues(wf->InputZerns);

    ((MainWindow*)(parent()))->updateMetrics(*wf);

}


void SurfaceManager::computeZerns()
{
    if (m_wavefronts.size() == 0)
        return;
    for (int i = 0; i < m_wavefronts.size(); ++i){
        wavefront *wf = m_wavefronts[m_currentNdx];
        wf->dirtyZerns = true;
        wf->useGBSmoothing = false;
    }
    emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(1000);

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
                file << wf->workData.at<double>(row,col) << std::endl;
            else
                file << wf->data.at<double>(row,col) << std::endl;
        }
    }

    file << "outside ellipse " << wf->m_outside.m_center.x()
         << " " << wf->m_outside.m_center.y()
         << " " << wf->m_outside.m_radius
         << " "  << wf->m_outside.m_radius << std:: endl;

    file << "obstruction ellipse " << wf->m_inside.m_center.x()
         << " " << wf->m_inside.m_center.y()
         << " " << wf->m_inside.m_radius
         << " " << wf->m_inside.m_radius << std:: endl;

    file << "DIAM " << wf->diameter << std::endl;
    file << "ROC " << wf->roc << std::endl;
    file << "Lambda " << wf->lambda << std::endl;
}

void SurfaceManager::SaveWavefronts(bool saveNulled){
    QList<int> list = m_surfaceTools->SelectedWaveFronts();
    if (list.size() <= 1 && m_wavefronts.size() > 0) {
        QSettings settings;
        QString lastPath = settings.value("lastPath","").toString();
        QString fileName = QFileDialog::getSaveFileName(0,
                                                        tr("Write wave font file"), lastPath + "/" + m_wavefronts[m_currentNdx]->name,
                                                        tr("wft (*.wft)"));
        if (fileName.isEmpty())
            return;
        if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".wft"); }
        QString lastDir = QFileInfo(fileName).absoluteDir().path();
        settings.setValue("lastPath", lastDir);
        writeWavefront(fileName, m_wavefronts[m_currentNdx], saveNulled);

    }
    else if (list.size() > 1){
        QSettings settings;
        QString path = settings.value("mirrorConfigFile").toString();
        QFile fn(path);
        QFileInfo info(fn.fileName());
        QString dd = info.dir().absolutePath();

        QString dir = QFileDialog::getExistingDirectory(0, tr("Open Directory")
                                                        ,dd,
                                                        QFileDialog::ShowDirsOnly
                                                        | QFileDialog::DontResolveSymlinks);
        if (dir == "")
            return;
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
            QString fullPath = dir + QDir::separator() + fname;
            QFileInfo info;
            // check if file exists and if yes: Is it really a file and no directory?
            int cnt = 1;
            while (info.exists(fullPath)) {
                fullPath = dir + QDir::separator() + fname + QString().sprintf("_%d.wft", cnt++);;
            }
            writeWavefront(fullPath, wf, saveNulled);

        }
    }
}
void SurfaceManager::createSurfaceFromPhaseMap(cv::Mat phase, CircleOutline outside, CircleOutline center, QString name){

    wavefront *wf;

    if (m_wavefronts.size() >0 && (m_currentNdx == 0 &&  m_wavefronts[0]->name == "Demo")){
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
    wf->useGBSmoothing = false;
    m_currentNdx = m_wavefronts.size()-1;
    makeMask(m_currentNdx);
    m_surface_finished = false;
    emit generateSurfacefromWavefront(m_currentNdx, this);
    while (!m_surface_finished) {qApp->processEvents();}
    // check for swapped conic value
    if (md->cc * wf->InputZerns[8] < 0.){
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
            wf->dirtyZerns = true;
            wf->useGBSmoothing = false;
            emit generateSurfacefromWavefront(m_currentNdx, this);
        }
    }

    emit showTab(2);

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
        wf = m_wavefronts[0];
        emit nameChanged(wf->name, fileName);
        wf->name = fileName;
        wf->dirtyZerns = true;
    }
    else {
        wf = new wavefront();
        m_wavefronts << wf;

        wf->name = fileName;

        m_surfaceTools->addWaveFront(wf->name);
        m_currentNdx = m_wavefronts.size()-1;
    }
    double width;
    double height;
    file >> width;
    file >> height;

    cv::Mat data(height,width, CV_64F,0.);

    for( size_t y = 0; y < height; y++ ) {
        for( size_t x = 0; x < width; x++ ) {
            file >> data.at<double>(height - y-1,x);
        }
    }

    string line;
    QString l;
    mirrorDlg *md = mirrorDlg::get_Instance();

    double xm = width/2.,ym = (height)/2.,radm = min(xm,ym)-2 , roc = md->roc, lambda = md->lambda, diam = md->diameter;
    double xo = width/2., yo = height/2., rado = 0;

    string dummy;
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
            qDebug() << line.c_str();
            iss >> dummy >> dummy >> xo >> yo >> rado;
            continue;
        }
    }

    wf->m_outside = CircleOutline(QPointF(xm,height - ym), radm);
    if (rado == 0){
        xo = xm;
        yo = ym;
    }
    wf->m_inside = CircleOutline(QPoint(xo,yo), rado);

    wf->diameter = diam;


    if (roundl(diam * 10) != roundl(md->diameter* 10))
    {
        QString message("The mirror diameter (");
        message += QString().sprintf("%6.3lf",diam) +
                ") Of the wavefront does not match the config value of " + QString().sprintf("%6.3lf\n",md->diameter) +
                "Do you want to make the config match?";

        sync.lock();
        emit showMessage(message);
        pauseCond.wait(&sync);
        sync.unlock();
        if (messageResult == QMessageBox::Yes){
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
        sync.lock();

        emit showMessage(message);
        pauseCond.wait(&sync);
        sync.unlock();
        if (messageResult == QMessageBox::Yes){
            emit rocChanged(roc);
            mirrorParamsChanged = true;
        }
        else {
            roc = md->roc;

        }
    }

    wf->data= data;
    wf->roc = roc;
    wf->lambda = lambda;
    wf->useGBSmoothing = false;

    makeMask(m_currentNdx);

    //cv::Mat tt = data.clone();
    //cv::normalize(tt,tt,0.f,1.f,CV_MINMAX);
    //cv::imshow(" wf ", tt);
    //cv::imshow(" mask ", wf->mask);
    //cv::waitKey(1);
    emit generateSurfacefromWavefront(m_currentNdx, this);

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
        if (wf->useGBSmoothing != m_GB_enabled || wf->GBSmoothingValue != m_gbValue) {

            cv::GaussianBlur( wf->nulledData.clone(), wf->workData, cv::Size( m_gbValue, m_gbValue ),0,0);
        }
    }
    else if (wf->useGBSmoothing == true) {
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

    sendSurface(m_wavefronts[m_currentNdx]);;
}
QVector<int> histo(const std::vector<double> data, int bins, double min, double max){
    QVector<int> h(bins, 0);
    double interval = (max - min)/bins;
    for (unsigned int i = 0; i < data.size(); ++i){
        double to = interval;
        for (int j = 0; j <  bins; ++j){
            if (data[i] < to) {
                ++h[j];
                qDebug() << "hist " << j << " " << data[i];
                break;
            }
            to += interval;
        }
    }
    return h;
}

void SurfaceManager::saveAllWaveFrontStats(){
    if (m_wavefronts.size() == 0)
        return;
    if (!m_statsDlg->exec())
        return;
    if (m_wftStats == 0)
        m_wftStats = new wftStats((mirrorDlg::get_Instance()));
    if (m_statsDlg->removeAboveLimit) {
        for (int i = m_wavefronts.size(); i > 0; --i){
            if (m_wavefronts[i-1]->std > m_statsDlg->RMSLimitValue ){
                m_currentNdx = i-1;
                deleteCurrent();
            }
        }
    }
    m_wftStats->computeWftStats(m_wavefronts,m_currentNdx);
    m_wftStats->computeZernStats(m_currentNdx);
    m_wftStats->computeWftRunningAvg(m_wavefronts,m_currentNdx);
    while (m_statsDlg->removeOutliers && m_wftStats->outliers.size()){
        qSort(m_wftStats->outliers.begin(), m_wftStats->outliers.end(), qGreater<int>());
        deleteWaveFronts(m_wftStats->outliers);
        m_wftStats->computeWftStats(m_wavefronts,m_currentNdx);
        m_wftStats->computeZernStats(m_currentNdx);
        m_wftStats->computeWftRunningAvg(m_wavefronts,m_currentNdx);
    }

    m_wftStats->makeWftPlot(m_wavefronts, m_currentNdx, m_statsDlg );
    m_wftStats->makeZernPlot();
    m_wftStats->makeHistoPlot();
    return;

    QSettings settings;
    QString path = settings.value("mirrorConfigFile").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString csvName;
    bool saveStats = false;
    if (m_statsDlg->saveCsv){
        csvName = QFileDialog::getSaveFileName(0,
                                               tr("Save stats CSV file"), path,
                                               tr("csv (*.csv)"));
        if (!csvName.isEmpty())
            saveStats = true;

    }

    QString dir = info.dir().path();
    dir = dir.right(dir.size() - dir.lastIndexOf("/")-1);
    QString title = m_wavefronts[0]->name;
    title = title.left(title.lastIndexOf("/"));
    title = title.right(title.size() - title.lastIndexOf(dir));
    QString t2 = title.replace("/","_");
    QString fName = info.dir().path() + "\\" + t2 +  QString(".csv");
    std::ofstream file;

    if (saveStats){

        file.open((csvName.toStdString().c_str()));
        if (!file.is_open()) {
            QMessageBox::warning(0, tr("Save Stats csv file"),
                                 tr("Cannot write file %1: ")
                                 .arg(fName));
            return;
        }
    }
    else
        file.open("/Dev/Null");


    file << title.toStdString() << ",'wavefront RMS'"<< ",,Z4,Z5,Z6,Z7,Z8,Z9,Z10 " << std::endl;
    QString out;
    cv::Mat mZerns(m_wavefronts.size(),Z_TERMS,CV_64F,0.);
    int row = 0;
    mirrorDlg *md = mirrorDlg::get_Instance();

    QPolygonF spherical;
    QPolygonF sphericaRunningAvg;
    double sperAvg = 0.;
    QPolygonF coma;
    QPolygonF otherTerm;
    QVector<int> trueNdx;
    for (int i = 0; i < m_wavefronts.size(); ++i)
    {
        wavefront *wf = m_wavefronts[(m_currentNdx + i )% m_wavefronts.size()];
        QPolygonF zernPoints;
        QFile fn(wf->name);
        QFileInfo fileInfo(fn.fileName());
        QString filename(fileInfo.fileName());
        QString out(filename );
        file << filename.toStdString() << ",";
        file << wf->std << ",";

        for(int ndx = 0; ndx < Z_TERMS; ++ndx){
            out+= ",";
            out += QString::number(wf->InputZerns[ndx]);
            file << "," << QString::number(wf->InputZerns[ndx]).toStdString();
            double v = wf->InputZerns[ndx];

            // apply software Null if needed
            if (ndx == 8 and md->doNull)
                v -= md->z8 * md->cc;
            double Sigma = computeRMS(ndx,v) * md->lambda/550.;

            if (ndx == 8) {
                spherical << QPointF(row,Sigma);
                sperAvg += Sigma;
                sphericaRunningAvg << QPointF(row,sperAvg/(i+1));
            }

            mZerns.at<double>(row,ndx) =  Sigma * md->lambda/550.;

        }

        file << std::endl;
        ++row;

    }
    file.close();

    QVector<QwtIntervalSample> zernMinMax;
    QVector<QwtIntervalSample> zernStd;
    QVector<QString> zNames;
    QVector< QVector < double> > zerns;  //each row has the zerns for that sample
    int col = 4;
    for (int zern = 4; zern <= 35; ++zern){
        QString zname(zernsNames[zern]);
        bool isPair = !QString(zernsNames[zern]).contains("Spheri");
        cv::Mat c = mZerns.col(zern);
        QVector<double> zpoints;
        for (int i = 0; i < c.rows; ++i){

            int row = (i + m_currentNdx) % c.rows;
            double v = c.at<double>(row);
            if (isPair){
                cv::Mat c2 = mZerns.col(zern+1);
                double v2 = c2.at<double>(row);
                double s = sqrt(v * v + v2 * v2);
                v = s;
                zname += " ";
                zname += zernsNames[zern+1];
            }

            if (i == 0) {
                zNames << zname;
                //qDebug() << zname;
            }
            zpoints << v;
        }
        zerns << zpoints;
        std::vector<double > zvec = zpoints.toStdVector();
        cv::Mat aSampleOfZerns(zvec);
        std::sort( zvec.begin(), zvec.end() ); // sort the Zvalues
        double median;
        int sampleCnt = zvec.size();
        if (sampleCnt % 2==0) {
            median = (zvec[zvec.size()/2-1]+zvec[zvec.size()/2])/2;
        }
        else {
            median = zvec[(zvec.size()-1)/2];
        }
        double top = zvec[sampleCnt * .25];
        double bottom = zvec[sampleCnt * .75];
        double min;
        double max;
        minMaxIdx(aSampleOfZerns, &min,&max);

        zernMinMax.append( QwtIntervalSample( col, min, max ) );
        //qDebug() << "minmax " << col << " " << min << " " << max;

        zernStd.append(QwtIntervalSample(col,bottom,top));
        zernStd.append(QwtIntervalSample(col++, bottom, median));
        if (isPair) ++zern;
    }



    // normalize the size to the most common size
    int last = m_wavefronts.length();
    QHash<QString,int> sizes;
    for (int i = 0; i < last; ++i){
        QString size;
        size.sprintf("%d %d",m_wavefronts[i]->workData.rows, m_wavefronts[i]->workData.cols);
        if (*sizes.find(size))
        {
            ++sizes[size];
        }
        else
            sizes[size] = 1;
    }
    int max = 0;
    QString maxkey;
    foreach(QString v, sizes.keys()){
        int a = sizes[v];
        if (a > max) {
            max = a;
            maxkey = v;
        }
    }
    int rrows, rcols;

    QTextStream s(&maxkey);

    s >> rrows >> rcols;
    cv::Mat mask = m_wavefronts[0]->workMask.clone();
    cv::resize(mask,mask,Size(rcols,rrows));
    QVector<wavefront*> twaves = m_wavefronts;



    QwtPlot *pl = new QwtPlot(0);//m_statsDlg->m_wavefrontPlot;//new QwtPlot(QwtText(info2.baseName()),0);
    //pl->detachItems( QwtPlotItem::Rtti_PlotCurve);
    QwtPlotCurve *curve1;
    QwtPlotCurve* curve2 = new QwtPlotCurve();

    QPolygonF wftPoints;

    QPolygonF avgPoints;
    QPolygonF outliersInner;
    QPolygonF outliersOuter;



    cv::Mat sum = cv::Mat::ones(rrows,rcols, m_wavefronts[m_currentNdx]->workData.type());

    curve1 = new QwtPlotCurve(QString().sprintf("RMS Running Average"));


    if (m_statsDlg->doComa){
        QwtPlotCurve *curveComa = new QwtPlotCurve("Coma");
        curveComa->setSamples(coma);
        curveComa->setPen(QColor("SteelBlue"),2);
        curveComa->attach(pl);
    }


    if (m_statsDlg->doSA){
        QwtPlotCurve *curveSA = new QwtPlotCurve("SA");

        curveSA->setSamples(spherical);
        curveSA->setPen(Qt::magenta,2);
        curveSA->attach(pl);
        QwtPlotCurve *SAA = new QwtPlotCurve("SA Avg");
        SAA->setSamples(sphericaRunningAvg);
        SAA->setPen(Qt::magenta,1);
        SAA->attach(pl);
    }

    int samndx[last];
    for (int i = 0; i < last; ++i)
        samndx[i] = i;
    std::random_shuffle (samndx,samndx + last -1);

    for (int j = 0; j < last; ++j){
        int i = (m_currentNdx + j) % m_wavefronts.size();
        //i = samndx[j];
        cv::Mat resized = twaves[i]->workData.clone();
        if (twaves[i]->workData.rows != rrows || twaves[i]->workData.cols != rcols){
            cv::resize(twaves[i]->workData,resized, Size(rcols, rrows));
        }
        sum += resized;
        cv::Mat avg = sum/(j+1);
        cv::Scalar mean,std;
        cv::meanStdDev(resized,mean,std,mask);
        double stdi = std.val[0]* md->lambda/550.;
        cv::meanStdDev(avg,mean,std,mask);
        avgPoints << QPointF(j,std.val[0] * md->lambda/550.);
        wftPoints << QPointF(j,stdi);
        trueNdx << i;
    }

    QwtPlot *hp = new QwtPlot(0);
    // create wavefront stats
    {
        cv::Mat wftStatsx(avgPoints.size(), 1, CV_64F);  // used to compute std and mean
        std::vector<double> vecWftRMS;    // used to compute quartiles

        for (int ndx = 0; ndx < wftPoints.size(); ++ndx){
            wftStatsx.at<double>(ndx) = wftPoints[ndx].y();
            vecWftRMS.push_back( wftPoints[ndx].y());
        }

        std::sort( vecWftRMS.begin(), vecWftRMS.end() ); // sort vecFromMat
        double median;
        int sampleCnt = vecWftRMS.size();
        if (sampleCnt % 2==0) {
            median = (vecWftRMS[vecWftRMS.size()/2-1]+vecWftRMS[vecWftRMS.size()/2])/2;
        }
        else {
            median = vecWftRMS[(vecWftRMS.size()-1)/2];
        }
        double bottom = vecWftRMS[sampleCnt * .25];
        double top = vecWftRMS[sampleCnt * .75];
        // outlier detection http://www.itl.nist.gov/div898/handbook/prc/section1/prc16.htm

        double IQ = top - bottom;
        double lowerOuterFence = bottom - 3. * IQ;
        double upperOuterFence = top + 3. * IQ;
        double lowerInnerFence = bottom - 1. * IQ;
        double upperInnerFence = top + 1.5 * IQ;
        qDebug() << upperInnerFence;
        QList<int> outliers;
        for (int ndx = 0; ndx < wftPoints.size(); ++ndx){
            double v = wftStatsx.at<double>(ndx);
            qDebug() << trueNdx[ndx] << " " << v << " " << m_wavefronts[trueNdx[ndx]]->name;
            if (v <= lowerOuterFence || v >= upperOuterFence){
                outliersOuter << QPointF(2,v);
                outliers << trueNdx[ndx];
                qDebug() << "outer " << v;
            }
            else if (v <= lowerInnerFence || v >= upperInnerFence) {
                outliersInner << QPointF(2,v);
                outliers << trueNdx[ndx];
                qDebug() << "inner " << v;
            }
        }
        if (m_statsDlg->removeOutliers && outliers.size()){
            qSort(outliers.begin(), outliers.end(), qGreater<int>());
            deleteWaveFronts(outliers);
            //saveAllWaveFrontStats();
            return;
        }
        cv::Scalar mean,std;
        cv::meanStdDev(wftStatsx,mean,std);
        double min;
        double maxx;
        minMaxIdx(wftStatsx, &min,&maxx);
        zernMinMax.append( QwtIntervalSample( 2, min, maxx ) );
        zernStd.append(QwtIntervalSample(2,top,bottom));
        zernStd.append(QwtIntervalSample(2, top, median));
        int histSize = 11;
        double fminx = .5 * min;
        double fmaxx = (double)maxx;


        QVector<int> hist = histo(vecWftRMS, 11, fminx,fmaxx);
        QwtPlotHistogram *histPlot = new QwtPlotHistogram("Wave Fronts RMS");
        //QwtPlotBarChart *histPlot = new QwtPlotBarChart("Wave Fronts RMS");
        QVector<QwtIntervalSample> histData;
        double interval = (fmaxx - fminx)/histSize;
        for (int i = 0; i < hist.size(); ++i){
            QwtInterval interv(i * interval, (i+1)*interval);
            //qDebug() << interv.minValue() << " " << interv.maxValue() << " " << hist[i];
            histData << QwtIntervalSample((double)hist[i],interv);

        }

        histPlot->setSamples(histData);

        histPlot->attach(hp);
        hp->setAxisTitle(QwtPlot::xBottom,"RMS");
        hp->setTitle("Wave Front RMS Histogram");
        hp->resize(640,480);
        hp->show();

    }
    curve1->setSamples(avgPoints);
    curve1->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    curve1->setPen(Qt::red,2);
    curve1->attach(pl);
    std::random_shuffle (twaves.begin(),twaves.end());
    if (m_statsDlg->doInputs)
        curve2->setSamples(wftPoints);


    const char *colors[] =
    {
        "Green",
        "LightSalmon",
        "SteelBlue",
        "Yellow",
        "Fuchsia",
        "PaleGreen",
        "PaleTurquoise",
        "Cornsilk",
        "HotPink",
        "Peru",
        "Maroon"
    };
    // draw all zern graphs

    if (m_statsDlg->doZernGroup){
        const int zernToCombinedNx[] = {0,0,0,0,0,0,1,1,2,3,3,4,4,5,5,6,7,7,8,8,9,9,10,10,11,
                                        12,12,13,13,14,14,15,15,16,16,17};

        for (int zern = zernToCombinedNx[m_statsDlg->zernFrom]; zern <= zernToCombinedNx[m_statsDlg->zernTo]; ++zern){
            QwtPlotCurve *zcurve = new QwtPlotCurve(QString().sprintf("%s",zNames[zern].toStdString().c_str()));
            QPolygonF points;
            int cnt = 0;
            foreach(double v, zerns[zern]){
                points << QPointF(cnt++,v);
            }

            zcurve->setSamples(points);
            zcurve->setPen(QColor(colors[zern%10]),2);
            zcurve->attach(pl);
        }
    }

    pl->setCanvasBackground(QColor(Qt::black));
    pl->enableAxis( QwtPlot::yRight );
    curve2->setPen(Qt::red,1);


    QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
                                       QBrush( Qt::red ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
    curve2->setSymbol( symbol );

    curve2->attach(pl);
    curve2->setTitle("Input .wft's");

    pl->insertLegend( new QwtLegend() , QwtPlot::TopLegend);
    QFont label_font("Helvetica");
    label_font.setPointSize(10);
    pl->legend()->setFont(label_font);
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen( Qt::white, 0, Qt::DotLine );
    grid->setMinorPen( Qt::gray, 0 , Qt::DotLine );
    grid->attach(pl);
    // finally, refresh the plot

    pl->setTitle(title);



    pl->replot();
    QwtScaleDiv sd = pl->axisScaleDiv(QwtPlot::yLeft);
    double up = sd.upperBound();
    double lower = sd.lowerBound();

    pl->setAxisScaleDraw(QwtPlot::yRight,new StrehlScaleDraw());
    QwtScaleEngine * se1 = pl->axisScaleEngine(QwtPlot::yRight);
    QwtScaleDiv sd1 = se1->divideScale(lower,up, 20,1);
    pl->setAxisScaleDiv(QwtPlot::yRight, sd1);
    if (m_statsDlg->showFileNames)
        pl->setAxisScaleDraw(QwtPlot::xBottom, new wftNameScaleDraw(m_wavefronts, m_currentNdx));
    QwtScaleEngine *se = pl->axisScaleEngine(QwtPlot::xBottom) ;
    double x1 = 0;
    double x2 = m_wavefronts.size()-1;
    double step =  ceilf(x2/20);

    QwtScaleDiv sd2 = se->divideScale(x1, x2, x2, 1, step);
    pl->setAxisScaleDiv(QwtPlot::xBottom,sd2);
    //pl->setAxisScale(QwtPlot::yRight,lower, up );
    pl->setAxisTitle(0,"RMS");
    pl->setAxisTitle(QwtPlot::yRight,"Strehl");
    pl->setAxisTitle(QwtPlot::xBottom,"Wave Front");
    pl->resize(800,600);
    pl->replot();
    QwtScaleDiv stldiv = pl->axisScaleDiv(QwtPlot::xBottom);
    pl->show();


    QwtIntervalSymbol *bar = new QwtIntervalSymbol( QwtIntervalSymbol::Bar );
    QwtIntervalSymbol *box = new QwtIntervalSymbol( QwtIntervalSymbol::Box );
    QwtPlot *myPlot = new QwtPlot(0);//  m_statsDlg->m_zernPlot;
    myPlot->detachItems( QwtPlotItem::Rtti_PlotCurve);
    QwtPlotIntervalCurve *zernMinMaxCurve = new QwtPlotIntervalCurve("minmax");
    QwtPlotIntervalCurve *zernStdCurve = new QwtPlotIntervalCurve("std");
    zernMinMaxCurve->setSymbol( bar );
    zernMinMaxCurve->setSamples( zernMinMax );
    zernMinMaxCurve->attach( myPlot );
    zernMinMaxCurve->setStyle( QwtPlotIntervalCurve::NoCurve );
    myPlot->setAxisTitle(QwtPlot::xBottom, "Zernike terms");
    myPlot->setAxisTitle(QwtPlot::yLeft, "RMS");
    myPlot->setAxisScale(QwtPlot::xBottom,0,22,1);
    myPlot->setAxisScaleDraw( QwtPlot::xBottom, new ZernScaleDraw(zNames) );
    zernStdCurve->setSymbol( box );
    zernStdCurve->setSamples( zernStd );
    zernStdCurve->attach( myPlot);
    zernStdCurve->setStyle( QwtPlotIntervalCurve::NoCurve );

    if (outliersInner.size()){
        QwtPlotCurve *outers = new QwtPlotCurve("Outer outliers");

        outers->setPen(Qt::blue,1);


        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::HLine,
                                           QBrush( Qt::blue ), QPen( Qt::magenta, 2 ), QSize( 10, 5 ) );
        outers->setSymbol( symbol );
        outers->setSamples(outliersInner);
        outers->attach(myPlot);
    }
    if (outliersOuter.size()){
        QwtPlotCurve *outers = new QwtPlotCurve("Outer outliers");

        outers->setPen(Qt::red,1);


        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::HLine,
                                           QBrush( Qt::red ), QPen( Qt::red, 2 ), QSize( 10, 5 ) );
        outers->setSymbol( symbol );
        outers->setSamples(outliersOuter);
        outers->attach(myPlot);
    }
    myPlot->resize(800,400);
    myPlot->replot();
    myPlot->show();

    fName.replace(".csv",".pdf");
    if (m_statsDlg->savePDF){
        QString fileName = QFileDialog::getSaveFileName(0,
                                                        tr("Save stats pdf file"), fName,
                                                        tr("pdf (*.pdf)"));
        if (fileName.isEmpty())
            return;
        if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
        QString fname = fileName;
        const QSizeF sizeMM(200,300);

        const double mmToInch = 1.0 / 25.4;
        const QSizeF size = sizeMM * mmToInch * 85;

        const QRectF topRect( 0.0, 10.0, size.width(), size.height()/3 );

        const QString fmt("pdf");
        fname.replace(".csv",".pdf");

        QPrinter printer(QPrinter::HighResolution);
        printer.setColorMode( QPrinter::Color );
        printer.setFullPage( true );
        printer.setPaperSize( sizeMM, QPrinter::Millimeter );
        printer.setDocName( title );
        printer.setOutputFileName( fname );
        printer.setOutputFormat( QPrinter::PdfFormat );
        printer.setResolution(85);

        QPainter painter( &printer );

        QwtPlotRenderer renderer;
        renderer.render( pl, &painter, topRect );


        QRectF d1(10,size.height()/3+20, size.width()-20,size.height()/3-10);
        renderer.render(myPlot, &painter, d1 );

        QRectF d2(10,2 * size.height()/3+20, size.width()-20,size.height()/3-10);
        renderer.render(hp, &painter, d2);
    }
    if (m_statsDlg->saveJpg){

        fName.replace(".pdf",".jpg");
        const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
        QStringList filter;
        if ( imageFormats.size() > 0 )
        {
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
        }
        fName = QFileDialog::getSaveFileName(0,
                                             tr("Save stats plot"), fName,filter.join( ";;" ));


        if (fName.isEmpty())
            return;

        const QSizeF size(800,800);
        const QRect imageRect = QRect(0,0,size.width(),size.height());
        const QRectF topRect( 0.0, 1.0, size.width(), size.height()/2 - 20 );
        QImage image( imageRect.size(), QImage::Format_ARGB32 );

        image.fill( QColor( Qt::white ).rgb() );

        QPainter painter( &image );
        QwtPlotRenderer renderer;
        renderer.render( pl, &painter, topRect );
        QRectF d1(0,size.height()/2+10, size.width(),size.height()/2-10);
        renderer.render(myPlot, &painter, d1 );
        painter.end();

        image.save( fName, QFileInfo( fName ).suffix().toLatin1() );

    }
}
void SurfaceManager::enableTools(){

    m_toolsEnableTimer->stop();
    qDebug() <<"timer"<<inprocess;
    if (inprocess == 0){
        m_surfaceTools->setEnabled(true);
        qDebug() << "enabling tools";
    }
}

void SurfaceManager::surfaceGenFinished(int ndx) {
    m_toolsEnableTimer->start(1000);
    if (workToDo > 0)
        emit progress(++workProgress);
    qDebug() << inprocess <<"finnished " << ndx << m_wavefronts[ndx]->name;
    mutex.lock();
    --inprocess;

    mutex.unlock();
    if (ndx == m_currentNdx && inprocess == 0)
        sendSurface(m_wavefronts[ndx]);
    else
        computeMetrics(m_wavefronts[ndx]);

    m_surface_finished = true;
    if (workProgress == workToDo)
        workToDo = 0;

}

// Update all surfaces since some control has changed.  Skip current surface it has already been done
void SurfaceManager::backGroundUpdate(){
    m_waveFrontTimer->stop();
    workProgress = 0;
    qDebug() << "disabling tools";
    if (m_wavefronts.size() > 1)
        m_surfaceTools->setEnabled(false);
    workToDo = m_wavefronts.size()-1;
    pd->setLabelText("Updating all Surfaces");
    pd->setRange(0,workToDo);
    for (int i = 1; i < m_wavefronts.size(); ++i){
        int ndx = (i + m_currentNdx) % m_wavefronts.size();
        m_wavefronts[ndx]->dirtyZerns = true;
        emit generateSurfacefromWavefront(ndx, this);
    }
}


void SurfaceManager::average(QList<int> list){
    QList<wavefront *> wflist;
    for (int i = 0; i < list.size(); ++i){
        wflist.append(m_wavefronts[list[i]]);
    }
    average(wflist);
}
void SurfaceManager::average(QList<wavefront *> wfList){


    // normalize the size to the most common size

    QHash<QString,int> sizes;
    for (int i = 0; i < wfList.size(); ++i){
        QString size;
        size.sprintf("%d %d",wfList[i]->workData.rows, wfList[i]->workData.cols);
        if (*sizes.find(size))
        {
            ++sizes[size];
        }
        else
            sizes[size] = 1;
    }
    int max = 0;
    QString maxkey;
    foreach(QString v, sizes.keys()){
        int a = sizes[v];
        if (a > max) {
            max = a;
            maxkey = v;
        }
    }
    int rrows, rcols;

    QTextStream s(&maxkey);

    s >> rrows >> rcols;
    cv::Mat mask = m_wavefronts[0]->workMask.clone();
    cv::resize(mask,mask,Size(rcols,rrows));
    cv::Mat sum = cv::Mat::ones(rrows,rcols, m_wavefronts[m_currentNdx]->workData.type());
    for (int j = 0; j < wfList.size(); ++j){
        cv::Mat resized = wfList[j]->data.clone();
        if (wfList[j]->data.rows != rrows || wfList[j]->data.cols != rcols){
            cv::resize(wfList[j]->data, resized, Size(rcols, rrows));
        }
        sum += resized;
    }
    sum = sum/wfList.size();
    wavefront *wf = new wavefront();
    *wf = *wfList[0];
    wf->data = sum;
    wf->mask = mask;
    wf->workMask = mask.clone();
    m_wavefronts << wf;
    wf->useGBSmoothing = false;
    wf->name = "Average";
    wf->dirtyZerns = true;
    m_surfaceTools->addWaveFront(wf->name);
    m_currentNdx = m_wavefronts.size()-1;
    emit generateSurfacefromWavefront(m_currentNdx, this);
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
        cv::Mat tmp = oldWf->data.clone();
        wf->data = tmp;
        m_wavefronts << wf;
        m_surfaceTools->addWaveFront(wf->name);
        m_currentNdx = m_wavefronts.size()-1;
        cv::Point2f ptCp(tmp.cols*0.5, tmp.rows*0.5);
        cv::Mat M = cv::getRotationMatrix2D(ptCp, angle, 1.0);
        cv::warpAffine(tmp, wf->data , M, tmp.size(), cv::INTER_CUBIC);
        wf->m_inside = oldWf->m_inside;
        wf->m_outside = oldWf->m_outside;


        // rotate mask too
        cv::Mat tmpMask;
        cv::warpAffine(oldWf->workMask, wf->workMask, M, oldWf->workMask.size(), cv::INTER_CUBIC);
        wf->mask = oldWf->workMask.clone();
        wf->workMask = wf->mask.clone();
        wf->dirtyZerns = true;
        wf->useGBSmoothing = false;
        m_surface_finished = false;
        emit generateSurfacefromWavefront(m_currentNdx,this);
        while (!m_surface_finished) {qApp->processEvents();}
    }
}
void SurfaceManager::subtract(wavefront *wf1, wavefront *wf2){

    int size1 = wf1->data.rows * wf1->data.cols;
    int size2 = wf2->data.rows * wf2->data.cols;
    cv::Mat resize = wf2->data.clone();
    if (size1 != size2) {
        cv::resize(resize,resize, cv::Size(wf1->data.cols, wf1->data.rows));
    }
    cv::Mat result = wf1->data - resize;
    wavefront *resultwf = new wavefront;
    *resultwf = *wf1;
    resultwf->data = result.clone();
    m_wavefronts << resultwf;
    QStringList n1 = wf1->name.split("/");
    QStringList n2 = wf2->name.split("/");
    resultwf->name = n1[n1.size()-1] + "-" + n2[n2.size()-1];
    m_surfaceTools->addWaveFront(resultwf->name);
    resultwf->dirtyZerns = true;
    resultwf->useGBSmoothing = false;
    m_currentNdx = m_wavefronts.size() -1;
    m_surface_finished = false;
    save_restore<bool> doNull(&(mirrorDlg::get_Instance()->doNull), false);
    emit generateSurfacefromWavefront(m_currentNdx,this);
    while (!m_surface_finished){qApp->processEvents();}

}

void SurfaceManager::subtractWavefronts(){
    QList<QString> list;
    for (int i = 0; i < m_wavefronts.size(); ++i){
        list.append(m_wavefronts[i]->name);
    }
    subtractWavefronatsDlg dlg(list);
    if (dlg.exec()){
        int ndx2 = dlg.getSelected();
        wavefront *wf1 = m_wavefronts[m_currentNdx];
        wavefront *wf2 = m_wavefronts[ndx2];
        subtract(wf1,wf2);
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
    pd->setLabelText("Rotating Wavefronts");
    pd->setRange(0, list.size());
    for (int i = 0; i < list.size(); ++i) {
        m_wavefronts[list[i]]->data *= -1;
        m_wavefronts[list[i]]->dirtyZerns = true;
        m_wavefronts[list[i]]->useGBSmoothing = false;
        emit generateSurfacefromWavefront(list[i],this);
    }
}

#include "wftexaminer.h"
wftExaminer *wex;
void SurfaceManager::inspectWavefront(){
    wex = new wftExaminer(m_wavefronts[m_currentNdx]);
    wex->show();
}
void SurfaceManager::surfaceBaseChanged(bool b) {

    useZernikeBase = b;
    emit generateSurfacefromWavefront(m_currentNdx, this);
    m_waveFrontTimer->start(1000);
}
#include <sstream>



textres SurfaceManager::Phase2(QList<rotationDef *> list, QList<int> inputs, int avgNdx ){
    QTextEdit *editor = new QTextEdit;

    QTextDocument *doc = editor->document();
    textres results;
    results.Edit = editor;
    const int Width = 400 * .9;
    const int Height = 300 * .9;
    QImage *contour =  new QImage(Width ,Height, QImage::Format_ARGB32 );

    QPrinter printer(QPrinter::HighResolution);
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( true );
    printer.setOutputFileName( "stand.pdf" );
    printer.setOutputFormat( QPrinter::PdfFormat );
    //printer.setResolution(85);
    printer.setPaperSize(QPrinter::A4);

    QPainter painterPDF( &printer );
    cv::Mat xastig = cv::Mat::zeros(list.size(), 1, CV_64F);
    cv::Mat yastig = cv::Mat::zeros(list.size(), 1, CV_64F);
    cv::Mat standxastig = cv::Mat::zeros(list.size(), 1, CV_64F);
    cv::Mat standyastig = cv::Mat::zeros(list.size(), 1, CV_64F);
    cv::Mat standastig = cv::Mat::zeros(list.size(), 1, CV_64F);
    QVector<cv::Mat> standwfs;
    QVector<double> astigMag;
    editor->resize(printer.pageRect().size());
    doc->setPageSize(printer.pageRect().size());
    for (int i = 0; i < list.size(); ++i){

        // rotate the average to match the input
        QList<int> toRotate;
        toRotate.append(avgNdx);
        int ndx = m_wavefronts.size(); // index of result of rotation
        m_surface_finished = false;
        rotateThese(-list[i]->angle,toRotate);
        while(!m_surface_finished){qApp->processEvents();}
        m_surface_finished = false;
        subtract(m_wavefronts[inputs[i]], m_wavefronts[ndx]);
        ++ndx;      // now ndx point to the stand only wavefront
        while(!m_surface_finished){qApp->processEvents();}

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

        //m_wavefronts[ndx]->InputZerns[8] = -mirrorDlg::get_Instance()->z8;
        cv::Mat m = computeWaveFrontFromZernikes(m_wavefronts[inputs[0]]->data.cols,m_wavefronts[ndx]->InputZerns, zernsToUse );

        standwfs << m;

        xastig.at<double>(i,0) = m_wavefronts[inputs[i]]->InputZerns[4];
        yastig.at<double>(i,0) = m_wavefronts[inputs[i]]->InputZerns[5];
    }

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
    QVector<QwtPlotCurve *> curves;
    QVector<QwtPlotMarker *> markers;
    QVector<QwtPlot *> astigPlots;

    QwtPlotRenderer renderer;
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend);
    QPainter painter( contour );


    double maxAstigMag = *(std::max_element(astigMag.begin(), astigMag.end()));

    QString html = "<html><head/><body><h1>Test Stand Astig Analysis</h1>"
            "<h3>Step 2. Stand induced astig at each rotation position:</h3>";
    html.append( "<table  style='ds margin-top:0px; margin-bottom:0px; margin-left:10px; margin-right:10px;'"
                 " width='70%' cellspacing='1' cellpadding='1'>"

                 "<tr><b><td>rotation angle</td><td>X astig</td><td>Y astig</td><td> magnitude</td><td>astig ange</td></b></tr>");
    for (int i = 0; i < list.size(); ++i){
        double xval = standxastig.at<double>(i,0);
        double yval = standyastig.at<double>(i,0);
        html.append("<tr><td><align = 'right'> " + QString().number(list[i]->angle,'f',3 ) + "</td>" );
        html.append("<td>" + QString().number(xval,'f',3) + "</td>"
                    "<td>" + QString().number(yval,'f',3) + "</td>"
                    "<td>" + QString().number(sqrt(pow(xval,2) + pow(yval,2)),'f',3) + "</td>"
                    "<td>" + QString().number((180./ M_PI) * atan2(yval,xval),'f',3) + "</td></right></tr>");
    }
    html.append("<tr></b><td>MEAN</b></td><td>" + QString().number(standXMean[0],'f',3) + "</td><td>" +
                QString().number(standYMean[0],'f',3) + "</td><td>" + QString().number(standMean[0],'f',3) + "</td></tr>");
    html.append("<tr><b><td><b>STD</b></td><td><ALIGN ='right'>" + QString().number(standXStd[0],'f',3) + "</td><td>" +
                QString().number(standyStd[0],'f',3) + "</td><td>" + QString().number(standStd[0],'f',3) + "</td></tr>");
    html.append("</table>");

    int cnt = 0;
    QString imagesHtml = "<table  style='ds margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;'"
                 " 'width='100%' cellspacing='2' cellpadding='0'>";
    for (int i = 0; i < list.size(); ++i){
        if (cnt++ == 0)
            imagesHtml.append("<tr>");
        // make plot of stand astig
        QwtPlotCurve *curve = new QwtPlotCurve(QString().number(list[i]->angle));
        curves << curve;
        QPolygonF points1;
        points1 << QPointF(0.,0.);
        points1 << QPointF(standxastig.at<double>(i,0), standyastig.at<double>(i,0));
        QColor color(Qt::GlobalColor( 6 + i%13 ) );
        QPen pen(color);
        curve->setPen(pen);
        curve->setSamples(points1);
        curve->attach(pl1);
        QwtPlotMarker *m = new QwtPlotMarker();
        m->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, color,color, QSize(5,5)));

        m->setYValue(m_wavefronts[inputs[i]]->InputZerns[5]);
        m->setXValue(m_wavefronts[inputs[i]]->InputZerns[4]);
        m->attach(pl1);

        // make contour plots of astig zernike terms of stand
        ContourPlot *cp = new ContourPlot();
        cp->m_zRangeMode = "Min/Max";
        wavefront * wf = new wavefront;
        wf = m_wavefronts[inputs[i]];
        wf->data = wf->workData = standwfs[i];
        cv::Scalar mean,std;
        cv::meanStdDev(wf->data,mean,std);
        wf->std = std[0];
        qDebug() << "std " << std[0];
        wf->min = -maxAstigMag * 2;
        wf->max =  maxAstigMag * 2;
        wf->name = QString().sprintf("%06.2lf",list[i]->angle);
        qDebug() << wf->name;
        cp->setSurface(wf);
        cp->resize(Width,Height);
        cp->replot();
        contour->fill( QColor( Qt::white ).rgb() );
        renderer.render( cp, &painter, QRect(0,0,Width,Height) );
        QString imageName = QString().sprintf("mydata://zern%s.png",wf->name.toStdString().c_str());
        imageName.replace("-","CCW");
        qDebug() << imageName;
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(*contour));
        results.res.append (imageName);
        imagesHtml.append("<td><p> <img src='" +imageName + "' /></p></td>");
        if (cnt == 2){
            cnt = 0;
            imagesHtml.append("</tr>");
        }
    }

    imagesHtml.append("</table>");
    pl1->setAxisScale(QwtPlot::xBottom,min(-.1,xmin),max(.1,xmax));
    pl1->setAxisScale(QwtPlot::yLeft, min(-.1, ymin), max(.1,ymax));
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
    meanMark->setSymbol(new QwtSymbol(QwtSymbol::Star1,QColor(255,255,255), QColor(0,0,0), QSize(30,30)));
    meanMark->setLabel(QString("Mean") );
    meanMark->setYValue(standYMean[0]);
    meanMark->setXValue(standXMean[0]);
    meanMark->setLabelAlignment( Qt::AlignTop );
    meanMark->attach(pl1);
    QPolygonF stdCircle;
    double SE = standStd[0]/sqrt(standastig.rows);
    for (double rho = 0; rho <= 2 * M_PI; rho += M_PI/32.){
        stdCircle << QPointF(standXMean[0]+SE * cos(rho),standYMean[0] + SE * sin(rho));
    }
    QwtPlotCurve *curveStandStd = new QwtPlotCurve("Stand Error");
    curveStandStd->setPen(Qt::darkYellow,3,Qt::DotLine );
    curveStandStd->setSamples(stdCircle);
    curveStandStd->attach(pl1);
    pl1->resize(300,300);
    pl1->replot();

    contour->fill( QColor( Qt::white ).rgb() );
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend, false);
    renderer.render( pl1, &painter, QRect(0,0,Width,Height) );
    QString imageName = "mydata://plot.png";
    doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(*contour));
    results.res.append (imageName);
    html.append("<p> <img src='" + imageName + "' /></p>");
    html.append("<p font-size:12pt> The plot above shows the astig of each input file plotted as colored squares. "
               "It shows the stand only astig ploted as lines. The line ends at the atig value. "
               "The length of the line represents the magnitude of the astig.<br>"
               "The mean value of test stand only astig is plotted as a large star.</p>"
                "<p font-size:12pt>If the variation for the astig values is small then "
                "The stand removal was good.  Idealy the STD (standard deviation) should be"
                " less than .1 which means less than .1 wave pv on the surface of the mirror</p><br>"
                "<p font-size:12pt>The contour plots below are of what is beleived to be test stand only induced errors at each rotation. "
                "Check that they are similar at each rotation.  If not then maybe "
                "stand induced error is not same at each rotation then the stand removal is not reliable. "
                "However it is unlikely that they will all look exactly the same.</p>");


    html.append(imagesHtml);
    html.append("</body></html>");
    editor->setHtml(html);
    return results;
}

void SurfaceManager::computeStandAstig(QList<rotationDef *> list){
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // check for pairs
    QVector<rotationDef*> lookat = list.toVector();
    while (lookat.size()){
        for (int i = 0; i < lookat.size(); ++i){
            double angle1 = lookat[i]->angle;
            double found = false;
            for (int j = i+1; j < lookat.size(); ++j){
                if (lookat[j]->angle == angle1 -90 || lookat[j]->angle == angle1 + 90)
                {
                    found = true;
                    lookat.remove(j);
                    lookat.remove(i);
                    found = true;
                    break;
                }
            }

            if (!found){
                QMessageBox::warning(0, tr("Error"),
                                     QString().sprintf("No 90 deg pair for %s and angle %6.2lf",
                                                       lookat[i]->fname.toStdString().c_str(),
                                                       lookat[i]->angle));
                lookat.remove(i);
            }
            break;
        }
    }
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
            "<table border='1'style='ds margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px;'"
            " 'width='100%' cellspacing='2' cellpadding='0'>");
    html.append("<tr><td><p align='center'><b>Unrotated inputs</b></p></td>");
    html.append("<td><p align='center'><b> Counter Rotated </b></p></td></tr>");


    QTextDocument *doc = editor->document();
    QList<QString> doc1Res;
    doc->setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number
    QList<int> rotated;
    QList<int> inputs;
    editor->append("<tr>");
    int startingNdx = m_wavefronts.size() -1;
    contour.fill( QColor( Qt::white ).rgb() );
    for (int i = 0; i < list.size(); ++i){

        QPainter painter( &contour );
        int ndx = m_wavefronts.size();
        inputs.append(ndx);
        // get input file

        m_surface_finished = false;
        QStringList loadList;
        loadList << list[i]->fname;
        emit load(loadList, this);
        while (!m_surface_finished){qApp->processEvents();} // wait for wavefront to be computed

        wavefront * wf = m_wavefronts[m_currentNdx];
        unrotatedNdxs.append(m_currentNdx);
        plot->setSurface(wf);
        plot->replot();
        renderer.render( plot, &painter, QRect(0,0,contourWidth,contourHeight) );

        QString imageName = QString().sprintf("mydata://%s.png",list[i]->fname.toStdString().c_str());
        QString angle = QString().sprintf("%6.2lf Deg",list[i]->angle);
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour));
        doc1Res.append(imageName);
        html.append("<tr><td><p align='center'> <img src='" +imageName + "' /><br><b>" + angle + "</b></p></td>");


        // counter rotate it

        QList<int> l;
        l.append(ndx);
        ndx = m_wavefronts.size();

        m_surface_finished = false;
        rotateThese(list[i]->angle,l);
        rotated.append(ndx);
        wf = m_wavefronts[ndx];
        while (!m_surface_finished){qApp->processEvents();}
        plot->setSurface(wf);
        plot->replot();

        contour.fill( QColor( Qt::white ).rgb() );
        renderer.render( plot, &painter, QRect(0,0,contourWidth,contourHeight) );
        angle = QString().sprintf("%6.2lf Deg",list[i]->angle);
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
        wlist << m_wavefronts[rotated[i]];
    }

    int avgNdx = m_wavefronts.size();  // the index of the average

    m_surface_finished = false;

    average(wlist);
    while (!m_surface_finished){qApp->processEvents();}
    emit(nameChanged(m_wavefronts[m_currentNdx]->name, QString("AverageStandRemoved")));
    QTextEdit *page2 = new QTextEdit;
    page2->resize(600,800);
    QTextDocument *doc2 = page2->document();
    QList<QString> doc2Res;
    html = ("<html><head/><body><h1>Test Stand Astig Removal</h1>"
            "<h3>Step 2. Averaged surface with stand induced terms removed:</h3>");

    //computeMetrics(m_wavefronts[m_currentNdx]);

    plot->setSurface(m_wavefronts[m_currentNdx]);
    contour = QImage(450,450, QImage::Format_ARGB32 );
    contour.fill( QColor( Qt::white ).rgb() );
    QPainter painter( &contour );

    renderer.render( plot, &painter, QRect(0,0,450,450) );

    QString imageName = "mydata://AvgAstigremoved.png";
    doc2->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(contour));
    doc2Res.append(imageName);
    html.append("<p> <img src='" +imageName + "' /></p>");
    html.append("</body></html>");
    page2->setDocument(doc2);
    page2->setHtml(html);
    //page2->show();

    // calculate stand astig for each input
    // for each input rotate the average by the input angle and subtract it from the input
    // plot the astig of each of the inputs which will be the stand only astig.


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

     //Delete all work wavefronts except the overall average with stand removed.
     QList<int> deleteThese;
     for (int i = m_wavefronts.size() -1; i > startingNdx; --i){
         if (i != avgNdx)
            deleteThese.append(i);
     }

     deleteWaveFronts(deleteThese);

    QApplication::restoreOverrideCursor();
}

void SurfaceManager::computeTestStandAstig(){
    standAstigWizard *wiz = new standAstigWizard(this);
    wiz->show();
}

void SurfaceManager::saveAllContours(){
    QSettings settings;
    QString lastPath = settings.value("projectPath","").toString();
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

void SurfaceManager::showAllContours(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ContourPlot *plot =new ContourPlot(0,0);//m_contourPlot;
    //plot->m_minimal = true;
    int width = 420;
    int height = 350;

    int rows =  ceil((double)m_wavefronts.size()/4.);
    int columns = min(m_wavefronts.size(),int(ceil((double)m_wavefronts.size()/rows)));
    const QSizeF size(columns * (width + 10), rows * (height + 10));
    const QRect imageRect = QRect(0,0,size.width(),size.height());
    m_allContours = QImage( imageRect.size(), QImage::Format_ARGB32 );
    m_allContours.fill( QColor( Qt::white ).rgb() );
    QPainter painter( &m_allContours );
    QwtPlotRenderer renderer;
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
    renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
    renderer.setDiscardFlag(QwtPlotRenderer::DiscardLegend,false);
    renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales,false );
    for (int i = 0; i < m_wavefronts.size(); ++i)
    {
        wavefront * wf = m_wavefronts[i];
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
    QRect rec = QApplication::desktop()->screenGeometry();
    height = 2 * rec.height()/3;
    width = rec.width();
    w->resize(width,height);
    w->show();
    QApplication::restoreOverrideCursor();
}
void SurfaceManager::report(){
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Save PDF report", QString(mirrorDlg::get_Instance()->getProjectPath() +
                                                                                        "/Report.pdf"), "*.pdf");
    if (fileName.isEmpty())
        return;
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }

    QPrinter printer(QPrinter::HighResolution);
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( true );
    printer.setOutputFileName( fileName );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setResolution(85);
    printer.setPaperSize(QPrinter::A4);

    QTextEdit *editor = new QTextEdit;
    editor->resize(printer.pageRect().size());

    mirrorDlg *md = mirrorDlg::get_Instance();
    wavefront *wf = m_wavefronts[m_currentNdx];

    metricsDisplay *metrics = metricsDisplay::get_instance();



    QString title("<html><body><table width = '100%'><tr><td></td><td><h1><center>Interferometry Report for " +
                  md->m_name + "</center></td><td>"
                  + QDate::currentDate().toString() +
                  " " +QTime::currentTime().toString()+"</td></tr></table>");

    QString html = "<p style=\'font-size:15px'>"
            "<table border='1' width = '100%'><tr><td>Diameter: " + QString().number(md->diameter,'f',1) + " mm</td><td>"
            "ROC: " +  QString().number(md->roc,'f',1) + " mm</td>"
            "<td>Fnumber: " + QString().number(md->FNumber,'f',1) + "</td></tr>"
            "<tr><td> RMS: " + QString().number(wf->std,'f',3) + " waves at 550nm</td><td>Strehl: " + metrics->mStrehl->text() +
            "</td><td>Best Fit Conic: "+metrics->mCC->text() + "</td></tr>"

            "</table></p>";

    // zerenike values
    QString zerns = "<p>Zernike Values RMS:<br><table width='100%'><tr><td><table  border='1' width='40%'>";

    int half = Z_TERMS/2 +2;
    for (int i = 3; i < half; ++i){
        double val = wf->InputZerns[i];
        bool enabled = zernEnables[i];
        if ( i == 3 && m_surfaceTools->m_useDefocus){
           val = m_surfaceTools->m_defocus;
           enabled = true;
        }
        if ( i == 8 && md->doNull){
            val += md->z8;
        }
        zerns.append("<tr><td>" + QString(zernsNames[i]) + "</td><td>" + QString().number(val,'f',3) + "</td><td>" +
                     QString((enabled) ? QString("") : QString("Disabled")) + "</td></tr>");
    }


    zerns.append("</table></td><td><table border='1' width = '50%'>");

    for (int i = half; i < Z_TERMS; ++i){
        zerns.append("<tr><td>" + QString(zernsNames[i]) + "</td><td>" + QString().number(computeRMS(i,wf->InputZerns[i]),'f',3) + "</td><td>" +
                     QString((zernEnables[i]) ? QString("") : QString("Disabled")) + "</td></tr>");
    }
    zerns.append("</table></td></tr></table></p>");
        //qDebug() << zerns;
    QString tail = "</body></html>";

    QImage image = QImage(600,500, QImage::Format_ARGB32 );

    QPainter painter(&image);
    ContourPlot *plot =new ContourPlot(0,0,false);//m_contourPlot;
    plot->setSurface(wf);
    QwtPlotRenderer renderer;
    renderer.render(plot,&painter,QRect(0,0,600,500));
    QTextDocument *doc = editor->document();
    QString contour("mydata://contour.png");
    doc->addResource(QTextDocument::ImageResource,  QUrl(contour), QVariant(image));
    QString contourHtml = "<p> <img src='" +contour + "'</p>";
    QString threeD("threeD");
    image.fill( QColor( Qt::white ).rgb() );
    QImage ddd = m_oglPlot->renderPixmap(600,400).toImage();

    doc->addResource(QTextDocument::ImageResource,  QUrl(threeD), QVariant(ddd));
    contourHtml.append("<p> <img src='" +threeD + "'</p>");
    QImage i2 = QImage(((MainWindow*)(parent()))->m_profilePlot->size(),QImage::Format_ARGB32);
    QPainter p2(&i2);
    image.fill( QColor( Qt::white ).rgb());
    //renderer.render(m_profilePlot->m_plot, &painter,QRect(0,0,600,300));
    ((MainWindow*)(parent()))->m_profilePlot->render(&p2);
    QImage i2Scaled = i2.scaled(printer.pageRect().size().width()-50,800,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QString profile("mydata://profile.png");
    doc->addResource(QTextDocument::ImageResource,  QUrl(profile), QVariant(i2Scaled));
    contourHtml.append("<p> <img src='" +profile + "'</p>");

    SimulationsView *sv = SimulationsView::getInstance(0);
    if (sv->needs_drawing){
        sv->on_MakePB_clicked();
    }
    QImage svImage = QImage(sv->size(),QImage::Format_ARGB32 );
    QPainter p3(&svImage);
    sv->render(&p3);
    QImage svImageScaled = svImage.scaled(printer.pageRect().size().width()-50,800,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    QString svpng("mydata://sv.png");
    doc->addResource(QTextDocument::ImageResource,  QUrl(svpng), QVariant(svImageScaled));
    contourHtml.append("<p> <img src='" +svpng + "'</p>");
    editor->setHtml(title + html +zerns + contourHtml+ tail);
    editor->print(&printer);
    editor->show();
}
