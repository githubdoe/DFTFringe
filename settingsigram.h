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
#ifndef SETTINGSIGRAM_H
#define SETTINGSIGRAM_H

#include <QDialog>
#include "lensetablemodel.h"
#include <QItemSelectionModel>

namespace Ui {
class settingsIGram;
}

class settingsIGram : public QDialog
{
    Q_OBJECT

public:
    explicit settingsIGram(QWidget *parent = 0);
    ~settingsIGram();
    bool m_removeDistortion;
    QStringList m_lenseParms;
    void updateLenses(QString str);
signals:
    void igramLinesChanged(int,int,QColor,QColor, double, int, int);
private slots:
    void on_edgeLineColorPb_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_centerSpinBox_valueChanged(int arg1);

    void on_centerLineColorPb_clicked();

    void on_buttonBox_accepted();


    void on_removeDistortion_clicked(bool checked);

    void eraseItem();

    void showContextMenu(const QPoint &pos);


    void on_lenseTableView_clicked(const QModelIndex &index);

private:

    Ui::settingsIGram *ui;
    QColor edgeColor;
    QColor centerColor;
    int edgeWidth;
    int centerWidth;
    LenseTableModel *lensesModel;
    QVector<QStringList> m_lensData;
    QModelIndex currentNdx;
    void saveLensData();
    QItemSelectionModel *selectionModel;
};

#endif // SETTINGSIGRAM_H
