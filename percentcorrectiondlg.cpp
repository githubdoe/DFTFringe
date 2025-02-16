#include "percentcorrectiondlg.h"
#include "ui_percentcorrectiondlg.h"
#include "qwt_plot_grid.h"
#include "qwt_scale_div.h"
#include "qwt_plot_barchart.h"
#include <qwt_scale_draw.h>
#include "qwt_plot_marker.h"
#include <QSettings>
#include "surfaceanalysistools.h"
#include "mirrordlg.h"
#include <qwt_legend.h>
#include <qwt_plot_canvas.h>
#include <QToolTip>
#include "surfacemanager.h"
#include <QJsonDocument>
#include <qwt_plot_shapeitem.h>


ZonePicker::ZonePicker( QwtPlot *plot, QList<double> *zones):
    QObject( plot ), m_zonelist(zones),m_radius(100.),
    d_selectedMarker( NULL ), d_selectedZone(-1)

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
    canvas->setMouseTracking(true);

}
void ZonePicker::reset(){
    moving = false;
    onazone = false;
    d_lastFound = NULL;

}
QwtPlot *ZonePicker::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot *ZonePicker::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}

bool ZonePicker::event( QEvent *ev )
{
    if ( ev->type() == QEvent::User )
    {
        return true;
    }
    return QObject::event( ev );
}

