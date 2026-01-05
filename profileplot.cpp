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
#include <QtGlobal>

extern double outputLambda;

#include <iostream>
#include <stdio.h>
#include <fstream>

#include <vector>
#include <cmath>
#include <algorithm>
#include <map>
#include "percentcorrectiondlg.h"
#include "profilecurve.h"

#define DEGTORAD  M_PI/180.;
double g_angle = 270. * DEGTORAD; //start at 90 deg (pointing east)
double y_offset = 0.;



ProfilePlot::ProfilePlot(QWidget *parent , ContourTools *tools):
    QWidget( parent ), m_wf(0), m_tools(tools),
     m_showSurface(1.),m_showNm(1.),dragging(false),
     offsetType("Middle"), m_defocusValue(0.),ui(new Ui::ProfilePlot)
{

    m_pcdlg = new percentCorrectionDlg;
    QObject::connect(m_pcdlg, &percentCorrectionDlg::make_percent_correction, this, &ProfilePlot::make_correction_graph);
    zoomed = false;
    m_defocus_mode = false;
    m_plot = new QwtPlot(this);
    m_plot->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_plot, &QwtPlot::customContextMenuRequested, this, &ProfilePlot::showContextMenu);
    m_plot->setCanvasBackground(Qt::darkGray);
    m_pk = new profilePlotPicker(m_plot);
    QObject::connect(m_pk, &profilePlotPicker::offset, this, &ProfilePlot::SetYoffset);
    QHBoxLayout * l1 = new QHBoxLayout();
    QVBoxLayout *v1 = new QVBoxLayout();
    showNmCB = new QCheckBox("Show in Nanometers",this);
    showSurfaceCB =  new QCheckBox("Surface error");

    l1->addStretch();

    showSlopeError = new QCheckBox("Show Slope: ");


    slopeLimitSB = new QDoubleSpinBox();


    QSettings set;
    m_displayPercent = set.value("xScalepercent", false).toBool();
    m_displayInches = set.value("xScaleInches", false).toBool();
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

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(slopeLimitSB, &QDoubleSpinBox::valueChanged, this, &ProfilePlot::slopeLimit);
#else
        // QDoubleSpinBox::valueChanged is overloaded in Qt5
    connect(slopeLimitSB, QOverload<double>::of(&QDoubleSpinBox::valueChanged), this, &ProfilePlot::slopeLimit);
#endif
    //
    connect(showSlopeError,&QAbstractButton::clicked, this, &ProfilePlot::showSlope);

    l1->addWidget(showSlopeError);
    l1->addWidget(slopeLimitSB);
    l1->addWidget(showNmCB);
    l1->addWidget(showSurfaceCB);
    l1->addStretch();
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

    // Using the old SIGNAL/SLOT syntax because problems with QWT.
    // Qt is not able to match signal at runtime even if compile time checks all passed.
    // ChatGPT tells it might be an ABI problem with QWT library but I (JST) have been unable to fix for now (2025-10-20).
    connect(compass, SIGNAL(valueChanged(double)), this ,SLOT(angleChanged(double)));
    //connect(compass,&QwtAbstractSlider::valueChanged,this ,&ProfilePlot::angleChanged);

    connect(m_tools, &ContourTools::newDisplayErrorRange,
            this, &ProfilePlot::newDisplayErrorRange);
    connect(m_tools, &ContourTools::contourZeroOffsetChanged, this, &ProfilePlot::zeroOffsetChanged);
    connect(showNmCB, &QAbstractButton::clicked, this, &ProfilePlot::showNm);
    connect(showSurfaceCB, &QAbstractButton::clicked,this, &ProfilePlot::showSurface);

    ui->setupUi(this);
    populate();
}
ProfilePlot::~ProfilePlot(){
    delete ui;
    delete m_pcdlg;
    delete m_pk;
}



