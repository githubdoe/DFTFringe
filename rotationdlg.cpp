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
#include "rotationdlg.h"
#include "ui_rotationdlg.h"
#include <opencv2/opencv.hpp>
#include <qsettings.h>

RotationDlg::RotationDlg( const QList<int> &list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotationDlg), list(list)
{
    ui->setupUi(this);
    QSettings set;
    ui->angleSB->setValue(set.value("rotationAngle", 90).toDouble());
    ui->CCWCB->setChecked(set.value("rotationSign", 1).toInt() == -1);
}

RotationDlg::~RotationDlg()
{
    delete ui;
}

void RotationDlg::on_buttonBox_accepted()
{
    int sign = (ui->CCWCB->isChecked()) ?  -1:1;
    QSettings set;
    set.setValue("rotationAngle" ,ui->angleSB->value());
    set.setValue("rotationSign", sign);
    emit rotateTheseSig(sign * ui->angleSB->value(), list);
}
