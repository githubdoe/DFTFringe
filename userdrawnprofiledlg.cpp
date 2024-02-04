#include "userdrawnprofiledlg.h"
#include "ui_userdrawnprofiledlg.h"
#include "arbitrarywavefronthelp.h"
UserDrawnProfileDlg *UserDrawnProfileDlg::m_instance = 0;
UserDrawnProfileDlg::UserDrawnProfileDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UserDrawnProfileDlg)
{
    ui->setupUi(this);
    QRect rec = QGuiApplication::primaryScreen()->geometry();
    resize(rec.width()/2,rec.height()/2);
    connect(ui->pww, &ArbitraryWavWidget::yScaleChanged, this, &UserDrawnProfileDlg::yScaleChanged );
}
UserDrawnProfileDlg* UserDrawnProfileDlg::get_instance(){
    if (m_instance == 0){
        m_instance = new UserDrawnProfileDlg();
    }
    return m_instance;
}
UserDrawnProfileDlg::~UserDrawnProfileDlg()
{
    delete ui;
}

void UserDrawnProfileDlg::on_cmbUnits_currentIndexChanged(int unit)
{
    switch (unit) {
    case 0:
        ui->pww->ww_unit = ArbitraryWavWidget::in;
        break;
    case 1:
        ui->pww->ww_unit = ArbitraryWavWidget::cm;
        break;
    case 2:
    default:
        ui->pww->ww_unit = ArbitraryWavWidget::mm;
        break;
    }
    ui->pww->repaint();

}


void UserDrawnProfileDlg::on_load_clicked()
{
    ui->pww->readJson();
}


void UserDrawnProfileDlg::on_save_clicked()
{
    ui->pww->saveJson();
}


void UserDrawnProfileDlg::on_buttonBox_accepted()
{
    bOkPressed=true;
    close();
}

void UserDrawnProfileDlg::setDiameter(double _mirror_diam){
    ui->pww->setRadius(_mirror_diam/2);
}

void UserDrawnProfileDlg::prepare(int size) {
    ui->pww->prepare(size);
}
double UserDrawnProfileDlg::getValue(double rho) {
    return ui->pww->getValue(rho);
}

void UserDrawnProfileDlg::on_helpButton_clicked()
{
    ArbitraryWavefrontHelp dlg;
    dlg.setModal(true);
    dlg.exec();
}


void UserDrawnProfileDlg::on_buttonBox_rejected()
{
    bOkPressed=false;
    close();
}

void UserDrawnProfileDlg::yScaleChanged(double val){
    ui->yScale->blockSignals(true);
    ui->yScale->setValue(val);
    ui->yScale->blockSignals(false);
}

void UserDrawnProfileDlg::on_yScale_valueChanged(double arg1)
{
    ui->pww->setYscale(arg1);
}

