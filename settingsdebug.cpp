#include "settingsdebug.h"
#include "ui_settingsdebug.h"
#include <qsettings.h>
settingsDebug::settingsDebug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDebug)
{
    ui->setupUi(this);
    QSettings set;
    ui->checkBox->setChecked(set.value("DebugShowMask",false).toBool());
}

settingsDebug::~settingsDebug()
{
    delete ui;
}
bool settingsDebug::showMask(){
    return ui->checkBox->isChecked();
}

void settingsDebug::on_checkBox_clicked(bool arg)
{
    QSettings set;
    set.setValue("DebugShowMask",arg );
}
