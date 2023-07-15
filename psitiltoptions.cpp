#include "psitiltoptions.h"
#include "ui_psitiltoptions.h"

PSITiltOptions::PSITiltOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PSITiltOptions)
{
    ui->setupUi(this);
    ui->AstigPlot->hide();
    ui->DefocusPlot->hide();
    ui->AstigPlot->hide();
}

PSITiltOptions::~PSITiltOptions()
{
    delete ui;
}



void PSITiltOptions::on_zernLimits_activated(int /*index*/)
{

}
