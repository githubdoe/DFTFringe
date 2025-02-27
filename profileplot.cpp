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
#include "simigramdlg.h"
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
#include "surfaceanalysistools.h"
#include "surfacemanager.h"
#include "zernikeprocess.h"
#include <QAbstractTableModel>
#include "plotcolor.h"
extern double outputLambda;

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <cmath>
#include "percentcorrectiondlg.h"
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
     offsetType("Middle"), m_defocusValue(0.),ui(new Ui::ProfilePlot)
{

    m_pcdlg = new percentCorrectionDlg;
    QObject::connect(m_pcdlg, SIGNAL(make_percent_correction(int)), this, SLOT(make_correction_graph(int)));
    zoomed = false;
    m_defocus_mode = false;
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
    showPercentCorrection = new QCheckBox("Show Correction");
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
    connect(showPercentCorrection,SIGNAL(clicked(bool)), this, SLOT(showCorrection(bool)));
    l1->addWidget(showPercentCorrection);
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
        palette().color( backgroundRole() ).lighter( 120 ) );
    palette0.setColor( QPalette::WindowText,
        palette0.color( QPalette::Base ) );
    compass = new QwtCompass( this );
    compass->setLineWidth( 4 );
    compass->setFrameShadow( QwtCompass::Raised );
    //compass->setMode( QwtCompass::RotateNeedle );


    palette0.setColor( QPalette::Base, Qt::darkBlue );
    palette0.setColor( QPalette::WindowText,
                       QColor( Qt::darkBlue ).darker( 120 ) );
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
ProfilePlot::~ProfilePlot(){
    delete ui;
    delete m_pcdlg;
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
    showPercentCorrection->setChecked(false);
    m_pcdlg->close();
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
    showPercentCorrection->setChecked(false);
    m_pcdlg->close();
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
    if (m_wf == 0)
        return;
    g_angle = a * PITORAD;

    if (type == 2 || type == 0)
        populate();
    m_plot->replot();
    emit profileAngleChanged(M_2_PI - g_angle +M_PI/4.);
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

void ProfilePlot::setDefocusWaveFront( cv::Mat_<double> wf){
    m_defocus_wavefront = wf.clone();
}

void ProfilePlot::setDefocusValue(double val){
        m_defocusValue = val;
        m_defocus_mode = false;
        if (val != 0.){
            m_defocus_mode = true;

            populate();
            m_plot->replot();
        }
}

QPolygonF ProfilePlot::createProfile(double units, wavefront *wf){
    QPolygonF points;

    double steps = 1./wf->m_outside.m_radius;
    mirrorDlg &md = *mirrorDlg::get_Instance();
    double radius = md.m_clearAperature/2.;
    double obs_radius = md.obs/2.;

    for (double rad = -1.; rad < 1.; rad += steps){
        int dx, dy;
        double radn = rad * wf->m_outside.m_radius;
        double radx = rad * radius;
        double e = 1.;


        if (md.isEllipse()){
            e = md.m_verticalAxis/md.diameter;
        }
        dx = radn * cos(g_angle + M_PI_2) + wf->m_outside.m_center.x();
        dy = -radn * e * sin(g_angle + M_PI_2) + wf->m_outside.m_center.y();
        if (dy >= wf->data.rows || dx >= wf->data.cols || dy < 0 || dx < 0){
            continue;

        }
        if (abs(radx) < obs_radius){
            points << QPointF(radx,0.0);
            continue;
        }

        if (wf->workMask.at<bool>(dy,dx)){
                double defocus = 0.;

                if (m_defocus_mode){
                    defocus = (m_defocusValue)* (-1. + 2. * rad * rad);
                    points << QPointF(radx,(units * (m_defocus_wavefront((int)dy,(int)dx) + defocus ) *
                                            wf->lambda/outputLambda)  +y_offset * units);
                }
                else {

                    points << QPointF(radx,(units * (wf->workData((int)dy,(int)dx) ) *
                                        wf->lambda/outputLambda)  +y_offset * units);

                }
            }
            //else points << QPointF(radx,0.0);
    }

    if (m_showSlopeError && ((type==1 && !m_showCorrection) || (type == 0) || (type == 2))){
        double arcsecLimit = (slopeLimitArcSec/3600) * M_PI/180;
        double xDel = points[0].x() - points[1].x();
        double hDelLimit =m_showNm *  m_showSurface * ((outputLambda/m_wf->lambda)*fabs(xDel * tan(arcsecLimit)) /(outputLambda * 1.e-6));

        for (int i = 0; i < points.size() - 1; ++i){
            double hdel = (points[i].y()- points[i+1].y());
            if (fabs(points[i].x()) < obs_radius || fabs(points[i+1].x()) < obs_radius)
                continue;
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
// create a smoothed wave front with only spherical terms.
//  Use that to get zernike values to send to percent completion feature
// display the profile and then send the zerns to percent completion
// have to decide what is maxOrder

zernikeProcess *zp = NULL;
int maxOrder = 18;
void ProfilePlot::make_correction_graph( int maxOrder){
    m_showCorrection = true;

    // for each selected wave front
    zernikeProcess zp;
    zp.setMaxOrder(maxOrder);

    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    QList<int> list = saTools->SelectedWaveFronts();
    if (list.length() == 0)
        list << 0;
    QVector < surfaceData *> surfs;
    for (int i = 0; i < list.size(); ++i){
        QStringList path = wfs->at(list[i])->name.split("/");
        QString name = path.last().replace(".wft","");

        // compute zerns using the new maxorder zern set
        zp.initGrid(*wfs->at(list[i]), maxOrder);
        std::vector<double> theZerns = zp.ZernFitWavefront(*wfs->at(list[i]));

        // send zernike values to the correction dialog to have it plot correction based on them.

        QColor penColor = Settings2::m_profile->getColor(i);
        // give the plot routine new zernike values for each curve.
        mirrorDlg *md = mirrorDlg::get_Instance();
        surfs << new surfaceData( md->lambda, penColor, theZerns);

    }
    m_pcdlg->setData(surfs);
}
#ifdef notnow
extern double getZernSurface( double RoC, double MirrorRad, std::vector<double> Zernikes, double radius);

QPolygonF ProfilePlot::createZernProfile(wavefront *wf){
    mirrorDlg &md = *mirrorDlg::get_Instance();
    double radius = md.m_clearAperature/2.;			// mirror radius of clear aperture

    double RoC = md.roc;
    if (zp == NULL)
        zp = new zernikeProcess;
    zp->initGrid(*wf, maxOrder);
    std::vector<double> theZerns;
    theZerns = zp->ZernFitWavefront(*wf);

    // make the profl
    int wx = wf->data.size[1];

    int numType = CV_64FC1;
    cv::Mat result = cv::Mat::zeros(wx,wx,  numType);



    // apply the artificial null
    theZerns[8] -= md.z8 * md.cc;
    // build surface with only spherical terms
    for (unsigned long long i = 4; i < zp->m_zerns.n_rows; ++i){

        double S1 = 0.0;
        unsigned int z = 8;
        for (unsigned int j = 5; z < theZerns.size(); ++j){

            double val = theZerns[z];
             S1 +=  val * zp->m_zerns(i,z);

            z = j * j/4 + j;
        }
        int x =  zp->m_col[i];
        int y =  zp->m_row[i];
       if (S1 == 0.0) S1 += .0000001;
        result.at<double>(y,x) = S1;
        if (y == wx/2.){
            qDebug() << x << y << S1;
        }

    }
    QPolygonF surf;
    int half = result.cols/2;
    for (int i = 0; i < result.cols; ++i){
        // map  0 - n   to   -radius to radius
        double x = -radius +  2 * ((double)i/result.cols) * radius;
        surf << QPointF(x,result.at<double>(i,half));
        //qDebug() << surf.last();
    }
    //m_pcdlg->plot(1, theZerns, radius, md.roc,
                  //md.cc,md.z8, md.lambda, outputLambda,Qt::black);
    return surf;
}

extern double getZernSurface( double RoC, double MirrorRad, std::vector<double> Zernikes, double radius);
#endif
void ProfilePlot::populate()
{

    m_plot->detachItems(QwtPlotItem::Rtti_PlotItem);
    compass->setGeometry(QRect(80,80,70,70));
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
    grid->setMajorPen( Qt::blue,0.0,Qt::DotLine);
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



        switch (type) {
    case 0:{        // show one
        QStringList path = wfs->at(0)->name.split("/");
        QString name;
        int l = path.length();
        if (l >= 2){
            name = path[l-2] + "/" + path[l-1];
        }
        else
            name = wfs->at(0)->name;
        QwtPlotCurve *cprofile = new QwtPlotCurve( name );
        cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
        //m_plot->insertLegend( new QwtLegend() , QwtPlot::BottomLegend);
        //cprofile->setLegendAttribute( QwtPlotCurve::LegendShowLine, false );
        cprofile->setPen( Qt::black );
        cprofile->attach( m_plot );
        QPolygonF points = createProfile( m_showNm * m_showSurface,m_wf);

        cprofile->setSamples( points );

        break;
    }

    case 1: {   // show 16 diameters

        surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
        QList<int> list = saTools->SelectedWaveFronts();
        bool firstPlot = true;
        QColor penColor = QColor("blue");

        for (int indx = 0; indx < list.size(); ++indx){
            if (indx > 0) penColor = QColor(plotColors[indx % 10]);
            QPolygonF avg;
            QString t = "Average of all 16 diameters";
            QwtText title(t);
            title.setRenderFlags( Qt::AlignHCenter | Qt::AlignBottom );

            QFont font;
            font.setPointSize(12);
            title.setFont( font );
            title.setColor(Qt::blue);
            QwtPlotTextLabel *titleItem = new QwtPlotTextLabel();
            titleItem->setText( title );
            titleItem->attach( m_plot );

            double startAngle = g_angle;
            QPolygonF sum;
            QMap<int,int> count;
            for (int i = 0; i < 16; ++i){
                QPolygonF points;
                g_angle = startAngle + i * M_PI/ 16;

                QwtPlotCurve *cprofile = new QwtPlotCurve( );
                cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
                cprofile->setLegendAttribute( QwtPlotCurve::LegendShowSymbol, false );
                cprofile->setPen( Qt::black );

                points = createProfile( m_showNm * m_showSurface,wfs->at(list[indx]));
                if (i == 0) {
                    sum = points;
                    for (int j = 0; j < sum.length(); ++j)
                        count[j] = 1;
                }
                else {
                    for(int j = 0; j < fmin(sum.length(),points.length());++j){
                        sum[j].ry()  += points[j].y();;

                        if (count.contains(j)) count[j] += 1 ;
                        else count[j] = 1;
                    }
                }
                if (!m_showCorrection){
                    cprofile->setSamples( points);
                    cprofile->attach( m_plot );
                }
            }

            // plot the average profile
            int i = 0;
            foreach(QPointF p, sum){
                avg << QPointF(p.x(),p.y()/(count[i++]));
            }
            QString name("average");
            if (m_showCorrection){
                QStringList path = wfs->at(list[indx])->name.split("/");
                name = path.last().replace(".wft","");
            }
            QwtPlotCurve *cprofileavg = new QwtPlotCurve( name);
            cprofileavg->setRenderHint( QwtPlotItem::RenderAntialiased );
            cprofileavg->setLegendAttribute( QwtPlotCurve::LegendShowSymbol, false );
            cprofileavg->setLegendIconSize(QSize(50,20));
            cprofileavg->setPen( QPen(penColor,5) );
            cprofileavg->setSamples( avg);
            cprofileavg->attach( m_plot );
            g_angle = startAngle;

        }


        break;
    }
    case 2:{    // show each wave front

        m_plot->insertLegend( new QwtLegend() , QwtPlot::BottomLegend);
        surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
        QList<int> list = saTools->SelectedWaveFronts();
        if (list.size() <2){
            list.clear();
            for (int i = 0; i < wfs->size(); ++i){
                list << i;
            }
        }
        for (int i = 0; i < list.size(); ++i){
            QStringList path = wfs->at(list[i])->name.split("/");
            QString name = path.last().replace(".wft","");

            QwtPlotCurve *cprofile = new QwtPlotCurve(name );
            int width = Settings2::m_profile->lineWidth();
            if (name == m_wf->name.split("/").last().replace(".wft",""))
                width = Settings2::m_profile->selectedWidth();
            cprofile->setLegendIconSize(QSize(50,20));
            cprofile->setPen(QPen(Settings2::m_profile->getColor(i),width));
            cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
            cprofile->setSamples( createProfile( m_showNm * m_showSurface,wfs->at(list[i])));
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
    mY->setLinePen( Qt::blue, 0, Qt::DashDotLine );
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
    mX->setLinePen( Qt::blue, 0, Qt::DashDotLine );
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

void ProfilePlot::contourPointSelected(const QPointF &pos){
    if (m_wf == 0)
        return;
    double delx = pos.x() - m_wf->data.rows/2;
    double dely = pos.y() - m_wf->data.cols/2;

    double angle = atan2(delx,dely);  // swaped x and y to rotate by 90 deg.
    double angle2 = angle;
    const double twopi = M_PI * 2.;
    // force 0 to 360
    if (angle < 0)
        angle = twopi + angle;

    g_angle = angle;
    double deg = angle * 180./M_PI; // radians to degrees
    compass->blockSignals(true);
    compass->setValue(deg);
    populate();
    m_plot->replot();
    compass->blockSignals(false);

}
void ProfilePlot::showCorrection(bool show){
    //m_showCorrection = show;
    if (show){
        m_pcdlg->show();
        m_pcdlg->raise();

    }
    else
        m_pcdlg->close();


}
