#include "settingsgeneral.h"
#include "ui_settingsgeneral.h"

settingsGeneral::settingsGeneral(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsGeneral)
{
    ui->setupUi(this);
}

settingsGeneral::~settingsGeneral()
{
    delete ui;
}
bool settingsGeneral::useRMS(){
    return ui->rmsRb->isChecked();
}
