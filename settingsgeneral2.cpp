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
}

SettingsGeneral2::~SettingsGeneral2()
{
    delete ui;
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
