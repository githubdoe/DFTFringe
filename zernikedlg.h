#ifndef ZERNIKEDLG_H
#define ZERNIKEDLG_H

#include <QDialog>
#include <QtCore>
#include <QtGui>
#include "zernikedlg.h"
#include <QAbstractTableModel>
#include <opencv/cv.h>
#include <vector>
#define Z_TERMS 34

namespace Ui {
class zernikeDlg;
}


class ZernTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    ZernTableModel(QObject *parent);
    int rowCount(const QModelIndex &parent = QModelIndex()) const ;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setValues(std::vector<double> vals);
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex & index) const ;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    std::vector<double> values;

signals:
    void zernChanged();
};



#endif // ZERNIKEDLG_H
