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
#include <QFileDialog>
#include <QMessageBox>
#include "zernikeprocess.h"
#include <QLineEdit>


percentCorrectionDlg::percentCorrectionDlg( QWidget *parent) :
    QDialog(parent),m_showZones(false),
    ui(new Ui::percentCorrectionDlg)
{
    ui->setupUi(this);

    resize(1000,800);


    mirrorDlg &md = *mirrorDlg::get_Instance();
    m_radius = md.m_clearAperature/2.;
    QSettings set;
    ui->minvalue->blockSignals(true);
    ui->maxvalue->blockSignals(true);
    ui->minvalue->setValue(set.value("percent_correction_min",-10).toDouble());
    ui->maxvalue->setValue(set.value("percent_correction_max", 120).toDouble());
    ui->minvalue->blockSignals(false);
    ui->maxvalue->blockSignals(false);
    ui->maxOrder->blockSignals(true);
    m_maxOrder = set.value("percentMaxOrder",18).toUInt();
    ui->maxOrder->setValue(m_maxOrder);
    int mmax = m_maxOrder/2;
    int nzerns = (mmax+1)*(mmax+1);
    ui->noOfTerms->setText(QString::number(nzerns));
    ui->maxOrder->blockSignals(false);
    ui->percentTable->setStyleSheet("background-color: lightgray;");
    QList<int> sizes;
    sizes << 500 << 100;
    ui->splitter->setSizes(sizes);
    m_number_of_zones = set.value("percent number of zones", 5).toInt();

    ui->numberOfZones->blockSignals(true);
    ui->numberOfZones->setValue(m_number_of_zones);
    ui->numberOfZones->blockSignals(false);
    if (set.contains("percent_Correction_dialogGeometry")) {
        setGeometry(set.value("percent_Correction_dialogGeometry").toRect());
    }

    makeZones();


}



double  g_laserLambda = 550.; // a global so a none member function can access it.

void percentCorrectionDlg::saveSettings(){
    QJsonObject myJsonObject;
    myJsonObject["ROC"] = m_roc;
    myJsonObject["mirror radius"] = m_radius;

    QJsonArray jzones;
    for (const auto &item : zoneCenter) {
        jzones.append((double)item/m_radius);   // zone centers are saved as a percentage.
    }
    myJsonObject["zones"] = jzones;
    QSettings set;
    QJsonDocument jsonDoc(myJsonObject);
    set.setValue("correctionZones", jsonDoc.toJson(QJsonDocument::Compact));
    set.setValue("percentMaxOrder",m_maxOrder);
}



