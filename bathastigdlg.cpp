#include "bathastigdlg.h"
#include "ui_bathastigdlg.h"
#include "mirrordlg.h"
#include "math.h"
bathAstigDlg::bathAstigDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bathAstigDlg)
{
    ui->setupUi(this);
    mirrorDlg &md = *mirrorDlg::get_Instance();
    ui->diamSb->setValue(md.diameter);
    ui->rocsb->setValue(md.roc);
    ui->lambdaSb->setValue(md.lambda);
    ui->sepSb->setValue(5.);
    compute();
}

bathAstigDlg::~bathAstigDlg()
{
    delete ui;
}

void bathAstigDlg::compute(){
    mirrorDlg &md = *mirrorDlg::get_Instance();
    double val = ui->diamSb->value() * ui->diamSb->value() * ui->sepSb->value() * ui->sepSb->value() /
            (32. * ui->rocsb->value() * ui->rocsb->value() * ui->rocsb->value());
    ui->pvAstig->setText( QString::number(round(1e9 * val/md.lambda)/1000.));
}

void bathAstigDlg::on_diamSb_valueChanged(double)
{
    compute();
}

void bathAstigDlg::on_rocsb_valueChanged(double)
{
    compute();
}

void bathAstigDlg::on_sepSb_valueChanged(double)
{
    compute();
}

void bathAstigDlg::on_lambdaSb_valueChanged(double)
{
    compute();
}
