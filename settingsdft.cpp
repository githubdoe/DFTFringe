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
#include "settingsdft.h"
#include "ui_settingsdft.h"
#include <QSettings>
settingsDFT::settingsDFT(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDFT)
{
    ui->setupUi(this);
    QSettings set;
    ui->ShowDFTTHumbCB->setChecked( set.value("DFTshowThumb", false).toBool());
    flipv = set.value("DFT Flip Vertical", false).toBool();
    ui->flipVert->setChecked(flipv);
    fliph = set.value("DFT Flip Horizontal", false).toBool();
    ui->flipHorizontal->setChecked(fliph);
}

settingsDFT::~settingsDFT()
{
    delete ui;
}

bool settingsDFT::showThumb(){
    return ui->ShowDFTTHumbCB->isChecked();
}

void settingsDFT::on_ShowDFTTHumbCB_clicked(bool)
{
    QSettings set;
    set.setValue("DFTshowThumb", ui->ShowDFTTHumbCB->isChecked());
}



void settingsDFT::on_showDownSizedIgram_clicked(bool checked)
{
    QSettings set;
    set.setValue("DFTshowDownSizedIgram", checked);
}


void settingsDFT::on_flipVert_clicked(bool checked)
{
    QSettings set;
    flipv = checked;
    set.setValue("DFT Flip Vertical", checked);
}

void settingsDFT::on_flipHorizontal_clicked(bool checked)
{
    QSettings set;
    fliph = checked;
    set.setValue("DFT Flip Horizontal", checked);
}
