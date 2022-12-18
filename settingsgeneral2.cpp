#include "settingsgeneral2.h"
#include "ui_settingsgeneral2.h"
#include <QSettings>
#include "contourrulerparams.h"
#include <QMessageBox>
extern double outputLambda;
SettingsGeneral2::SettingsGeneral2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsGeneral2),m_useSVD(false),
    m_showConditionNumbers(false), valsChanged(false)
{
    ui->setupUi(this);
    QSettings set;
    m_useStarTestMakeOnly = set.value("useMakeStarTest", false).toBool();
    m_waveFrontSize = set.value("wavefrontDownSizeValue", 650).toInt();
    m_downsize = set.value("wavefrontShouldDownsize", false).toBool();
    ui->downSizeCB->blockSignals(true);
    ui->wavefrontSizeSb->blockSignals(true);
    ui->downSizeCB->setChecked(m_downsize);
    ui->wavefrontSizeSb->setValue(m_waveFrontSize);
    ui->wavefrontSizeSb->blockSignals(false);
    ui->downSizeCB->blockSignals(false);
    ui->outputLambda->blockSignals(true);
    ui->outputLambda->setValue(set.value("outputLambda", 550.).toDouble());
    ui->outputLambda->blockSignals(false);
    ui->useAnnular->setChecked(set.value("useAnnular", false).toBool());
    ui->percenObstruction->setValue(set.value("percentObs", 1.).toDouble());

}

SettingsGeneral2::~SettingsGeneral2()
{
    delete ui;
}

void SettingsGeneral2::on_wavefrontSizeSb_valueChanged(int arg1){
    m_waveFrontSize = arg1;
    QSettings set;
    set.setValue("wavefrontDownSizeValue", arg1);
}

void SettingsGeneral2::on_applyOffsets_clicked(bool checked){
   QSettings set;
   set.setValue("applyOffsets", checked);
}

void SettingsGeneral2::on_downSizeCB_clicked(bool checked){
    QSettings set;
    set.setValue("wavefrontShouldDownsize", checked);
    m_downsize = checked;
}


void SettingsGeneral2::updateContour(){
    emit updateContourPlot();
}

void SettingsGeneral2::on_rulerParms_clicked(){
    ContourRulerParams dlg;
    connect(&dlg, SIGNAL(updateParms()), this, SLOT(updateContour()));
    dlg.exec();

}

void SettingsGeneral2::on_outputLambda_valueChanged(double val){
   QSettings set;
   set.setValue("outputLambda", val);
   valsChanged = true;

}

void SettingsGeneral2::on_memThreshValue_valueChanged(int val){
    QSettings set;
    set.setValue("lowMemoryThreshold", val);
}

void SettingsGeneral2::on_AstigDistGraphWidth_valueChanged(int val){
    QSettings set;
    set.setValue("AstigDistGraphWidth", val);
}
double SettingsGeneral2::getObs(){
    if (ui->useAnnular->isChecked())
        return ui->percenObstruction->value() * .01;
    else return 1.;
}
void SettingsGeneral2::on_checkBox_clicked(bool checked)
{
    m_useSVD = checked;

}
void SettingsGeneral2::on_starTestMakeCb_clicked(bool checked)
{
    m_useStarTestMakeOnly = checked;
    QSettings set;
    set.setValue("useMakeStarTest", checked);
}
bool SettingsGeneral2::useSVD(){
    return m_useSVD;
}

bool SettingsGeneral2::showConditionNumbers(){
    return m_showConditionNumbers;
}

void SettingsGeneral2::on_showConditionNumbersCb_clicked(bool checked)
{
    m_showConditionNumbers = checked;
}
void SettingsGeneral2::on_apply_clicked(){
    if (valsChanged){
       QSettings set;
       outputLambda = set.value("outputLambda", 550.).toDouble();
        emit outputLambdaChanged(set.value("outputLambda", 550.).toDouble());
        valsChanged = false;
    }
}

void SettingsGeneral2::on_useAnnular_clicked(bool checked)
{
    QSettings set;
    set.setValue("useAnnular", checked);
    if (checked)
       QMessageBox::warning(0,"_____________________warning______________________", "This is an experimental feature and probably not accurate.");
}


void SettingsGeneral2::on_percenObstruction_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("percendObs", arg1);
}

