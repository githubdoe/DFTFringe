#include "outlineplots.h"
#include "ui_outlineplots.h"

outlinePlots::outlinePlots(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::outlinePlots)
{
    ui->setupUi(this);
}

outlinePlots::~outlinePlots()
{
    delete ui;
}
QwtPlot *outlinePlots::getPLot(int id){
    switch (id){
    case 1:
        return ui->plot1;
        break;
    case 2:
        return ui->plot2;
        break;
    case 3:
        return ui->plot3;
        break;
    case 4:
        return ui->plot4;
        break;
    }
}
