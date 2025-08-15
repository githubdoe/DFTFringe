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
#ifndef COLORMAPVIEWERDLG_H
#define COLORMAPVIEWERDLG_H

#include <QDialog>

#include <QListWidgetItem>
namespace Ui {
class colorMapViewerDlg;
}

class colorMapViewerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit colorMapViewerDlg( QWidget *parent = 0);
    ~colorMapViewerDlg();
    QString m_selection;

void genList(const QString &path);
private slots:
    void on_browsePb_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();


    void on_aboutpb_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    Ui::colorMapViewerDlg *ui;
};

#endif // COLORMAPVIEWERDLG_H
