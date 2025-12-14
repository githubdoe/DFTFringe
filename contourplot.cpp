
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
#include <opencv2/opencv.hpp>
#include "wavefront.h"
#include <QtGui/qevent.h>
#include <QtGui>
#include <opencv2/highgui/highgui.hpp>
#include "dftcolormap.h"
#include <QDebug>
#include <math.h>
#include "utils.h"
#include <qwt_round_scale_draw.h>
#include <qwt_plot_shapeitem.h>
#include <QSettings>
#include <qwt_picker_machine.h>
#include <QPainterPath>
#include <qwt_plot_marker.h>
#include "spdlog/spdlog.h"
#include <QTimer>


double zOffset = 0;
double lastx = -1.;
double lasty = -1.;

// ============================================================================
// MyZommer - Custom zoomer to show data value at cursor
// ============================================================================
class MyZoomer: public QwtPlotZoomer
{
public:
    ContourPlot* thePlot;
    MyZoomer( QWidget *canvas ,ContourPlot* plot):
        QwtPlotZoomer( canvas ), thePlot(plot)
    {
        setTrackerMode( AlwaysOn );
    }

    // Override zoom rectangle to maintain image aspect ratio
    void zoom(const QRectF &rect) override
    {
        if ((thePlot == nullptr) || (thePlot->m_wf == nullptr))
        {
            QwtPlotZoomer::zoom(rect);
            return;
        }

        // Get the canvas dimensions and aspect ratio (accounts for axes, labels, colorbar, etc.)
        QWidget *canvas = thePlot->canvas();
        double canvasWidth = canvas->width();
        double canvasHeight = canvas->height();
        if (canvasWidth <= 0 || canvasHeight <= 0)
        {
            QwtPlotZoomer::zoom(rect);
            return;
        }
        double canvasRatio = canvasWidth / canvasHeight;

        // Get the selected zoom rectangle dimensions and its aspect ratio
        double selWidth = rect.width();
        double selHeight = rect.height();
        double selRatio = selWidth / selHeight;

        QRectF adjustedRect = rect;

        // The zoom rectangle should have the same aspect ratio as the canvas
        // to ensure no distortion when displayed
        if (selRatio > canvasRatio)
        {
            // Selected rect is wider than canvas → expand height to match canvas ratio
            double newHeight = selWidth / canvasRatio;
            double extra = (newHeight - selHeight) / 2.0;
            adjustedRect.setTop(rect.top() - extra);
            adjustedRect.setBottom(rect.bottom() + extra);
        }
        else if (selRatio < canvasRatio)
        {
            // Selected rect is taller than canvas → expand width to match canvas ratio
            double newWidth = selHeight * canvasRatio;
            double extra = (newWidth - selWidth) / 2.0;
            adjustedRect.setLeft(rect.left() - extra);
            adjustedRect.setRight(rect.right() + extra);
        }
        // Set guard to prevent intermediate aspect-ratio updates while zooming
        thePlot->m_inZoomOperation = true;
        QwtPlotZoomer::zoom(adjustedRect);
        // Ensure guard is cleared on next event loop tick in case rescale() isn't called
        QTimer::singleShot(0, thePlot, SLOT(clearZoomFlag()));
    }

    // Override zoom state change to reapply aspect ratio when unzooming
    void rescale() override
    {
        QwtPlotZoomer::rescale();

        // Check if we're back at the base zoom level
        if ((thePlot != nullptr) && zoomRectIndex() == 0)
        {
            thePlot->updateAspectRatio();
        }

        // Clear zoom guard (may have been set in zoom())
        if (thePlot != nullptr){
            thePlot->clearZoomFlag();
        }
    }

