#include "nullmarginhelpdlg.h"
#include "ui_nullmarginhelpdlg.h"

nullMarginHelpDlg::nullMarginHelpDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nullMarginHelpDlg)
{
    ui->setupUi(this);

}

nullMarginHelpDlg::~nullMarginHelpDlg()
{
    delete ui;
}
