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
#ifndef PREFSDLG_H
#define PREFSDLG_H

#include <QDialog>

namespace Ui {
class prefsDlg;
}

class prefsDlg : public QDialog
{
    Q_OBJECT
signals:
    void saveGeometry();

public:
    explicit prefsDlg(QWidget *parent = 0);
    ~prefsDlg();

private slots:
    void on_pushButton_clicked();

private:
    Ui::prefsDlg *ui;
};

#endif // PREFSDLG_H
