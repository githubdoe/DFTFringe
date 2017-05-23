#include "pixelstats.h"
#include "ui_pixelstats.h"
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_plot_marker.h>
#include <qdebug>
#include <QScreen>
#include <QScrollBar>
#include <QVBoxLayout>
#include <qapplication.h>
#include <qevent.h>
#include <qwhatsthis.h>
#include <qpainter.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "contourplot.h"
#include "wavefront.h"
#include "math.h"
double g_lb, g_ub;
int idx = -1;   // ndx of which marker is being moved by mouse.
QwtPlotMarker* g_lbMarker;
QwtPlotMarker* g_centerMarker;
QwtPlotMarker* g_ubMarker;
CanvasPicker::CanvasPicker( QwtPlot *plot ):
    QObject( plot ),
    d_selectedMarker( NULL )

{
    QwtPlotCanvas *canvas = qobject_cast<QwtPlotCanvas *>( plot->canvas() );
    canvas->installEventFilter( this );

    // We want the focus, but no focus rect. The
    // selected point will be highlighted instead.

    canvas->setFocusPolicy( Qt::StrongFocus );
#ifndef QT_NO_CURSOR
    canvas->setCursor( Qt::PointingHandCursor );
#endif
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();

    const char *text =
        "All points can be moved using the left mouse button "
        "or with these keys:\n\n"
        "- Up:\t\tSelect next curve\n"
        "- Down:\t\tSelect previous curve\n"
        "- Left, �-�:\tSelect next point\n"
        "- Right, �+�:\tSelect previous point\n"
        "- 7, 8, 9, 4, 6, 1, 2, 3:\tMove selected point";
    canvas->setWhatsThis( text );

    shiftCurveCursor( true );
}

QwtPlot *CanvasPicker::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot *CanvasPicker::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}

bool CanvasPicker::event( QEvent *ev )
{
    if ( ev->type() == QEvent::User )
    {
        showCursor( true );
        return true;
    }
    return QObject::event( ev );
}

bool CanvasPicker::eventFilter( QObject *object, QEvent *event )
{
    if ( plot() == NULL || object != plot()->canvas() )
        return false;

    switch( event->type() )
    {
        case QEvent::FocusIn:
        {
            showCursor( true );
            break;
        }
        case QEvent::FocusOut:
        {
            showCursor( false );
            break;
        }
        case QEvent::Paint:
        {
            QApplication::postEvent( this, new QEvent( QEvent::User ) );
            break;
        }
        case QEvent::MouseButtonPress:
        {
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
            select( mouseEvent->pos() );
            return true;
        }
        case QEvent::MouseMove:
        {
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );

            move( mouseEvent->pos() );
            return true;
        }


        default:
            break;
    }

    return QObject::eventFilter( object, event );
}

// Select the point at a position. If there is no point
// deselect the selected point

void CanvasPicker::select( const QPoint &pos )
{
    QwtPlotMarker *mark = NULL;
    double dist = 10e10;

    double px = pos.x();


    const QwtPlotItemList& itmList = plot()->itemList();

    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker ){

            QwtPlotMarker *m = static_cast<QwtPlotMarker *>( *it );

            const QwtScaleMap xMap = plot()->canvasMap(2);
            double x = plot()->invTransform(m->xAxis(), pos.x() );
            const double cx = xMap.transform( m->xValue());

            double d = px - cx;

            if ( abs(d) < 10 )
            {

                mark = m;
                if (m->xValue() == g_lb){
                    idx = 0;
                    dist = d;

                    break;
                }
                else if (m->xValue() == g_ub){
                    idx = 1;
                    dist = d;

                    break;
                }
                else idx = -1;
            }
        }
    }

    showCursor( false );
    d_selectedMarker = NULL;


    if ( mark && dist < 10 ) // 10 pixels tolerance
    {

        d_selectedMarker = mark;

        showCursor( true );
    }
}



