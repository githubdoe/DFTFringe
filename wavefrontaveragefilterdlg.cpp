#include "wavefrontaveragefilterdlg.h"
#include "ui_wavefrontaveragefilterdlg.h"
#include <QSettings>
wavefrontAverageFilterDlg::wavefrontAverageFilterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::wavefrontAverageFilterDlg)
{
    ui->setupUi(this);
    QSettings set;
    ui->rmsValue->setValue(set.value("wavefrontAverageRMSLimit", .1).toDouble());
}

wavefrontAverageFilterDlg::~wavefrontAverageFilterDlg()
{
    delete ui;
}
double wavefrontAverageFilterDlg::rms(){
    return ui->rmsValue->value();
}

void wavefrontAverageFilterDlg::on_buttonBox_accepted()
{
    QSettings set;
    set.setValue("wavefrontAverageRMSLimit", rms());
}
