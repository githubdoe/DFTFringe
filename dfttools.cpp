#include "dfttools.h"
#include "ui_dfttools.h"
#include "vortex.h"

DFTTools::DFTTools(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::DFTTools)
{
    ui->setupUi(this);
    ui->colorChannelCB->addItem("Auto");
    ui->colorChannelCB->addItem("ALL RGB");
    ui->colorChannelCB->addItem("Red");
    ui->colorChannelCB->addItem("Green");
    ui->colorChannelCB->addItem("Blue");


}
void DFTTools::connectTo(QWidget *view){
    connect(view, SIGNAL(updateFilterSize(int)),this, SLOT(setCenterFilterValue(int)));
}
DFTTools::~DFTTools()
{
    delete ui;
}



void DFTTools::on_colorChannelCB_activated(const QString &arg1)
{
    emit dftChannel(arg1);
}


void DFTTools::setCenterFilterValue(int v){
    ui->filterAdjuster->setValue(v);
}

void DFTTools::on_filterAdjuster_valueChanged(double arg1)
{
    emit dftCenterFilter(arg1);
}

// compute surface
void DFTTools::on_pushButton_clicked()
{
    emit makeSurface();
}

void DFTTools::on_computeDFT_pressed()
{
    emit doDFT();
}
