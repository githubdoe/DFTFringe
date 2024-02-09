#include "annulushelpdlg.h"
#include "ui_annulushelpdlg.h"

annulusHelpDlg::annulusHelpDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::annulusHelpDlg)
{
    ui->setupUi(this);
}

annulusHelpDlg::~annulusHelpDlg()
{
    delete ui;
}
