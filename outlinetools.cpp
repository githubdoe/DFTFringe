#include "outlinetools.h"
#include "ui_outlinetools.h"

OutlineTools::OutlineTools(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::OutlineTools)
{
    ui->setupUi(this);
}

OutlineTools::~OutlineTools()
{
    delete ui;
}





void OutlineTools::on_minus_clicked()
{

}

void OutlineTools::on_pluss_clicked()
{

}