    // when holding shift key, show data value at cursor
    QwtText trackerTextF( const QPointF &pos ) const override
    {
        if (thePlot->m_wf == 0)
            return QwtText("");
        if(!QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))
            return QwtText("");
        if (pos.x() == lastx && pos.y() == lasty){
            double v = thePlot->d_spectrogram->data()->value(pos.x(),pos.y());
            QString t =  QString("%1").arg(v, 0, 'f');

            QwtText label(t);

            label.setColor(QColor(255,255,255));
            label.setBorderPen(QPen(QColor(100,0,0), 3));
            label.setBorderRadius(5);
            label.setBackgroundBrush(QColor(65, 177, 225, 150));

            QFont font("MS Shell Dlg 2", 18);
            label.setFont(font);
            return QwtText(label);
        }
        lastx = pos.x();
        lasty = pos.y();
        QColor bg( Qt::white );
        bg.setAlpha( 200 );
        double v = thePlot->d_spectrogram->data()->value(pos.x(),pos.y());
        int half = thePlot->m_wf->data.rows/2.;
        double delx = pos.x() - half;
        double dely = pos.y() - half;
        //double angle = atan2(dely,delx);
        double R = sqrt( delx * delx + dely * dely )/thePlot->m_wf->data.rows;
        double r = R * thePlot->m_wf->diameter;
        QwtText text(QString("%1 R:%2mm").arg(v, 0, 'f').arg(r, 3, 'f', 2));
        text.setFont(QFont("Arial",12));
        text.setBackgroundBrush( QBrush( bg ) );
        thePlot->selected(pos);
        return text;
    }
signals:
    void select(QString);
};

// ============================================================================
// SpectrogramData - Raster data provider for the spectrogram display
// ============================================================================
class SpectrogramData: public QwtRasterData
{
public:
    SpectrogramData();
    const wavefront *m_wf;
    void setSurface(const wavefront *surface);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // keep compatibility with newer version of QWT used in QT6
    QwtInterval interval(Qt::Axis axis) const override;
    void setInterval(Qt::Axis axis, const QwtInterval &interval);
#endif
    virtual double value( double x, double y ) const override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // keep compatibility with newer version of QWT used in QT6
private:
    QwtInterval m_xInterval;
    QwtInterval m_yInterval;
    QwtInterval m_zInterval;
#endif
};

SpectrogramData::SpectrogramData(): m_wf(0)
{

}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
// keep compatibility with newer version of QWT used in QT6
QwtInterval SpectrogramData::interval(Qt::Axis axis) const
{
    switch (axis)
    {
        case Qt::XAxis:
            return m_xInterval;
        case Qt::YAxis:
            return m_yInterval;
        case Qt::ZAxis:
            return m_zInterval;
        default:
            return QwtInterval();
    }
}

void SpectrogramData::setInterval(Qt::Axis axis, const QwtInterval &interval)
{
    switch (axis)
    {
        case Qt::XAxis:
            m_xInterval = interval;
            break;
        case Qt::YAxis:
            m_yInterval = interval;
            break;
        case Qt::ZAxis:
            m_zInterval = interval;
            break;
        default:
            break;
    }
}
#endif

void SpectrogramData::setSurface(const wavefront *surface) {
    m_wf = surface;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // keep compatibility with newer version of QWT used in QT6

    // Store interval info for use in `interval()`
    m_xInterval = QwtInterval(0, m_wf->workData.cols);
    m_yInterval = QwtInterval(0, m_wf->workData.rows);
#else
    setInterval( Qt::XAxis, QwtInterval(0,m_wf->workData.cols));
    setInterval( Qt::YAxis, QwtInterval(0, m_wf->workData.rows));
#endif
}

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

// ============================================================================
// ContourPlot - Main contour plot class
// ============================================================================

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
        for ( double level = iz.minValue(); level <= iz.maxValue() + val; level += val) {
            contourLevels << level;
        }

        d_spectrogram->setContourLevels( contourLevels );
        d_spectrogram->setDisplayMode( QwtPlotSpectrogram::ContourMode,true );
        set.setValue("contourShowLines", true);
        d_spectrogram->setDefaultContourPen(m_do_fill ? QPen(m_contourPen) : QPen(Qt::NoPen));
    }
    replot();
}

