#include "defocusdlg.h"
#include "ui_defocusdlg.h"
#include <QMessageBox>
#include <QSettings>
defocusDlg::defocusDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::defocusDlg)
{
    ui->setupUi(this);
    QSettings set;

     if (set.contains("defocus dialogGeometry")) {
        setGeometry(set.value("defocus dialogGeometry").toRect());

    }
}

defocusDlg::~defocusDlg()
{
    delete ui;
}




void defocusDlg::on_multiplier_valueChanged(int arg1)
{
    double s = ui->defocusSlider->value()/100.;
    ui->defocusVal->blockSignals(true);
    ui->defocusVal->setValue( arg1 * s);
    ui->defocusVal->blockSignals(false);
    emit defocus(arg1 * s);
}

#include "math.h"
void defocusDlg::on_defocusVal_valueChanged(double arg1)
{
    if (ui->multiplier->value() * ui->defocusSlider->value()/100. != arg1) {
        int mul = fabs(arg1/.5);
        double slid = arg1/mul;

        ui->multiplier->blockSignals(true);
        ui->spinValue->blockSignals(true);
        ui->spinValue->setValue(slid);
        ui->multiplier->setValue(mul);
        ui->multiplier->blockSignals(false);
        ui->spinValue->blockSignals(false);

    }
    else {
        ui->spinValue->blockSignals(true);
        ui->spinValue->setValue(arg1);

        ui->multiplier->blockSignals(true);
        ui->multiplier->setValue(1.);
        ui->multiplier->blockSignals(false);
        ui->spinValue->blockSignals(false);
    }

    emit defocus(arg1);
}


void defocusDlg::on_spinValue_valueChanged(double arg1)
{
    ui->defocusSlider->blockSignals(true);
    ui->defocusSlider->setValue(arg1 * 100);
    double val = ui->multiplier->value() * arg1;
    ui->defocusVal->blockSignals(true);
    ui->defocusVal->setValue(val);
    ui->defocusVal->blockSignals(false);
    ui->defocusSlider->blockSignals(false);
    emit defocus(val);
}
#include "mirrordlg.h"
#include <QDebug>
void defocusDlg::on_defocusSlider_valueChanged(int value)
{
    double val = value/100.;
    ui->spinValue->blockSignals(true);
    ui->spinValue->setValue(val);
    ui->spinValue->blockSignals(false);
    val *= ui->multiplier->value();
    ui->defocusVal->blockSignals(true);
    val *= ui->multiplier->value();
    ui->defocusVal->setValue(val);
    ui->defocusVal->blockSignals(false);

    double f = mirrorDlg::get_Instance()->FNumber;
    double mm = f * f * 8. * value * .00055;  //mmeters
    m_defocusInmm = mm;
    qDebug() << "defocus offset" << mm;
    ui->Focusoffset->setText(QString("%1mm").arg(mm, 6,'f',3));
    emit defocus(val);
}


void defocusDlg::on_pushButton_clicked()
{
    QMessageBox::information(this, "_____________________ROC offset help.____________________________",
                "<b>Make sure the Zernike defocus term is not checked before using this control.</b> "
                "<p>Used by mirror makers to adjusted the mirror's desired focal lengh a small"
                " amount in order to raise the middle or edge of the surface.</p>"
                " <p>The slider value range is from -1 to +1 and multiplied by the multiplier to generate an offset in waves.</p>"
                "<p>The resulting focal length offset is displayed in mm at the lower right.</p>");
}

void defocusDlg::moveEvent(QMoveEvent *event) {
    QSettings set;
    set.setValue("defocus dialogGeometry", geometry());
    QDialog::moveEvent(event);

}
void defocusDlg::closeEvent(QCloseEvent *event) {
    QSettings set;
    set.setValue("defocus dialogGeometry", geometry());
    QDialog::closeEvent(event);
}
