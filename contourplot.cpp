
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

#include <qprinter.h>
#include <qprintdialog.h>

#include <qwt_plot_spectrogram.h>
#include <qwt_scale_widget.h>
#include <qwt_scale_draw.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_renderer.h>
#include <qwt_plot_grid.h>
#include "contourplot.h"
#include "opencv/cv.h"
#include "wavefront.h"
#include <QtGui/qevent.h>
#include <qwt_plot_rescaler.h>
#include <QtGui>
#include <opencv/cv.h>
#include "opencv/highgui.h"
#include "dftcolormap.h"
#include <QDebug>
#include <math.h>
double zOffset = 0;

class MyZoomer: public QwtPlotZoomer
{
public:
    MyZoomer( QWidget *canvas ):
        QwtPlotZoomer( canvas )
    {
        setTrackerMode( AlwaysOn );
    }

    virtual QwtText trackerTextF( const QPointF &pos ) const
    {
        QColor bg( Qt::white );
        bg.setAlpha( 200 );

        QwtText text = QwtPlotZoomer::trackerTextF( pos );
        text.setBackgroundBrush( QBrush( bg ) );
        return text;
    }
};


SpectrogramData::SpectrogramData(): m_wf(0)
{
}

void SpectrogramData::setSurface(wavefront *surface) {
    m_wf = surface;
    setInterval( Qt::XAxis, QwtInterval(0, m_wf->workData.cols));
    setInterval( Qt::YAxis, QwtInterval(0, m_wf->workData.rows));

}

double SpectrogramData::value( double x, double y ) const
{

    if (m_wf == 0)
        return sqrt(x * x + y * y) -.1;

    if (y >= m_wf->workData.rows || x >= m_wf->workData.cols || y < 0 || x < 0){
        //qDebug() << QString().sprintf("Warning contour index out of range %d %d %lf %lf %lf %d %d ",
        //                              dx,dy,x,y,m_sf, m_wf->workData.cols, m_wf->workData.rows);

        return -10.0;
    }
    if ((m_wf->workMask.at<uint8_t>((int)y,(int)x)) != 255){
        return -10;
    }
    //qDebug() << m_wf->workData.at<double>(y,x);
    return  (m_wf->workData.at<double>(y,x)* m_wf->lambda/550.) -zOffset;

}



void ContourPlot::setColorMap(int ndx){
    QwtInterval iz = d_spectrogram->data()->interval( Qt::ZAxis );
    d_spectrogram->setColorMap( new dftColorMap(ndx,m_wf,!m_useMiddleOffset ));
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setColorMap( iz, new dftColorMap(m_colorMapNdx,m_wf,!m_useMiddleOffset) );
    if (!m_minimal)
        setAxisScale( QwtPlot::yRight, iz.minValue()  ,iz.maxValue() );
}



void ContourPlot::ContourMapColorChanged(int ndx) {
    m_colorMapNdx = ndx;
    QSettings set;
    set.setValue("colorMapType",ndx);
    setColorMap(ndx);
    replot();
}

void ContourPlot::contourWaveRangeChanged(double val ){
    m_waveRange = val;
    setZRange();
}

void ContourPlot::contourIntervalChanged(double val){
    contourRange = val;
    QSettings set;
    set.setValue("contourRange",val);
    QwtInterval iz = d_spectrogram->data()->interval( Qt::ZAxis );
    QList<double> contourLevels;
    for ( double level = iz.minValue(); level < iz.maxValue(); level += val)
        contourLevels += level;
    d_spectrogram->setContourLevels( contourLevels );
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, true );
    replot();
}

void ContourPlot::showContoursChanged(double val){
    contourRange = val;
    if (val == 0.)
        d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, false );
    else {
        QwtInterval iz = d_spectrogram->data()->interval( Qt::ZAxis );
        QList<double> contourLevels;
        for ( double level = iz.minValue(); level < iz.maxValue(); level += val)
            contourLevels += level;
        d_spectrogram->setContourLevels( contourLevels );
        d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, true );
        d_spectrogram->setDefaultContourPen(QPen(m_contourPen));
    }
    replot();
}