/****************************************************
 * zone data.  If none to begin with use 5 zones.
 * if last zones then use those.
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
        zoneCenters << QString::number(zoneCenter,'f',0).toDouble();
    }
    return zoneCenters;
}
void percentCorrectionDlg::updateZoneTable(){

    QStringList vertLabels;

    QStringList hLabels;

    ui->percentTable->clear();
    ui->percentTable->setColumnCount(zoneCenter.size());
    ui->percentTable->setRowCount(2);
    ui->percentTable->blockSignals(true);


    for (int i = 0; i < zoneCenter.size(); ++i) {

        QTableWidgetItem *item = new QTableWidgetItem(QString("%1mm").arg(zoneCenter[i],0,'f',0));
        item->setTextAlignment(Qt::AlignCenter);
        ui->percentTable->setItem(0, i, item);
        item->setTextAlignment(Qt::AlignCenter);
        vertLabels<< QString().number(i);
        hLabels << QString().number(i+1);
    }


    ui->percentTable->setHorizontalHeaderLabels(hLabels);

    ui->percentTable->setColumnCount(zoneCenter.size());
    ui->percentTable->blockSignals(false);
}
QJsonDocument percentCorrectionDlg::loadZonesFromJson(QString str){
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    QJsonObject jsonData=jsonDoc.object();
    QJsonArray zones = jsonData["zones"].toArray();


    zoneCenter.clear();
    ui->percentTable->clearContents();
    ui->percentTable->setColumnCount(zones.size());

    for (int i = 0; i < zones.size(); ++i) {
        double d = zones[i].toDouble()* m_radius;

        zoneCenter.append(QString::number(d,'f',0).toDouble() );
    }
    return jsonDoc;
}

arma::mat percentCorrectionDlg::makeZoneZerns(QList<double> centers){
   std::vector<double> rhovec, thetavec;

   // creaate rowvec or rho and theta.  Theta will only be 0.
   for (int i = 0; i < centers.length(); ++i){

         double rho = centers[i];
         rhovec.push_back(rho);
         thetavec.push_back(0.);
   }

   arma::rowvec rhov(rhovec), thetav(thetavec);

   zernikeProcess zp;
   zp.setMaxOrder(m_maxOrder);

   arma::mat theZs = zp.zpmC(rhov, thetav, m_maxOrder);
   return theZs;
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

        QJsonDocument doc = loadZonesFromJson(jsonString);
        // if number of zones has changed then generate new zones.
        if ((zoneCenter.size() != m_number_of_zones)  || (m_radius != doc.object()["mirror radius"].toDouble())){
            zoneCenter = generateZoneCenters(m_radius, m_number_of_zones);
            saveSettings();
        }

    }
    updateZoneTable();
    zoneZerns = makeZoneZerns(zoneCenter);
}

// the profile version needs the null removed and is in output lambda (usually 550);
// zernike based does not need the null removed but needs to use laser wave length
double percentCorrectionDlg::getZernSurface( double RoC, double MirrorRad, std::vector<double> Zernikes, double x, double null = 0.,
                                             bool useavg = false){

    double num1 = x / MirrorRad;
    arma::rowvec rhov(1), thetav(1);
    rhov[0] = x/m_radius;  thetav[0] = 0.;
    zernikeProcess zp;
    zp.setMaxOrder(m_maxOrder);

    arma::mat theZs = zp.zpmC(rhov, thetav, m_maxOrder);

    double val = 0;
    if (useavg) {

        // find x location in profile
        val = m_avg[m_avg.length()-1].y();
        for (int i = 0; i < m_avg.length(); ++i){
            if (m_avg[i].x() == x){
                val = m_avg[i].y();
                break;
            }
            if (m_avg[i].x() > x){
                double dx = m_avg[i].x() - m_avg[i-1].x();
                double dy = m_avg[i].y() - m_avg[i-1].y();
                val = m_avg[i-1].y()  + (x - m_avg[i-1].x())*(dy/dx);
                break;

            }

        }
        val += (null) * theZs(0,8);
        double spherey = RoC - sqrt(pow(RoC, 2.0) - pow(x, 2.0));
        double zerny = val * m_outputLambda * .5E-6;
        double surf =   spherey + zerny;
        return surf;
    }
    else {

        // for each spherical term
        int z = 8;
        for(unsigned int j = 6; z < theZs.n_cols; j+=2){

            val += Zernikes[z] * theZs(0,z);
            z = j * j /4 + j;
        }
        double spherey = RoC - sqrt(pow(RoC, 2.0) - pow(x, 2.0));
        double zerny = val * m_lambda_nm * .5E-6;
        double surf =   spherey + zerny;
        return surf;
    }



}

// profile from profile plot with null removed
void percentCorrectionDlg::setProfile(QPolygonF profile){
    m_avg = profile;

}
// will use zernike values to compute two surface points x+- .01x  away from x
//      then compute normal slope from that.
double percentCorrectionDlg::getnormalSlope(double RoC, double radius, std::vector<double> Zernikes, double x, double null = 0, bool useAvg = false){

    double num1 = x / 100.0;
    double surface1 = getZernSurface(RoC, radius, Zernikes, x - num1, null,useAvg);  // problem  with zonendx  (the delta is not same as zonendx)
    double surface2 = getZernSurface(RoC, radius, Zernikes, x + num1, null,useAvg);
    double slope =  (surface2 - surface1)/ (2 * num1);
    slope = -1.0 / slope;

    return slope;
}
// b == knife y value
// y = mx + b
// b = y - mx

double percentCorrectionDlg::GetActualKE(double RoC, double MirrorRad, std::vector<double> Zernikes, double x, double null, bool use_avg)
{

    double slope = getnormalSlope(RoC, MirrorRad, Zernikes, x, null, use_avg);
    double surface = getZernSurface(RoC, MirrorRad, Zernikes, x, null, use_avg);

    double actualKe =  surface - x * slope  - RoC;

    return actualKe;
}
// 5.3 = x/slope + 5.3 - roc  x/slope = roc   slope = x/roc
double getIdealKE(double RoC, double Z_Radius)
{
  return pow(Z_Radius, 2.0) / (2.0 * RoC);
}
QPolygonF percentCorrectionDlg::makePercentages(surfaceData *surf){

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

    QPolygonF percentage;
    // start at the mirrors middle

    QVector<double> IdealzoneKnife;
    QVector<double> ActualZoneKnife;
    QVector<double> idealDelta;
    QVector<double> actualDelta;
    IdealzoneKnife << 0.;
    ActualZoneKnife << 0.0;

    mirrorDlg *md = mirrorDlg::get_Instance();
    double nullval = md->z8 * md->cc;
    // process each zone center

    QPolygonF idealknives;
    QPolygonF actualknives;
    QPolygonF zernKnives;
    QPolygonF zernSurf;

    double idealoffset = 0.;
    double zernOffset = 0.;

    for (int zone = 0; zone < zoneCenter.length(); ++zone){
        double x = zoneCenter[zone];

        //idealSurface << QPointF(x, normIdealSlope);
        double idealknife = getIdealKE(m_roc, x);

        // when using average profile as the profile we need to remove the artificial
        //   null from the profile thus the nullval being passed.
        double zernKnife = GetActualKE(m_roc, m_radius, surf->zernvalues, x, nullval ,ui->useProfile->isChecked());

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

    QPolygonF correction = getZoneCorrection(idealDeltas, zernDeltas);

    return correction;
}


void percentCorrectionDlg::plotProfile(){

    mirrorDlg *md = mirrorDlg::get_Instance();
    double nullval = md->z8 * md->cc;
    for (int i = 0; i < surfs.length(); ++ i) {

        QwtPlotCurve *Curve = new QwtPlotCurve();
        QPolygonF profile;
        for(double r = -m_radius; r <= m_radius; r += 1. ){
            double y = getZernSurface(m_roc, m_radius, surfs[i]->zernvalues, fabs(r), nullval, false);

            double sphery = m_roc - sqrt(pow(m_roc, 2.0) - pow(r, 2.0));
            y -= sphery;
            y /= m_lambda_nm * .5E-6;
            profile << QPointF(r,  y);
        }

        Curve->setSamples(profile);
        Curve->attach(ui->plot);
        Curve->setPen(Qt::green,5);
        Curve->attach(ui->plot);
    }
    QPolygonF profile2;
    // now plot the m_avg surface
    int i = 0;
    for(double r = 0; r < m_avg.length(); r += 1. ){

       // qDebug() << "r" << r << m_avg[i];
                    double y = m_avg[r].y();//getZernSurface(m_roc, m_radius, surfs[i]->zernvalues, fabs(r), true);

        double sphery = m_roc - sqrt(pow(m_roc, 2.0) - pow(r, 2.0));
        //y -= sphery;
        //y /= m_lambda_nm * .5E-6;
        profile2 << QPointF(m_avg[r].x(),  y);

    }
        QwtPlotCurve *Curve2 = new QwtPlotCurve();
        Curve2->setSamples(profile2);
        Curve2->attach(ui->plot);
        Curve2->setPen(Qt::blue,5);
        Curve2->attach(ui->plot);

    ui->plot->setAxisScale(QwtPlot::xBottom, -m_radius, m_radius);
    ui->plot->setAxisAutoScale(QwtPlot::yLeft);

    ui->plot->setAxisTitle( ui->plot->yLeft, "Error in waves at 550 nm" );
    ui->plot->replot();
}
void percentCorrectionDlg::plot(){
    QPolygonF expectedslope;
    QPolygonF actualslope;
    QPolygonF percentplot;
    QPolygonF franzPrecentPlot;
    QPolygonF actualSurface;
    QPolygonF idealSurface;
    QPolygonF correction;

    QPolygonF data;

    std::vector<double> zernikes;

    // first get the zernike poly spherical terms at each zone center.
    for (int c = 0; c < zoneCenter.length(); ++c){

    }

    ui->plot->detachItems(QwtPlotItem::Rtti_PlotItem);

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setZ(1);

    grid->enableXMin(true);
    grid->enableYMin(true);

    grid->setPen( Qt::gray, 0, Qt::DotLine );
    grid->setMajorPen( Qt::blue, 2.0,Qt::DotLine);
    grid->setMinorPen(Qt::black, 1.0, Qt::DotLine);
    grid->attach( ui->plot);


//    if (!ui->correction->isChecked()){
//        plotProfile();
//        return;
//    }

    ui->percentTable->blockSignals(true);
    ui->percentTable->setRowCount(surfs.length() + 1);
    QTableWidgetItem *item = new QTableWidgetItem("Zone Center");
    ui->percentTable->setVerticalHeaderItem(0, item);
    // for each surface draw the percent plot
    for (int i = 0; i < surfs.length(); ++ i) {
        // make percentages
        QPolygonF percent = makePercentages( surfs[i]);

        // Create 3D bar data
        QVector<double> row;

        for(int j = 0; j < percent.length(); ++j){
            QTableWidgetItem *item = new QTableWidgetItem(QString::number(percent[j].y(),'f',0) + "%");
            item->setTextAlignment(Qt::AlignCenter);
            item->setFlags(item->flags() & ~Qt::ItemIsEditable);
            ui->percentTable->setItem(i+1,j+1,item);

            row << percent[j].y();
        }
        QTableWidgetItem *item = new QTableWidgetItem(surfs[i]->m_name);
        item->setForeground(surfs[i]->penColor);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);

        ui->percentTable->setVerticalHeaderItem(i+1, item);
        m_seriesName << surfs[i]->m_name;


        QPolygonF bars;

        if (surfs.length() < 2) {
            // draw zone rectangles

            double width;
            for(int i = 0; i < percent.length(); ++i){

                double y = percent[i].y();

                if (i < percent.length()-1)
                    width= .80 * (percent[i+1].x() - percent[i].x()) ;

                QwtPlotShapeItem *rectangleItem = new QwtPlotShapeItem();

                rectangleItem->setRect(QRectF(percent[i].x() - width/2. ,0,width,y));

                QPen pen(Qt::black);
                pen.setWidth(3);
                rectangleItem->setPen(pen);
                rectangleItem->setBrush(QBrush(Qt::lightGray));
                rectangleItem->attach(ui->plot);
                rectangleItem->setZ(0);
                QwtPlotMarker *label = new QwtPlotMarker();
                label->setLineStyle(QwtPlotMarker::NoLine);
                label->setLabel(QString("%1\%").arg(y, 0, 'f',1)) ;
                label->setValue(percent[i].x(), y-10);
                label->attach(ui->plot);
            }

        }



        if (surfs.length() == 1) {
            // draw markers
            for (int i = 0; i < zoneCenter.size(); ++i) {
                double center= zoneCenter[i];
                QwtPlotMarker *marker = new QwtPlotMarker();
                marker->setLineStyle(QwtPlotMarker::VLine); // Set the line style to vertical
                marker->setLinePen(Qt::red,2,Qt::DashLine);
                marker->setLabelAlignment(Qt::AlignLeft);
                marker->setXValue(center);
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
        }



        QwtPlotCurve *slopeCurve3 = new QwtPlotCurve();

        slopeCurve3->setSamples(percent);
        slopeCurve3->attach(ui->plot);
        slopeCurve3->setPen(surfs[i]->penColor,5);

        ui->plot->setAxisTitle( ui->plot->yLeft, "Percent correction" );
        ui->plot->setAxisScale(ui->plot->yRight, -10, 20, 1);
        ui->plot->setAxisTitle( ui->plot->xBottom, "Mirror Radius mm" );
        slopeCurve3->setZ(1);
        ui->plot->setAxisScale(QwtPlot::yLeft, ui->minvalue->value(), ui->maxvalue->value());
        ui->plot->setAxisScale(QwtPlot::xBottom, 0, m_radius);
    }
    ui->percentTable->blockSignals(false);
    // Add the series to the graph
    ui->plot->replot();
}

bool compare(QVector< surfaceData *> data1, QVector< surfaceData *> data2){
    if (data1.length() != data2.length()){
        qDebug() << "different length";
        return false;
    }
    for (int ndx = 0; ndx < data1.length(); ++ ndx){
        if (data1[ndx]->igramlambda != data2[ndx]->igramlambda ||
                data1[ndx]->m_name != data2[ndx]->m_name ||
                data1[ndx]->penColor != data2[ndx]->penColor ||
                data1[ndx]->zernvalues != data2[ndx]->zernvalues)

            return false;

    }
    return true;
}

void percentCorrectionDlg::setData( QVector< surfaceData *> data) {

    bool different = !compare(data,surfs);
    qDebug() << "true if different" << different;
    mirrorDlg &md = *mirrorDlg::get_Instance();
    m_roc = md.roc;
    m_lambda_nm = md.lambda;
    QSettings set;
    m_outputLambda = set.value("outputLambda").toDouble();

    m_radius = md.m_clearAperature/2.;
    surfs = data;
    ui->percentTable->setRowCount(data.length());

    QStringList labels;
    labels << "Zone\ncenter";
    foreach(auto t, data){
        labels << t->m_name;

    }
    ui->percentTable->setVerticalHeaderLabels(labels);
    ui->percentTable->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    makeZones();

    plot();
    //if (different)
        //emit make_percent_correction(m_maxOrder);
}

percentCorrectionDlg::~percentCorrectionDlg()
{
    delete ui;
}


void percentCorrectionDlg::on_minvalue_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("percent_correction_min",arg1);
    plot();
}


void percentCorrectionDlg::on_maxvalue_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("percent_correction_max", arg1);
    plot();
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

    makeZones();
    plot();

}


void percentCorrectionDlg::on_help_clicked()
{
   QToolTip::showText(ui->help->mapToGlobal(QPoint()),ui->help->toolTip(),ui->help);
}



void percentCorrectionDlg::on_loadZones_clicked()
{
    QSettings set;
    QString path = set.value("projectPath").toString();
    QString extensionTypes(tr( "zone file (*.zones)"));
    QString fileName = QFileDialog::getOpenFileName(0,
                        tr("Read zone file"), path,
                        extensionTypes);
    if (fileName.isEmpty())
        return;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file";
        return;
    }

    QTextStream in(&file);
    QString line = in.readLine();

    file.close();

    QJsonDocument doc = loadZonesFromJson(line);
    QJsonObject jsonData=doc.object();
    if (jsonData.contains("aborted"))
        return;
    QJsonArray zones = jsonData["zones"].toArray();
    m_number_of_zones = zones.size();
    ui->numberOfZones->blockSignals(true);
    ui->numberOfZones->setValue(m_number_of_zones);
    ui->numberOfZones->blockSignals(false);
    saveSettings();
    //emit percent_plot_changed();

}


void percentCorrectionDlg::on_saveZones_clicked()
{
    QSettings set;
    QString path = set.value("projectPath").toString();
    QString extensionTypes(tr( "zone file (*.zones)"));
    QString fileName = QFileDialog::getSaveFileName(0,
                        tr("Save zone file"), path,
                        extensionTypes);
    if (fileName.isEmpty())
        return;
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".zones"); }
    QString jsonString = set.value("correctionZones").toString();
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << " failed to read zones file.";
        //return false; // Failed to open file for writing
    }

    QTextStream out(&file);
    out << jsonString;

    file.close();
    set.setValue("projectPath", QFileInfo(fileName).absolutePath());
}




void percentCorrectionDlg::on_Generate_clicked()
{
    emit make_percent_correction();
}


void percentCorrectionDlg::on_maxOrder_valueChanged(int arg1)
{
    m_maxOrder = arg1;

    int mmax = arg1/2;
    int ncol = (mmax+1)*(mmax+1);
    ui->noOfTerms->setText(QString::number(ncol));
    saveSettings();
}


void percentCorrectionDlg::on_percentTable_itemChanged(QTableWidgetItem *item)
{

    zoneCenter[item->column()] = item->text().toDouble();

    saveSettings();
    plot();
}



void percentCorrectionDlg::on_useProfile_clicked(bool checked)
{
    plot();
}


void percentCorrectionDlg::on_useZernikies_clicked(bool checked)
{
    plot();
}

