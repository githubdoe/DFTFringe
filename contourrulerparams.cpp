#include "contourrulerparams.h"
#include "ui_contourrulerparams.h"

ContourRulerParams::ContourRulerParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContourRulerParams)
{
    ui->setupUi(this);
}

ContourRulerParams::~ContourRulerParams()
{
    delete ui;
}
