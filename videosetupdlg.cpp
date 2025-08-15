#include "videosetupdlg.h"
#include "ui_videosetupdlg.h"
#include <QSettings>
videoSetupDlg::videoSetupDlg(const QImage &img, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::videoSetupDlg)
{
    ui->setupUi(this);

    ui->width->setValue(img.width() & 0xFFFFFFFFFFFFFFE);
    ui->height->setValue(img.height() & 0xFFFFFFFFFFFFFFE);
    QSettings set;
    ui->angle->setValue(set.value("vidStepAngle", 5).toDouble());
    ui->fps->setValue(set.value("vidFPS", 10).toInt());
    ui->width->setValue(set.value("vidWidth", 400).toInt());
    ui->height->setValue(set.value("vidHeight", 300).toInt());
    m_imageSize = img.size();
    bool fullSize = set.value("vidFullSize", true).toBool();
    if (fullSize){
        ui->fullSize->setChecked( fullSize);
        ui->width->setValue(m_imageSize.width() & 0xffffffffffffffe);
        ui->height->setValue(m_imageSize.height() & 0xffffffffffffffe);
        ui->width->setEnabled(false);
        ui->height->setEnabled(false);
    }
}

videoSetupDlg::~videoSetupDlg()
{
    delete ui;
}
int videoSetupDlg::fps(){
    return ui->fps->value();
}
int videoSetupDlg::width(){
    return ui->width->value() & 0xfffffffffffffffe;
}
int videoSetupDlg::height(){
    return ui->height->value() & 0xfffffffffffffffe;
}
double videoSetupDlg::angleStep(){
    return ui->angle->value();
}
void videoSetupDlg::on_buttonBox_accepted(){
    QSettings set;
    set.setValue("vidStepAngle", ui->angle->value());
    set.setValue("vidFPS", ui->fps->value());
    set.setValue("vidWidth", ui->width->value());
    set.setValue("vidHeight", ui->height->value());
    set.setValue("vidFullSize", ui->fullSize->isChecked());
}


void videoSetupDlg::on_fullSize_clicked(bool checked)
{
    if (checked){
        ui->width->setValue(m_imageSize.width() & 0xffffffffffffffe);
        ui->height->setValue(m_imageSize.height() & 0xffffffffffffffe);
        ui->width->setEnabled(false);
        ui->height->setEnabled(false);
    }
    else {
        ui->width->setEnabled(true);
        ui->height->setEnabled(true);
    }
}
