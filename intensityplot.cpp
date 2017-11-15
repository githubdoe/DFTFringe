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
#include "intensityplot.h"
#include <QtWidgets>
#include <QtCore>
#include <QtGui>
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
#include <math.h>
#include <opencv/highgui.h>
#include <qevent.h>
#include <qwt_scale_draw.h>
#define PITORAD  M_PI/180.
static double i_angle;

class iSurfaceData: public QwtSyntheticPointData
{

public:
    cv::Mat m_plane;
    int m_rad;
    iSurfaceData(cv::Mat plane):
        QwtSyntheticPointData( plane.cols ),
      m_plane(plane), m_rad(plane.cols/2),m_angle(90)
    {

    }

    virtual double y( double x ) const
    {
        int dx, dy;
        int xx = m_rad - x;
        dx = m_rad  - xx * cos(i_angle);
        dy = m_rad  - xx * sin(i_angle);
        if (dy >= m_plane.rows || dx >= m_plane.cols || dy < 0 || dx < 0){
            //qDebug() << QString().sprintf("profile Warning index out of range %d %d", dx,dy);
            return 0.0;
        }
        double v = m_plane.at<uchar>(dy,dx);
       // double nv = pow((v/255),2.2) * 255;
        return v;

    }


private:
    double m_angle;
};

bool intensityPlot::eventFilter( QObject *object, QEvent *event )
{
    if ( event->type() == QEvent::MouseButtonPress )
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
        startPos = mouseEvent->pos();
        return true;

    }

    return QObject::eventFilter( object, event );
}

intensityPlot::intensityPlot(QWidget *parent):
 m_showRed(true), m_showGreen(true),    m_showBlue(true)

{
    setTitle( "intensity profiles" );

    // canvas
    QwtPlotCanvas *canvas = new QwtPlotCanvas();
    canvas->setLineWidth( 1 );

    setCanvas( canvas );
    canvas->installEventFilter(this);

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
    QwtCompass *compass = new QwtCompass( this );
    compass->setLineWidth( 4 );
    compass->setFrameShadow( QwtCompass::Raised );
    compass->setGeometry(QRect(100,10,70,70));
    compass->setMode( QwtCompass::RotateNeedle );


    palette0.setColor( QPalette::Base, Qt::darkBlue );
    palette0.setColor( QPalette::WindowText,
                       QColor( Qt::darkBlue ).dark( 120 ) );
    palette0.setColor( QPalette::Text, Qt::white );

    QwtCompassScaleDraw *scaleDraw = new QwtCompassScaleDraw();
    scaleDraw->enableComponent( QwtAbstractScaleDraw::Ticks, true );
    scaleDraw->enableComponent( QwtAbstractScaleDraw::Labels, false );
    scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
    scaleDraw->setTickLength( QwtScaleDiv::MinorTick, 1 );
    scaleDraw->setTickLength( QwtScaleDiv::MediumTick, 1 );
    scaleDraw->setTickLength( QwtScaleDiv::MajorTick, 3 );

    compass->setScaleDraw( scaleDraw );

    compass->setScaleMaxMajor( 36 );
    compass->setScaleMaxMinor( 5 );

    compass->setNeedle(
        new QwtCompassMagnetNeedle( QwtCompassMagnetNeedle::ThinStyle ) );
    compass->setValue( 0 );
    compass->setOrigin( -90 );
    connect(compass,SIGNAL(valueChanged(double)),this ,SLOT(angleChanged(double)));

    populate();
    resize(QGuiApplication::primaryScreen()->availableSize() * 1./ 5.);
}


void intensityPlot::angleChanged(double a){
    i_angle = (a * PITORAD - M_PI_2);
    replot();
}



void intensityPlot::setSurface(QImage img){

    m_img = img;
    cv::Mat imgMat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine());
    //cv::cvtColor(imgMat,imgMat,CV_BGRA2RGBA);
    split(imgMat,planes);
    cv::Mat s[4];
    cv::resize(planes[0],s[0], cv::Size(200,200));
    cv::resize(planes[1],s[1], cv::Size(200,200));
    cv::resize(planes[2],s[2], cv::Size(200,200));
    //cv::imshow("Red", s[0]);
    //cv::imshow("Green",s[1]);
    //cv::imshow("Blue",s[2]);
    //cv::waitKey(1);
    setAxisScale( yLeft, 0,255);
    populate();
    replot();
    show();
}

void intensityPlot::populate()
{
    setAxisTitle( yLeft, "Pixel value" );
    //setAxisScale( xBottom, -100, 100.0 );
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->attach( this);
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    if (m_img.width() == 0)
        return;
    // axes
    detachItems( QwtPlotItem::Rtti_PlotCurve);
    double rad = m_img.width();
    setAxisScale( xBottom, 0, rad );
    QColor colors[] = {Qt::blue, Qt::green, Qt::red};

    // Insert new curves
    for (int chan = 0; chan < 3; ++chan){
        if (chan == 0 && !m_showBlue) continue;
        if (chan == 1 && !m_showGreen) continue;
        if (chan == 2 && !m_showRed) continue;
        QwtPlotCurve *cprofile = new QwtPlotCurve( "" );
        cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
        cprofile->setLegendAttribute( QwtPlotCurve::LegendShowLine, false );
        cprofile->setPen( colors[chan] );
        cprofile->attach( this );
        cprofile->setData( new iSurfaceData(planes[chan]) );
    }

}

void intensityPlot::updateGradient()
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

void intensityPlot::resizeEvent( QResizeEvent *event )
{
    QwtPlot::resizeEvent( event );

    // Qt 4.7.1: QGradient::StretchToDeviceMode is buggy on X11
    //updateGradient();
}

void intensityPlot::showRed(bool checked){
    m_showRed = checked;
    populate();
    replot();
}
void intensityPlot::showGreen(bool checked){
    m_showGreen = checked;
    populate();
    replot();
}
void intensityPlot::showBlue(bool checked){
    m_showBlue = checked;
    populate();
    replot();
}
