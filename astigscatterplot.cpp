#include "astigscatterplot.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include "myplotpicker.h"
astigScatterPlot::astigScatterPlot(QWidget *parent):QwtPlot( parent ),m_max(.3)

{

    Picker = new myPlotPicker( this );
    connect(Picker, &myPlotPicker::mySelected,this,&astigScatterPlot::selectedwave);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );

    d_origin = new QwtPlotMarker();
    d_origin->setLineStyle( QwtPlotMarker::Cross );
    d_origin->setValue( 0.,0. );
    d_origin->setLinePen( Qt::gray, 1.0, Qt::DashLine );
    d_origin->attach( this );

    d_curve = new QwtPlotCurve();
    d_curve->setStyle( QwtPlotCurve::Lines );
    d_curve->setPen( canvas()->palette().color( QPalette::WindowText ) );
    d_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    d_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
    d_curve->attach( this );
    setAxisScale(QwtPlot::xBottom, -.3, .3);
    setAxisScale(QwtPlot::yLeft,   -.3, .3);
    setAxisTitle(QwtPlot::xBottom, "X");
    setAxisTitle(QwtPlot::yLeft, "Y");
    d_curve->setPen( Qt::red );
    d_curve->setStyle( QwtPlotCurve::Dots );
    d_curve->setSymbol( NULL );
    setTitle("astig");
    //setMinimumHeight(300);
    replot();
}

void astigScatterPlot::selectedwave(const QString &m){
    emit waveSeleted(m);
}


void astigScatterPlot::addValue(const QString &name, QPointF p){
    QwtPlotMarker *m = new QwtPlotMarker(name);
    if (fabs(p.x()) > m_max)
        m_max = fabs(p.x());
    if (fabs(p.y()) > m_max)
        m_max = fabs(p.y());
    setAxisScale(QwtPlot::xBottom, -m_max, m_max);
    setAxisScale(QwtPlot::yLeft,   -m_max, m_max);
    m->setValue(p.x(), p.y());

    QSize s = QSize(4,4);
    m->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QColor(Qt::red),QColor(Qt::red), s));
    m->attach(this);
    replot();
}