void ContourPlot::applyZeroOffset(bool useMiddle){
    double min,max;
    max = m_wf->max;
    min = m_wf->min;
    if (useMiddle){
        zOffset = 0.;
        m_useMiddleOffset = true;

    }
    else {
        zOffset = min;
        m_useMiddleOffset = false;
        max -= zOffset;
        min -= zOffset;

    }
    setZRange();

    //emit setMinMaxValues(min, max);
}

void ContourPlot::setZRange(){
    double zmin = 0, zmax = 0;

    SpectrogramData *data = (SpectrogramData*)d_spectrogram->data();
    if (m_zRangeMode == "Auto"){
        double std = fmax(m_wf->std, .01);
        zmin = m_wf->mean - 3 * std;
        if (!m_useMiddleOffset)
            zmin = 0;
        zmax = m_wf->mean + 3 * std;
        m_waveRange = (zmax - zmin);

    }
    else if (m_zRangeMode == "Min/Max"){
        zmin = m_wf->min;
        zmax = m_wf->max;
    }
    else if (m_zRangeMode == "Fractions of Wave"){
        zmin = -m_waveRange/2;
        zmax = -zmin;
    }

    QwtInterval zInt(zmin,zmax);

    data->setInterval( Qt::ZAxis, zInt);
    setColorMap(m_colorMapNdx);

    double range = zmax - zmin;
    bool changed = false;
    double lineCount = range/contourRange;
    while ( lineCount > 10)
    {
        contourRange += .05;
        lineCount = range/contourRange;
        changed = true;
    }
    if (changed){
        emit newContourRange(contourRange);
    }
    emit setMinMaxValues(zmin,zmax);
    emit setWaveRange(zmax - zmin);

}

void ContourPlot::contourColorRangeChanged(const QString &arg1){
    m_zRangeMode = arg1;
    setZRange();
    replot();
}

void ContourPlot::contourZeroOffsetChanged(const QString & val){
    applyZeroOffset(val == "Middle");
    replot();
}

void ContourPlot::drawCanvas(QPainter* p)
{
    QwtPlot::drawCanvas( p );  // <<---

    QStaticText txt("number");

}
void ContourPlot::setSurface(wavefront * wf) {
    m_wf = wf;
    if (wf == 0)
        return;
    initPlot();

    m_tools->enablTools(true);
    m_zOffset = 0.;

    SpectrogramData *data = (SpectrogramData*)d_spectrogram->data();
    data->setSurface(wf);

    if (!m_useMiddleOffset)
        applyZeroOffset(false);

    setZRange();


    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "wavefront error" );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorBarWidth(30);
    if (!m_minimal){
        enableAxis( QwtPlot::yRight );
        enableAxis(QwtPlot::yLeft);
    }
    else
    {
        enableAxis(QwtPlot::yLeft, false);
        enableAxis(QwtPlot::xBottom, false);
    }
    QStringList path = wf->name.split("/");
    QString name;
    int l = path.length();
    if (l >= 3){
        name = path[l-3] + "/" + path[l-2] + "/" + path[l-1];
    }
    else
        name = wf->name;

    setFooter(name + QString().sprintf(" %6.3lfrms",wf->std));
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, false );
//    QwtPlotRescaler *d_rescaler = new QwtPlotRescaler(canvas());
//    d_rescaler->setRescalePolicy(QwtPlotRescaler::Fitting);
//    d_rescaler->setReferenceAxis(QwtPlot::yLeft);
//    d_rescaler->setAspectRatio(QwtPlot::xBottom, 1.0);
//    d_rescaler->setAspectRatio(QwtPlot::yRight, 0.0);

//    d_rescaler->setIntervalHint(Qt::XAxis, d_spectrogram->data()->interval( Qt::XAxis ));
//    d_rescaler->setIntervalHint(Qt::YAxis, d_spectrogram->data()->interval( Qt::YAxis ));
//    for ( int axis = 0; axis < QwtPlot::axisCnt; axis++ )
//        setAxisAutoScale(axis);
    plotLayout()->setAlignCanvasToScales(true);
    showContoursChanged(contourRange);

    replot();
    //resize(QSize(width()-1,height()-1));
    //resize(QSize(width()+1,height()+1));
}

