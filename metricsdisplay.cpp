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
#include "metricsdisplay.h"
#include "ui_metricsdisplay.h"
#include <vector>
#include "mainwindow.h"
#include "zernikeprocess.h"

metricsDisplay *metricsDisplay::m_instance = 0;
metricsDisplay::metricsDisplay(QWidget *parent) :
    QDockWidget(parent),shouldEnableAll(false),
    ui(new Ui::metricsDisplay)
{
    ui->setupUi(this);
    mDiam = ui->metDiam;
    mROC = ui->metROC;
    mRMS = ui->metRMS;
    mStrehl = ui->metStrehl;

    mCC = ui->metConic;
    tableModel = new ZernTableModel(ui->tableView, &zernEnables);

    ui->tableView->setModel(tableModel);
}
void metricsDisplay::resizeRows(int cnt){
    tableModel->resizeRows(cnt);



    qDebug() << "resize metric zern table" << cnt;
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
void metricsDisplay::setWavePerFringe(double val, double lambda){
    ui->wavesPerFringe->setText(QString("Waves Per Fringe: %1").arg(val, 2, 'f', 1));
    ui->lambda->setText(QString("Igram laser wavelength: %1 nm").arg(lambda, 6, 'f', 2));
    mirrorDlg *md = mirrorDlg::get_Instance();
    QString donull = (md->doNull) ? (QString("SANull: %1").arg(md->z8 * md->cc, 6, 'f', 4)) : "";
    ui->desiredConicLb->setText(QString("Desired Conic: %1 ").arg( md->cc, 6, 'f', 2) + donull);
    if (md->isEllipse()){
        ui->desiredConicLb->setText("");
        ui->zernTitle->setText("Zernike Values not computed for Flats");
    }
}
void metricsDisplay::setOutputLambda(double val){
    ui->label_5->setText(QString("Wavefront RMS at %1 nm").arg(val, 6, 'f', 1));
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
    int start = 0;
    if (shouldEnableAll)
        start = 8;

    for (unsigned int i = start; i < tableModel->m_enables->size(); ++i)
        tableModel->m_enables->at(i) = shouldEnableAll;

    tableModel->m_enables->at(4) = shouldEnableAll;
    tableModel->m_enables->at(5) = shouldEnableAll;
    shouldEnableAll = !shouldEnableAll;
    ui->DisableAll->setText((shouldEnableAll) ? "Enable All":"Disable All");
    emit recomputeZerns();
}



void metricsDisplay::on_sphericalPb_pressed()
{
    shouldEnableAll = false;
    on_DisableAll_clicked();
    for (int i = 0; i < tableModel->rowCount(); ++i){
        QString txt = tableModel->data(tableModel->index(i,0)).toString();
        if (txt.contains("Spherical")){
            tableModel->m_enables->at(i) = true;
        }
    }
    emit recomputeZerns();
}
void metricsDisplay::setZernTitle(const QString &title){
    ui->zernTitle->setText(title);
}
