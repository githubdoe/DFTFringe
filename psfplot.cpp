#include "psfplot.h"
#include "ui_psfplot.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_textlabel.h>
psfPlot::psfPlot(QWidget *parent) :
    QwtPlot(parent),
    ui(new Ui::psfPlot)
{
    ui->setupUi(this);
    QwtPlotLegendItem *customLegend = new QwtPlotLegendItem();
    customLegend->setAlignment(Qt::AlignLeft | Qt::AlignBottom);
    customLegend->attach(this);
    QwtPlotTextLabel *t = new QwtPlotTextLabel();
    QwtText title( "PSF" );
    title.setRenderFlags( Qt::AlignHCenter | Qt::AlignTop );
    t->setText(title);
    t->attach(this);
}

psfPlot::~psfPlot()
{
    delete ui;
}

void psfPlot::clear(){
    detachItems( QwtPlotItem::Rtti_PlotCurve);
    enableAxis(QwtPlot::yLeft, false);


}

void psfPlot::setData(cv::Mat wf, QString label, QPen color){
    int nx = wf.cols/2;
    QwtPlotCurve *curve1 = new QwtPlotCurve(label);
    QPolygonF points1;
    double w = sqrt(wf.cols);
    for (int x = nx; x >0; --x){

        points1 << QPointF(nx - x, log10(wf.at<double>(nx,x)/w));
    }
    curve1->setPen(color);
    curve1->setSamples(points1);
    curve1->attach(this);
    replot();
    show();
}

