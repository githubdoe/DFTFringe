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
#include "zernikedlg.h"
#include "ui_zernikedlg.h"
#include <vector>
#include <QStandardItem>
#include <QDebug>
#include "zernikes.h"
#include "mainwindow.h"
#include "zernikeprocess.h"
#include "surfaceanalysistools.h"
#include "settings2.h"
#include <vector>
/////////////////////////////////////////////////////////////////////////////
// Zernike_terms message handlers

ZernTableModel::ZernTableModel(QObject *parent, std::vector<bool> *enables, bool editEnable)
    :QAbstractTableModel(parent),  m_enables(enables),canEdit(editEnable), m_nulled(false)
{
    zernikeProcess &zp = *zernikeProcess::get_Instance();
    values = std::vector<double>(zp.m_norms.size(), 0.);

}


void ZernTableModel::setValues(const std::vector<double> &vals, bool nulled){
    m_nulled = nulled;
    values = vals;
    m_enables->resize(vals.size(),true);
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    update();
    emit dataChanged(topLeft, bottomRight);
}

void ZernTableModel::update(){
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    emit dataChanged(topLeft, bottomRight);

}

void ZernTableModel::resizeRows(const int rowCnt){
    int oldrowcnt = rowCount();
    emit layoutAboutToBeChanged();
    values.resize(rowCnt);
    m_enables->resize(rowCnt);

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
int ZernTableModel::rowCount(const QModelIndex & /*parent*/) const
{
   return values.size();
}

int ZernTableModel::columnCount(const QModelIndex & /*parent*/) const
{
    return 2;
}
QVariant ZernTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Zernike Term");
            case 1:
                return QString("Wyant     RMS ");
            case 2:
                return QString("third");
            }
        }
    }
    return QVariant();
}
QVariant ZernTableModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (index.column() == 0){
            if (index.row() <= 48)
                return zernsNames[index.row()];
            else {
                int row = index.row();
                int sr = floor(sqrt(row+1));

                return (QString("%1 %2").arg(index.row()).arg(
                      ( sr * sr == index.row()+1)? "Spherical" : ""));
            }
        }
        if (index.column() == 1){
            if (index.row() == 3 && surfaceAnalysisTools::get_Instance()->m_useDefocus){
                return QString("%1").arg(surfaceAnalysisTools::get_Instance()->m_defocus, 6, 'f', 3);
            }

            mirrorDlg &md = *mirrorDlg::get_Instance();
            if (index.row() == 8 && md.doNull && !m_nulled){
                double val = values[8] - md.z8 * md.cc;
                return QString("%1  %2").arg(val, 6, 'f', 3).arg( computeRMS(8, val), 6, 'f', 3);
            }

            return QString("%1  %2").arg(values[index.row()], 6, 'f', 3).arg(computeRMS(index.row(), values[index.row()]), 6, 'f', 3);
        }
    }
    if (role == Qt::CheckStateRole){

        if (index.column() == 1){
            if (index.row() == 3 && surfaceAnalysisTools::get_Instance()->m_useDefocus){
                return Qt::PartiallyChecked;
            }

            return m_enables->at(index.row()) ? Qt::Checked : Qt::Unchecked;
        }
    }
    return QVariant();
}
bool ZernTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    if (role == Qt::EditRole)
    {
        //save value from editor
        if (index.column() == 1)
            values[index.row()]  = value.toDouble();
        if (index.column() == 0)
            m_enables->at(index.row()) = value.toBool();
    }
    if (role == Qt::CheckStateRole){
        m_enables->at(index.row()) = value.toBool();
    }
    return true;
}

Qt::ItemFlags ZernTableModel::flags(const QModelIndex & index) const
{
    if (index.column() == 0)
        return Qt::NoItemFlags;
    if (index.column() == 1){
        if (canEdit)
            return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsEditable;
        else
            return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;
    }
    return Qt::NoItemFlags;
}

