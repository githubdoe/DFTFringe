#include "percentcorrectiondlg.h"
#include "ui_percentcorrectiondlg.h"
#include "qwt_plot_grid.h"
#include "qwt_scale_div.h"
#include "qwt_plot_barchart.h"
#include <qwt_scale_draw.h>
#include "qwt_plot_marker.h"
#include <QSettings>
#include "surfaceanalysistools.h"
#include "mirrordlg.h"
#include <qwt_legend.h>
#include <QToolTip>
#include "surfacemanager.h"

percentCorrectionDlg::percentCorrectionDlg( QWidget *parent) :
    QDialog(parent),m_showZones(false),
    ui(new Ui::percentCorrectionDlg)
{
    ui->setupUi(this);

    resize(1000,800);
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    QSettings set;
    ui->minvalue->blockSignals(true);
    ui->maxvalue->blockSignals(true);
    ui->minvalue->setValue(set.value("percent_correction_min",-10).toDouble());
    ui->maxvalue->setValue(set.value("percent_correction_max", 120).toDouble());
    ui->minvalue->blockSignals(false);
    ui->maxvalue->blockSignals(false);

    m_number_of_zones = set.value("percent number of zones", 5).toInt();
    ui->numberOfZones->blockSignals(true);
    ui->numberOfZones->setValue(m_number_of_zones);
    ui->numberOfZones->blockSignals(false);
    if (set.contains("percent_Correction_dialogGeometry")) {
        setGeometry(set.value("percent_Correction_dialogGeometry").toRect());
    }
}
//  .0000018
double g_laserLambda = 550.;
double getZernSurface( double RoC, double MirrorRad, double Zernikes[], double radius){

         double num1 = radius / MirrorRad;
         if (num1 > 1.0)
           num1 = 1.0;
         double num2 = num1 * num1;
         double num3 = num2 * num2;
         double num4 = num3 * num2;
         double num5 = num3 * num3;
         double num6 = num5 * num2;

         double num7 = 0.0 + Zernikes[8] * (1.0 - 6.0 * num2 + 6.0 * num3) + Zernikes[15] * (1.0 + 12.0 * num2 - 30.0 * num3 + 20.0 * num4) + Zernikes[24] * (1.0 - 20.0 * num2 + 90.0 * num3 - 140.0 * num4 + 70.0 * num5) + Zernikes[35] * (1.0 + 30.0 * num2 - 210.0 * num3 + 560.0 * num4 - 630.0 * num5 + 252.0 * num6);
         double surf = RoC - sqrt(pow(RoC, 2.0) - pow(radius, 2.0)) + num7 *  g_laserLambda * 1.E-6/2.;
         qDebug() << "z8" << Zernikes[8] << "num7" << num7 << "surf" << surf;
         return surf;
}

double getSlope(double RoC, double radius, double Zernikes[], double x){
    double num1 = x / 100.0;
    double surface1 = getZernSurface(RoC, radius, Zernikes, x - num1);
    double surface2 = getZernSurface(RoC, radius, Zernikes, x + num1);
  //double slope = -(1.0 / (2.0 * num1)) * (surface1 - surface2);
    double slope = -(1.0 / (2.0 * num1)) * (surface1 - surface2);
//qDebug() << "x" << x << "slope" << slope;
    return slope;
}
double GetActualKE(double RoC, double MirrorRad, double Zernikes[], double Z_Radius)
{
    double slope = getSlope(RoC, MirrorRad, Zernikes, Z_Radius);
    double surface = getZernSurface(RoC, MirrorRad, Zernikes, Z_Radius);
    //qDebug() << "x" << Z_Radius << "slope" << slope << "surface" << surface << "roc" << RoC;
    double actualKe = Z_Radius / slope + surface - RoC;
    //qDebug() << "actualKnife" << actualKe;
    return actualKe;
}
// 5.3 = x/slope + 5.3 - roc  x/slope = roc   slope = x/roc
double getIdealKE(double RoC, double Z_Radius)
{
  return pow(Z_Radius, 2.0) / (2.0 * RoC);
}
/*
 * paraby(x) = spherey(x) + profile(x) + ?
 *
 *
  */
