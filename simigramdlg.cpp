/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "simigramdlg.h"
#include "ui_simigramdlg.h"
#include <QSettings>
#include "mirrordlg.h"
#include <QDebug>
#include <QAbstractTableModel>
#include "surfacemanager.h"
#include "zernikeprocess.h"
#include "arbitrarywavefrontdlg.h"

zTableModel::zTableModel(QObject *parent, std::vector<bool> *enables, bool editEnable)
    :QAbstractTableModel(parent),  m_enables(enables),canEdit(editEnable)
{
    values = new std::vector<double>(Z_TERMS, 0.);
}

void zTableModel::resizeRows(const int rowCnt){
    int oldrowcnt = rowCount();
    emit layoutAboutToBeChanged();
    values->resize(rowCnt);


    if (rowCnt > oldrowcnt){
        insertRows(oldrowcnt, rowCnt - oldrowcnt);
    }
    else
    {
        removeRows(oldrowcnt -rowCnt, rowCnt);
    }
    emit layoutChanged();
    update();
}

void zTableModel::setValues(std::vector<double> *vals){
    int newRowCnt = vals->size();
    int oldRowCnt = values->size();
    emit layoutAboutToBeChanged();
    if (newRowCnt > oldRowCnt){
        insertRows(oldRowCnt, newRowCnt - oldRowCnt);
    }
    else
    {
        removeRows(oldRowCnt -newRowCnt, newRowCnt);
    }
    values = vals;
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    emit layoutChanged();
    emit dataChanged(topLeft, bottomRight);
    update();
}

void zTableModel::update(){
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);
}

int zTableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return values->size();
}

int zTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}
QVariant zTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Zernike Term");
            case 1:
                return QString("Wyant");
            case 2:
                return QString("third");
            }
        }
    }
    return QVariant();
}
QVariant zTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0){
            if (index.row() < Z_TERMS){
                return zernsNames[index.row()];
            }

            else {
                int row = index.row();
                int sr = floor(sqrt(row+1));

                return (QString().sprintf("%d %s", index.row(),
                      ( sr * sr == index.row()+1)? QString().sprintf("Spherical").toStdString().c_str(): ""));

            }
        }
        if (index.column() == 1){
            return QString().sprintf("%6.3lf",values->at(index.row()));
        }
    }

    return QVariant();
}
bool zTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        //save value from editor
        if (index.column() == 1)
            values->at(index.row())  = value.toDouble();

    }

    return true;
}

Qt::ItemFlags zTableModel::flags(const QModelIndex & index) const
{
    if (index.column() == 0)
        return 0;
    if (index.column() == 1){
        if (canEdit)
            return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsEditable;
        else
            return  Qt::ItemIsEnabled;
    }
    return 0;
}



simIgramDlg *simIgramDlg::m_instance = 0;
simIgramDlg::simIgramDlg(QWidget *parent) :
    QDialog(parent),
    m_doArbitrary(false),
    ui(new Ui::simIgramDlg)
{
    ui->setupUi(this);
    zernikeProcess &zp = *zernikeProcess::get_Instance();
    zernikes = std::vector<double>(zp.getNumberOfTerms(),0);
    enables = std::vector<bool>(zp.getNumberOfTerms());
    QSettings s;
    xtilt = s.value("simxtilt", 30).toDouble();
    zernikes[1] = xtilt;
    correction = s.value("simCorrection", 98.).toDouble();

    star = s.value("simStar", 0.).toDouble();
    ui->starPatternSb->setValue(star);
    ring = s.value("simRing",0.).toDouble();
    ui->ringPatterSb->setValue(ring);
    xastig = s.value("simXastig", 0.).toDouble();
    zernikes[4] = xastig;
    yastig = s.value("simYastig",0.).toDouble();
    zernikes[5] = yastig;
    defocus = s.value("simDefocus", 3.5).toDouble();
    zernikes[3] = defocus;
    ytilt = s.value("simYtilt", 0).toDouble();
    zernikes[2] = ytilt;
    size = s.value("simSize", 601).toDouble();
    ui->sizeSB->setValue(size);
    QString z8 = "Correction %";
    if (mirrorDlg::get_Instance()->cc == 0.){
        ui->correctionPb->setChecked(false);
    }

    doCorrection = true;
    tableModel = new zTableModel(this, &enables, true);
    tableModel->setValues(&zernikes);
    ui->tableView->setModel(tableModel);

}
simIgramDlg *simIgramDlg::get_instance(){
    if (m_instance == 0){
        m_instance = new simIgramDlg;
    }
    return m_instance;
}
void simIgramDlg::setNewTerms(std::vector<double> terms){

    zernikes = terms;
    tableModel->setValues(&zernikes);
    ui->correctionPb->setChecked(false);
    ui->Z8Pb->setChecked(true);
    doCorrection = false;
    update();
}
void simIgramDlg::showEvent(QShowEvent *){
    QString z8 = "Correction %";


}

simIgramDlg::~simIgramDlg()
{
    delete ui;
}

void simIgramDlg::on_buttonBox_accepted()
{
    QSettings s;
    xtilt = zernikes[1];
    s.setValue("simxtilt", xtilt);
    ytilt = zernikes[2];
    s.setValue("simYtilt",ytilt);
    defocus = zernikes[3];
    correction = zernikes[8];
    s.setValue("simCorrection",correction);
    xastig = zernikes[4];
    s.setValue("simXastig", xastig);
    yastig = zernikes[5];
    s.setValue("simYastig",yastig);
    star = ui->starPatternSb->value();
    s.setValue("simStar", star);
    ring = ui->ringPatterSb->value();
    s.setValue("simRing",ring);
    s.setValue("simDefocus",defocus);
    size = ui->sizeSB->value();
    s.setValue("simSize", size);
}

void simIgramDlg::on_importPb_clicked()
{
    SurfaceManager &sm = *SurfaceManager::get_instance();
    if (sm.m_wavefronts.size() == 0)
        return;
    zernikes = sm.getCurrent()->InputZerns;
    tableModel->setValues(&zernikes);
    ui->correctionPb->setChecked(false);
    ui->Z8Pb->setChecked(true);
    doCorrection = false;
    update();
}

void simIgramDlg::on_correctionPb_clicked(bool checked)
{
    doCorrection = checked;
}

void simIgramDlg::on_Z8Pb_clicked(bool checked)
{
    doCorrection = !checked;
}

void simIgramDlg::on_clearPiston_pressed()
{
    for (int i =0; i < 4; ++i){
        zernikes[i] = 0;

    }
    tableModel->setValues(&zernikes);
    update();
}

void simIgramDlg::on_clearAll_pressed()
{
    for (int i = 0; i < zernikes.size(); ++i){
        zernikes[i] = 0;
    }
    tableModel->setValues(&zernikes);
    update();
}



void simIgramDlg::on_editArbitrary_clicked()
{
    ArbitraryWavefrontDlg * dlg = ArbitraryWavefrontDlg::get_instance();
    mirrorDlg* md = mirrorDlg::get_Instance();
    if (md->diameter>0)
        dlg->setDiameter(md->diameter);
    dlg->setModal(true);
    dlg->exec();
    if (dlg->bOkPressed == false)
        return;
    m_doArbitrary=true;
    ui->includeArbitrary->setChecked(true);
}


void simIgramDlg::on_includeArbitrary_clicked(bool checked)
{
    m_doArbitrary = checked;
}

