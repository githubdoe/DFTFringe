/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "dfttools.h"
#include "ui_dfttools.h"
#include "vortex.h"
#include <QSettings>
#include "settings2.h"
DFTTools::DFTTools(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DFTTools)
{
    ui->setupUi(this);

    QSettings set;
    ui->dftSizeSB->setValue(set.value("DFTSize", 640).toInt());

}
void DFTTools::connectTo(QWidget *view){
    connect(view, SIGNAL(updateFilterSize(int)),this, SLOT(setCenterFilterValue(int)));
    connect(ui->gammaSl, SIGNAL(valueChanged(int)), view, SLOT(gamma(int)));
}
DFTTools::~DFTTools()
{
    delete ui;
}
void DFTTools::imageSize(QString txt){
    ui->imageSize->setText(txt);
}





void DFTTools::setCenterFilterValue(int v){
    ui->filterAdjuster->setValue(v);
}

void DFTTools::on_filterAdjuster_valueChanged(double arg1)
{
    emit dftCenterFilter(arg1);
}
#include <QDebug>
// compute surface
void DFTTools::on_pushButton_clicked()
{
    wasPressed = true;
    emit makeSurface();

}

void DFTTools::on_computeDFT_pressed()
{
    emit doDFT();
}


void DFTTools::setDFTSize(int val){
    ui->dftSizeSB->setValue(val);
}

void DFTTools::on_dftSizeSB_valueChanged(int value)
{    
    QSettings set;
    set.setValue("DFTSize", value);
}

void DFTTools::on_showResized_pressed()
{
    emit showResized();
}
