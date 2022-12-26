#include "arbitrarywavefronthelp.h"
#include "ui_arbitrarywavefronthelp.h"

ArbitraryWavefrontHelp::ArbitraryWavefrontHelp(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ArbitraryWavefrontHelp)
{
    ui->setupUi(this);
}

ArbitraryWavefrontHelp::~ArbitraryWavefrontHelp()
{
    delete ui;
}

void ArbitraryWavefrontHelp::on_pushButton_clicked()
{
    close();
}

