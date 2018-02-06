
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
#include "utils.h"
#include <qwt_plot_shapeitem.h>
#include <QSettings>
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
signals:
    void select(QString);
};


SpectrogramData::SpectrogramData(): m_wf(0)
{
}

void SpectrogramData::setSurface(wavefront *surface) {
    m_wf = surface;
    setInterval( Qt::XAxis, QwtInterval(0,m_wf->workData.cols));
    setInterval( Qt::YAxis, QwtInterval(0, m_wf->workData.rows));
}
#include <qwt_round_scale_draw.h>
double SpectrogramData::value( double x, double y ) const
{

    if (m_wf == 0)
        return sqrt(x * x + y * y) -.1;

    if (y >= m_wf->workData.rows || x >= m_wf->workData.cols || y < 0 || x < 0){
        return -10.0;
    }
    if ((m_wf->workMask.at<uint8_t>((int)y,(int)x)) != 255){
        return -10;
    }
    return  (m_wf->workData(y,x)* m_wf->lambda/outputLambda) -zOffset;

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
    replot();
}



void ContourPlot::showContoursChanged(double val){
    QSettings set;
    set.setValue("contourRange",val);
    contourRange = val;

    if (val == 0.){
        d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode,false);
        set.setValue("contourShowLines", false);
    }
    else {
        QwtInterval iz = d_spectrogram->data()->interval( Qt::ZAxis );
        QList<double> contourLevels;
        for ( double level = iz.minValue(); level <= iz.maxValue(); level += val)
            contourLevels << level;
        d_spectrogram->setContourLevels( contourLevels );
        d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode,true );
        set.setValue("contourShowLines", true);
        d_spectrogram->setDefaultContourPen(m_do_fill ? QPen(m_contourPen) : QPen(Qt::NoPen));
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
    emit setMinMaxValues(zmin,zmax);
    emit setWaveRange(zmax - zmin);

}
void ContourPlot::newDisplayErrorRange(double min,double max){
    QwtInterval zInt(min,max);
    SpectrogramData *data = (SpectrogramData*)d_spectrogram->data();
    data->setInterval( Qt::ZAxis, zInt);
    setColorMap(m_colorMapNdx);
    replot();
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
#include <QPainterPath>
#include <qwt_plot_shapeitem.h>
#include <qwt_plot_marker.h>
void ContourPlot::ruler(){

    detachItems(QwtPlotItem::Rtti_PlotShape);
    detachItems(QwtPlotItem::Rtti_PlotMarker);
    QSettings set;
    if (set.value("contourShowRuler", false).toBool()){
        int half = m_wf->data.rows/2.;
        double pixelsPermm =(m_wf->m_outside.m_radius/(m_wf->diameter/2));
        QPainterPath radials;
        for (int ang = 0; ang <= 360; ang += 30){

            double radians= ( ang * M_PI ) / 180. ;
            double sina = sin(radians);
            double cosa = cos(radians);
            // move to start
            int startx =  -half * cosa;
            int starty =  -half * sina;
            radials.moveTo( half + startx,half + starty);
            // line to end
            int endx = half - startx;
            int endy = half - starty;
            radials.lineTo(endx,endy);
        }

        QwtPlotShapeItem *item = new QwtPlotShapeItem( "");
        item->setShape(radials);
        item->setPen(Qt::gray,1);
        item->attach(this);
        for (int r = 10 ; r < m_wf->diameter/2; r+= 10){
            int rsize =  2 * r * pixelsPermm;
            QwtPlotShapeItem *item = new QwtPlotShapeItem( "");
            QRectF rect;
            QPointF p(half, half);
            rect.setSize( QSize(rsize,rsize) );
            rect.moveCenter(p );
            QPainterPath ppath;
            ppath.addEllipse( rect );
            item->setShape( ppath);
            QPen pen( Qt::gray, 1 );
            if (r % 50 == 0){
                pen = QPen(Qt::black,2);
                QwtPlotMarker *label = new QwtPlotMarker();
                label->setLineStyle(QwtPlotMarker::NoLine);
                QwtText t;
                label->setLabel(QString().number(r));
                label->setValue(rsize/2 + half, half);
                label->setLabelAlignment(Qt::AlignRight| Qt::AlignBottom);
                label->attach(this);
            }
            pen.setJoinStyle( Qt::MiterJoin );
            item->setPen( pen );
            //item->setBrush( fillColor );

            item->attach( this );
        }
        QwtPlotMarker *yAxis = new QwtPlotMarker();
        yAxis->setLineStyle(QwtPlotMarker::VLine);
        yAxis->setXValue(m_wf->data.cols/2);
        yAxis->setLinePen(Qt::black);
        yAxis->attach(this);
        QwtPlotMarker *xAxis = new QwtPlotMarker();
        xAxis->setLineStyle(QwtPlotMarker::HLine);
        xAxis->setYValue(m_wf->data.rows/2);
        xAxis->setLinePen(Qt::black);
        xAxis->attach(this);



    }
}

void ContourPlot::setSurface(wavefront * wf) {
    m_wf = wf;
    if (wf == 0)
        return;

    initPlot();
    m_tools->enablTools(true);
    m_zOffset = 0.;
    ruler();

    SpectrogramData *data = (SpectrogramData*)d_spectrogram->data();
    data->setSurface(wf);

    if (!m_useMiddleOffset)
        applyZeroOffset(false);

    setZRange();
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( QString().sprintf("wavefront error at %6.2lf nm", outputLambda) );
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
    if (l >= 2){
        name = path[l-2] + "/" + path[l-1];
    }
    else
        name = wf->name;


    setFooter(name + QString().sprintf(" %6.3lfrms %d X %d",wf->std, wf->data.cols, wf->data.rows));

    plotLayout()->setAlignCanvasToScales(true);
    showContoursChanged(contourRange);

    replot();
    //resize(QSize(width()-1,height()-1));
    //resize(QSize(width()+1,height()+1));
}
double ContourPlot::m_waveRange;
bool ContourPlot::m_useMiddleOffset = true;
int ContourPlot::m_colorMapNdx = 0;
QString ContourPlot::m_zRangeMode("Auto");
double ContourPlot::m_zOffset = 0.;
ContourPlot::ContourPlot( QWidget *parent, ContourTools *tools, bool minimal ):
    QwtPlot( parent ),m_wf(0),m_tools(tools), m_autoInterval(false),m_minimal(minimal),m_contourPen(Qt::white)
{
    d_spectrogram = new QwtPlotSpectrogram();
    QSettings settings;
    m_colorMapNdx = settings.value("colorMapType",0).toInt();
    contourRange = settings.value("contourRange", .1).toDouble();
    m_contourPen = QColor(settings.value("ContourLineColor", "white").toString());
    m_do_fill = settings.value("contourShowFill", true).toBool();


    plotLayout()->setAlignCanvasToScales( true );
    initPlot();

}
void ContourPlot::initPlot(){
    QSettings settings;
    bool fill = settings.value("contourShowFill", true).toBool();
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count
    d_spectrogram->setDefaultContourPen( fill ? QPen( m_contourPen): QPen(Qt::NoPen));
    d_spectrogram->setColorMap( new dftColorMap(settings.value("colorMapType",0).toInt()) );
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    d_spectrogram->setData( new SpectrogramData() );
    d_spectrogram->attach( this );

    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, fill );
    QList<double> contourLevels;
    double contourVal = contourRange;


    if (contourVal > 0.001){
        for ( double level = -1; level <1; level += contourVal)
         contourLevels << level;
        d_spectrogram->setContourLevels( contourLevels );
    } else{
        d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode,m_do_fill);
    }
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
    showSpectrogram(fill);
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
        d_spectrogram->setDefaultContourPen(m_do_fill ? QPen(m_contourPen) : QPen(Qt::NoPen));
    }

    replot();
}

void ContourPlot::showContour( bool on )
{
    QSettings set;
    set.setValue("contourShowLines", on);
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode, on );
    if (on)
        d_spectrogram->setDefaultContourPen(m_do_fill ? QPen(m_contourPen) : QPen(Qt::NoPen));
    replot();
}

void ContourPlot::showSpectrogram(bool on )
{
    QSettings set;
    set.setValue("contourShowFill", on);
    m_do_fill = on;
    d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ImageMode, on );
    d_spectrogram->setDefaultContourPen(m_do_fill ? QPen(m_contourPen) : QPen(Qt::NoPen));

    QwtPlotShapeItem *item = new QwtPlotShapeItem( "" );
    item->setItemAttribute( QwtPlotItem::Legend, true );
    item->setLegendMode( QwtPlotShapeItem::LegendShape );
    item->setLegendIconSize( QSize( 20, 20 ) );
    item->setRenderHint( QwtPlotItem::RenderAntialiased, true );

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

