#include "psiresizeimagesdlg.h"
#include "ui_psiresizeimagesdlg.h"
#include <QSettings>
#include <QString>
PSIResizeImagesDlg::PSIResizeImagesDlg(int resize,int max, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PSIResizeImagesDlg)
{
    ui->setupUi(this);
    QSettings set;
    ui->m_resize->setMaximum(max);
    ui->CurrentSize->setText(QString("%1").arg(max));
    ui->m_resize->setValue(resize);
}

int PSIResizeImagesDlg::getSize(){
    return ui->m_resize->value();
}

PSIResizeImagesDlg::~PSIResizeImagesDlg()
{
    delete ui;
}

void PSIResizeImagesDlg::on_m_resize_valueChanged(int arg1)
{
    QSettings set;
    int v = (arg1/100)*100;
    set.setValue("PSIResize", v);
    ui->m_resize->setValue(v);
}