// Move the selected point
void CanvasPicker::move( const QPoint &pos )
{
    if ( !d_selectedMarker )
        return;

    double x = plot()->invTransform(d_selectedMarker->xAxis(), pos.x() );

    d_selectedMarker->setXValue(x);
    if (idx ==0){
        g_lb = d_selectedMarker->xValue();
        d_selectedMarker->setLabel( QString().sprintf("%lf", x) ) ;

    }
    else {
        g_ub = d_selectedMarker->xValue();
        d_selectedMarker->setLabel( QString().sprintf("%lf", x) ) ;
    }
    g_centerMarker->setXValue( (g_ub + g_lb)/2.);
    g_centerMarker->setLabel(QString().sprintf("PV %lf", (g_ub -g_lb)));

    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
     */
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );

    showCursor( true );
    emit(markerMoved());
}

// Hightlight the selected point
void CanvasPicker::showCursor( bool showIt )
{
    if ( !d_selectedMarker )
        return;

}

// Select the next/previous curve
void CanvasPicker::shiftCurveCursor( bool up )
{
    QwtPlotItemIterator it;

    const QwtPlotItemList &itemList = plot()->itemList();

    QwtPlotItemList curveList;
    for ( it = itemList.begin(); it != itemList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker )
            curveList += *it;
    }
    /*
    if ( curveList.isEmpty() )
        return;

    it = curveList.begin();

    if ( d_selectedMark )
    {
        for ( it = curveList.begin(); it != curveList.end(); ++it )
        {
            if ( d_selectedCurve == *it )
                break;
        }
        if ( it == curveList.end() ) // not found
            it = curveList.begin();

        if ( up )
        {
            ++it;
            if ( it == curveList.end() )
                it = curveList.begin();
        }
        else
        {
            if ( it == curveList.begin() )
                it = curveList.end();
            --it;
        }
    }

    showCursor( false );
    d_selectedPoint = 0;
    d_selectedCurve = static_cast<QwtPlotCurve *>( *it );
    showCursor( true );
    */
}


/********************************************************************************
 * Pixel stats class
 *
 * ******************************************************************************/
pixelStats::pixelStats(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::pixelStats), slopeLimitArcSec(.5)
{
    ui->setupUi(this);
    ui->splitter->setHandleWidth(6);
    ui->splitter->setStyleSheet("QSplitter::handle {background-color: lightblue; border: 2px solid #777; border-radius: 4px;}");
    ui->image->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->image->setScaledContents(true);
    scrollArea = new QScrollArea();
    scrollArea->setWidget(ui->image);
    ui->verticalLayout->addWidget(scrollArea);
    scrollArea->setWidgetResizable(true);
    CanvasPicker *cp = new CanvasPicker(ui->histo);
    connect(cp,SIGNAL(markerMoved()), this, SLOT(on_bounds_valueChanged()));

    setWindowFlags(    Qt::WindowStaysOnTopHint);

}

pixelStats::~pixelStats()
{
    delete ui;


}

void pixelStats::setData(wavefront *w){
   m_wf = w;
   g_ub = m_wf->min +  (m_wf->max-m_wf->min) * .8;
   g_lb = m_wf->min +  (m_wf->max-m_wf->min) * .2;

   updateSurface();
   updateHisto();
}
cv::Mat mat2gray(const cv::Mat& src)
{
    cv::Mat dst;
    cv::normalize(src, dst, 0.0, 255.0, cv::NORM_MINMAX, CV_8U);

    return dst;
}
cv::Mat slope(wavefront * wf){
    cv::Mat gradx(wf->data.rows,wf->data.cols, CV_64F),grady(wf->data.rows,wf->data.cols, CV_64F), mag;
    double wAdjust = 550./wf->lambda;
    double wavePerPixel = wAdjust* (wf->diameter/(2 * (wf->m_outside.m_radius)))/
                                (wf->lambda * 1.e-6);

    const double radToArcSec = 206265.;
    for(int x = 0; x < wf->data.cols-1; ++x){
        for (int y = 0; y < wf->data.rows-1; ++y){
            if (wf->workData.at<bool>(x,y) && wf->workMask.at<bool>(x+1,y)){
                double h  = wf->workData.at<double>(x,y ) -
                                               wf->workData.at<double>(x+1,y);
                double slope = atan2( h, wavePerPixel) * radToArcSec;
                gradx.at<double>(x,y) = fabs(slope);

            }
            else{
                gradx.at<double>(x,y) = 0.;
            }
            if (wf->workMask.at<bool>(x,y) && wf->workMask.at<bool>(x,y+1)){
                double h = wf->workData.at<double>(x,y ) - wf->workData.at<double>(x,y+1);
                double slope = atan2(h, wavePerPixel) * radToArcSec;
                grady.at<double>(x,y) = fabs(slope);
            }
            else{
                grady.at<double>(x,y) = 0.;
            }
        }
    }

    cv::magnitude(gradx, grady, mag);
    cv::Mat maskedMag;
    mag.copyTo(mag, wf->workMask);
    return mag;
}

