#include "defocusdlg.h"
#include "ui_defocusdlg.h"
#include <QMessageBox>
defocusDlg::defocusDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::defocusDlg)
{
    ui->setupUi(this);
}

defocusDlg::~defocusDlg()
{
    delete ui;
}




void defocusDlg::on_multiplier_valueChanged(int /*arg1*/)
{
    on_defocusSlider_valueChanged(ui->defocusSlider->value());
}
#include <QDebug>
#include "math.h"
void defocusDlg::on_defocusVal_valueChanged(double arg1)
{
    if (fabs(arg1) > 1) {
        int mul = fabs(arg1/.5);
        double slid = arg1/mul;

        ui->multiplier->blockSignals(true);
        ui->spinValue->setValue(slid);

        ui->multiplier->blockSignals(false);
        ui->multiplier->setValue(mul);
    }
    else {

        ui->spinValue->setValue(arg1);

        ui->multiplier->blockSignals(true);
        ui->multiplier->setValue(1.);
        ui->multiplier->blockSignals(false);
    }
    emit defocus(arg1);
}


void defocusDlg::on_spinValue_valueChanged(double arg1)
{
    ui->defocusSlider->setValue(arg1 * 100);
}
#include "mirrordlg.h"
void defocusDlg::on_defocusSlider_valueChanged(int value)
{
    double val = value/100.;
    ui->spinValue->blockSignals(true);
    ui->spinValue->setValue(val);
    ui->spinValue->blockSignals(false);
    val *= ui->multiplier->value();
    ui->defocusVal->setValue(val);

    double f = mirrorDlg::get_Instance()->FNumber;
    double mm = f * f * 8. * value * .00055;  //mmeters
    qDebug() << "mm" << mm;
    ui->Focusoffset->setText(QString().sprintf("%6.3lfmm", mm));
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

