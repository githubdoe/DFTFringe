#include "settingsdft.h"
#include "ui_settingsdft.h"
#include <QSettings>
settingsDFT::settingsDFT(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDFT)
{
    ui->setupUi(this);
    QSettings set;
    ui->ShowDFTTHumbCB->setChecked( set.value("DFTshowThumb", false).toBool());
    ui->DFTSizeSB->setValue(set.value("DFTSize", 640).toInt());

}

settingsDFT::~settingsDFT()
{
    delete ui;
}

bool settingsDFT::showThumb(){
    return ui->ShowDFTTHumbCB->isChecked();
}

int settingsDFT::DFTSize() {
    return ui->DFTSizeSB->value();
}

void settingsDFT::on_DFTSizeSB_valueChanged(int)
{
    QSettings set;
    set.setValue("DFTSize", ui->DFTSizeSB->value());
}

void settingsDFT::on_ShowDFTTHumbCB_clicked(bool)
{
    QSettings set;
    set.setValue("DFTshowThumb", ui->ShowDFTTHumbCB->isChecked());
}
