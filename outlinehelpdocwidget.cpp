#include "outlinehelpdocwidget.h"
#include "ui_outlinehelpdocwidget.h"

outlineHelpDocWidget::outlineHelpDocWidget(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::outlineHelpDocWidget)
{
    ui->setupUi(this);
}

outlineHelpDocWidget::~outlineHelpDocWidget()
{
    delete ui;
}