void pixelStats::updateSurface(){

    cv::Mat sur = cv::Mat::zeros(m_wf->data.rows, m_wf->data.cols, CV_8UC3);
    double ub,lb;
    ub = g_ub;
    lb = g_lb;
    for (int x = 0; x < sur.cols; ++x){
        for (int y = 0; y < sur.rows; ++y){
            double v = m_wf->workData.at<double>(y,x);
            if (m_wf->workMask.at<bool>(x,y)){
                if (v > ub){
                    sur.at<cv::Vec3b>(y,x) =cv::Vec3b(255,0,0);
                }
                else if (v < lb){
                    sur.at<cv::Vec3b>(y,x) = cv::Vec3b(0,0,255);
                }
                else sur.at<cv::Vec3b>(y,x) = cv::Vec3b(220,220,220);
            }
        }
    }
    if (true){
        cv::Mat mag = slope(m_wf);

        //cv::imshow("mag", mat2gray(mag));
        //cv::waitKey(1);

        double vmin,vmax;

        cv::minMaxLoc(mag, &vmin, &vmax,0,0, m_wf->workMask);

        for (int x = 0; x < sur.cols; ++x){
            for (int y = 0; y < sur.rows; ++y){
                double v = mag.at<double>(y,x);
                if (m_wf->workMask.at<bool>(x,y)){
                    if (fabs(v) > slopeLimitArcSec){
                        int c = 255 -125 * (fabs(v)-slopeLimitArcSec)/vmax;
                        sur.at<cv::Vec3b>(y,x) =cv::Vec3b(c,c,0);
                    }
                    else mag.at<double>(y,x) = 0;
                }
            }
        }
/*
        // make contour plot
        ContourPlot *cp = new ContourPlot();


        wavefront * wf = new wavefront(*m_wf);


        wf->data = wf->workData = mag;
        wf->workMask = m_wf->workMask;
        cv::Scalar mean,std;
        cv::meanStdDev(wf->data,mean,std);
        qDebug() << std[0] << mean[0];
        wf->std = std[0] ;
        wf->useSANull = false;
        wf->min = vmin;
        wf->max =  vmax;
        wf->name = QString("slope error");

        cp->setSurface(wf);
        cp->m_zRangeMode = "Min/Max";
        cp->showContoursChanged(.1);
        cp->setZRange();
        cp->resize(500,500);
        cp->replot();
        cp->show();*/
    }

    if (ui->minmaxloc->isChecked()){
        double minv,maxv;
        cv::Point minp,maxp;
        cv::minMaxLoc(m_wf->workData, &minv,&maxv, &minp,&maxp, m_wf->workMask );

        cv::circle(sur, maxp, 10, cv::Scalar(255,0,0));
        cv::circle(sur, minp, 10, cv::Scalar(0,0,255));
    }
    cv::flip(sur,sur,0);



    QImage tmp = QImage((uchar*)sur.data,
                        sur.cols,
                        sur.rows,
                        sur.step,
                        QImage::Format_RGB888).copy();

    //ui->image->resize(m_wf->data.cols, m_wf->data.rows);
    // set a scaled pixmap to a w x h window keeping its aspect ratio
    ui->image->setPixmap(QPixmap::fromImage(tmp));//.scaled(labelSize, Qt::KeepAspectRatio)));
    if (!ui->fit->isChecked())
        ui->image->adjustSize();


}

