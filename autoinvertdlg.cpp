#include "autoinvertdlg.h"
#include "ui_autoinvertdlg.h"
#include "surfacemanager.h"

autoInvertDlg::autoInvertDlg(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
    ui(new Ui::autoInvertDlg)
{
    ui->setupUi(this);
    setSizeGripEnabled(true);
}

autoInvertDlg::~autoInvertDlg()
{
    delete ui;
}

void autoInvertDlg::on_btnUseConic_clicked()
{
    SurfaceManager *sm = SurfaceManager::get_instance();
    sm->m_inverseMode = invCONIC;
    accept();
}


void autoInvertDlg::on_btnManual_clicked()
{
    SurfaceManager *sm = SurfaceManager::get_instance();
    sm->m_inverseMode = invMANUAL;
    accept();
}


void autoInvertDlg::on_btnInside_clicked()
{
    SurfaceManager *sm = SurfaceManager::get_instance();
    sm->m_inverseMode = invINSIDE;
    accept();
}


void autoInvertDlg::on_btnOutside_clicked()
{
    SurfaceManager *sm = SurfaceManager::get_instance();
    sm->m_inverseMode = invOUTSIDE;
    accept();
}

void autoInvertDlg::setMainLabel(const QString & str) {
    ui->lblMain->setText(str);
}

void autoInvertDlg::enableConic(bool b) {
    ui->btnUseConic->setEnabled(b);
}

void autoInvertDlg::on_InvertWithoutAskingRb_clicked(bool checked)
{
    Q_UNUSED(checked);
}