void percentCorrectionDlg::replot(QColor penColor, bool addToPlot){


    int smoothrange = .1 * m_size;
    m_z8 = 0.;
    QPolygonF expectedslope;
    QPolygonF actualslope;
    QPolygonF percentplot;
    QPolygonF franzPrecentPlot;
    QPolygonF actualSurface;
    QPolygonF idealSurface;
    QPolygonF correction;

    QPolygonF data;
    SurfaceManager &sm = *SurfaceManager::get_instance();
    std::vector<double> zernikes;

    auto getSphereY = [] (double roc, double x){
        return roc - sqrt(pow(roc,2) - pow(x,2));
    };
    auto getNull = [] (double roc, double radius,  double x, double c8, double wavelength){
        double r = x/radius;
        qDebug() << "x" << x << "roc"<< roc << "rho" << r;
        return wavelength * 1.E-5 * .5 * c8 * (6. * pow(r,4) - 6. * pow(r,2) +1);
    };
    auto getConicY = [] (double roc, double conic, double x){
        double y = pow(x,2)/(roc + (sqrt(pow(roc,2) - ((conic + 1) * pow(x,2)))));
        return y;
    };
    auto average = [] (QPolygonF data, int width, int ndx){
        int cnt = 0;
        double sum = 0.;
        for (int i = ndx - width; i <= ndx + width; ++i){
            sum += data[i].y();
            ++cnt;
        }
        return sum/cnt;
    };

    double roc = m_roc + m_roc_offset;
    ui->roc->setValue( roc);
    zernikes = sm.getCurrent()->InputZerns;

    double wavelength =1e-6 *  m_outputLambda;  // surface error
    int smoothsize = 1 ;

    double C8 =  mirrorDlg::get_Instance()->z8;
    double conic = mirrorDlg::get_Instance()->cc;
    qDebug() << "c8" << C8;
    // smooth and convert to surface error.
    double min = 999999.;
    for (int i = 0; i < m_avg.size(); ++i){
        min = fmin(min, m_avg[i].y());
    }
    for (int i = m_avg.size()/2; i < m_avg.size(); ++i){
        //save error y in mm on surface
        data << QPointF(m_avg[i ].x(),  (m_avg[i].y() - min) * .5E-6 * m_outputLambda);


    }


    double exc_pct = 100. * pow(16.75,2.)/pow(m_radius,2.);
    QList<double> zoneCenter;
    QList<double> zoneedge;
    QList<int> indexOfZoneCenter;
    // crete the zones and the zone centers for later indications
    for (int i = 0; i < m_number_of_zones+1; ++i){
        zoneedge << m_radius * sqrt((1. -.01 * exc_pct) * i/m_number_of_zones + .01 * exc_pct);
        // find the x entry in the surface that is closest to the zone value
    }
    double lastzone = 0.;
    for (int i = 0; i < m_number_of_zones; ++i){
        int ndx = 0;
        double zoneCenterv = (lastzone + zoneedge[i])/2.;
        lastzone = zoneedge[i];
        for (; ndx < data.size(); ++ ndx){
            double d = data[ndx].x();
            if (d >= zoneCenterv)
                break;
        }
        zoneCenter << zoneCenterv;

        indexOfZoneCenter << ndx;

    }



    // start at the mirrors middle

    QVector<double> IdealzoneKnife;
    QVector<double> ActualZoneKnife;
    QVector<double> idealDelta;
    QVector<double> actualDelta;

    int zone = 0;
    IdealzoneKnife << 0.;
    ActualZoneKnife << 0.0;


    // process each zone center

    // process ideal knife
    int ndx = 1;
     //    while ( ndx < indexOfZoneCenter.length()){
    //        IdealzoneKnife << getIdealKE(roc, data[zoneCenter[ndx]].x()) - firstIdeal;
    //        idealDelta << IdealzoneKnife[ndx] - IdealzoneKnife[ndx-1];


    //    // process actual knife



    //        ActualZoneKnife << GetActualKE(roc, m_radius, zernikes.data(), data[zoneCenter[ndx]].x()) - firstAct;
    //        actualDelta << ActualZoneKnife[ndx] - ActualZoneKnife[ndx-1];
    //        double corr = 100 * (actualDelta.last() - idealDelta.last());
    //        correction << QPointF(data[zoneCenter[ndx]].x(), corr);
    //        ++ndx;
    //        qDebug() << "correction" << correction.last();
    //    }
    QVector<double> idealknives;
    QVector<double> actualknives;
    idealknives << 0.;
    actualknives << 0;
    int del = ui->del->value();
    int avgwidth = ui->avgwidth->value();
    for (int zone = 0; zone < zoneCenter.length(); ++zone){

        int ndx = indexOfZoneCenter[zone];

        double x1 = data[ndx-del].x();
        double idealy1  = getConicY(roc, conic, x1);
        double y1 = idealy1 + average(data, avgwidth, ndx-del);

        double x = data[ndx].x();
        double idealy = getConicY(roc, conic, x);
        double y = average(data, avgwidth, ndx) + idealy;

        double x2 = data[ndx+del].x();
        double idealy2 = getConicY(roc, conic, x2);
        qDebug() << "get idealy2" << roc << idealy;
        double y2 = average(data, avgwidth, ndx+del) + idealy2;


        //  x^2/(2*roc)       dx  = 4 * x/(2*roc)  = 2 * x/roc
        double idealSlope = (idealy2 - idealy1)/(x2-x1) + .0000000001;
        double normIdealSlope = -1./idealSlope;
        double actualslope = (y2 - y1)/(x2 - x1) + .00000000001;
        double normactualslope = -1./actualslope;

        double actualknife =  y - x * normactualslope - roc;
        double idealknife = idealy - x * normIdealSlope - roc;

        double idealDelta = idealknife - idealknives.last();
        double actualDelta = actualknife - actualknives.last();
        idealknives << idealknife;
        actualknives << actualknife;

        // plot knives

        idealSurface << QPointF(x, idealknife);
        actualSurface << QPointF(x, actualknife);
        qDebug() << "x" << x <<"ideal knife" << idealknife << "idealDeltal" << idealDelta << "actualDelta" << actualDelta;



        double correction = 100. * (actualDelta)/(idealDelta);
        qDebug() <<"corr" << correction;
        if (zone > 0)
            percentplot << QPointF( x, correction);
        //qDebug() << "percent" << percentplot;

    }
qDebug() << "roc" << roc;

    if (!addToPlot)
        ui->plot->detachItems(QwtPlotItem::Rtti_PlotItem);
    // draw markers
    for (int i = 0; i < zoneCenter.size(); ++i) {
        double center= zoneCenter[i];
        QwtPlotMarker *marker = new QwtPlotMarker();
        marker->setLineStyle(QwtPlotMarker::VLine); // Set the line style to vertical
        marker->setLinePen(Qt::red,2,Qt::DashLine);
        marker->setLabelAlignment(Qt::AlignLeft);
        marker->setXValue(center);
        marker->setYValue(-5);
        marker->attach(ui->plot);

        QwtPlotMarker *label = new QwtPlotMarker();
        label->setLineStyle(QwtPlotMarker::NoLine); // Set the line style to vertical
        label->setLabel(QString().number(center,'f',1));
        label->setLabelAlignment(Qt::AlignLeft);
        label->setXValue(center);
        label->setYValue(-5);
        label->attach(ui->plot);
        // draw zone number
        QwtPlotMarker *marker2 = new QwtPlotMarker();
        marker2->setLineStyle(QwtPlotMarker::NoLine);
        marker2->setLabel(QString().number(i));
        marker2->setLabelAlignment(Qt::AlignLeft);
        marker2->setYValue(.5);
        marker2->setXValue(center);
        marker2->attach(ui->plot);
    }
//    for (int i = 0; i < zoneedge.size(); ++i) {
//        double center= zoneedge[i];
//        QwtPlotMarker *marker = new QwtPlotMarker();
//        marker->setLineStyle(QwtPlotMarker::VLine); // Set the line style to vertical
//        marker->setLabel(QString().number(i+1) + " " + QString().number(center));
//        marker->setLinePen(Qt::blue,3);
//        marker->setLabelAlignment(Qt::AlignHCenter);
//        marker->setXValue(center); // Set the x-coordinate for the line
//        marker*/->attach(ui->plot); //
//    }
    QwtPlotGrid *grid = new QwtPlotGrid();

    grid->setZ(1);

    grid->enableXMin(true);
    grid->enableYMin(true);

    grid->setPen( Qt::gray, 0, Qt::DotLine );
    grid->setMajorPen( Qt::blue, 2.0,Qt::DotLine);
    grid->setMinorPen(Qt::black, 1.0, Qt::DotLine);
    grid->attach( ui->plot);

    QwtPlotCurve *slopeCurve3 = new QwtPlotCurve("Actual");
    slopeCurve3->setLegendAttribute(QwtPlotCurve::LegendShowSymbol,true );
    slopeCurve3->setLegendAttribute(QwtPlotCurve::LegendShowLine,true );
    slopeCurve3->setItemAttribute(QwtPlotCurve::Legend,true);
    qDebug() << "plot type" << ui->plotType->currentText();
    // if show knives
    if (ui->plotType->currentText() == "knife positions") {
        QwtPlotCurve *slopeCurve9 = new QwtPlotCurve("actual");
        slopeCurve9->setSamples(actualSurface);
        slopeCurve9->attach(ui->plot);
        slopeCurve9->setPen(penColor,5);
        slopeCurve9->setZ(0);
        slopeCurve9->setLegendIconSize(QSize(50,20));
        if (!addToPlot){
            QwtPlotCurve *slopeCurve5 = new QwtPlotCurve("ideal");
            slopeCurve5->setSamples(idealSurface);
            slopeCurve5->attach(ui->plot);
            slopeCurve5->setPen(Qt::gray,5, Qt::DotLine);
            slopeCurve5->setZ(5);
            slopeCurve5->setLegendIconSize(QSize(50,20));
            ui->plot->insertLegend( new QwtLegend() , QwtPlot::BottomLegend);
            ui->plot->setAxisAutoScale(QwtPlot::yLeft, true);

            ui->plot->setAxisTitle( ui->plot->yLeft, "knife offset from ROC in mm" );
            ui->plot->setAxisTitle( ui->plot->xTop, "Radius mm" );

        }
    }
    else {
        QwtPlotCurve *slopeCurve8 = new QwtPlotCurve("correction");
        slopeCurve8->setSamples(percentplot);
        slopeCurve8->attach(ui->plot);
        slopeCurve8->setPen(penColor,10);
        slopeCurve8->setZ(0);
        slopeCurve8->setLegendIconSize(QSize(50,20));
        ui->plot->setAxisTitle( ui->plot->yLeft, "Percent correction" );
        ui->plot->setAxisScale(ui->plot->yRight, -10, 20, 1);
        ui->plot->setAxisTitle( ui->plot->xTop, "Radius mm" );
        slopeCurve3->setZ(1);
        ui->plot->setAxisScale(QwtPlot::yLeft, ui->minvalue->value(), ui->maxvalue->value());
        ui->plot->setAxisScale(QwtPlot::xBottom, 0, m_radius);
    }
    ui->plot->replot();
}

