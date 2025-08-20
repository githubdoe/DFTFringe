#include "rmsplot.h"
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include "myplotpicker.h"
rmsPlot::rmsPlot(QWidget *parent):QwtPlot( parent ),m_max(.3)
{
    Picker = new myPlotPicker( this );
    connect(Picker, &myPlotPicker::mySelected, this, &rmsPlot::selectedwave);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->enableX( true );
    grid->enableXMin( true );
    grid->enableY( true );
    grid->enableYMin( false );
    grid->attach( this );

    d_curve = new QwtPlotCurve();
    d_curve->setStyle( QwtPlotCurve::Lines );
    d_curve->setPen( canvas()->palette().color( QPalette::WindowText ) );
    d_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    d_curve->setPaintAttribute( QwtPlotCurve::ClipPolygons, false );
    d_curve->attach( this );

    d_curve->setPen( Qt::red );
    d_curve->setStyle( QwtPlotCurve::Dots );
    d_curve->setSymbol( NULL );
    setTitle("RMS");
    m_max = 0;
    replot();
}
void rmsPlot::selectedwave(const QString &m){
    emit waveSeleted(m);
}
void rmsPlot::addValue(const QString &name, QPointF p){
    QwtPlotMarker *m = new QwtPlotMarker(name);
    if (p.y() > m_max)
        m_max = p.y();
    setAxisScale(QwtPlot::yLeft,   0, 1.25 * m_max);
    setAxisScale(QwtPlot::xBottom,   0, p.x() + 2);
    m->setValue(p.x(), p.y());

    QSize s = QSize(4,4);
    m->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, QColor(Qt::red),QColor(Qt::red), s));
    m->attach(this);
    replot();
}
