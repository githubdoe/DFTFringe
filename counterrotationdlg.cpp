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
#include "counterrotationdlg.h"
#include "ui_counterrotationdlg.h"

CounterRotationDlg::CounterRotationDlg(QString fn, double rot, bool dir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CounterRotationDlg)
{
    ui->setupUi(this);
    ui->fileName->setText(fn);
    ui->counterRotation->setText(QString("%1").arg(rot, 6, 'f', 1));
    ui->CW->setChecked(dir);
}
QString CounterRotationDlg::getRotation(){
    return ui->counterRotation->text();
}
bool CounterRotationDlg::isClockwise(){
    return ui->CW->isChecked();

}
void CounterRotationDlg::setCCW(bool flag){
    ui->CCW->setChecked(flag);
}

CounterRotationDlg::~CounterRotationDlg()
{
    delete ui;
}
