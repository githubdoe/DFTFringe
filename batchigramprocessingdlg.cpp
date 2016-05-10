#include "batchigramprocessingdlg.h"
#include "ui_batchigramprocessingdlg.h"

batchIgramProcessingDlg::batchIgramProcessingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::batchIgramProcessingDlg)
{
    ui->setupUi(this);
}

batchIgramProcessingDlg::~batchIgramProcessingDlg()
{
    delete ui;
}
