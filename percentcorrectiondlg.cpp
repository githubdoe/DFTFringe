#include "percentcorrectiondlg.h"
#include "ui_percentcorrectiondlg.h"
#include "qwt_plot_grid.h"
#include "qwt_scale_div.h"
#include "qwt_plot_barchart.h"
#include <qwt_scale_draw.h>

percentCorrectionDlg::percentCorrectionDlg( QWidget *parent) :
    QDialog(parent),m_showZones(false),
    ui(new Ui::percentCorrectionDlg)
{
    ui->setupUi(this);

    resize(1000,800);
}
void percentCorrectionDlg::replot(){

    double min = 1000.;
    double max = -1000.;
    int smoothrange = .1 * m_size;
    QPolygonF slp;
    QPolygonF slp2;
    QPolygonF smoothed;
    double rho;
    double dx = 2.0 / (m_avg.size() -1.0);
    for (int i = 2; i <= m_size - 2; ++i){
        double rho = m_avg[i].x() / m_radius;
        double slope = 0.5 * ((m_avg[i+2].y() - m_avg[i-2].y()) / (4 * dx) - (m_avg[(m_size - i) +2].y() - m_avg[(m_size - i) -2].y()) / (4 * dx) );   //slope of average profile avg (smoothed a bit), with artificial Null applied (for output wavelength)
        double slopeOfAvgDefocus = ( -3 * m_z8 * 4 * rho + (m_desiredZ8) * (24 * pow(rho , 3) - 12 * rho))  * (m_lambda_nm/m_outputLambda);       //slope of the defocus term needed for avg, PLUS the slope of the first spherical term removed by artificial null, converted to output wavelength
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
    ui->plot->detachItems(QwtPlotItem::Rtti_PlotItem);
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
    slopeCurve->setPen(QPen(QColor("green"),3));
    slopeCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol,false );
    slopeCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine,false );
    slopeCurve->setItemAttribute(QwtPlotCurve::Legend,false);
    slopeCurve->attach( ui->plot);
    ui->plot->setAxisTitle( ui->plot->yLeft, "Percent correction" );
    ui->plot->setAxisTitle( ui->plot->xTop, "Radius mm" );
    if (max < 120)
        max = 120;
    else max += 10.;
    if (min > -10)
        min = -10;
    else min-= 10.;
    slopeCurve->setZ(1);
    ui->plot->setAxisScale(QwtPlot::yLeft, min, max);
    ui->plot->replot();
}
void percentCorrectionDlg::plot( QPolygonF avg, double radius,double z8,
                                 double desiredZ8,
                                 double lambda_nm, double outputLambda){

    m_avg = avg;
    m_radius = radius;
    m_z8 = z8;
    m_desiredZ8 = desiredZ8;
    m_lambda_nm = lambda_nm;
    m_outputLambda = outputLambda;
    m_size = avg.size()-1;
    replot();
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


void percentCorrectionDlg::on_zones_valueChanged(int arg1)
{
    m_number_of_zones = arg1;
    replot();

}


void percentCorrectionDlg::on_checkBox_2_clicked(bool checked)
{
   m_showZones = checked;
   replot();
}

