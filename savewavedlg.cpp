#include "savewavedlg.h"
#include "ui_savewavedlg.h"
#include <QFileDialog>
SaveWaveDlg::SaveWaveDlg(QString lastPath, QWidget *parent) :
     QDialog(parent),lastPath(lastPath),
    ui(new Ui::SaveWaveDlg)
{
    ui->setupUi(this);
    ui->lineEdit->setText(lastPath);
}

SaveWaveDlg::~SaveWaveDlg()
{
    delete ui;
}

void SaveWaveDlg::on_normalRb_clicked(bool)
{

}

void SaveWaveDlg::on_browsePB_clicked()
{
    fileName = QFileDialog::getSaveFileName(0,
                        tr("Write wave font file"), lastPath,
                        tr("wft (*.wft)"));
    if (fileName.isEmpty())
        return;
    ui->lineEdit->setText(fileName);
}
