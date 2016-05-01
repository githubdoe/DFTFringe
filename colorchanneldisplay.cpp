#include "colorchanneldisplay.h"
#include "ui_colorchanneldisplay.h"
#include <opencv/cv.h>

ColorChannelDisplay::ColorChannelDisplay(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ColorChannelDisplay)
{
    ui->setupUi(this);
    ui->red->setScaledContents( true );
    ui->green->setScaledContents( true );
    ui->blue->setScaledContents( true );
    //ui->green->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

ColorChannelDisplay::~ColorChannelDisplay()
{
    delete ui;
}
void ColorChannelDisplay::setImage(QImage img){
    cv::Mat imgMat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine());
    cv::Mat planes[4];
    cvtColor(imgMat,imgMat,CV_BGR2RGB);
    split(imgMat,planes);

    for (int channel = 0; channel < 3; ++channel){
        cv::Mat planes[4];
        split(imgMat,planes);
        cv::Mat mm;
        for (int i = 0; i < 3; ++i){
            if (channel == i)
                continue;
            planes[i] *= 0;
        }
        merge(planes,3,mm);


        QImage tmp = QImage((uchar*)mm.data,
                            mm.cols,
                            mm.rows,
                            mm.step,
                            QImage::Format_RGB888).copy();

        QLabel *label;
        switch(channel){
        case 0:
            label = ui->red;
            break;
        case 1:
            label = ui->green;
            break;
        case 2:
            label = ui->blue;
            break;
        }
        QSize labelSize = label->size();
        // set a scaled pixmap to a w x h window keeping its aspect ratio
        label->setPixmap(QPixmap::fromImage(tmp.scaled(labelSize.width(),labelSize.height(),
                                                       Qt::KeepAspectRatio)));
    }
}
