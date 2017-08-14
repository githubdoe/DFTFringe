#include "settingsgeneral2.h"
#include "ui_settingsgeneral2.h"
#include <QSettings>

SettingsGeneral2::SettingsGeneral2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsGeneral2),m_useSVD(false),
    m_showConditionNumbers(false)
{
    ui->setupUi(this);
    QSettings set;
    m_useStarTestMakeOnly = set.value("useMakeStarTest", false).toBool();
    m_waveFrontSize = set.value("wavefrontDownSizeValue", 650).toInt();
    m_downsize = set.value("wavefrontShouldDownsize", false).toBool();
    ui->downSizeCB->blockSignals(true);
    ui->wavefrontSizeSb->blockSignals(true);
    ui->memThreshValue->blockSignals(true);
    ui->downSizeCB->setChecked(m_downsize);
    ui->wavefrontSizeSb->setValue(m_waveFrontSize);
    ui->memThreshValue->setValue(set.value("lowMemoryThreshold", 300).toInt());
    ui->memThreshValue->blockSignals(false);
    ui->wavefrontSizeSb->blockSignals(false);
    ui->downSizeCB->blockSignals(false);

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

void SettingsGeneral2::on_downSizeCB_clicked(bool checked){
    QSettings set;
    set.setValue("wavefrontShouldDownsize", checked);
    m_downsize = checked;
}

int SettingsGeneral2::memoryThreshold(){
    return ui->memThreshValue->value();
}


void SettingsGeneral2::on_memThreshValue_valueChanged(int val){
    QSettings set;
    set.setValue("lowMemoryThreshold", val);
}

void SettingsGeneral2::on_AstigDistGraphWidth_valueChanged(int val){
    QSettings set;
    set.setValue("AstigDistGraphWidth", val);
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
