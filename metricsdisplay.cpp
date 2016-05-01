#include "metricsdisplay.h"
#include "ui_metricsdisplay.h"
#include <vector>
#include "mainwindow.h"
#include "zernikeprocess.h"
metricsDisplay *metricsDisplay::m_instance = 0;
metricsDisplay::metricsDisplay(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::metricsDisplay)
{
    ui->setupUi(this);
    mDiam = ui->metDiam;
    mROC = ui->metROC;
    mRMS = ui->metRMS;
    mStrehl = ui->metStrehl;
    mvalues = ui->tableView;
    mCC = ui->metConic;
    tableModel = new ZernTableModel(parent);
    std::vector<double> val(Z_TERMS,0.);
    ui->tableView->setModel(tableModel);
}

QTableView * metricsDisplay::getZernView(){
    return ui->tableView;
}

metricsDisplay *metricsDisplay::get_instance(QWidget *parent){
    if (m_instance == 0){
        m_instance = new metricsDisplay(parent);
    }
    return m_instance;
}
void metricsDisplay::setWavePerFringe(double val){
    ui->wavesPerFringe->setText(QString().sprintf("Waves Per Fringe: %2.1lf",val));
}

void metricsDisplay::setName(QString name){
    QStringList list = name.split("/");
    int size = list.size();
    if (list.size() > 2){
        name = list[size -2] + "/ " + list[size-1];
    }
    ui->wftName->setText(name);
}

metricsDisplay::~metricsDisplay()
{
    delete ui;

}

void metricsDisplay::on_recomputePB_clicked()
{
    emit recomputeZerns();
}

void metricsDisplay::on_DisableAll_clicked()
{

    for (unsigned int i = 0; i < zernEnables.size(); ++i)
        zernEnables[i] = false;

    emit recomputeZerns();
}
