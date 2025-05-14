#include "hotkeysdlg.h"
#include "ui_hotkeysdlg.h"

HotKeysDlg::HotKeysDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HotKeysDlg)
{
    ui->setupUi(this);
}

HotKeysDlg::~HotKeysDlg()
{
    delete ui;
}
