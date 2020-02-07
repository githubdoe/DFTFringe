#include "psitiltoptions.h"
#include "ui_psitiltoptions.h"

PSITiltOptions::PSITiltOptions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PSITiltOptions)
{
    ui->setupUi(this);
}

PSITiltOptions::~PSITiltOptions()
{
    delete ui;
}
