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
#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/qabstract3dseries.h>
percentCorrectionDlg::percentCorrectionDlg( QWidget *parent) :
    QDialog(parent),m_showZones(false),
    ui(new Ui::percentCorrectionDlg)
{
    ui->setupUi(this);

    resize(1000,800);
    Q3DBars *widgetgraph = new Q3DBars();
    QWidget *container = QWidget::createWindowContainer(widgetgraph);

    QHBoxLayout *hLayout = new QHBoxLayout(ui->bar3D);

    hLayout->addWidget(container, 1);
    ui->bar3D->setWindowTitle(" how about this");
    //setWindowFlags(Qt::WindowStaysOnTopHint);

    QCheckBox *smoothCheckBox = new QCheckBox(ui->bar3D);
    smoothCheckBox->setText(QStringLiteral("Smooth bars"));
    smoothCheckBox->setChecked(false);
    hLayout->addWidget(smoothCheckBox);
    mirrorDlg &md = *mirrorDlg::get_Instance();
    m_radius = md.m_clearAperature/2.;
    QSettings set;
    ui->minvalue->blockSignals(true);
    ui->maxvalue->blockSignals(true);
    ui->minvalue->setValue(set.value("percent_correction_min",-10).toDouble());
    ui->maxvalue->setValue(set.value("percent_correction_max", 120).toDouble());
    ui->minvalue->blockSignals(false);
    ui->maxvalue->blockSignals(false);


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
    ui->zoneTable->clearContents();
    ui->zoneTable->setRowCount(zoneCenter.size());
    ui->zoneTable->blockSignals(true);
    for (int i = 0; i < zoneCenter.size(); ++i) {
        QTableWidgetItem *item = new QTableWidgetItem(QString::number(zoneCenter[i],'f',0));
        ui->zoneTable->setItem(i, 0, item);
        item->setTextAlignment(Qt::AlignRight);
    }
    ui->zoneTable->blockSignals(false);
}
QJsonDocument percentCorrectionDlg::loadZonesFromJson(QString str){
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(str.toUtf8(), &jsonError);
    QJsonObject jsonData=jsonDoc.object();
    QJsonArray zones = jsonData["zones"].toArray();


    zoneCenter.clear();
    ui->zoneTable->clearContents();
    ui->zoneTable->setRowCount(zones.size());

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
/*
 *     // creaate rowvec or rho and theta.  Theta will only be 0.
    std::vector<double> rhovec, thetavec;

    for (double r = 0; r < m_radius; ++r){
        double rho = r/m_radius;
        rhovec.push_back(rho);
        thetavec.push_back(0.);
    }
    arma::rowvec rhov(rhovec), thetav(thetavec);
    zernikeProcess zp;

    arma::mat theZs = zp.zpmC(rhov, thetav, maxorder);


        double rho = rhov(10);
        double num1 = rho;

        double num2 = num1 * num1;
        double num3 = num2 * num2;
        double num4 = num3 * num2;
        double num5 = num3 * num3;
        double num6 = num5 * num2;

        double z8 = (1.0 - 6.0 * num2 + 6.0 * num3);
        double z15 =(-1.0 + 12.0 * num2 - 30.0 * num3 + 20.0 * num4);
        double z24 = (1.0 - 20.0 * num2 + 90.0 * num3 - 140.0 * num4 + 70.0 * num5);


        qDebug() << "rho" << rho << z8 << z15 << z24;
        qDebug() << "rho" << rho << "theZs8" << theZs(10,8) << theZs(10,15) << theZs(10,24);
*/
int maxorder = 18;
double percentCorrectionDlg::getZernSurface( double RoC, double MirrorRad, std::vector<double> Zernikes, double x){

         double num1 = x / MirrorRad;



         arma::rowvec rhov(1), thetav(1);
         rhov[0] = x/m_radius;  thetav[0] = 0.;
         zernikeProcess zp;
         zp.setMaxOrder(m_maxOrder);

         arma::mat theZs = zp.zpmC(rhov, thetav, m_maxOrder);


         double val = 0;
         // for each spherical term
         int z = 8;
         for(unsigned int j = 6; z < theZs.n_cols; j+=2){
            val += Zernikes[z] * theZs(0,z);
            z = j * j /4 + j;
         }

//         double num2 = num1 * num1;
//         double num3 = num2 * num2;
//         double num4 = num3 * num2;
//         double num5 = num3 * num3;
//         double num6 = num5 * num2;


//         double num7 = 0.0 + Zernikes[8] * (1.0 - 6.0 * num2 + 6.0 * num3) +
//                 Zernikes[15] * (-1.0 + 12.0 * num2 - 30.0 * num3 + 20.0 * num4) +
//                 Zernikes[24] * (1.0 - 20.0 * num2 + 90.0 * num3 - 140.0 * num4 + 70.0 * num5) +
//                 Zernikes[35] * (-1.0 + 30.0 * num2 - 210.0 * num3 + 560.0 * num4 - 630.0 * num5 + 252.0 * num6);


//         qDebug() << "rho" << num1 << val << num7;
         double spherey = RoC - sqrt(pow(RoC, 2.0) - pow(x, 2.0));
         double zerny = val * m_lambda_nm * .5E-6;
         double surf =   spherey + zerny;
         return surf;
}

// will use zernike values to compute two surface points x+- .01x  away from x
//      then compute normal slope from that.
double percentCorrectionDlg::getnormalSlope(double RoC, double radius, std::vector<double> Zernikes, double x){

    double num1 = x / 100.0;
    double surface1 = getZernSurface(RoC, radius, Zernikes, x - num1);  // problem  with zonendx  (the delta is not same as zonendx)
    double surface2 = getZernSurface(RoC, radius, Zernikes, x + num1);
    double slope =  (surface2 - surface1)/ (2 * num1);
    slope = -1.0 / slope;

    return slope;
}
// b == knife y value
// y = mx + b
// b = y - mx

double percentCorrectionDlg::GetActualKE(double RoC, double MirrorRad, std::vector<double> Zernikes, double x)
{

    double slope = getnormalSlope(RoC, MirrorRad, Zernikes, x);
    double surface = getZernSurface(RoC, MirrorRad, Zernikes, x);

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
        double zernKnife = GetActualKE(m_roc, m_radius, surf->zernvalues, x);

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

    // for each surface draw the percent plot
    for (int i = 0; i < surfs.length(); ++ i) {
        // make percentages
        QPolygonF percent = makePercentages( surfs[i]);

        QPolygonF bars;

         if (surfs.length() < 2) {
        // draw zone rectangles
            if (ui->showBars->isChecked()){
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
        ui->plot->setAxisTitle( ui->plot->xBottom, "Radius mm" );
        slopeCurve3->setZ(1);
        ui->plot->setAxisScale(QwtPlot::yLeft, ui->minvalue->value(), ui->maxvalue->value());
        ui->plot->setAxisScale(QwtPlot::xBottom, 0, m_radius);
}
    ui->plot->replot();
}



void percentCorrectionDlg::setData( QVector< surfaceData *> data) {

    mirrorDlg &md = *mirrorDlg::get_Instance();
    m_roc = md.roc;
    m_lambda_nm = md.lambda;
    m_radius = md.m_clearAperature/2.;
    surfs = data;

    // create surface samples every 1mm.
    // THen change the zone logic to match.

    //it is possible that the zones have yet to be set go read them from the settings.
    // it will make them if needed.
    makeZones();
    plot();
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
        //qDebug() << "Failed to open file";
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






void percentCorrectionDlg::on_zoneTable_itemChanged(QTableWidgetItem *item)
{
    qDebug() << "item changed" << item->row() << item->text();
    zoneCenter[item->row()] = item->text().toDouble();

    saveSettings();
    plot();
}




void percentCorrectionDlg::on_showBars_clicked(bool checked)
{
    plot();
}


void percentCorrectionDlg::on_Generate_clicked()
{
    emit make_percent_correction(m_maxOrder);
}


void percentCorrectionDlg::on_maxOrder_valueChanged(int arg1)
{
    m_maxOrder = arg1;
    qDebug() << "maxorder" << arg1;
    int mmax = arg1/2;
    int ncol = (mmax+1)*(mmax+1);
    ui->noOfTerms->setText(QString::number(ncol));
}