void ProfilePlot::SetYoffset(const QString& name, double value){
    if (m_waveFrontyOffsets.contains(name)){
        m_waveFrontyOffsets[name] += value;
    }
    else{
        m_waveFrontyOffsets.insert(name,value);
    }

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


void ProfilePlot::zeroOffsetChanged(const QString &s){
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
    g_angle = a * DEGTORAD;


        populate();
    m_plot->replot();
    emit profileAngleChanged(M_PI_2 - g_angle);
}
void ProfilePlot::wheelEvent(QWheelEvent *event)
{
  double factor = 1/1.55;
  if( event->angleDelta().y() < 0) factor = 1.5;

 m_tools->setMinMaxValues(m_tools->m_min * factor, m_tools->m_max * factor);

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
    m_waveFrontyOffsets.clear();
    populate();
    m_plot->replot();
}

void ProfilePlot::setDefocusWaveFront( const cv::Mat_<double> &wf){
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
QPolygonF ProfilePlot::createAverageProfile(double /*umnits*/, wavefront * /*wf*/, bool /*removeNull = false*/){
    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    QList<int> list = saTools->SelectedWaveFronts();
    QPolygonF avg;
    for (int indx = 0; indx < list.size(); ++indx){

        double startAngle = g_angle;
        QPolygonF sum;
        QMap<int,int> count;
        for (int i = 0; i < 720; ++i){
            QPolygonF points;
            g_angle = startAngle + i * M_PI/ 720;

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

        }

        // plot the average profile
        int i = 0;
        foreach(QPointF p, sum){
            if (p.x() < 0) continue;
            avg << QPointF(p.x(),p.y()/(count[i++]));
        }

    }
//    if (removeNull){
//        // Nulled y = (z8 - null)term8(rho)     z8 * term8  - null * term8;
//        // remove artificial null effect.
//        mirrorDlg &md = *mirrorDlg::get_Instance();
//        QPolygonF avg2;
//        for (int i = 0; i < avg.length(); ++i){
//            if (avg[i].x() < 0)
//                continue;
//            double rho = avg[i].x() / md.diameter/2.;
//            double rho2 = rho * rho;
//            double y = avg[i].y();
//            y += md.z8 * md.cc * (1. + rho2 * (-6 + 6. * rho2));
//            avg2 << QPointF(avg[i].x(),y);
//        }
//        avg = avg2;
//    }

    return avg;
}


// creates set of points for a profile at a given angle.  Adds NaN values to points in the obstruction or outside the mirror
// Tee angle for the diameter is given in a global.
QPolygonF ProfilePlot::createProfile(double units, const wavefront *wf, bool allowOffset) {
    QPolygonF points;
    mirrorDlg &md = *mirrorDlg::get_Instance();

    // 1. Setup constants
    double steps = 1.0 / wf->m_outside.m_radius;
    double offset = allowOffset ? y_offset : 0.0;
    double radius = md.m_clearAperature / 2.0;
    double obs_radius = md.obs / 2.0;

    if (m_displayInches) {
        obs_radius /= 25.4;
    }

    // Main Sampling Loop
    for (double rad = -1.0; rad < 1.0; rad += steps) {
        double radn = rad * wf->m_outside.m_radius;
        double radx = rad * radius;

        if (m_displayInches) radx /= 25.4;

        if (m_displayPercent) {
            radx = 100.0 * radx / radius;
            obs_radius = 100.0 * (md.obs / 2.0) / radius;
        }

        double e = 1.0;
        if (md.isEllipse()) {
            e = md.m_verticalAxis / md.diameter;
        }

        // Calculate matrix coordinates
        int dx = radn * cos(g_angle + M_PI_2) + wf->m_outside.m_center.x();
        int dy = -radn * e * sin(g_angle + M_PI_2) + wf->m_outside.m_center.y();

        // Boundary Check: Ignore points outside the matrix
        if (dy >= wf->data.rows || dx >= wf->data.cols || dy < 0 || dx < 0) {
            continue;
        }

        // Handle the obstruction (The Hole)
        if (std::abs(radx) < obs_radius) {
            // Only add a NaN if the very last point was a real number.
            // This "lifts the pen" once and then skips the rest of the hole.
            if (!points.isEmpty() && !std::isnan(points.last().y())) {
                points << QPointF(radx, qQNaN());
            }
            continue;
        }

        // Mask and Data Processing
        if (wf->workMask.at<bool>(dy, dx)) {
            double val = (units * (wf->workData(dy, dx)) * wf->lambda / outputLambda) + (offset * units);

            if (m_defocus_mode) {
                double defocus = (m_defocusValue) * (-1.0 + 2.0 * rad * rad);
                val += (units * defocus * wf->lambda / outputLambda);
            }

            points << QPointF(radx, val);
        } else {
            // Lift the pen if we hit a mask gap outside the center hole
            if (!points.isEmpty() && !std::isnan(points.last().y())) {
                points << QPointF(radx, qQNaN());
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

void ProfilePlot::make_correction_graph(){


    int maxOrder = m_pcdlg->m_maxOrder;

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
        if (theZerns.size() == 0) return;
        // send zernike values to the correction dialog to have it plot correction based on them.

        QColor penColor = Settings2::m_profile->getColor(i);
        // give the plot routine new zernike values for each curve.
        mirrorDlg *md = mirrorDlg::get_Instance();
        surfs << new surfaceData( md->lambda, penColor, theZerns ,name);

    }
    QPolygonF avg = createAverageProfile(1., wfs->at(list[0]),true);
    m_pcdlg->setProfile(avg);
    m_pcdlg->setData(surfs);
}







// Struct to hold the sum of heights and the count for each distance bin
struct RadialBin {
    double sum = 0.0;
    int count = 0;
};
// the following code was generated by AI and may be used in the future to show the average profile
/**
 * @brief Computes the average height profile along all radial lines,
 * returning a vector where index r is the average height at radius r.
 * It assumes the input matrix is of type CV_64FC1 (double).
 * * @param matrix The 2D matrix (cv::Mat, expected CV_64FC1).
 * @param center The center point of the circular surface (cv::Point).
 * @param radius The radius of the circular surface in pixels.
 * @return A std::vector<double> where index r contains the average height at integer radius r.
 */
std::vector<double> compute_average_radial_profile(
    const cv::Mat& matrix,
    cv::Point center,
    int radius)
{
    // Check matrix validity
    if (matrix.empty() || matrix.channels() != 1 || matrix.type() != CV_64FC1) {
        qDebug() << "Error: Matrix must be non-empty, single-channel, and of type CV_64FC1.";
        return {};
    }

    int rows = matrix.rows;
    int cols = matrix.cols;

    double center_x = (double)center.x;
    double center_y = (double)center.y;

    // 1. Define the Bounding Box (ROI) for optimized iteration
    int min_y = std::max(0, center.y - radius);
    int max_y = std::min(rows - 1, center.y + radius);
    int min_x = std::max(0, center.x - radius);
    int max_x = std::min(cols - 1, center.x + radius);

    // Use a map to consolidate sums and counts for all points grouped by
    // their final integer radius (r = round(sqrt(r^2)))
    // Key: Integer Radius (r). Value: RadialBin struct.
    std::map<int, RadialBin> radial_bins_by_int_radius;

    // 2. Optimized Iteration and Binning
    for (int y = min_y; y <= max_y; ++y) {
        for (int x = min_x; x <= max_x; ++x) {

            double dx = x - center_x;
            double dy = y - center_y;

            double dist_sq_float = dx * dx + dy * dy;

            // Only process points within the defined radius
            if (dist_sq_float <= (double)radius * radius) {

                double value = matrix.at<double>(y, x);
                double radius_actual = std::sqrt(dist_sq_float);

                // Determine the bin index by rounding the actual radius
                int profile_index = static_cast<int>(std::round(radius_actual));

                // Ensure the index doesn't exceed the intended maximum radius R
                if (profile_index <= radius) {
                    radial_bins_by_int_radius[profile_index].sum += value;
                    radial_bins_by_int_radius[profile_index].count += 1;
                }
            }
        }
    }

    // 3. Compute the Final Profile Vector
    // Size is radius + 1 (for indices 0 up to radius R)
    std::vector<double> radial_profile(radius + 1, 0.0);

    for (const auto& pair : radial_bins_by_int_radius) {
        int r_index = pair.first;
        const RadialBin& bin = pair.second;

        // Calculate the average height for this integer radius bin
        double average_height = bin.sum / bin.count;

        // Store the result directly at the integer radius index
        radial_profile[r_index] = average_height;
    }

    return radial_profile;
}

void ProfilePlot::populate()
{
qDebug() << "Populate";
    m_plot->detachItems(QwtPlotItem::Rtti_PlotItem);
    compass->setGeometry(QRect(80,80,70,70));
    QString tmp("nanometers");
    if (m_showNm == 1.)
        tmp = QString("waves of %1 nm").arg(outputLambda, 6, 'f', 1);
    m_plot->setAxisTitle( m_plot->yLeft, "Error in " + tmp );
    QString XaxisTitle("Radius mm");
    if (m_displayInches)
        XaxisTitle = "Radius Inches";
    if (m_displayPercent)
        XaxisTitle = "Radius Percent";

    m_plot->setAxisTitle( m_plot->xBottom, XaxisTitle );

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
        QString t = QString("Surface Smoothing diameter %1% of surface diameter %2 mm")
                        .arg(smoothing, 6, 'f', 2)
                        .arg(val, 6, 'f', 1);
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
    if (m_displayInches){
        lower /= 25.4;
        upper /= 25.4;
    }
    if (m_displayPercent){
        lower = -100;
        upper = 100;
    }
    m_plot->setAxisScale(QwtPlot::xBottom, lower, upper);
    QwtScaleEngine * se1 = m_plot->axisScaleEngine(QwtPlot::xBottom);
    QwtScaleDiv sd1 = se1->divideScale(lower,upper, 40,5);
    m_plot->setAxisScaleDiv(QwtPlot::xBottom, sd1);
    m_plot->detachItems( QwtPlotItem::Rtti_PlotCurve);
    m_plot->detachItems( QwtPlotItem::Rtti_PlotMarker);

    double arcsecLimit = (slopeLimitArcSec/3600) * M_PI/180;

        m_plot->insertLegend( new QwtLegend() , QwtPlot::BottomLegend);
        surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
        QList<int> list = saTools->SelectedWaveFronts();
        // if no wave front was selected then use the last one

        for (int i = 0; i < list.size(); ++i){
            wavefront* wf = wfs->at(list[i]);
            QStringList path = wf->name.split("/");
            QString name = path.last().replace(".wft","");
            ProfileCurve *cprofile = new ProfileCurve(name );
            int width = Settings2::m_profile->lineWidth();
            if (name == m_wf->name.split("/").last().replace(".wft",""))
                width = Settings2::m_profile->selectedWidth();
            cprofile->setLegendIconSize(QSize(50,20));
            cprofile->setPen(QPen(Settings2::m_profile->getColor(i),width));
            cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );

            double units = m_showNm * m_showSurface;
            if (m_waveFrontyOffsets.contains(name))
                y_offset = m_waveFrontyOffsets[name];
            else if (m_waveFrontyOffsets.contains(name + " avg")){
                y_offset = m_waveFrontyOffsets[name + " avg"];
                qDebug() << "using avg";
            }

            // if show one angle
            if (m_show_oneAngle or (!m_showAvg and !m_show_16_diameters and !m_show_oneAngle)){

                QPolygonF points = createProfile(units, wf, true);
                if (points.size() >= 2) {
                    // Distance between two samples
                    double xDel = fabs(points[0].x() - points[1].x());

                    // Recalculate hDelLimit using this specific xDel
                    double hDelLimit = m_showNm * m_showSurface * ((outputLambda/m_wf->lambda) * fabs(xDel * tan(arcsecLimit)) / (outputLambda * 1.e-6));

                    cprofile->setSlopeSettings(m_showSlopeError, hDelLimit, Settings2::m_profile->slopeErrorWidth());
                }
                cprofile->setSamples(points);


                cprofile->attach( m_plot );
            }
            if (m_show_16_diameters){
              // compute 16 diameters
              QColor penColor = QColor(Settings2::m_profile->getColor(i%10));

              QString t = "Average of all 16 diameters";
              QwtText title(t);
              title.setRenderFlags( Qt::AlignHCenter | Qt::AlignBottom );

              QFont font;
              font.setPointSize(12);
              title.setFont( font );
              title.setColor(Qt::blue);
              QwtPlotTextLabel *titleItem = new QwtPlotTextLabel();
              titleItem->setText( title );
              //titleItem->attach( m_plot );

              double startAngle = g_angle;
              QPolygonF sum;
              QMap<int,int> count;
              qDebug() << "yoffsets" << m_waveFrontyOffsets << title.text();
              for (int idx = 0; idx < 16; ++idx){
                  QPolygonF points;
                  g_angle = startAngle + idx * M_PI/ 16;

                  QwtPlotCurve *cprofile = new QwtPlotCurve( );
                  cprofile->setRenderHint( QwtPlotItem::RenderAntialiased );
                  cprofile->setLegendAttribute( QwtPlotCurve::LegendShowSymbol, false );
                  cprofile->setItemAttribute(QwtPlotItem::Legend, false);
                  cprofile->setPen( penColor);
                  if (m_waveFrontyOffsets.contains(name))
                    y_offset = m_waveFrontyOffsets[name];
                  points = createProfile( m_showNm * m_showSurface,wf,true);
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
                  if (m_show_16_diameters){
                    cprofile->setSamples( points);
                    cprofile->attach( m_plot );
                  }
              }
            }
          if (m_showAvg){
              qDebug() << "inside avg" << y_offset;
              QColor penColor = QColor(Settings2::m_profile->getColor(i%10));
              // plot the average profile
              std::vector<double> avgRadius = compute_average_radial_profile(wf->workData,
                                            cv::Point(wf->m_outside.m_center.x(),wf->m_outside.m_center.y()), wf->m_outside.m_radius);

              QPolygonF right;  //right half of profile
              int edgeMaskWidth = 4;
              for (size_t i = 0; i < avgRadius.size()-edgeMaskWidth; ++i) {
                  double rr  = (double)(i)/(avgRadius.size()-edgeMaskWidth-1);
                  if (m_displayPercent){
                      rr *= 100.;
                  }
                  else if(m_displayInches){
                      rr *= wf->diameter/2;
                      rr /=25.4;
                  }
                  else {  // convert rr into mm.
                    rr *= wf->diameter/2;

                  }
                  double val = (units * avgRadius[i] * wf->lambda/outputLambda)  + y_offset * units;

                  right <<  QPointF(rr, val);
              }
              // make left have of profile
              QPolygonF left = right;
              std::reverse(left.begin(), left.end());
              left.pop_back();
              // Create a transform that scales the X axis by -1 and the Y axis by 1 (no scaling)
              QTransform transform;
              transform.scale(-1.0, 1.0);

              // Apply the transform to the polygon
              left = transform.map(left);
              left.append(right);

              QwtPlotCurve *cprofileavg = new QwtPlotCurve( name + " avg");
              cprofileavg->setRenderHint( QwtPlotItem::RenderAntialiased );
              cprofileavg->setLegendAttribute( QwtPlotCurve::LegendShowSymbol, false );
              cprofileavg->setLegendIconSize(QSize(50,20));
              cprofileavg->setPen( QPen(penColor, Settings2::m_profile->avgProfileWidth()) );
              cprofileavg->setSamples( left);
              cprofileavg->attach( m_plot );
              //g_angle = startAngle;
        }

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

void ProfilePlot::showContextMenu(QPoint pos)
{
    // Handle global position
    QPoint globalPos = mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.setToolTipsVisible(true);

    QAction* showFull = new QAction("FullScreen", this);
    showFull->setCheckable(true);
    showFull->setChecked(zoomed);
    connect(showFull, &QAction::triggered, this, &ProfilePlot::zoom);
    myMenu.addAction(showFull);

    QAction* showPercentage = new QAction("Change X axis to show percentage", this);
    connect(showPercentage, &QAction::triggered, this, &ProfilePlot::showXPercent);
    myMenu.addAction(showPercentage);

    QAction* showInches = new QAction("Change X asix to show inches",this);
    connect(showInches, &QAction::triggered, this, &ProfilePlot::showXInches);
    myMenu.addAction(showInches);

    QAction* showMM = new QAction("Change X axis to show mm",this);
    connect(showMM, &QAction::triggered, this, &ProfilePlot::showXMM);
    myMenu.addAction(showMM);

    QAction *showOneAngle = new QAction("show one angle",this);
    showOneAngle->setCheckable(true);
    showOneAngle->setChecked(m_show_oneAngle);
    connect(showOneAngle, &QAction::triggered, this, &ProfilePlot::toggleOneAngle);
    myMenu.addAction(showOneAngle);

    QAction *show16 = new QAction("show 16 diameters",this);
    show16->setCheckable(true);
    show16->setChecked(m_show_16_diameters);
    connect(show16, &QAction::triggered, this, &ProfilePlot::toggleShow16);
    myMenu.addAction(show16);

    QAction *showAvg = new QAction("Show avg", this);
    showAvg->setCheckable(true);
    showAvg->setChecked(m_showAvg);
    showAvg->setToolTip("Shows only the average of all diameters");
    connect(showAvg, &QAction::triggered, this, &ProfilePlot::toggleShowAvg);
    myMenu.addAction(showAvg);

    QAction *createAvg = new QAction("make surface from average profile", this);
    connect(createAvg, &QAction::triggered, this, &ProfilePlot::CreateWaveFrontFromAverage);
    myMenu.addAction(createAvg);

    QAction* showCorrection = new QAction("show percent of correction",this);
    connect(showCorrection, &QAction::triggered, this, &ProfilePlot::showCorrection);
    showCorrection->setToolTip("Show % correction of zone areas used in the Zambuto method of mirror figuring.");
    myMenu.addAction(showCorrection);

    // Show context menu at handling position
    myMenu.exec(globalPos);
}
void ProfilePlot::saveXscaleSettings(){
    QSettings set;
    set.setValue("xScalepercent", m_displayPercent);
    set.setValue("xScaleInches", m_displayInches);
}
void ProfilePlot::toggleShow16(){
    m_show_16_diameters = !m_show_16_diameters;
qDebug() << "show 16";
    populate();
    m_plot->replot();
}
void ProfilePlot::toggleShowAvg(){
    m_showAvg = !m_showAvg;

    populate();
    m_plot->replot();
}
void ProfilePlot::toggleOneAngle(){
    m_show_oneAngle = !m_show_oneAngle;
qDebug() << "show one";
    populate();
    m_plot->replot();
}
void ProfilePlot::showXPercent(){
    m_displayPercent = true;
    m_displayInches = false;
    populate();
    m_plot->replot();
    saveXscaleSettings();
}
void ProfilePlot::showXInches(){
    m_displayInches = true;
    m_displayPercent = false;
    populate();
    m_plot->replot();
    saveXscaleSettings();
}
void ProfilePlot::showXMM(){
    m_displayInches = false;
    m_displayPercent = false;
    populate();
    m_plot->replot();
    saveXscaleSettings();

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

void ProfilePlot::contourPointSelected(QPointF pos){
    if (m_wf == 0)
        return;
    double delx = pos.x() - m_wf->data.rows/2;
    double dely = pos.y() - m_wf->data.cols/2;

    double angle = atan2(delx,dely);  // swaped x and y to rotate by 90 deg.
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
void ProfilePlot::showCorrection(){
    if (m_wf == 0)
    {
        return;
    }

    m_pcdlg->show();
    m_pcdlg->raise();
    make_correction_graph();


}

/**
 * @brief Performs linear interpolation between two values.
 * @param a The value at index floor(x).
 * @param b The value at index ceil(x).
 * @param t The fractional part of x (t = x - floor(x)).
 * @return The interpolated value.
 */
double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

/**
 * @brief Creates an OpenCV matrix representing a radially symmetric circular surface,
 * using linear interpolation for sub-integer radial distances.
 *
 * @param radialProfile The vector of doubles representing the value from center to edge (index 0 to N-1).
 * @return cv::Mat The square matrix representing the circular surface.
 */
cv::Mat createInterpolatedCircularSurface(const std::vector<double>& radialProfile) {
    if (radialProfile.empty()) {
        std::cerr << "Error: Input radialProfile vector is empty." << std::endl;
        return cv::Mat();
    }

    // The radius of the circle is determined by the max index in the profile.
    // Max distance is profile size - 1.
    const int profileSize = radialProfile.size();
    const double maxRadiusDistance = static_cast<double>(profileSize - 1);

    // The surface size (diameter) will be twice the radius + 1 (for the center pixel).
    int surfaceSize = 2 * static_cast<int>(maxRadiusDistance) + 1;

    // 1. Initialize the output matrix
    cv::Mat surface = cv::Mat::zeros(surfaceSize, surfaceSize, CV_64FC1);

    // Center coordinates
    double centerX = maxRadiusDistance;
    double centerY = maxRadiusDistance;

    // 2. Iterate over all pixels in the matrix
    for (int y = 0; y < surfaceSize; ++y) {
        // Get a pointer to the current row for fast access
        double* rowPtr = surface.ptr<double>(y);

        for (int x = 0; x < surfaceSize; ++x) {
            // Calculate coordinates relative to the center
            double dx = x - centerX;
            double dy = y - centerY;

            // Calculate the current pixel's distance from the center (r_pixel)
            double r_pixel = std::sqrt(dx * dx + dy * dy);

            // 3. Boundary Check and Interpolation

            // Check if the pixel is within the circular area defined by the profile length
            if (r_pixel <= maxRadiusDistance) {

                // --- Perform Linear Interpolation ---

                // Index 1 (Lower bound)
                int index1 = static_cast<int>(std::floor(r_pixel));

                // Index 2 (Upper bound)
                int index2 = static_cast<int>(std::ceil(r_pixel));

                // Fractional part 't' used for interpolation
                double t = r_pixel - index1;

                // Value for the pixel
                double interpolatedValue = 0.0;

                if (index1 == index2) {
                    // This happens when r_pixel is an exact integer distance (e.g., 5.0).
                    // We only need to use the single value.
                    interpolatedValue = radialProfile[index1];
                } else {
                    // Get the values at the two surrounding integer indices
                    double value1 = radialProfile[index1];
                    // Ensure index2 is within bounds (only fails if r_pixel > maxRadiusDistance,
                    // but boundary check above already handles this for the last element).
                    double value2 = radialProfile[index2];

                    // Apply linear interpolation
                    interpolatedValue = lerp(value1, value2, t);
                }

                rowPtr[x] = interpolatedValue;

            } else {
                // The pixel is outside the circular surface area. Value remains 0.0.
            }
        }
    }

    return surface;
}
void ProfilePlot::CreateWaveFrontFromAverage(){

    std::vector<double> avgRadius = compute_average_radial_profile(m_wf->workData,
                                  cv::Point(m_wf->m_outside.m_center.x(),m_wf->m_outside.m_center.y()), m_wf->m_outside.m_radius);

    //create a matrix from the avgRadius profile.
    mirrorDlg *md = mirrorDlg::get_Instance();
    // first add the null back into it.
    if (md->doNull){
        for (unsigned int i = 0; i < avgRadius.size(); ++i) {
            double R2 = (double(i))/(avgRadius.size() -1);
            R2 *= R2;
            avgRadius[i] += md->z8 * md->cc * (1. + R2 * (-6 + 6. * R2));;
        }
    }
    cv::Mat result = createInterpolatedCircularSurface(avgRadius);
    SurfaceManager *sm = SurfaceManager::get_instance();
    sm->createSurfaceFromPhaseMap(result,
                              m_wf->m_outside,
                              m_wf->m_inside,
                              QString("avg"), WavefrontOrigin::Average);
}
