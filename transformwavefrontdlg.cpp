#include "transformwavefrontdlg.h"
#include "ui_transformwavefrontdlg.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
TransformWaveFrontDlg::TransformWaveFrontDlg( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransformWaveFrontDlg)
{
    ui->setupUi(this);
}

TransformWaveFrontDlg::~TransformWaveFrontDlg()
{
    delete ui;
}



void TransformWaveFrontDlg::on_ChangeWaveLength_clicked()
{
    emit changeWavelength(ui->newWaveLength->value());
}

void TransformWaveFrontDlg::on_flipLR_clicked()
{
    emit flipLR();


}

void TransformWaveFrontDlg::on_Resize_clicked()
{
    emit resizeW( ui->newSize->value());
}

void TransformWaveFrontDlg::on_flipVertical_clicked()
{
    emit flipV();
}
