#include "lensdialog.h"
#include "ui_lensdialog.h"

lensDialog::lensDialog(QStringList parms, QWidget *parent) :
    QDialog(parent),m_parms(parms),
    ui(new Ui::lensDialog)
{
    ui->setupUi(this);
    ui->name->setText(parms[0]);
    ui->xFv->setText(parms[1]);
    ui->yFv->setText(parms[2]);
    ui->XFc->setText(parms[3]);
    ui->YFc->setText(parms[4]);
    ui->K1->setText(parms[5]);
    ui->K2->setText(parms[6]);
    ui->P1->setText(parms[7]);
    ui->P2->setText(parms[8]);
    ui->K3->setText(parms[9]);
}

lensDialog::~lensDialog()
{
    delete ui;
}

void lensDialog::on_buttonBox_accepted()
{
    m_parms << ui->name->text();
    m_parms << "\n";
    m_parms << ui->xFv->text() + "\n";
    m_parms << ui->yFv->text() + "\n";
    m_parms << ui->XFc->text() + "\n";
    m_parms << ui->YFc->text() + "\n";
    m_parms << ui->K1->text() + "\n";
    m_parms << ui->K2->text() + "\n";
    m_parms << ui->P1->text() + "\n";
    m_parms << ui->P2->text() + "\n";
    m_parms << ui->K3->text() + "\n";
}
