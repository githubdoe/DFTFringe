#include "profileplot.h"
#include "ui_profileplot.h"
#include <QtWidgets>
#include <qwt_compass.h>
#include <qwt_compass_rose.h>
#include <qwt_wheel.h>
#include <qwt_knob.h>
#include <qwt_dial_needle.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_point_data.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_magnifier.h>
#include <qwt_plot_grid.h>
#include <qwt_text.h>
#include <qwt_symbol.h>
#include <qwt_math.h>
#include <qwt_scale_engine.h>
#include <math.h>
#include "wavefront.h"
#include "profileplot.h"
#include <qevent.h>
#include <qwt_scale_draw.h>
#include <qlayout.h>
#include "settings2.h"

#define PITORAD  M_PI/180.;
double g_angle = 270. * PITORAD; //start at 90 deg (pointing east)
double y_offset = 0.;


bool ProfilePlot::eventFilter( QObject *object, QEvent *event )
{
    if ( event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        startPos = mouseEvent->pos();
        dragging = true;
        return true;

    }
    else if (dragging && event->type() == QEvent::MouseMove){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        QPoint p = mouseEvent->pos();
        y_offset = .005 * (startPos.y() - p.y());
        populate();
        m_plot->replot();
        return true;
    }
    else if (dragging && event->type() == QEvent::MouseButtonRelease){
        dragging = false;
        return true;
    }

    return QObject::eventFilter( object, event );
}

ProfilePlot::ProfilePlot(QWidget *parent , ContourTools *tools):
    QWidget( parent ), m_wf(0), m_tools(tools),
    dragging(false),offsetType("Middle"),
    ui(new Ui::ProfilePlot),m_showNm(1.), m_showSurface(1.)
{

    m_plot = new QwtPlot(this);
    type = 0;
    QHBoxLayout * l1 = new QHBoxLayout();
    QVBoxLayout *v1 = new QVBoxLayout();
    showNmCB = new QCheckBox("Show in Nanometers",this);
    showSurfaceCB =  new QCheckBox("Surface error");
    Show16 = new QRadioButton("16 diamgeters of current wavefront",this);
    OneOnly = new QRadioButton("one diameter of current wavefront",this);
    OneOnly->setChecked(true);
    ShowAll = new QRadioButton("All wavefronts",this);
    connect(Show16, SIGNAL(clicked()), this, SLOT(show16()));
    connect(OneOnly, SIGNAL(clicked()), this, SLOT(showOne()));
    connect(ShowAll, SIGNAL(clicked()), this, SLOT(showAll()));
    l1->addStretch();
    l1->addWidget(showNmCB);
    l1->addWidget(showSurfaceCB);
    l1->addWidget(OneOnly);
    l1->addWidget(Show16);
    l1->addWidget(ShowAll);
    v1->addLayout(l1);
    v1->addWidget(m_plot,10);

    setLayout(v1);


    m_plot->setTitle( "Profile of wavefront error" );


    //QPalette canvasPalette( Qt::white );
    //canvasPalette.setColor( QPalette::Foreground, QColor( 133, 190, 232 ) );
    //canvas->setPalette( canvasPalette );


    m_plot->canvas()->installEventFilter(this);

    // panning with the left mouse button
    //( void ) new QwtPlotPanner( canvas );

    // zoom in/out with the wheel
    //( void ) new QwtPlotMagnifier( canvas );

    QPalette palette0;
    for ( int c = 0; c < QPalette::NColorRoles; c++ )
    {
        const QPalette::ColorRole colorRole =
            static_cast<QPalette::ColorRole>( c );

        palette0.setColor( colorRole, QColor() );
    }

    palette0.setColor( QPalette::Base,
        palette().color( backgroundRole() ).light( 120 ) );
    palette0.setColor( QPalette::WindowText,
        palette0.color( QPalette::Base ) );
    compass = new QwtCompass( this );
    compass->setLineWidth( 4 );
    compass->setFrameShadow( QwtCompass::Raised );
    //compass->setMode( QwtCompass::RotateNeedle );


    palette0.setColor( QPalette::Base, Qt::darkBlue );
    palette0.setColor( QPalette::WindowText,
                       QColor( Qt::darkBlue ).dark( 120 ) );
    palette0.setColor( QPalette::Text, Qt::white );



    QwtCompassWindArrow *needle = new QwtCompassWindArrow(
        QwtCompassWindArrow::Style2, QColor(Qt::red),
        QColor( Qt::gray ));
    compass->setNeedle( needle );

    QwtCompassScaleDraw *scaleDraw = new QwtCompassScaleDraw();
    scaleDraw->enableComponent( QwtAbstractScaleDraw::Ticks, true );
    scaleDraw->enableComponent( QwtAbstractScaleDraw::Labels, false );
    scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    scaleDraw->setTickLength( QwtScaleDiv::MinorTick, 0 );
    scaleDraw->setTickLength( QwtScaleDiv::MediumTick, 0 );
    scaleDraw->setTickLength( QwtScaleDiv::MajorTick, 5 );

    compass->setScaleDraw( scaleDraw );
    compass->setScaleMaxMajor( 36 );
    //compass->setScaleMaxMinor( 5 );

    //compass->setNeedle(
       // new QwtCompassMagnetNeedle( QwtCompassMagnetNeedle::ThinStyle ) );
    compass->setValue( 270 );
    compass->setOrigin( -90 );
    connect(compass,SIGNAL(valueChanged(double)),this ,SLOT(angleChanged(double)));
    connect(m_tools, SIGNAL(newDisplayErrorRange(double,double)),
            this, SLOT(newDisplayErrorRange(double,double)));
    connect(m_tools, SIGNAL(contourZeroOffsetChanged(QString)), this, SLOT(zeroOffsetChanged(QString)));
    connect(showNmCB, SIGNAL(clicked(bool)), this, SLOT(showNm(bool)));
    connect(showSurfaceCB, SIGNAL(clicked(bool)),this, SLOT(showSurface(bool)));

    ui->setupUi(this);
    populate();
}
void ProfilePlot::showNm(bool flag){
    m_showNm = (flag) ? 550. : 1.;
    setSurface(m_wf);
}
void ProfilePlot::showSurface(bool flag){
    m_showSurface = (flag) ? .5: 1. ;
    setSurface(m_wf);
}

