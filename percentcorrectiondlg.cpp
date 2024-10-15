#include "percentcorrectiondlg.h"
#include "ui_percentcorrectiondlg.h"
#include "qwt_plot_grid.h"
#include "qwt_scale_div.h"
#include "qwt_plot_barchart.h"
#include <qwt_scale_draw.h>
#include "qwt_plot_marker.h"
#include <QSettings>
#include "surfaceanalysistools.h"

percentCorrectionDlg::percentCorrectionDlg( QWidget *parent) :
    QDialog(parent),m_showZones(false),
    ui(new Ui::percentCorrectionDlg)
{
    ui->setupUi(this);

    resize(1000,800);
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

    double min = 1000.;
    double max = -1000.;
    int smoothrange = .1 * m_size;
    QPolygonF slp;
    QPolygonF slp2;
    QPolygonF smoothed;
    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    double exc_pct = 100. * pow(16.75,2.)/pow(m_radius,2.);
    QList<double> zoneCenter;
    for (int i = 0; i <= m_number_of_zones; ++i){
        zoneCenter << m_radius * sqrt((1. -.01 * exc_pct) * i/m_number_of_zones + .01 * exc_pct);
    }
qDebug() << "zones" << zoneCenter;
    double dx = 2.0 / (m_avg.size() -1.0);
    for (int i = 2; i <= m_size - 2; ++i){
        double rho = m_avg[i].x() / m_radius;
        double slope = 0.5 * ((m_avg[i+2].y() - m_avg[i-2].y()) / (4 * dx) - (m_avg[(m_size - i) +2].y() - m_avg[(m_size - i) -2].y()) / (4 * dx) );   //slope of average profile avg (smoothed a bit), with artificial Null applied (for output wavelength)
        double slopeOfAvgDefocus = ( -3 * m_z8 * 4 * rho + (m_desiredZ8) * (24 * pow(rho , 3) - 12 * rho))  * (m_lambda_nm/m_outputLambda);       //slope of the defocus term needed for avg, PLUS the slope of the first spherical term removed by artificial null, converted to output wavelength
        if (saTools->m_useDefocus){
            slopeOfAvgDefocus -= saTools->m_defocus * (-1. + 2. * pow(rho,2) );
        }
        /* The profileplot avg is the measured profile minus artificial Null. The above term restores the slope of the original measured surface...
        */
        double slopedefocused = slope - slopeOfAvgDefocus;                                //slope of the restored original profile, without artificial null
        double desiredSlope = (3 * m_desiredZ8 * 4 * rho + m_desiredZ8 * (24 * pow(rho , 3) - 12 * rho))* (m_lambda_nm/m_outputLambda);   // slope of defocused perfect parabola, converted to output wavelength
        double percentCorr = ( slopedefocused / desiredSlope ) + 2 * (-m_z8 / m_desiredZ8);

        if (rho > .10){
            slp << QPointF(m_avg[i].x(), 100. * (percentCorr));
        }

    }
    for (int i = 0; i < slp.size()-smoothrange; i++){
        double smoothavg = 0;
        for (int j = i; j < i + smoothrange; ++j){
            smoothavg += slp[j].y();

        }
        smoothed << QPointF(slp[i + smoothrange/2].x(),smoothavg/ (double)smoothrange);
        if (smoothed.last().y() < min){
            min = smoothed.last().y();
        }
        if (smoothed.last().y() > max){
            max = smoothed.last().y();
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
    grid->setMajorPen( Qt::blue, 2.0,Qt::SolidLine);
    grid->setMinorPen(Qt::black, 2.0, Qt::DotLine);
    grid->attach( ui->plot);


    QwtPlotCurve *slopeCurve = new QwtPlotCurve;
    slopeCurve->setSamples(slp);
    slopeCurve->setRenderHint( QwtPlotItem::RenderAntialiased );
    slopeCurve->setPen(QPen(penColor,3));
    slopeCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol,false );
    slopeCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine,false );
    slopeCurve->setItemAttribute(QwtPlotCurve::Legend,false);
    slopeCurve->attach( ui->plot);
    ui->plot->setAxisTitle( ui->plot->yLeft, "Percent correction" );
    ui->plot->setAxisTitle( ui->plot->xTop, "Radius mm" );
    slopeCurve->setZ(1);
    ui->plot->setAxisScale(QwtPlot::yLeft, ui->minvalue->value(), ui->maxvalue->value());
    ui->plot->replot();
}
void percentCorrectionDlg::plot( QPolygonF avg, double radius,double z8,
                                 double desiredZ8,
                                 double lambda_nm, double outputLambda,
                                 QColor penColor, bool addToPlot){

    m_avg = avg;
    m_radius = radius;
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

