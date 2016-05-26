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
#include "simigramdlg.h"
#include "ui_simigramdlg.h"
#include <QSettings>
simIgramDlg *simIgramDlg::m_instance = 0;
simIgramDlg::simIgramDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::simIgramDlg)
{
    ui->setupUi(this);
    QSettings s;
    xtilt = s.value("simxtilt", 30).toDouble();
    ui->xTiltSb->setValue(xtilt);
    correction = s.value("simCorrection", 98.).toDouble();
    ui->correctionSB->setValue(correction);
    star = s.value("simStar", 0.).toDouble();
    ui->starPatternSb->setValue(star);
    ring = s.value("simRing",0.).toDouble();
    ui->ringPatterSb->setValue(ring);
    xastig = s.value("simXastig", 0.).toDouble();
    ui->xAstigSb->setValue(xastig);
    yastig = s.value("simYastig",0.).toDouble();
    ui->yAstigSb->setValue(yastig);
    defocus = s.value("simDefocus", 3.5).toDouble();
    ui->defocusSb->setValue(defocus);
    ytilt = s.value("simYtilt", 0).toDouble();
    ui->yTiltSb->setValue(ytilt);
    size = s.value("simSize", 601).toDouble();
    ui->sizeSB->setValue(size);
}
simIgramDlg *simIgramDlg::get_instance(){
    if (m_instance == 0){
        m_instance = new simIgramDlg;
    }
    return m_instance;
}

simIgramDlg::~simIgramDlg()
{
    delete ui;
}

void simIgramDlg::on_buttonBox_accepted()
{
    QSettings s;
    xtilt = ui->xTiltSb->value();
    s.setValue("simxtilt", xtilt);
    ytilt = ui->yTiltSb->value();
    s.setValue("simYtilt",ytilt);
    defocus = ui->defocusSb->value();
    correction = ui->correctionSB->value();
    s.setValue("simCorrection",correction);
    xastig = ui->xAstigSb->value();
    s.setValue("simXastig", xastig);
    yastig = ui->yAstigSb->value();
    s.setValue("simYastig",yastig);
    star = ui->starPatternSb->value();
    s.setValue("simStar", star);
    ring = ui->ringPatterSb->value();
    s.setValue("simRing",ring);
    s.setValue("simDefocus",defocus);
    zernNdx = ui->zernikeSb->value();
    zernValue = ui->zernValue->value();

    noise = ui->noiseSb->value();
    size = ui->sizeSB->value();
    s.setValue("simSize", size);
}
