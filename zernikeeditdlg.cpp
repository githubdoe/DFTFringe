#include "zernikeeditdlg.h"
#include "ui_zernikeeditdlg.h"

zernikeEditDlg::zernikeEditDlg(SurfaceManager *sfm, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::zernikeEditDlg), m_sm(sfm)
{
    ui->setupUi(this);
    tableModel = new ZernTableModel(parent,true);
    std::vector<double> val(Z_TERMS,0.);
    ui->tableView->setModel(tableModel);
}

zernikeEditDlg::~zernikeEditDlg()
{
    delete ui;
}

void zernikeEditDlg::on_disable_clicked()
{

}

void zernikeEditDlg::on_createSurface_clicked()
{

}

void zernikeEditDlg::on_clearAll_clicked()
{

}

void zernikeEditDlg::on_read_clicked()
{

}

void zernikeEditDlg::on_save_clicked()
{

}

void zernikeEditDlg::on_useCurrent_clicked()
{
    tableModel->setValues(m_sm->m_wavefronts[m_sm->m_currentNdx]->InputZerns);
}