void ProfilePlot::showOne(){
    type = 0;
    populate();
    m_plot->replot();
}
void ProfilePlot::show16(){
    type = 1;
    populate();
    m_plot->replot();
}
void ProfilePlot::showAll(){
    type = 2;
    populate();
    m_plot->replot();
}
void ProfilePlot::zeroOffsetChanged(QString s){
    if (offsetType == s)
        return;
    double mul = m_showNm * m_showSurface;
    offsetType = s;
    double max = m_wf->max;
    double min = m_wf->min;
    if (s == "Middle"){
        y_offset = m_wf->mean;
        m_plot->setAxisScale(m_plot->yLeft, min * 1.5 * mul, max * 1.5 * mul);
    }
    else {
        y_offset =  -m_wf->min;

        m_plot->setAxisScale(m_plot->yLeft, 0, max * 5. * mul);
    }
    m_plot->replot();
}

void ProfilePlot::newDisplayErrorRange(double min, double max){
    double mul = m_showNm * m_showSurface;

    m_plot->setAxisScale(m_plot->yLeft, min * mul  ,max * mul );
    m_plot->replot();
}

void ProfilePlot::angleChanged(double a){
    g_angle = a * PITORAD;
    if (type == 2 || type == 0)
        populate();
    m_plot->replot();
}