void  pixelStats::updateHisto(){

    ui->histo->detachItems( QwtPlotItem::Rtti_PlotCurve);
    ui->histo->detachItems( QwtPlotItem::Rtti_PlotMarker);
    ui->histo->detachItems(QwtPlotItem::Rtti_PlotItem);
    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int hbins = 1000, sbins = 32;
    int histSize[] = {hbins, sbins};
    // hue varies from 0 to 179, see cvtColor


    cv::Mat hist;

    cv::Mat values;
    cv::Mat mask;
    m_wf->workData.convertTo(values, CV_32F);
    m_wf->workMask.convertTo(mask, CV_8U);


    float hranges[] = { m_wf->min, m_wf->max};
    const float* ranges[] = { hranges };
    int channels[] = {0, 1};
    cv::calcHist( &values, 1, channels, m_wf->workMask, // do not use mask
             hist, 1, histSize, ranges,
             true, // the histogram is uniform
             false );
    double maxVal=0;
    minMaxLoc(hist, 0, &maxVal, 0, 0);


    double histInterval = (m_wf->max - m_wf->min)/hbins;
    QwtPlotHistogram *histPlot = new QwtPlotHistogram("Wave Fronts PV");
    histPlot->setStyle(QwtPlotHistogram::Columns);
    QVector<QwtIntervalSample> histData;

    for (int i = 0; i < hbins; ++i){
        QwtInterval interv(m_wf->min + (i) * histInterval, m_wf->min + (i+1)*histInterval);

        histData << QwtIntervalSample((double)(hist.at<float>(i)),interv);

    }

    histPlot->setSamples(histData);
    histPlot->attach(ui->histo);
    ui->histo->setAxisTitle(QwtPlot::xBottom,"error waves");
    ui->histo->setTitle("");
    //  ...a vertical line at upper bound
    QwtPlotMarker *muY = g_ubMarker = new QwtPlotMarker();
    muY->setLineStyle( QwtPlotMarker::VLine );
    muY->setLinePen( Qt::red, 1 );

    double ub = g_ub;
    muY->setLabel( QString().sprintf("%lf", ub) ) ;
    muY->setXValue(ub );
    muY->attach( ui->histo);
    histPlot->setSamples(histData);
    histPlot->attach(ui->histo);
    ui->histo->setAxisTitle(QwtPlot::xBottom,"error waves");
    ui->histo->setTitle("");

    //  ...a vertical line at lower bound
    QwtPlotMarker *mlY = g_lbMarker = new QwtPlotMarker();
    mlY->setLineStyle( QwtPlotMarker::VLine );
    mlY->setLinePen( Qt::blue, 1 );

    double lb = g_lb;
    mlY->setLabel( QString().sprintf("%lf", lb) ) ;
    mlY->setXValue(lb );
    mlY->attach( ui->histo);

    QwtPlotMarker *lab = g_centerMarker = new QwtPlotMarker();
    lab->setLabel(QString().sprintf("PV %lf", (ub -lb)));
    lab->setLineStyle( QwtPlotMarker::VLine );
    lab->setLinePen(Qt::black,0, Qt::DotLine);
    lab->setXValue((ub+lb)/2.);
    lab->setLabelAlignment(  Qt::AlignTop );
    lab->attach(ui->histo);


    //wftHistogram->resize(640,480);
    ui->histo->replot();
    QList<int> sizes;
    sizes << 200 << 500;
    ui->splitter->setSizes(sizes);
}



void pixelStats::on_bounds_valueChanged()
{
    updateSurface();
}

void pixelStats::on_fit_clicked(bool checked)
{
    scrollArea->setWidgetResizable(true);

    //ui->image->resize(m_wf->data.cols/2, m_wf->data.rows/2);
    //const QPixmap *p = ui->image->pixmap();
    //ui->image->setPixmap(p->scaled(ui->image->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    //ui->image->adjustSize();
    //ui->image->resize(ui->image->pixmap()->size());
}

void pixelStats::on_radioButton_2_clicked()
{
    scrollArea->setWidgetResizable(false);
    ui->image->adjustSize();

}

void pixelStats::on_minmaxloc_clicked(bool checked)
{
    updateSurface();
}

void pixelStats::on_arcSecs_valueChanged(double arg1)
{
    slopeLimitArcSec = arg1;
    updateSurface();
}
