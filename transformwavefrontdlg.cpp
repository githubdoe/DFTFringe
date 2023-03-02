#include "transformwavefrontdlg.h"
#include "ui_transformwavefrontdlg.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "surfacemanager.h"
TransformWaveFrontDlg::TransformWaveFrontDlg( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TransformWaveFrontDlg)
{
    ui->setupUi(this);
    wavefront *wf = SurfaceManager::get_instance()->getCurrent();
    ui->newWaveLength->setValue(wf->lambda);
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
