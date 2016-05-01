#include "simigramdlg.h"
#include "ui_simigramdlg.h"
#include <QSettings>
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
