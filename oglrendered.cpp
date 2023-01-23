#include "oglrendered.h"
#include "ui_oglrendered.h"

oglRendered::oglRendered(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::oglRendered)
{
    ui->setupUi(this);
}

oglRendered::~oglRendered()
{
    delete ui;
}
QLabel *oglRendered::getModel(){
    return ui->model;
}
QLabel *oglRendered::getLegend(){
    return ui->legend;
}