bool ZonePicker::eventFilter( QObject *object, QEvent *event )
{
    if ( plot() == NULL || object != plot()->canvas() )
        return false;

    switch( event->type() )
    {
        case QEvent::FocusIn:
        {
            break;
        }
        case QEvent::FocusOut:
        {
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
        case QEvent::MouseButtonRelease:
        {
            moving = false;
            //select( mouseEvent->pos() );
            if (d_selectedZone != -1)
                emit(zoneMoved(d_selectedZone, d_selectedMarker->xValue()));
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

void ZonePicker::select( const QPoint &pos )
{
    QwtPlotMarker *mark = NULL;
    double dist = 10e10;

    double px = pos.x();
    d_selectedMarker = NULL;
    if (findItem(pos.x(), 10,QwtPlotItem::Rtti_PlotMarker)){
        d_selectedMarker->setVisible(true);
        moving = true;
    }



}

void ZonePicker::edit(bool b){
    m_editing = b;
}

// Move the selected point
void ZonePicker::move( const QPoint &pos )
{
    if (m_editing && !moving){
        QwtPlotMarker *found = findItem(pos.x(), 10, QwtPlotItem::Rtti_PlotMarker);
        if (found){
            if (!onazone){
                d_lastFound = found;
                highlight(d_lastFound, true);
                onazone = true;
                emit zoneSelected(d_selectedZone);
                plot()->replot();
            }
        }
        else if (onazone){
            // turn off highlight
            if (d_lastFound)
                highlight(d_lastFound,false);

            plot()->replot();
            onazone = false;
        }
        return;
    }
    if ( d_selectedZone == -1)
        return;

    double x = plot()->invTransform(d_selectedMarker->xAxis(), pos.x() );

    if ( (d_selectedZone == m_zonelist->last() && (x >= m_radius * .9))  ||
         ((d_selectedZone < m_zonelist->length()-1) && x >= (*m_zonelist)[d_selectedZone+1]))
        return;
    if (d_selectedZone > 1 && x <= (*m_zonelist)[d_selectedZone -1])
        return;


    d_selectedMarker->setXValue(x);

    d_selectedMarker->setLabel( QString("%1\n%2\%").arg(x, 0, 'f',1).arg(100. * x/m_radius,0,'f',1 )) ;

    emit(zoneMoved(d_selectedZone, d_selectedMarker->xValue()));


    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
     */
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );


}

QwtPlotMarker *ZonePicker::findItem(double x, double tolerance, QwtPlotItem::RttiValues type){
    const QwtPlotItemList& itmList = plot()->itemList();
    double px = x;
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {

        if ( ( *it )->rtti() == type ){

            QwtPlotMarker *m = static_cast<QwtPlotMarker *>( *it );
            if (m->lineStyle() != QwtPlotMarker::VLine)
                continue;
            const QwtScaleMap xMap = plot()->canvasMap(2);

            const double cx = xMap.transform( m->xValue());

            double d = px - cx;

            if ( abs(d) < tolerance ){

                for (int i = 0; i < m_zonelist->length(); ++i){
                    double t = m_zonelist->at(i);
                    if (t == m->xValue()){
                        d_selectedZone = i;
                        d_selectedMarker = m;
                        return m;
                    }
                }
            }

        }
    }
    return NULL;
}
void ZonePicker::highlight(QwtPlotMarker *m, bool flag){

    // change pen color if selected.
    if (flag)
        m->setLinePen(Qt::darkBlue,4,Qt::DashLine);
    else
        m->setLinePen(Qt::red,2,Qt::DashLine);

}
percentCorrectionDlg::percentCorrectionDlg( QWidget *parent) :
    QDialog(parent),m_showZones(false),
    ui(new Ui::percentCorrectionDlg)
{
    ui->setupUi(this);

    resize(1000,800);
    //setWindowFlags(Qt::WindowStaysOnTopHint);
    QSettings set;
    ui->minvalue->blockSignals(true);
    ui->maxvalue->blockSignals(true);
    ui->minvalue->setValue(set.value("percent_correction_min",-10).toDouble());
    ui->maxvalue->setValue(set.value("percent_correction_max", 120).toDouble());
    ui->minvalue->blockSignals(false);
    ui->maxvalue->blockSignals(false);
    ui->zoneValue->hide();
    m_number_of_zones = set.value("percent number of zones", 5).toInt();

    ui->numberOfZones->blockSignals(true);
    ui->numberOfZones->setValue(m_number_of_zones);
    ui->numberOfZones->blockSignals(false);
    if (set.contains("percent_Correction_dialogGeometry")) {
        setGeometry(set.value("percent_Correction_dialogGeometry").toRect());
    }

    zonePicker = new ZonePicker(ui->plot, &zoneCenter);
    connect(zonePicker,SIGNAL(zoneMoved(int, double)), this,SLOT(zoneMoved(int, double)));
    connect(zonePicker,SIGNAL(zoneSelected(int)),this,SLOT(zoneSelected(int)));
}
//  .0000018
double g_laserLambda = 550.; //fixme should get this from the parent.

void percentCorrectionDlg::saveSettings(){
    QJsonObject myJsonObject;
    myJsonObject["ROC"] = m_roc;
    myJsonObject["mirror radius"] = m_radius;

    QJsonArray jzones;
    for (const auto &item : zoneCenter) {
        jzones.append(item);
    }
    myJsonObject["zones"] = jzones;
    QSettings set;
    QJsonDocument jsonDoc(myJsonObject);
    set.setValue("correctionZones", jsonDoc.toJson(QJsonDocument::Compact));

}
/****************************************************
 * zone data.  If none to begin with use 5 zones.
 * if last zones then use those.
 * if user wants to change let hime but make him press accept
 * allow loading and saveing of zones
 * allways save current accpeted zones in settings probalby in json format.
*/
QList<double> generateZoneCenters(double radius, int number_of_zones){
    double exc_pct = 100. * pow(16.75,2.)/pow(radius,2.);
    QList<double> zoneCenters;
    QList<double> zoneedge;
    // create the zones and the zone centers
    for (int i = 0; i < number_of_zones+1; ++i){
        zoneedge << radius * sqrt((1. -.01 * exc_pct) * i/number_of_zones + .01 * exc_pct);
    }

    double lastzone = 0.;
    for (int i = 0; i < number_of_zones; ++i){
        double zoneCenter = (lastzone + zoneedge[i])/2.;
        lastzone = zoneedge[i];
        zoneCenters << zoneCenter;
    }
    return zoneCenters;
}
void percentCorrectionDlg::makeZones(){

    QSettings set;
    if (!set.contains("correctionZones")){
        generateZoneCenters(m_radius, m_number_of_zones);
        saveSettings();
    }
    else { // read zones from settings
        // if mirror is different now than last then make the same number of zones but with new radius values
        QString jsonString = set.value("correctionZones").toString();

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonString.toUtf8(), &jsonError);
        QJsonObject jsonData=jsonDoc.object();
        QJsonArray zones = jsonData["zones"].toArray();
        double roc = jsonData["roc"].toDouble();
        zoneCenter.clear();
        for (int i = 0; i < zones.size(); ++i) {
            zoneCenter.append(zones[i].toDouble());
        }
        // if number of zones has changed then generate new zones.
        if ((zones.size() != m_number_of_zones)  || (m_radius != jsonData["mirror radius"].toDouble())){
            zoneCenter = generateZoneCenters(m_radius, m_number_of_zones);
            saveSettings();
        }

    }


}
double getZernSurface( double RoC, double MirrorRad, double Zernikes[], double radius){

         double num1 = radius / MirrorRad;
         if (num1 > 1.0)
           num1 = 1.0;
         double num2 = num1 * num1;
         double num3 = num2 * num2;
         double num4 = num3 * num2;
         double num5 = num3 * num3;
         double num6 = num5 * num2;

         double num7 = 0.0 + Zernikes[8] * (1.0 - 6.0 * num2 + 6.0 * num3) + Zernikes[15] * (1.0 + 12.0 * num2 - 30.0 * num3 + 20.0 * num4) + Zernikes[24] * (1.0 - 20.0 * num2 + 90.0 * num3 - 140.0 * num4 + 70.0 * num5) + Zernikes[35] * (1.0 + 30.0 * num2 - 210.0 * num3 + 560.0 * num4 - 630.0 * num5 + 252.0 * num6);
         double spherey = RoC - sqrt(pow(RoC, 2.0) - pow(radius, 2.0));
         double zerny = num7 * g_laserLambda * .5E-6;
         double surf =   spherey + zerny;
         return surf;
}
// will use zernike values to compute two surface points x+- .01x  away from x
//      then compute normal slope from that.
double getnormalSlope(double RoC, double radius, double Zernikes[], double x){
    double num1 = x / 100.0;
    double surface1 = getZernSurface(RoC, radius, Zernikes, x - num1);
    double surface2 = getZernSurface(RoC, radius, Zernikes, x + num1);
    double slope =  (surface2 - surface1)/ (2 * num1);
    slope = -1.0 / slope;

    return slope;
}
// b == knife y value
// y = mx + b
// b = y - mx

double GetActualKE(double RoC, double MirrorRad, double Zernikes[], double Z_Radius)
{
    double slope = getnormalSlope(RoC, MirrorRad, Zernikes, Z_Radius);
    double surface = getZernSurface(RoC, MirrorRad, Zernikes, Z_Radius);

    double actualKe =  surface - Z_Radius * slope  - RoC;

    return actualKe;
}
// 5.3 = x/slope + 5.3 - roc  x/slope = roc   slope = x/roc
double getIdealKE(double RoC, double Z_Radius)
{
  return pow(Z_Radius, 2.0) / (2.0 * RoC);
}
/*
 * paraby(x) = spherey(x) + profile(x) + ?
 *
 *
  */
void percentCorrectionDlg::replot(QColor penColor, bool addToPlot){

    int smoothrange = .1 * m_size;
    m_z8 = 0.;
    QPolygonF expectedslope;
    QPolygonF actualslope;
    QPolygonF percentplot;
    QPolygonF franzPrecentPlot;
    QPolygonF actualSurface;
    QPolygonF idealSurface;
    QPolygonF correction;

    QPolygonF data;
    SurfaceManager &sm = *SurfaceManager::get_instance();
    std::vector<double> zernikes;

    auto getSphereY = [] (double roc, double x){
        return roc - sqrt(pow(roc,2) - pow(x,2));
    };
    auto getNull = [] (double roc, double radius,  double x, double c8, double wavelength){
        double r = x/radius;

        double val =  wavelength * 1.E-5 * .5 * c8 * (6. * pow(r,4) - 6. * pow(r,2) +1);
        return val;
    };
    // return in mm
    auto getConicY = [] (double roc, double conic, double x){
        double y = pow(x,2)/(roc + (sqrt(pow(roc,2) - ((conic + 1) * pow(x,2)))));
        return y;
    };
    auto average = [] (QPolygonF &data, int width, int ndx){
        return data[ndx].y();
        int cnt = 0;
        double sum = 0.;
        for (int i = ndx - width; i <= ndx + width; ++i){
            sum += data[i].y();
            ++cnt;
        }
        return sum/cnt;
    };
    auto knifeDeltas = [](QPolygonF &knives){
        double last = knives[0].y();
        QPolygonF deltas;
        for (int i = 1; i < knives.length(); ++i ){
            deltas << QPointF( knives[i].x(),knives[i].y() - last);
            last = knives[i].y();
        }
        return deltas;

    };
    auto getZoneCorrection = [] (QPolygonF &ideal, QPolygonF test){
        QPolygonF percent;
        for (int i = 0; i < ideal.length(); ++i){
            percent << QPointF( test[i].x(), 100. * test[i].y()/ideal[i].y());
        }
        return percent;
    };


    double roc = m_roc + m_roc_offset;

    zernikes = sm.getCurrent()->InputZerns;

    double wavelength =1e-6 *  m_outputLambda;  // surface error
    int smoothsize = 1 ;

    double C8 =  mirrorDlg::get_Instance()->z8;
    double conic = mirrorDlg::get_Instance()->cc;

    // smooth and convert to surface error in mm.
//    double min = 999999.;
//    for (int i = 0; i < m_avg.size(); ++i){
//        min = fmin(min, m_avg[i].y());
//    }
    for (int i = (m_avg.size()-1)/2; i < m_avg.size(); ++i){
        //save error y in mm on surface
        // Lets try adding the null back into it
        // then later on add a sphere to it and see if it matches the zern version
        data << QPointF(m_avg[i ].x(),  (m_avg[i].y()) * .5E-6  * m_outputLambda );



    }


    // find the x entry in the surface that is closest to the zone value
    indexOfZoneCenter.clear();

    for (int i = 0; i < m_number_of_zones; ++i){
        int ndx = 0;
        for (; ndx < data.size(); ++ ndx){
            double d = data[ndx].x();
            if (d >= zoneCenter[i])
                break;
        }
        //zoneCenter << zoneCenter[i];
        indexOfZoneCenter << ndx;
    }

    // start at the mirrors middle

    QVector<double> IdealzoneKnife;
    QVector<double> ActualZoneKnife;
    QVector<double> idealDelta;
    QVector<double> actualDelta;

    int zone = 0;
    IdealzoneKnife << 0.;
    ActualZoneKnife << 0.0;


    // process each zone center

    QPolygonF idealknives;
    QPolygonF actualknives;
    QPolygonF zernKnives;
    QPolygonF zernSurf;




    double idealoffset = 0.;
    double zernOffset = 0.;

    for (int zone = 0; zone < zoneCenter.length(); ++zone){

        int ndx = indexOfZoneCenter[zone];

        double x = data[ndx].x();
        double idealy = getConicY(roc, conic, x);
        double y = data[ndx].y() + idealy;
        double zernsurf = getZernSurface(roc,m_radius, zernikes.data(), x);

        zernSurf << QPointF(x, zernsurf);


        //idealSurface << QPointF(x, normIdealSlope);
        double idealknife = getIdealKE(roc, x);
        double zernKnife = GetActualKE(roc, m_radius, zernikes.data(), x);

        if (zone == 0){
            idealoffset = idealknife;
            zernOffset = zernKnife;
        }
        idealknives << QPointF(x,idealknife - idealoffset);
        zernKnives << QPointF(x,zernKnife - zernOffset);


        // plot knives

    }

    QPolygonF idealDeltas = knifeDeltas(idealknives);
    QPolygonF zernDeltas = knifeDeltas(zernKnives);

    correction = getZoneCorrection(idealDeltas, zernDeltas);
    QPolygonF profileCorrections = getZoneCorrection(idealDeltas, zernDeltas);


    QPolygonF bars;




    if (!addToPlot)
        ui->plot->detachItems(QwtPlotItem::Rtti_PlotItem);

    // draw zone rectangles
    if (!ui->editEnable->isChecked()){
        double width;
        for(int i = 0; i < profileCorrections.length(); ++i){

            double y = profileCorrections[i].y();

            if (i < profileCorrections.length()-1)
             width= .80 * (profileCorrections[i+1].x() - profileCorrections[i].x()) ;

            QwtPlotShapeItem *rectangleItem = new QwtPlotShapeItem();

             rectangleItem->setRect(QRectF(profileCorrections[i].x()-width/2,0,width,y));

            QPen pen(Qt::black);
            pen.setWidth(3);
            rectangleItem->setPen(pen);
            rectangleItem->setBrush(QBrush(Qt::lightGray));
            rectangleItem->attach(ui->plot);
            rectangleItem->setZ(0);
            QwtPlotMarker *label = new QwtPlotMarker();
            label->setLineStyle(QwtPlotMarker::NoLine);
            label->setLabel(QString("%1\%").arg(y, 0, 'f',1)) ;
            label->setValue(profileCorrections[i].x(), y-10);
            label->attach(ui->plot);
        }
    }
    // draw markers
    for (int i = 0; i < zoneCenter.size(); ++i) {
        double center= zoneCenter[i];
        QwtPlotMarker *marker = new QwtPlotMarker();
        marker->setLineStyle(QwtPlotMarker::VLine); // Set the line style to vertical
        marker->setLinePen(Qt::red,2,Qt::DashLine);
        marker->setLabelAlignment(Qt::AlignLeft);
        marker->setXValue(center);
        if (!ui->editEnable->isChecked())
            marker->setVisible(false);
        marker->attach(ui->plot);

        QwtPlotMarker *label = new QwtPlotMarker();
        label->setLineStyle(QwtPlotMarker::NoLine);
        label->setLabel(QString("%1\n%2\%").arg(center, 0, 'f',1).arg(100. * center/m_radius,0,'f',1 )) ;
        label->setLabelAlignment(Qt::AlignCenter);
        label->setXValue(center);
        label->setYValue(-23);
        label->attach(ui->plot);

        // draw zone number
        QwtPlotMarker *marker2 = new QwtPlotMarker();
        marker2->setLineStyle(QwtPlotMarker::NoLine);
        marker2->setLabel(QString().number(i+1));
        marker2->setLabelAlignment(Qt::AlignCenter);
        marker2->setYValue(-5);
        marker2->setXValue(center);
        marker2->attach(ui->plot);
    }

    QwtPlotGrid *grid = new QwtPlotGrid();

    grid->setZ(1);

    grid->enableXMin(true);
    grid->enableYMin(true);

    grid->setPen( Qt::gray, 0, Qt::DotLine );
    grid->setMajorPen( Qt::blue, 2.0,Qt::DotLine);
    grid->setMinorPen(Qt::black, 1.0, Qt::DotLine);
    grid->attach( ui->plot);

    ui->plot->insertLegend( new QwtLegend() , QwtPlot::BottomLegend);


        QwtPlotCurve *slopeCurve3 = new QwtPlotCurve("zern correction");
        slopeCurve3->setLegendAttribute(QwtPlotCurve::LegendShowSymbol,true );
        slopeCurve3->setLegendAttribute(QwtPlotCurve::LegendShowLine,true );
        slopeCurve3->setItemAttribute(QwtPlotCurve::Legend,true);
        slopeCurve3->setSamples(profileCorrections);
        slopeCurve3->attach(ui->plot);
        slopeCurve3->setPen(Qt::blue,5);




        ui->plot->setAxisTitle( ui->plot->yLeft, "Percent correction" );
        ui->plot->setAxisScale(ui->plot->yRight, -10, 20, 1);
        ui->plot->setAxisTitle( ui->plot->xTop, "Radius mm" );
        slopeCurve3->setZ(1);
        ui->plot->setAxisScale(QwtPlot::yLeft, ui->minvalue->value(), ui->maxvalue->value());
        ui->plot->setAxisScale(QwtPlot::xBottom, 0, m_radius);

    ui->plot->replot();
}
void percentCorrectionDlg::zoneSelected(int ndx){
    m_currentSelectedZone = ndx;
    ui->zoneValue->blockSignals(true);
    ui->zoneValue->setValue(zoneCenter[ndx]);
    ui->zoneValue->blockSignals(false);
}
void percentCorrectionDlg::zoneMoved(int ndx, double value){

    zoneCenter[ndx] = value;
    ui->zoneValue->blockSignals(true);
    ui->zoneValue->setValue(value);
    ui->zoneValue->blockSignals(false);
    saveSettings();
    replot();
}
void percentCorrectionDlg::plot( QPolygonF avg, double radius,double roc, double z8,
                                 double desiredZ8,
                                 double lambda_nm, double outputLambda,
                                 QColor penColor, bool addToPlot){



    zonePicker->m_radius = radius;
    m_avg = avg;
    m_radius = radius;
    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    if (saTools->m_useDefocus){ // if defocus compute new SA null Desired Z8
        double fnumber = mirrorDlg::get_Instance()->FNumber;
        m_roc_offset = fnumber * fnumber * 8. * saTools->m_defocus * .055;  //mmeters
    }
    else {
        m_roc = roc;
    }

    m_z8 = z8;
    m_desiredZ8 = desiredZ8;
    g_laserLambda = m_lambda_nm = lambda_nm;
    m_outputLambda = outputLambda;
    m_size = avg.size()-1;

    //it is possible that the zones have yet to be set go read them from the settings.
    // it will make them if needed.
    makeZones();
    replot(penColor, addToPlot);
}

percentCorrectionDlg::~percentCorrectionDlg()
{
    delete ui;
}


void percentCorrectionDlg::on_minvalue_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("percent_correction_min",arg1);
    emit percent_plot_changed();
}


void percentCorrectionDlg::on_maxvalue_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("percent_correction_max", arg1);
    emit percent_plot_changed();
}

