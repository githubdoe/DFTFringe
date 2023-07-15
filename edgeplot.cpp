#include "edgeplot.h"
#include "ui_edgeplot.h"
#include <cmath>
#include <QSettings>
EdgePlot::EdgePlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EdgePlot)
{
    ui->setupUi(this);
    QSettings s;

    m_mag = s.value("edgeMag", .5).toDouble();
    m_percent = s.value("edgePercent", .85).toDouble();
    m_sharp =  s.value("edgeSharp",3).toDouble();

    m_chart = new QChart;
    ui->chart->setChart(m_chart);


    m_chart->setTitle("percent edge");
    m_breakPoint = m_percent * 100;
    m_data = new QLineSeries();
    m_chart->legend()->hide();
    m_marker = new QLineSeries();
    m_percent = ui->percentRho->value()/100.;
    ui->percentRho->blockSignals(true);
    ui->power->blockSignals(true);
    ui->height->blockSignals(true);
    ui->percentRho->setValue(m_percent * 100);
    ui->height->setValue(m_mag * 10);
    ui->power->setValue(m_sharp * 100);
    ui->percentRho->blockSignals(false);
    ui->power->blockSignals(false);
    ui->height->blockSignals(false);
    Update();
}

EdgePlot::~EdgePlot()
{
    delete ui;
}
void EdgePlot::Update(){
    m_chart->removeSeries(m_data);
    m_chart->removeSeries(m_marker);
    m_data->clear();
    m_data->append(0,0);
    m_data->append(m_breakPoint,0);
    int e = 100 - m_breakPoint;

    m_sharp = 1 + ui->power->value()/10.;
    for (double i = m_breakPoint; i <= 100; i += .1){
        double p = ((double)i-m_breakPoint)/e;

        double v = pow(p,m_sharp);
        double k = -v * m_mag;

        m_data->append(i, k);
    }
    m_marker->clear();
    m_marker->append(m_breakPoint, 0);
    m_marker->append(m_breakPoint, -5);
    m_chart->addSeries(m_marker);
    m_chart->addSeries(m_data);

    m_chart->createDefaultAxes();

    m_chart->axisX()->setRange(-1,105);

    m_chart->axisY()->setRange(-10,10);


    m_chart->axisX()->setTitleText("start edge at percent of radius of mirror");
    m_chart->axisY()->setTitleText("waves");
}
void EdgePlot::on_height_valueChanged(int value)
{
    m_mag = (double)value/10.;
    ui->magvalue->setText(QString::number(m_mag));

    Update();
}


void EdgePlot::on_percentRho_valueChanged(int value)
{
    m_breakPoint = value;
    m_percent = (double)value/100.;


    Update();
}


void EdgePlot::on_power_valueChanged(int /*value*/)
{
    Update();
}




