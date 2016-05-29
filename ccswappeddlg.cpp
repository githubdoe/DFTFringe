#include "ccswappeddlg.h"
#include "ui_ccswappeddlg.h"

CCSwappedDlg::CCSwappedDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CCSwappedDlg)
{
    ui->setupUi(this);
}

CCSwappedDlg::~CCSwappedDlg()
{
    delete ui;
}
selection CCSwappedDlg::getSelection(){
    if (ui->noneRb->isChecked())
        return NONE;
    if (ui->negativeRb->isChecked())
        return NEGATIVE;
    else return POSITIVE;
}
