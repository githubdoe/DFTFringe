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
#include "mirrordlg.h"
#include <qwt_plot_textlabel.h>
extern double outputLambda;

#define PITORAD  M_PI/180.;
double g_angle = 270. * PITORAD; //start at 90 deg (pointing east)
double y_offset = 0.;


bool ProfilePlot::eventFilter( QObject *object, QEvent *event )
{
    if ( event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        if (mouseEvent->button() == Qt::RightButton){
            showContextMenu(mouseEvent->pos());
            return false;
        }
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
     m_showSurface(1.),m_showNm(1.),dragging(false),
     offsetType("Middle"),ui(new Ui::ProfilePlot), m_defocusValue(0.)
{
    zoomed = false;
    m_plot = new QwtPlot(this);
    type = 0;
    QHBoxLayout * l1 = new QHBoxLayout();
    QVBoxLayout *v1 = new QVBoxLayout();
    showNmCB = new QCheckBox("Show in Nanometers",this);
    showSurfaceCB =  new QCheckBox("Surface error");
    Show16 = new QRadioButton("16 diameters of current wavefront",this);
    OneOnly = new QRadioButton("one diameter of current wavefront",this);
    OneOnly->setChecked(true);
    ShowAll = new QRadioButton("All wavefronts",this);
    connect(Show16, SIGNAL(clicked()), this, SLOT(show16()));
    connect(OneOnly, SIGNAL(clicked()), this, SLOT(showOne()));
    connect(ShowAll, SIGNAL(clicked()), this, SLOT(showAll()));
    l1->addStretch();
    showSlopeError = new QCheckBox("Show Slope: ");
    slopeLimitSB = new QDoubleSpinBox();


    QSettings set;
    slopeLimitArcSec = set.value("slopeLimitArcSec", 1.).toDouble();
    slopeLimitSB->setValue(slopeLimitArcSec);
    slopeLimitSB->setPrefix(" > ");
    slopeLimitSB->setSuffix(" arcseconds on wavefront");
    slopeLimitSB->setToolTip("<html><head/><body><p>Besides PV or RMS measurements of the wavefrontit is also important"
                             " to check for slope values that exceed 1 arc second on the wave front.</p><p>This display"
                             " colors any pixel whoes neighbor pixel creates a slope in the wavefront greater than the "
                             "value set in this control in arc seconds.</p><p>These values are highly dependent on the "
                             "smoothing value used for the wavefront. The usual method of choosing a good smoothing value "
                             "is</p><p>to look at the smoothed simulated foucault image until it matches what the real "
                             "Foucault image is or until it seems similar to other foucault images </p><p>you have seen."
                             "There is always the risk that too much smoothing will reduce the slope of actual details.</p></body></html>");
    m_showSlopeError = set.value("slopeShow", true).toBool();
    if (!m_showSlopeError)
        slopeLimitSB->hide();
    showSlopeError->setChecked(m_showSlopeError);
    connect(slopeLimitSB, SIGNAL(valueChanged(double)), this, SLOT(slopeLimit(double)));
    connect(showSlopeError,SIGNAL(clicked(bool)), this, SLOT(showSlope(bool)));

    l1->addWidget(showSlopeError);
    l1->addWidget(slopeLimitSB);
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
void ProfilePlot::showSlope(bool val){
    m_showSlopeError = val;
    if (!val)
        slopeLimitSB->hide();
    else
        slopeLimitSB->show();

    QSettings set;
    set.setValue("slopeShow",val);
    populate();
    m_plot->replot();
}
void ProfilePlot::slopeLimit(double val){
    slopeLimitArcSec = val;
    QSettings set;
    set.setValue("slopeLimitArcSec",val);
    populate();
    m_plot->replot();
}

void ProfilePlot::showNm(bool flag){
    m_showNm = (flag) ? outputLambda : 1.;
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

void ProfilePlot::setDefocusValue(double val){
        m_defocusValue = val;
        if (val != 0.){
            populate();
            m_plot->replot();
        }
}

QPolygonF ProfilePlot::createProfile(double units, wavefront *wf){
    QPolygonF points;
    double steps = 1./wf->m_outside.m_radius;
    double radius = wf->diameter/2.;
    for (double rad = -1.; rad < 1; rad += steps){
        int dx, dy;
        double radn = rad * wf->m_outside.m_radius;
        double radx = rad * radius;
        double e = 1.;

        mirrorDlg &md = *mirrorDlg::get_Instance();
        if (md.isEllipse()){
            e = md.m_verticalAxis/md.diameter;
        }
        dx = radn * cos(g_angle + M_PI_2) + wf->m_outside.m_center.x();
        dy = -radn * e * sin(g_angle + M_PI_2) + wf->m_outside.m_center.y();
        if (dy >= wf->data.rows || dx >= wf->data.cols || dy < 0 || dx < 0){
            points << QPointF(radx,0.0);

        }
        else if (wf->workMask.at<bool>(dy,dx)){
            double defocus = 0.;
            if (m_defocusValue != 0.){
                defocus = m_defocusValue * (-1. + 2. * rad * rad);
            }
            points << QPointF(radx,(units * (wf->workData((int)dy,(int)dx) + defocus) *
                                    wf->lambda/outputLambda)  +y_offset * units);
        }


    }
    if (m_showSlopeError){
        double arcsecLimit = (slopeLimitArcSec/3600) * M_PI/180;
        double xDel = points[0].x() - points[1].x();
        double hDelLimit =m_showNm *  m_showSurface * ((outputLambda/m_wf->lambda)*fabs(xDel * tan(arcsecLimit)) /(outputLambda * 1.e-6));

        for (int i = 0; i < points.size() - 1; ++i){
            double hdel = (points[i].y()- points[i+1].y());
            if (fabs(hdel) > hDelLimit){

                QVector<QPointF> pts;
                QwtPlotCurve *limitCurve = new QwtPlotCurve;
                pts<< points[i] << points[i+1];
                limitCurve->setSamples(pts);

                limitCurve->setPen(QPen(QColor("orange"),4));
                limitCurve->setLegendAttribute(QwtPlotCurve::LegendShowSymbol,false );
                limitCurve->setLegendAttribute(QwtPlotCurve::LegendShowLine,false );
                limitCurve->setItemAttribute(QwtPlotCurve::Legend,false);
                limitCurve->attach( m_plot);

            }
        }
    }
    return points;
}

void ProfilePlot::populate()
{


    compass->setGeometry(QRect(70,5,70,70));
    QString tmp("nanometers");
    if (m_showNm == 1.)
        tmp = QString().sprintf("waves of %6.1lf nm",outputLambda);
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
    QSettings settings;
    double smoothing = settings.value("GBValue", 20).toInt();
    m_plot->detachItems(QwtPlotItem::Rtti_PlotTextLabel);
    if (m_wf->m_outside.m_radius > 0 && settings.value("GBlur", false).toBool()){
        double val = .01 * (m_wf->diameter) * smoothing;
        QString t = QString().sprintf("Surface Smoothing diameter %6.2lf%% of surface diameter %6.1lf mm", smoothing , val );
        QwtText title(t);
        title.setRenderFlags( Qt::AlignHCenter | Qt::AlignTop );

        QFont font;
        font.setPointSize(12);
        title.setFont( font );

        QwtPlotTextLabel *titleItem = new QwtPlotTextLabel();
        titleItem->setText( title );
        titleItem->attach( m_plot );
    }
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
        QPolygonF points = createProfile( m_showNm * m_showSurface,m_wf);
        cprofile->setSamples( points );





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
            points = createProfile( m_showNm * m_showSurface,m_wf);
            cprofile->setSamples( points);
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
    muY->setLabel( QString::fromLatin1( " 1/8 wave on wavefront" ) );
    muY->setLabelAlignment( Qt::AlignRight | Qt::AlignTop );
    muY->setLineStyle( QwtPlotMarker::HLine );
    muY->setLinePen( Qt::red, 0, Qt::DashDotLine );
    muY->setYValue( 0.125 * mul );
    muY->attach( m_plot );

    //  ...a horizontal line at lower error margin...
    QwtPlotMarker *mlY = new QwtPlotMarker();
    mlY->setLabel( QString::fromLatin1( " -1/8 wave on wavefront" ) );
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
void ProfilePlot::zoom(){
    zoomed = !zoomed;
    emit zoomMe(zoomed);
}

void ProfilePlot::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    QString txt = (zoomed)? "Restore to MainWindow" : "FullScreen";
    myMenu.addAction(txt,  this, SLOT(zoom()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
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

