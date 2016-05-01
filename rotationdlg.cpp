#include "rotationdlg.h"
#include "ui_rotationdlg.h"

RotationDlg::RotationDlg( QList<int> list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RotationDlg), list(list)
{
    ui->setupUi(this);
}

RotationDlg::~RotationDlg()
{
    delete ui;
}

void RotationDlg::on_buttonBox_accepted()
{
    int sign = (ui->CCWCB->isChecked()) ?  -1:1;
    emit rotateTheseSig(sign * ui->angleSB->value(), list);
}
