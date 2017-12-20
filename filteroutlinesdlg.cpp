#include "filteroutlinesdlg.h"
#include "ui_filteroutlinesdlg.h"

FilterOutlinesDlg::FilterOutlinesDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterOutlinesDlg)
{
    ui->setupUi(this);
    filterOperation = FOEQUAL;
    actionType = oFRESIZE;

}

FilterOutlinesDlg::~FilterOutlinesDlg()
{
    delete ui;
}

void FilterOutlinesDlg::on_actionLoad_clicked(bool checked)
{
    if (checked){
        actionType = oFLOAD;
    }
}

void FilterOutlinesDlg::on_actionAverae_clicked(bool checked)
{
    if (checked){
        actionType = oFAVERAGE;
    }
}

void FilterOutlinesDlg::on_actionDelete_clicked(bool checked)
{
    if (checked){
        actionType = oFDELETE;
    }
}

void FilterOutlinesDlg::on_actionResize_clicked(bool checked)
{
    if (checked){
        actionType = outlineFilterAction::oFRESIZE;
    }
}

void FilterOutlinesDlg::on_less_clicked(bool checked)
{
    if (checked){
        filterOperation = FOLESSTHAN;
    }
}

void FilterOutlinesDlg::on_equal_clicked(bool checked)
{
    if (checked){
        filterOperation = FOEQUAL;
    }
}

void FilterOutlinesDlg::on_greator_clicked(bool checked)
{
    if (checked){
        filterOperation = FOGREATERTHAN;
    }
}

void FilterOutlinesDlg::on_radiusValue_valueChanged(double arg1)
{
    radiusTarget = arg1;
}

void FilterOutlinesDlg::on_resizeValue_valueChanged(double arg1)
{
    resizeValue = arg1;
}
void FilterOutlinesDlg::setRadius(double rad)
{
    radiusTarget = resizeValue = rad;
    ui->radiusValue->setValue(rad);
    ui->resizeValue->setValue(rad);
    update();
}

void FilterOutlinesDlg::on_any_clicked(bool checked)
{
    filterOperation = FOANY;
}