void ContourPlot::applyZeroOffset(bool useMiddle){
    //double max = m_wf->max;
    double min = m_wf->min;
    if (useMiddle){
        zOffset = 0.;
        m_useMiddleOffset = true;
    }
    else {
        zOffset = min;
        m_useMiddleOffset = false;
        //max -= zOffset;
        //min -= zOffset;
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
        zmin = m_min;
        zmax = m_max;
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
    m_min = min;
    m_max = max;
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
}

void ContourPlot::ruler(){

    detachItems(QwtPlotItem::Rtti_PlotShape);
    detachItems(QwtPlotItem::Rtti_PlotMarker);
    QSettings set;
    if (set.value("contourShowRuler", false).toBool()){
        int half = m_wf->data.rows/2.;
        double pixelsPermm =(m_wf->m_outside.m_radius/(m_wf->diameter/2));
        QPainterPath radials;
        for (int ang = 0; ang <= 360; ang += m_radialDeg){

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
        item->setPen(m_rulerPen);
        item->attach(this);
        double rad = m_wf->diameter/2;
        for (double r = .1 ; r < 1.; r+= .1){
            int rsize =  2 * r * rad * pixelsPermm;
            QwtPlotShapeItem *item = new QwtPlotShapeItem( "");
            QRectF rect;
            QPointF p(half, half);
            rect.setSize( QSize(rsize,rsize) );
            rect.moveCenter(p );
            QPainterPath ppath;
            ppath.addEllipse( rect );
            item->setShape( ppath);
            QPen pen = m_rulerPen;
            if (r == .5){
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
        yAxis->setLinePen(Qt::black,2);
        yAxis->attach(this);
        QwtPlotMarker *xAxis = new QwtPlotMarker();
        xAxis->setLineStyle(QwtPlotMarker::HLine);
        xAxis->setYValue(m_wf->data.rows/2);
        xAxis->setLinePen(Qt::black,2);
        xAxis->attach(this);
    }
}

void ContourPlot::selected(QPointF pos){
    if (m_wf==0)
        return;

    if(!QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier))
        return;

    int half = m_wf->data.rows/2.;
    double delx = pos.x() - half;
    double dely = pos.y() - half;
    double angle = atan2(dely,delx);
    if (angle != m_lastAngle){
        drawProfileLine(angle);
        if (m_linkProfile)
            emit sigPointSelected(pos);
        m_lastAngle = angle;
    }
}

void ContourPlot::drawProfileLine(const double angle){

    ruler();
    double sina = sin(angle);
    double cosa = cos(angle);
    QPainterPath radials;
    int half = m_wf->data.rows/2.;
    // move to start
    int startx =  -half * cosa;
    int starty =  -half * sina;
    radials.moveTo( half + startx,half + starty);
    // line to end
    int endx = half - startx;
    int endy = half - starty;
    radials.lineTo(endx,endy);

    QwtPlotShapeItem *item = new QwtPlotShapeItem( "");
    item->setShape(radials);
    item->setPen(QColor(50,50,50,100),3,Qt::DashDotDotLine);

    QwtPlotShapeItem *itemb = new QwtPlotShapeItem( "");
    itemb->setShape(radials);
    itemb->setPen(QColor(250,250,250,100),10);
    itemb->setBrush(QBrush(QColor(255,255,255)));
    itemb->attach(this);
    item->attach(this);
    replot();
}


void ContourPlot::setSurface(const wavefront * wf) {
    m_wf = wf;
    if (wf == 0)
        return;

    initPlot();
    if(m_tools != nullptr)
        m_tools->enablTools(true);
    m_zOffset = 0.;
    ruler();

    SpectrogramData *data = (SpectrogramData*)d_spectrogram->data();
    data->setSurface(wf);

    if (!m_useMiddleOffset)
        applyZeroOffset(false);

    setZRange();
    QwtScaleWidget *rightAxis = axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( QString("wavefront error at %1 nm").arg(outputLambda, 6, 'f', 2) );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorBarWidth(30);
    if (!m_minimal){
        enableAxis(QwtPlot::yRight);
        enableAxis(QwtPlot::yLeft);
    }
    else{
        enableAxis(QwtPlot::yLeft, false);
        enableAxis(QwtPlot::xBottom, false);
    }
    QStringList path = wf->name.split("/");
    QString name;
    int l = path.length();
    if (l >= 2){
        name = path[l-1];
    }
    else
        name = wf->name;
    name = name.replace(".wft","");

    setFooter(name + QString(" %1 rms %2 X %3").arg(wf->std, 6, 'f', 3).arg(wf->data.cols).arg(wf->data.rows));

    setAxisScale(QwtPlot::xBottom, 0, wf->data.cols);
    setAxisScale(QwtPlot::yLeft, 0, wf->data.rows);

    // Enforce aspect ratio on first draw
    updateAspectRatio();

    // Set canvas alignment after rescale
    plotLayout()->setAlignCanvasToScales(true);

    spdlog::get("logger")->trace("ContourPlot::setSurface {}x{}", wf->data.cols, wf->data.rows);

    showContoursChanged(contourRange); //TODO setSurface should not have to call showContoursChanged
    tracker_->setZoomBase(true);
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
    QwtPlot( parent ),m_wf(0),m_tools(tools),m_minimal(minimal), m_linkProfile(true),m_contourPen(Qt::white), m_inZoomOperation(false)
{
    spdlog::get("logger")->trace("ContourPlot::ContourPlot");
    d_spectrogram = new QwtPlotSpectrogram();
    picker_ = new QwtPlotPicker(this->canvas());
    picker_->setStateMachine(new QwtPickerClickPointMachine);

    tracker_ = new MyZoomer(this->canvas(), this);

    // Using the old SIGNAL/SLOT syntax because problems with QWT.
    // Qt is not able to match signal at runtime even if compile time checks all passed.
    // ChatGPT tells it might be an ABI problem with QWT library but I (JST) have been unable to fix for now (2025-10-20).
    connect(picker_, SIGNAL(selected(const QPointF&)), SLOT(selected(const QPointF&)));
    //connect(picker_, QOverload<const QPointF&>::of(&QwtPlotPicker::selected), this, &ContourPlot::selected);

    QSettings settings;
    m_colorMapNdx = settings.value("colorMapType",0).toInt();
    contourRange = settings.value("contourRange", .1).toDouble();
    m_contourPen = QColor(settings.value("ContourLineColor", "white").toString());
    m_do_fill = settings.value("contourShowFill", true).toBool();
    m_rulerPen = QPen(QColor(settings.value("ContourRulerColor", "grey").toString()));
    m_radialDeg = settings.value("contourRulerRadialDeg", 30).toInt();
    m_linkProfile = settings.value("linkProfilePlot", true).toBool();
    plotLayout()->setAlignCanvasToScales( true );

    initPlot();
    canvas()->installEventFilter(this);

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
        sd->setMinimumExtent( fm.horizontalAdvance( "100.00" ) );

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

    replot();
}

void ContourPlot::setAlpha( int alpha )
{
    d_spectrogram->setAlpha( alpha );
    replot();
}

void ContourPlot::updateAspectRatio()
{
    static bool isReentering = false;

    if (m_wf == nullptr){
        return;
    }

    // Prevent re-entrancy
    if (isReentering){
        return;
    }
    isReentering = true;

    QWidget *c = canvas();
    int w = c->width();
    int h = c->height();

    if (w <= 0 || h <= 0){
        isReentering = false;
        return;
    }

    double imgWidth  = m_wf->data.cols;
    double imgHeight = m_wf->data.rows;

    double dataRatio = imgWidth / imgHeight;
    double pixRatio  = double(w) / double(h);

    double x1;
    double x2;
    double y1;
    double y2;

    if (pixRatio > dataRatio)
    {
        // Canvas is wider → Y fits perfectly, X must be expanded
        double scaledWidth = imgHeight * pixRatio;  // what X must become
        double extra = (scaledWidth - imgWidth) * 0.5;
        x1 = -extra;
        x2 = imgWidth + extra;

        y1 = 0;
        y2 = imgHeight;
    }
    else
    {
        // Canvas is taller → X fits perfectly, Y must be expanded
        double scaledHeight = imgWidth / pixRatio;
        double extra = (scaledHeight - imgHeight) * 0.5;
        y1 = -extra;
        y2 = imgHeight + extra;

        x1 = 0;
        x2 = imgWidth;
    }

    setAxisScale(QwtPlot::xBottom, x1, x2);
    setAxisScale(QwtPlot::yLeft,  y1, y2);

    replot();

    isReentering = false;
}

bool ContourPlot::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == canvas() && event->type() == QEvent::Resize)
    {
        if (!m_inZoomOperation) {
            updateAspectRatio();
        }
        return false;
    }
    return QwtPlot::eventFilter(obj, event);
}

void ContourPlot::clearZoomFlag()
{
    m_inZoomOperation = false;
}

#ifndef QT_NO_PRINTER

void ContourPlot::printPlot()
{
    QPrinter printer( QPrinter::HighResolution );
    printer.setPageOrientation( QPageLayout::Landscape );
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
