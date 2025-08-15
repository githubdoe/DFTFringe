#include "jitteroutlinedlg.h"
#include "ui_jitteroutlinedlg.h"
#include "mainwindow.h"
jitterOutlineDlg *jitterOutlineDlg::m_instance = 0;

jitterOutlineDlg::jitterOutlineDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::jitterOutlineDlg)
{
    ui->setupUi(this);
}

jitterOutlineDlg *jitterOutlineDlg::getInstance(MainWindow *mw){
    if (m_instance ==0){
        m_instance=  new jitterOutlineDlg;
        connect(m_instance->ui->startPb, SIGNAL(clicked(bool)), mw,SLOT(startJitter()));
        connect(m_instance->ui->StopPb, SIGNAL(clicked(bool)),mw,SLOT(stopJitter()));
    }
    return m_instance;

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
