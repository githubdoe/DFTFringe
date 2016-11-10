#include "lensetablemodel.h"
#include <QDebug>
#include <QBrush>
LenseTableModel::LenseTableModel(QObject *parent)
    : QAbstractTableModel(parent), m_current(0)
{
}


QVariant LenseTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            case 0:
                return QString("Lens");
            case 1:
                return QString("K1");
            case 2:
                return QString("K2");
            case 3:
                return QString("P1");
            case 4:
                return QString("p2");
            case 5:
                return QString("K3");
            case 6:
                return QString("FL pixels");
            case 7:
                return QString("x center");
            case 8:
                return QString("y center");


            }
        }
    }
    return QVariant();
}

bool LenseTableModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}


int LenseTableModel::rowCount(const QModelIndex &parent) const
{

    return m_data->size();
}

int LenseTableModel::columnCount(const QModelIndex &parent) const
{
    return 9;
}

QVariant LenseTableModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
        return QVariant();


    if (role == Qt::DisplayRole){

       return m_data->at(index.row())[index.column()];


    }

    return QVariant();
}
void LenseTableModel::setCurrentRow(int row){
    m_current = row;
    qDebug() << "current " << row;
}

bool LenseTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    qDebug() << index << value;
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags LenseTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable |  Qt::ItemIsEditable;

}

bool LenseTableModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}



bool LenseTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    m_data->removeAt(row);
    endRemoveRows();
}


void LenseTableModel::setLensData( QVector<QStringList> *data){
    beginResetModel();
    m_data = data;
    endResetModel();
}
