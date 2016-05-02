#include "counterrotationdlg.h"
#include "ui_counterrotationdlg.h"

CounterRotationDlg::CounterRotationDlg(QString fn, double rot, bool dir, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CounterRotationDlg)
{
    ui->setupUi(this);
    ui->fileName->setText(fn);
    ui->counterRotation->setText(QString().sprintf("%6.1lf", rot));
    ui->CW->setChecked(dir);
}
QString CounterRotationDlg::getRotation(){
    return ui->counterRotation->text();
}
bool CounterRotationDlg::isClockwise(){
    return ui->CW->isChecked();

}
void CounterRotationDlg::setCCW(bool flag){
    ui->CCW->setChecked(flag);
}

CounterRotationDlg::~CounterRotationDlg()
{
    delete ui;
}
