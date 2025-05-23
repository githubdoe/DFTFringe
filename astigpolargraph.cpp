#include "astigpolargraph.h"
#include "ui_astigpolargraph.h"
#include "surfacemanager.h"


astigPolargraph::astigPolargraph(    QList<astigSample>list, QWidget *parent) :

        QDialog(parent),ui(new Ui::astigPolargraph)
{
    ui->setupUi(this);

    chart = new QPolarChart();

    // process each wave front and place astig on the chart
    ui->waveFrontTable->setRowCount(list.size());
    ui->waveFrontTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Configure selection mode (e.g., single selection)
    ui->waveFrontTable->setSelectionMode(QAbstractItemView::SingleSelection);
    QValueAxis *angularAxis = new QValueAxis();
    angularAxis->setTickCount(9); // First and last ticks are co-located on 0/360 angle.
    angularAxis->setLabelFormat("%.0f");
    angularAxis->setShadesVisible(true);
    angularAxis->setShadesBrush(QBrush(QColor(249, 249, 255)));
    chart->addAxis(angularAxis, QPolarChart::PolarOrientationAngular);

    QValueAxis *radialAxis = new QValueAxis();
    radialAxis->setTickCount(5);
    radialAxis->setLabelFormat("%.1f");
    chart->addAxis(radialAxis, QPolarChart::PolarOrientationRadial);
    double maxAstig = 1.;

    QVector<wavefront *>  wavefronts =SurfaceManager::get_instance()->m_wavefronts;
    QScreen *screen = QGuiApplication::primaryScreen();
    qreal screenDPI = screen->physicalDotsPerInchX();
    int pensize = 5 * screenDPI/256.;  // adjust pen size to screen resolution.  256 is DPI of my 4K 17 inch laptop
    if (pensize < 1)
        pensize = 1;
    for(int ndx = 0; ndx < list.length(); ++ndx){

        QString name = list[ndx].m_name;
        int slashndx = name.lastIndexOf('/');
        QString shortName = name.mid(name.lastIndexOf('/',slashndx-1));
        QTableWidgetItem *item = new QTableWidgetItem(shortName, 0);
        ui->waveFrontTable->setItem(ndx,0,item);
        double xastig = list[ndx].m_xastig;
        double yastig = list[ndx].m_yastig;
        double mag = sqrt(xastig * xastig + yastig * yastig);
        if (mag > maxAstig) maxAstig = mag;

        double angle = (atan2(yastig,xastig)/2.) * 180./M_PI;

        angle = 90 - angle;

        astigSample sample(name, angle, mag);

        m_list << sample;
        QScatterSeries *series = new QScatterSeries();

        int lastndx = name.lastIndexOf('/');
        if (lastndx != -1)
            name = name.mid(lastndx);
        series->setName(name);
        series->append(angle,mag);
        series->append(angle+180,mag);
        chart->addSeries(series);
        series->attachAxis(radialAxis);
        series->attachAxis(angularAxis);

        QLineSeries *line = new QLineSeries();
        line->append(angle,mag);
        line->append(angle+180,mag);
        chart->addSeries(line);
        line->attachAxis(radialAxis);
        line->attachAxis(angularAxis);
        line->setName(name);
            connect(line, &QLineSeries::hovered, this, &astigPolargraph::tooltip);
            connect(line, &QLineSeries::clicked, this, &astigPolargraph::clicked);
        chart->legend()->markers(line)[0]->setVisible(false);

        line->setPen(QPen(series->brush(),pensize));

        QTableWidgetItem *pv = new QTableWidgetItem(QString().number(mag), 0);
        item->setForeground(series->brush());
        ui->waveFrontTable->setItem(ndx, 1, pv);
        QTableWidgetItem *anglewidget = new QTableWidgetItem(QString().number(angle,'f',1), 0);
        ui->waveFrontTable->setItem(ndx, 2, anglewidget);
    }

    chart->setTitle("Magnitude and axis of high edge");
    if (list.length() > 4)
        chart->legend()->setAlignment(Qt::AlignRight);
    else chart->legend()->setAlignment(Qt::AlignBottom);

    maxAstig = ceil(maxAstig);
    radialAxis->setRange(0, maxAstig);
    angularAxis->setRange(0, 360);
    ui->waveFrontTable->resizeColumnsToContents();
    ui->polarChart->setChart(chart);

}
void astigPolargraph::tooltip(QPointF point, bool state)
{
    if (state) {
        findClosestPoint(point);
    }

}
void astigPolargraph::clicked(QPointF point)
{

   findClosestPoint(point);

}
astigPolargraph::~astigPolargraph()
{
    delete ui;
    delete chart;
}

void astigPolargraph::hideHoverHelp(){
    ui->hoverText->hide();
}
int astigPolargraph::findClosestPoint(const QPointF clickedPoint){

    QPointF closest(INT_MAX, INT_MAX);
    qreal distance(INT_MAX);
    int closeNdx = -1;
    int ndx = 0;
    for (auto sample : m_list) {
        QPointF currentPoint(sample.m_xastig, sample.m_yastig);
        qreal currentDistance = qSqrt((currentPoint.x() - clickedPoint.x())
                                      * (currentPoint.x() - clickedPoint.x())
                                      + (currentPoint.y() - clickedPoint.y())
                                      * (currentPoint.y() - clickedPoint.y()));

        if (currentDistance < distance) {
            distance = currentDistance;
            closest = currentPoint;
            closeNdx = ndx;

        }
        ++ndx;
    }
    QString name = m_list[closeNdx].m_name;
    emit waveSeleted(name);
    int slashndx = name.lastIndexOf('/');
    QString shortName = name.mid(name.lastIndexOf('/',slashndx-1));

    QList<QTableWidgetItem*> items = ui->waveFrontTable->findItems(shortName, Qt::MatchEndsWith);


    if (items.length() > 0){

        ui->waveFrontTable->selectRow(items[0]->row());
    }
    return closeNdx;
}

void astigPolargraph::on_waveFrontTable_itemClicked(QTableWidgetItem *item)
{
    QString name = ui->waveFrontTable->item(item->row(),0)->text();
    emit waveSeleted(name);

    int lastndx = name.lastIndexOf('/');
    if (lastndx != -1)
        name = name.mid(lastndx);


    int seriesCount = chart->series().count();

    for (int i = 0; i < seriesCount; ++i) {
      QAbstractSeries* series = chart->series().at(i);
      if (series) {

           if (series->type()== QAbstractSeries::SeriesTypeLine){
              if (series->name() == name){
                  QLineSeries *line = static_cast<QLineSeries*>(series);
                  // Create a pen object to get the current pen attributes.
                  QPen pen = line->pen();
                  if (pen.style() == Qt::DotLine){
                      pen.setStyle(Qt::SolidLine);
                      pen.setWidth(pen.width()/4);
                  }
                  else {
                    pen.setStyle(Qt::DotLine);
                    pen.setWidth(pen.width() * 4);
                  }

                  line->setPen(pen);
              }
           }

      }
    }
}

