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
percentCorrectionDlg::percentCorrectionDlg( QWidget *parent) :
    QDialog(parent),m_showZones(false),
    ui(new Ui::percentCorrectionDlg)
{
    ui->setupUi(this);

    resize(1000,800);
    setWindowFlags(Qt::WindowStaysOnTopHint);
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
void percentCorrectionDlg::replot(QColor penColor, bool addToPlot){
    auto z8null =[] (double rho, double z8) {
        return z8 * (6 * pow(rho, 4) - 6 * pow(rho, 2 + 1));};

    double min = 1000.;
    double max = -1000.;
    int smoothrange = .1 * m_size;

    QPolygonF expectedslope;
    QPolygonF actualslope;
    QPolygonF percentplot;
    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    double exc_pct = 100. * pow(16.75,2.)/pow(m_radius,2.);
    QList<double> zoneCenter;
    for (int i = 0; i <= m_number_of_zones; ++i){
        zoneCenter << m_radius * sqrt((1. -.01 * exc_pct) * i/m_number_of_zones + .01 * exc_pct);
    }

    double DesiredZ8 = m_desiredZ8;

    if (saTools->m_useDefocus){ // if defocus compute new SA null Desired Z8
        double fnumber = mirrorDlg::get_Instance()->FNumber;
        double rocDel = fnumber * fnumber * 8. * saTools->m_defocus * .055;  //mmeters
        double newRoc = m_roc + rocDel;
        DesiredZ8 = (pow(m_radius * 2.,4) * 1000000.) /
            (384. * pow(newRoc, 3) * m_lambda_nm);
    }

    double size = m_avg.size();
    double dx = 2.0 / (m_avg.size() -1.0);
    double sag = m_roc - sqrt( pow(m_roc,2)- pow((m_radius * 2.),2)/4.);
    for (int i = 2; i <= m_size - 2; ++i){
        double rho1 = m_avg[i+2].x() / m_radius;
        double rho2 = m_avg[i-2].x() /m_radius;
        if (rho1 > .10){

            // make expected slope
            double parabExpectedSlope = -sag * (pow(rho2,2) - pow(rho1,2))/dx;

            // make actual slope buy adding the error to the expected
            double error1 = .0001 * m_avg[i-2].y() * m_lambda_nm * (m_lambda_nm/m_outputLambda);
            double error2 = .0001 * m_avg[i+2].y() * m_lambda_nm * (m_lambda_nm/m_outputLambda);

            double parabActualSlope = -sag * ((pow( rho2,2) + error2) - (pow(rho1,2) + error1))/dx;
            actualslope << QPointF(m_avg[i].x(), parabActualSlope );
            expectedslope << QPointF( m_avg[i].x(),parabExpectedSlope);
            double percent = 100.  - 100. * fabs( parabExpectedSlope -parabActualSlope)/((parabExpectedSlope + parabActualSlope)/2.) ;
            percentplot << QPointF(m_avg[i].x(),percent);

        }

    }

    if (!addToPlot)
        ui->plot->detachItems(QwtPlotItem::Rtti_PlotItem);
    for (int i = 0; i < zoneCenter.size()-1; ++i) {
        double centerv = (zoneCenter[i] + zoneCenter[i+1])/2.;
        QwtPlotMarker *marker = new QwtPlotMarker();
        marker->setLineStyle(QwtPlotMarker::VLine); // Set the line style to vertical
        marker->setLabel(QString().number(i+1));
        marker->setLinePen(Qt::red,2,Qt::DashLine);
        marker->setLabelAlignment(Qt::AlignLeft);
        marker->setXValue(centerv); // Set the x-coordinate for the line
        marker->attach(ui->plot); //
    }
    QwtPlotGrid *grid = new QwtPlotGrid();

    grid->setZ(1);

    grid->enableXMin(true);
    grid->enableYMin(true);

    grid->setPen( Qt::gray, 0, Qt::DotLine );
    grid->setMajorPen( Qt::blue, 2.0,Qt::DotLine);
    grid->setMinorPen(Qt::black, 1.0, Qt::DotLine);
    grid->attach( ui->plot);

    QwtPlotCurve *slopeCurve8 = new QwtPlotCurve("expected slope");
    slopeCurve8->setSamples(expectedslope);
    slopeCurve8->attach(ui->plot);
    slopeCurve8->setPen(Qt::green,5);
    slopeCurve8->setZ(0);
    slopeCurve8->setLegendIconSize(QSize(50,20));

    QwtPlotCurve *slopeCurve9 = new QwtPlotCurve("actual slope");
    slopeCurve9->setSamples(actualslope);
    slopeCurve9->attach(ui->plot);
    slopeCurve9->setPen(Qt::red,5);
    slopeCurve9->setZ(0);
    slopeCurve9->setLegendIconSize(QSize(50,20));

    QwtPlotCurve *slopeCurve5 = new QwtPlotCurve("percent");
    slopeCurve5->setSamples(percentplot);
    slopeCurve5->attach(ui->plot);
    slopeCurve5->setPen(penColor,5, Qt::DotLine);
    slopeCurve5->setZ(5);
    slopeCurve5->setLegendIconSize(QSize(50,20));

    ui->plot->insertLegend( new QwtLegend() , QwtPlot::BottomLegend);
    QwtPlotCurve *slopeCurve3 = new QwtPlotCurve("Actual");

    slopeCurve3->setLegendAttribute(QwtPlotCurve::LegendShowSymbol,true );
    slopeCurve3->setLegendAttribute(QwtPlotCurve::LegendShowLine,true );
    slopeCurve3->setItemAttribute(QwtPlotCurve::Legend,true);

    ui->plot->setAxisTitle( ui->plot->yLeft, "Percent correction" );
    ui->plot->setAxisTitle( ui->plot->xTop, "Radius mm" );
    slopeCurve3->setZ(1);
    ui->plot->setAxisScale(QwtPlot::yLeft, ui->minvalue->value(), ui->maxvalue->value());
    ui->plot->replot();
}
void percentCorrectionDlg::plot( QPolygonF avg, double radius,double roc, double z8,
                                 double desiredZ8,
                                 double lambda_nm, double outputLambda,
                                 QColor penColor, bool addToPlot){

    m_avg = avg;
    m_radius = radius;
    m_roc = roc;
    m_z8 = z8;
    m_desiredZ8 = desiredZ8;
    m_lambda_nm = lambda_nm;
    m_outputLambda = outputLambda;
    m_size = avg.size()-1;
    replot(penColor, addToPlot);
}

percentCorrectionDlg::~percentCorrectionDlg()
{
    delete ui;
}







void percentCorrectionDlg::on_checkBox_clicked(bool checked)
{

    if (checked){
        setWindowFlags(Qt::WindowStaysOnTopHint);
        show();
    }
    else
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);

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

