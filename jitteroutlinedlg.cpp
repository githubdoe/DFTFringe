#include "jitteroutlinedlg.h"
#include "ui_jitteroutlinedlg.h"

jitterOutlineDlg::jitterOutlineDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::jitterOutlineDlg)
{
    ui->setupUi(this);
    connect(ui->startPb, &QAbstractButton::clicked, this, &jitterOutlineDlg::startRequested);
    connect(ui->StopPb, &QAbstractButton::clicked, this, &jitterOutlineDlg::stopRequested);
}

jitterOutlineDlg::~jitterOutlineDlg()
{
    delete ui;
}
void jitterOutlineDlg::status(const QString &status){
    ui->status->setText(status);
}

QProgressBar *jitterOutlineDlg::getProgressBar(){
    return ui->progressBar;
}
int jitterOutlineDlg::getStep(){
    return ui->stepSB->value();
}

int jitterOutlineDlg::getStart(){
    return ui->startSB->value();
}

int jitterOutlineDlg::getEnd(){
    return ui->endSB->value();
}
int jitterOutlineDlg::getType(){
    if (ui->scanX->isChecked()) return 1;
    if (ui->scanY->isChecked()) return 2;
    if (ui->scanRad->isChecked()) return 3;
    return 0;
}
