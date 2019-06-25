#include "outlinedialog.h"
#include "ui_outlinedialog.h"

outlineDialog::outlineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::outlineDialog)
{
    ui->setupUi(this);
}

outlineDialog::~outlineDialog()
{
    delete ui;
}