void percentCorrectionDlg::closeEvent(QCloseEvent *event) {
    QSettings set;
    set.setValue("percent_Correction_dialogGeometry", geometry());
    QDialog::closeEvent(event);
}
void percentCorrectionDlg::moveEvent(QMoveEvent *event) {
    QSettings set;
    set.setValue("percent_Correction_dialogGeometry", geometry());
    QDialog::moveEvent(event);
}

void percentCorrectionDlg::on_numberOfZones_valueChanged(int arg1)
{
    m_number_of_zones = arg1;
    QSettings set;
    set.setValue("percent number of zones", arg1);
    zonePicker->reset();
    makeZones();
    replot();

}


void percentCorrectionDlg::on_help_clicked()
{
   QToolTip::showText(ui->help->mapToGlobal(QPoint()),ui->help->toolTip(),ui->help);
}



void percentCorrectionDlg::on_loadZones_clicked()
{

}


void percentCorrectionDlg::on_saveZones_clicked()
{

}


void percentCorrectionDlg::on_editEnable_toggled(bool checked)
{
    zonePicker->edit(checked);
    ui->zoneValue->setEnabled(checked);
    if (!checked)
        ui->zoneValue->hide();
    else
        ui->zoneValue->show();
    replot();
}


void percentCorrectionDlg::on_zoneValue_valueChanged(double arg1)
{
    zoneCenter[m_currentSelectedZone] = arg1;
    replot();

}

