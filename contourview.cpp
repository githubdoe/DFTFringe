#include "contourview.h"
#include "ui_contourview.h"

contourView::contourView(QWidget *parent, ContourTools *tools) :
    QWidget(parent),
    ui(new Ui::contourView)
{
    ui->setupUi(this);
    ui->widget->setTool(tools);
    connect(ui->widget, SIGNAL(newContourRange(double)), ui->doubleSpinBox , SLOT(setValue(double)));
}

contourView::~contourView()
{
    delete ui;
}
ContourPlot *contourView::getPlot(){
    return ui->widget;
}

void contourView::on_doubleSpinBox_valueChanged(double arg1)
{
    ui->widget->contourIntervalChanged(arg1);
}


void contourView::on_pushButton_pressed()
{
    emit showAllContours();
}
