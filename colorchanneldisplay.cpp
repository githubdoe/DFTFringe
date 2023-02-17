/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "colorchanneldisplay.h"
#include "ui_colorchanneldisplay.h"
#include <opencv2/opencv.hpp>

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
void ColorChannelDisplay::setImage(cv::Mat imgMat){

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
