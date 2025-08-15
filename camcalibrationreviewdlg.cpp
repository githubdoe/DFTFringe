#include "camcalibrationreviewdlg.h"
#include "ui_camcalibrationreviewdlg.h"
#include <QDebug>
CamCalibrationReviewDlg::CamCalibrationReviewDlg(const std::vector<cv::Mat> &images, const std::vector<cv::Mat> &keypoints,
                                                 const std::vector<cv::Mat> &corrected, QWidget *parent) :
    QDialog(parent),
    m_ndx(0), m_images(images), m_keypoints(keypoints), m_corrected(corrected),
    ui(new Ui::CamCalibrationReviewDlg), m_overlay(false), m_viewType(0)
{
    ui->setupUi(this);
    ui->input->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->input->setScaledContents(false);
    ui->verticalScrollBar->setRange(0,m_images.size()-1);
    cv::Mat mm = m_images[0];
    showImage(0);
}

void CamCalibrationReviewDlg::showImage(int ndx){
    cv::Mat mm;
    switch (m_viewType){
    case 0 :
        mm = m_corrected[ndx];
        ui->imageType->setText("Corrected Image");
        break;
    case 1:
        mm = m_images[ndx];
        ui->imageType->setText("Original Image");
        break;
    case 2:
        mm = m_keypoints[ndx];
        ui->imageType->setText("control points on original image");
        break;
    }


    QImage tmp = QImage((uchar*)mm.data,
                        mm.cols,
                        mm.rows,
                        mm.step,
                        QImage::Format_RGB888).copy();
    QSize labelSize = ui->input->size();
    // set a scaled pixmap to a w x h window keeping its aspect ratio
    ui->input->setPixmap(QPixmap::fromImage(tmp.scaled(labelSize, Qt::KeepAspectRatio, Qt::FastTransformation)));

}
void CamCalibrationReviewDlg::resizeEvent( QResizeEvent * ){
    showImage(m_ndx);
}

CamCalibrationReviewDlg::~CamCalibrationReviewDlg()
{
    delete ui;
}

void CamCalibrationReviewDlg::on_verticalScrollBar_valueChanged(int value)
{
    m_ndx = value;
    showImage(value);
}

void CamCalibrationReviewDlg::on_Overlay_pressed()
{
    m_overlay = true;
    showImage(m_ndx);
}

void CamCalibrationReviewDlg::on_Overlay_released()
{
    ++m_viewType;
    m_viewType %= 3;
    showImage(m_ndx);

}
