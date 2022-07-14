/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "psfplot.h"
#include "ui_psfplot.h"
#include <qwt_plot_curve.h>
#include <qwt_plot_legenditem.h>
#include <qwt_plot_textlabel.h>
#include <QPushButton>
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
    enableAxis(QwtPlot::xBottom, false);
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

void psfPlot::setData(cv::Mat wf, QString label, QPen color, bool doLog){
    int nx = wf.cols/2;
    QwtPlotCurve *curve1 = new QwtPlotCurve(label);
    QPolygonF points1;
    double w = sqrt(wf.cols);
    if (doLog){
        wf += cv::Scalar::all(1);                    // switch to logarithmic scale
        cv::log(wf, wf);
    }
    for (int x = nx; x < nx + nx/4; ++x){

        points1 << QPointF(x, /*log10*/(wf.at<double>(nx,x)/w));
    }
    curve1->setPen(color);
    curve1->setSamples(points1);
    curve1->attach(this);
    replot();
    show();
}

