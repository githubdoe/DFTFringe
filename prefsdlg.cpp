#include "prefsdlg.h"
#include "ui_prefsdlg.h"

prefsDlg::prefsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::prefsDlg)
{
    ui->setupUi(this);
}

prefsDlg::~prefsDlg()
{
    delete ui;
}

void prefsDlg::on_pushButton_clicked()
{
    emit saveGeometry();
}