void percentCorrectionDlg::plot( QPolygonF avg, double radius,double roc, double z8,
                                 double desiredZ8,
                                 double lambda_nm, double outputLambda,
                                 QColor penColor, bool addToPlot){

    m_avg = avg;
    m_radius = radius;
    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    if (saTools->m_useDefocus){ // if defocus compute new SA null Desired Z8
        double fnumber = mirrorDlg::get_Instance()->FNumber;
        m_roc_offset = fnumber * fnumber * 8. * saTools->m_defocus * .055;  //mmeters
    }
    else {
        m_roc = roc;
    }



    m_z8 = z8;
    m_desiredZ8 = desiredZ8;
    g_laserLambda = m_lambda_nm = lambda_nm;
    m_outputLambda = outputLambda;
    m_size = avg.size()-1;
    replot(penColor, addToPlot);
}

percentCorrectionDlg::~percentCorrectionDlg()
{
    delete ui;
}


void percentCorrectionDlg::on_minvalue_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("percent_correction_min",arg1);
    emit percent_plot_changed();
}


void percentCorrectionDlg::on_maxvalue_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("percent_correction_max", arg1);
    emit percent_plot_changed();
}

void percentCorrectionDlg::closeEvent(QCloseEvent *event) {
    QSettings set;
    set.setValue("percent_Correction_dialogGeometry", geometry());
    QDialog::closeEvent(event);
}
void percentCorrectionDlg::moveEvent(QMoveEvent *event) {
    QSettings set;
    set.setValue("percent_Correction_dialogGeometry", geometry());
    QDialog::moveEvent(event);
}
void percentCorrectionDlg::on_numberOfZones_valueChanged(int arg1)
{
    m_number_of_zones = arg1;
    QSettings set;
    set.setValue("percent number of zones", arg1);
    emit percent_plot_changed();

}





void percentCorrectionDlg::on_help_clicked()
{
   QToolTip::showText(ui->help->mapToGlobal(QPoint()),ui->help->toolTip(),ui->help);
}


void percentCorrectionDlg::on_plotType_currentTextChanged(const QString &arg1)
{
    if (arg1 == "knife positions"){
        ui->maxvalue->setEnabled(false);
        ui->minvalue->setEnabled(false);
    }
    else {
        ui->maxvalue->setEnabled(true);
        ui->minvalue->setEnabled(true);
    }
    emit percent_plot_changed();
}





void percentCorrectionDlg::on_testbox_valueChanged(double arg1)
{
    qDebug() << "text box" << arg1;
    m_roc_offset = arg1;

    ui->roc->setValue(m_roc + arg1);
    replot();
}


void percentCorrectionDlg::on_del_valueChanged(int arg1)
{
    replot();
}


void percentCorrectionDlg::on_avgwidth_valueChanged(int arg1)
{
    replot();
}

