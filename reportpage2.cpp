#include "reportpage2.h"
#include "ui_reportpage2.h"

ReportPage2::ReportPage2(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReportPage2)
{
    ui->setupUi(this);
}

ReportPage2::~ReportPage2()
{
    delete ui;
}
QLabel *ReportPage2::Contour(){
    return ui->contour;
}
QLabel *ReportPage2::Profile(){
    return ui->profile;
}
QLabel *ReportPage2::Surface(){
    return ui->surface;
}
