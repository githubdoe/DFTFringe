#include "rejectedwavefrontsdlg.h"
#include "ui_rejectedwavefrontsdlg.h"

rejectedWavefrontsDlg::rejectedWavefrontsDlg(QStringList list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::rejectedWavefrontsDlg)
{
    ui->setupUi(this);
    ui->listWidget->addItems(list);
}

rejectedWavefrontsDlg::~rejectedWavefrontsDlg()
{
    delete ui;
}
