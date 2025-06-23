#include "autoinvertdlg.h"
#include "ui_autoinvertdlg.h"
#include "surfacemanager.h"

autoInvertDlg::autoInvertDlg(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint),
    ui(new Ui::autoInvertDlg)
{
    ui->setupUi(this);
}

autoInvertDlg::~autoInvertDlg()
{
    delete ui;
}

autoInvertDlg *autoInvertDlg::get_Instance(){
    static autoInvertDlg m_Instance{};
    return &m_Instance;
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

