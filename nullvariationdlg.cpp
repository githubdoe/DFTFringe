#include "nullvariationdlg.h"
#include "ui_nullvariationdlg.h"
#include "mirrordlg.h"

nullVariationDlg::nullVariationDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nullVariationDlg), isMm(true)
{
    ui->setupUi(this);
    mirrorDlg *md = mirrorDlg::get_Instance();
    ui->diameter->setText(QString::number(md->diameter));
    ui->roc->setText(QString::number(md->roc));
}

nullVariationDlg::~nullVariationDlg()
{
    delete ui;
}


void nullVariationDlg::on_useInch_clicked(bool checked)
{
    isMm = !checked;
    ui->diameter->blockSignals(true);
    ui->roc->blockSignals(true);
    ui->rocDelta->blockSignals(true);
    ui->diameterDelta->blockSignals(true);
    double mul = (checked) ? 1./25.4 : 25.4;
    double diam = ui->diameter->text().toDouble() * mul;
    ui->diameter->setText(QString::number(diam));
    double roc = ui->roc->text().toDouble() * mul;
    ui->roc->setText(QString::number(roc));
    double tol = ui->rocDelta->value() * mul;
    ui->rocDelta->setValue(tol);
    tol = ui->diameterDelta->value() *mul;
    ui->diameterDelta->setValue(tol);
    ui->diameter->blockSignals(false);
    ui->roc->blockSignals(false);
    ui->rocDelta->blockSignals(false);
    ui->diameterDelta->blockSignals(false);
    if (checked){
        ui->rocDelta->setSingleStep(.062);
        ui->diameterDelta->setSingleStep(.062);
    }else {
        ui->rocDelta->setSingleStep(1.);
        ui->diameterDelta->setSingleStep(1.);
    }
    on_pushButton_clicked();

}

void nullVariationDlg::on_useMm_clicked(bool checked)
{
    on_useInch_clicked(!checked);
}

void nullVariationDlg::on_roc_textChanged(const QString )
{
        on_pushButton_clicked();
}

void nullVariationDlg::on_diameter_textChanged(const QString)
{
    on_pushButton_clicked();
}

void nullVariationDlg::on_rocDelta_valueChanged(double)
{
        on_pushButton_clicked();
}

void nullVariationDlg::on_diameterDelta_valueChanged(double)
{
    on_pushButton_clicked();
}

void nullVariationDlg::on_pushButton_clicked()
{
    double mul = (isMm) ? 1. : 25.4;

    double roc_tol = ui->rocDelta->text().toDouble() * mul;
    double d_tol = ui->diameterDelta->text().toDouble() * mul;
    double diam =  ui->diameter->text().toDouble() * mul;
    double roc = ui->roc->text().toDouble() * mul;

    double rocMax = roc + roc_tol;
    double diamMin = diam -d_tol;
    double diamMax = diam + d_tol;
    double rocMin = roc - roc_tol;
    mirrorDlg *md = mirrorDlg::get_Instance();

    double center = (diam * diam * diam * diam)/(roc * roc * roc);

    double max_error =  center - (diamMin* diamMin * diamMin * diamMin)/(rocMax *rocMax * rocMax);
    max_error = 1000000. * max_error/(384. * md->lambda);

    double min_error = center - (diamMax* diamMax * diamMax* diamMax )/
                        (rocMin *rocMin* rocMin);

    double Null_PV_error = max_error - min_error;
    ui->result->setText(QString::number(Null_PV_error));
}