void ProfilePlot::setSurface(wavefront * wf){
    m_wf = wf;
    QString surf("wavefront");
    if (m_showSurface == .5)
        surf = "surface";
    m_plot->setTitle( "Profile of " + surf + " error" );
    if (wf == 0)
        return;
    if (offsetType == "Minimum")
        y_offset = wf->min;
    else
        y_offset = -wf->mean;

    double mul = m_showNm * m_showSurface;
    m_plot->setAxisScale( m_plot->yLeft, fmin(-.125,m_tools->m_min) * 1.2 * mul ,
                          fmax(.125,m_tools->m_max) * 1.2 * mul);
    populate();
    m_plot->replot();
}
QPolygonF ProfilePlot::createProfile(double units, wavefront *wf){
    QPolygonF points;
    double steps = 1./wf->m_outside.m_radius;
    for (double rad = -1.; rad < 1.; rad += steps){
        int dx, dy;
        double radn = rad * wf->m_outside.m_radius;
        double radx = rad * wf->diameter/2.;
        dx = radn * cos(g_angle + M_PI_2) + wf->m_outside.m_center.x();
        dy = -radn * sin(g_angle + M_PI_2) + wf->m_outside.m_center.y();
        if (dy >= wf->data.rows || dx >= wf->data.cols || dy < 0 || dx < 0){
            points << QPointF(radx,0.0);
        }
        if (wf->workMask.at<bool>(dy,dx)){
            points << QPointF(radx,(units * (wf->workData.at<double>((int)dy,(int)dx)) *
                                    wf->lambda/550.) +y_offset * units);
        }
        else
            points << QPointF(radx, 0.0);
    }
    return points;
}
void ProfilePlot::populate()
{
    compass->setGeometry(QRect(70,5,70,70));
    QString tmp("nanometers");
    if (m_showNm == 1.)
        tmp = "wavelength at 550 nm";
    m_plot->setAxisTitle( m_plot->yLeft, "Error in " + tmp );
    m_plot->setAxisTitle( m_plot->xBottom, "Radius mm" );

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->attach( m_plot);
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->setMajorPen( Qt::black,0.0,Qt::DotLine);
    if (m_wf == 0)
        return;
    // axes
    double lower = -m_wf->diameter/2 - 10;
    double upper = m_wf->diameter/2 + 10;
    m_plot->setAxisScale(QwtPlot::xBottom, lower, upper);
    QwtScaleEngine * se1 = m_plot->axisScaleEngine(QwtPlot::xBottom);
    QwtScaleDiv sd1 = se1->divideScale(lower,upper, 40,5);
    m_plot->setAxisScaleDiv(QwtPlot::xBottom, sd1);
    m_plot->detachItems( QwtPlotItem::Rtti_PlotCurve);
    m_plot->detachItems( QwtPlotItem::Rtti_PlotMarker);
    //double b = 10. * ceil((m_wf->workData.cols/2.)/10.)+10;
    //m_plot->setAxisScale( QwtPlot::xBottom, -b, b,50);
    //m_plot->setAxisScaleDraw(m_plot->xBottom, new RadiusScaleDraw(m_wf->m_outside.m_radius,m_wf->diameter));

    // Insert new curves
    switch (type) {
    case 0:{
        QwtPlotCurve *cprofile = new QwtPlotCurve( "" );
        cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
        cprofile->setLegendAttribute( QwtPlotCurve::LegendShowLine, false );
        cprofile->setPen( Qt::black );
        cprofile->attach( m_plot );
        cprofile->setSamples( createProfile( m_showNm * m_showSurface,m_wf) );

        break;
    }
    case 1: {
        double startAngle = g_angle;
        for (int i = 0; i < 16; ++i){
            QPolygonF points;
            g_angle = startAngle + i * M_PI/ 16;
            QwtPlotCurve *cprofile = new QwtPlotCurve( "" );
            cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
            cprofile->setLegendAttribute( QwtPlotCurve::LegendShowLine, false );
            cprofile->setPen( Qt::black );

            cprofile->setSamples( createProfile( m_showNm * m_showSurface,m_wf));
            cprofile->attach( m_plot );

        }

        g_angle = startAngle;
        break;

    }
    case 2:{

        m_plot->insertLegend( new QwtLegend() , QwtPlot::BottomLegend);
        for (int i = 0; i < wfs->size(); ++i){
            QwtPlotCurve *cprofile = new QwtPlotCurve(wfs->at(i)->name );
            cprofile->setPen(QPen(Settings2::m_profile->getColor(i)));
            cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
            cprofile->setSamples( createProfile( m_showNm * m_showSurface,wfs->at(i)));
            cprofile->attach( m_plot );


        }

            break;
        }
    default:
        break;
    }



    // Insert markers

    //  ...a horizontal line at y = 0...
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLabel( QString::fromLatin1( "y = 0" ) );
    mY->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    mY->setLineStyle( QwtPlotMarker::HLine );
    mY->setYValue( 0.0 );
    mY->attach( m_plot);
    double mul = m_showNm * m_showSurface;
    //  ...a horizontal line at upper error margin...
    QwtPlotMarker *muY = new QwtPlotMarker();
    muY->setLabel( QString::fromLatin1( " 1/4 wave on wavefront" ) );
    muY->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    muY->setLineStyle( QwtPlotMarker::HLine );
    muY->setLinePen( Qt::red, 0, Qt::DashDotLine );
    muY->setYValue( 0.125 * mul );
    muY->attach( m_plot );

    //  ...a horizontal line at lower error margin...
    QwtPlotMarker *mlY = new QwtPlotMarker();
    mlY->setLabel( QString::fromLatin1( " -1/4 wave on wavefront" ) );
    mlY->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    mlY->setLineStyle( QwtPlotMarker::HLine );
    mlY->setLinePen( Qt::red, 0, Qt::DashDotLine );
    mlY->setYValue( -0.125 * mul );
    mlY->attach( m_plot );

    //  ...a vertical line at x = 2 * pi
    QwtPlotMarker *mX = new QwtPlotMarker();

    mX->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
    mX->setLabelOrientation( Qt::Vertical );
    mX->setLineStyle( QwtPlotMarker::VLine );
    mX->setLinePen( Qt::black, 0, Qt::DashDotLine );
    mX->setXValue(0 );
    mX->attach( m_plot );

}

void ProfilePlot::updateGradient()
{

    QPalette pal = palette();

    const QColor buttonColor = pal.color( QPalette::Button );

    QLinearGradient gradient( rect().topLeft(), rect().bottomLeft() );
    gradient.setColorAt( 0.0, Qt::white );
    gradient.setColorAt( 0.7, buttonColor );
    gradient.setColorAt( 1.0, buttonColor );

    pal.setBrush( QPalette::Window, gradient );
    setPalette( pal );
}

void ProfilePlot::resizeEvent( QResizeEvent *)
{
    //m_plot->resizeEvent( event );

    // Qt 4.7.1: QGradient::StretchToDeviceMode is buggy on X11
    //updateGradient();
}

    void ProfilePlot::setWavefronts(QVector<wavefront *> *wf){
        wfs = wf;
    }

