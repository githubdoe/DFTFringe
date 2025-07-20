#include "settingsigramimportconfig.h"
#include "ui_settingsigramimportconfig.h"
#include <QFileDialog>
#include <QSettings>
settingsIgramImportConfig::settingsIgramImportConfig(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsIgramImportConfig)
{
    ui->setupUi(this);
    QSettings set;
    ui->importPath->setText(set.value("importIgramPath",".").toString());

    ui->openPath->setChecked(set.value("importIgramOpenOnly", false).toBool());
    ui->openMostRecent->setChecked(set.value("importIgramOpenMostRecent", true).toBool());

}

settingsIgramImportConfig::~settingsIgramImportConfig()
{
    delete ui;
}




void settingsIgramImportConfig::on_browse_clicked()
{
    QSettings set;
    QString defaultPath = set.value("lastPath",".").toString();
    //QString path = set.value("importIgramPath",defaultPath).toString(); //unused
    QString directory = QFileDialog::getExistingDirectory(
        0, "Choose directory", "",
        QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    ui->importPath->setText(directory);

}



#include <QDebug>

void settingsIgramImportConfig::on_buttonBox_accepted()
{
    QSettings set;
    set.setValue("importIgramPath",ui->importPath->text());


    set.setValue("importIgramOpenOnly", ui->openPath->isChecked());
    set.setValue("importIgramOpenMostRecent", ui->openMostRecent->isChecked());
}

