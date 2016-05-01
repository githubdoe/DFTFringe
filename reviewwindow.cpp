#include "reviewwindow.h"
#include "ui_reviewwindow.h"

reviewWindow::reviewWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::reviewWindow)
{
    ui->setupUi(this);
    s1 = ui->TopHSplitter;
    s2 = ui->bottomHSplitter;
}

reviewWindow::~reviewWindow()
{
    delete ui;
}
