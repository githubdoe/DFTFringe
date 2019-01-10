#include "wavefrontfilterdlg.h"
#include "ui_wavefrontfilterdlg.h"
#include "QSettings"
wavefrontFilterDlg::wavefrontFilterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wavefrontFilterDlg)
{
    ui->setupUi(this);
    ui->deleteFile->hide();
    ui->astigPlot->hide();
    connect(ui->astigPlot, SIGNAL(waveSeleted(QString)), this, SLOT(waveSelected(QString)));
    ui->rmSPlot->hide();
    connect(ui->rmSPlot, SIGNAL(waveSeleted(QString)), this, SLOT(waveSelected(QString)));
    QSettings set;
    ui->rmsValue->blockSignals(true);
    ui->rmsValue->setValue(set.value("filterRMS", .1).toDouble());
    ui->rmsValue->blockSignals(false);
    ui->beepEnable->setChecked(set.value("RMSBeep>", true).toBool());
    ui->remove->setChecked(set.value("filterRemoveFlag", false).toBool());
    resize(200,200);
}

wavefrontFilterDlg::~wavefrontFilterDlg()
{
    delete ui;
}
void wavefrontFilterDlg::waveSelected(QString w){
    emit waveWasSelected(w);
}
void wavefrontFilterDlg::setRemoveFileMode(){
    ui->remove->setText("Remove File From loaded wavefronts");
}

bool wavefrontFilterDlg::shouldFilterFile(){
    return ui->deleteFile->isChecked();
}
bool wavefrontFilterDlg::shouldFilterWavefront(){
    return ui->remove->isChecked();
}
double wavefrontFilterDlg::rms(){
    return ui->rmsValue->value();
}

void wavefrontFilterDlg::on_rmsValue_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("filterRMS", arg1);
}
void wavefrontFilterDlg::addAstigValue(QString name, QPointF value){
    ui->astigPlot->addValue(name,value);
}
void wavefrontFilterDlg::addRMSValue(QString name, QPointF value){
    ui->rmSPlot->addValue(name, value);
}
void wavefrontFilterDlg::plot(){

    ui->deleteFile->show();
    resize (800, 400);
    ui->astigPlot->show();
    ui->rmSPlot->show();
    ui->astigPlot->replot();
    ui->rmSPlot->replot();
}

void wavefrontFilterDlg::on_beepEnable_clicked(bool checked)
{
    QSettings set;
    set.setValue("RMSBeep>", checked);
}

void wavefrontFilterDlg::on_remove_clicked(bool checked)
{
    QSettings set;
    set.setValue("filterRemoveFlag", checked);
}
