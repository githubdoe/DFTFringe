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

#define CV_INTER_NN cv::INTER_NEAREST
#define CV_INTER_LINEAR cv::INTER_LINEAR
#define CV_INTER_LANCZOS4 cv::INTER_LANCZOS4
#define CV_INTER_CUBIC cv::INTER_CUBIC
#define CV_INTER_AREA cv::INTER_AREA

RotationDlg::RotationDlg( QList<int> list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotationDlg), list(list)
{
    ui->setupUi(this);
    int interpModes[] = {CV_INTER_NN, CV_INTER_LINEAR, CV_INTER_LANCZOS4, CV_INTER_CUBIC, CV_INTER_AREA};
    QString names[] = {"Nearest Neighbor", "Linear", "Lanczos","Cubic","Area"};
    for (int i = 0; i < 5; ++i){
        ui->interp->addItem(names[i],interpModes[i]);
    }
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
    int interp = ui->interp->currentData().toInt();
    QSettings set;
    set.setValue("rotationAngle" ,ui->angleSB->value());
    set.setValue("rotationSign", sign);
    emit rotateTheseSig(sign * ui->angleSB->value(), list);
}
