#include "psiphasedisplay.h"
#include "ui_psiphasedisplay.h"

PSIphaseDisplay::PSIphaseDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PSIphaseDisplay)
{
    ui->setupUi(this);
}

PSIphaseDisplay::~PSIphaseDisplay()
{
    delete ui;
}