ContourPlot::ContourPlot( QWidget *parent, ContourTools *tools, bool minimal ):
    QwtPlot( parent ),m_wf(0),m_tools(tools), m_useMiddleOffset(true),m_colorMapNdx(0)
    ,m_zRangeMode("Auto"), m_autoInterval(false),m_minimal(minimal),m_contourPen(Qt::white)
{
    d_spectrogram = new QwtPlotSpectrogram();
    QSettings settings;
    m_colorMapNdx = settings.value("colorMapType",0).toInt();
    contourRange = settings.value("contourRange", .1).toDouble();
    m_contourPen = QColor(settings.value("ContourLineColor", "white").toString());





    plotLayout()->setAlignCanvasToScales( true );
    initPlot();

    // LeftButton for the zooming
    // MidButton for the panning
    // RightButton: zoom out by 1
    // Ctrl+RighButton: zoom out to full size
/*
    QwtPlotZoomer* zoomer = new MyZoomer( canvas() );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
                             Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
                             Qt::RightButton );

    QwtPlotPanner *panner = new QwtPlotPanner( canvas() );
    panner->setAxisEnabled( QwtPlot::yRight, false );
    panner->setMouseButton( Qt::MidButton );
*/
    // Avoid jumping when labels with more/less digits
    // appear/disappear when scrolling vertically



    //const QColor c( Qt::darkBlue );
    //zoomer->setRubberBandPen( c );
    //zoomer->setTrackerPen( c );

}
void ContourPlot::initPlot(){

    QSettings settings;
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count
    d_spectrogram->setDefaultContourPen( QPen( m_contourPen));
    d_spectrogram->setColorMap( new dftColorMap(settings.value("colorMapType",0).toInt()) );
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    d_spectrogram->setData( new SpectrogramData() );
    d_spectrogram->attach( this );
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, true );

    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, false );
    QList<double> contourLevels;
    for ( double level = -1; level < 1; level += .05)
        contourLevels += level;
    d_spectrogram->setContourLevels( contourLevels );
    const QFontMetrics fm( axisWidget( QwtPlot::yLeft )->font() );
    if (!m_minimal){
        QwtScaleDraw *sd = axisScaleDraw( QwtPlot::yLeft );
        sd->setMinimumExtent( fm.width( "100.00" ) );

        QwtPlotGrid *grid = new QwtPlotGrid();
        grid->enableXMin(true);
        //grid->enableYMin(true);
        grid->setPen( Qt::gray, 0.0, Qt::DotLine );
        grid->attach( this);
    }
    showSpectrogram(1);
}

void ContourPlot::setTool(ContourTools *tool){
    m_tools = tool;
    if (tool)
        m_tools->connectTo(this);
}

void ContourPlot::on_line_color_changed(QColor c)
{
    m_contourPen = c;
    //qDebug() << c.name();
    d_spectrogram->setDefaultContourPen(QPen(c));
    QSettings settings;
    settings.setValue("ContourLineColor", c.name());
    replot();
}

void ContourPlot::contourFillChanged(int val)
{
    if (val){
        d_spectrogram->setDefaultContourPen(QPen(m_contourPen));
    }

    replot();
}

void ContourPlot::showContour( bool on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    if (on)
        d_spectrogram->setDefaultContourPen(QPen(m_contourPen));
    replot();
}

void ContourPlot::showSpectrogram(int on )
{
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen(
                on ? QPen( m_contourPen,1 ) : QPen( Qt::NoPen ) );

    replot();
}

void ContourPlot::setAlpha( int alpha )
{
    d_spectrogram->setAlpha( alpha );
    replot();
}

#ifndef QT_NO_PRINTER

void ContourPlot::printPlot()
{
    QPrinter printer( QPrinter::HighResolution );
    printer.setOrientation( QPrinter::Landscape );
    printer.setOutputFileName( "spectrogram.pdf" );

    QPrintDialog dialog( &printer );
    if ( dialog.exec() )
    {
        QwtPlotRenderer renderer;

        if ( printer.colorMode() == QPrinter::GrayScale )
        {
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasBackground );
            renderer.setDiscardFlag( QwtPlotRenderer::DiscardCanvasFrame );
            renderer.setLayoutFlag( QwtPlotRenderer::FrameWithScales );
        }

        renderer.renderTo( this, printer );
    }
}

#endif

