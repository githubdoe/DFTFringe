#include "renamewavefrontdlg.h"
#include "ui_renamewavefrontdlg.h"

renameWavefrontDlg::renameWavefrontDlg(QString name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::renameWavefrontDlg)
{
    ui->setupUi(this);
    ui->name->setText(name);
}

renameWavefrontDlg::~renameWavefrontDlg()
{
    delete ui;
}
QString renameWavefrontDlg::name(){
    return ui->name->text();
}
