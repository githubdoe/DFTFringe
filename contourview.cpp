/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "contourview.h"
#include "ui_contourview.h"
#include <QMenu>
#include "math.h"
#include "pixelstats.h"
#include <QSettings>

contourView::contourView(QWidget *parent, ContourTools *tools) :
    QWidget(parent),
    ui(new Ui::contourView), zoomed(false), tools(tools)
{
    ui->setupUi(this);
    ui->widget->setTool(tools);
    QSettings set;
    ui->doubleSpinBox->setValue(set.value("contourRange", .100).toDouble());
    ui->fillContourCB->setChecked(set.value("contourFillContour", true).toBool());
    ui->showRuler->setChecked(set.value("contourShowRuler",false).toBool());
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showContextMenu(QPoint)));
    ps = new pixelStats;
}

contourView::~contourView()
{
    delete ui;
}
void contourView::zoom(){
    zoomed = !zoomed;
    emit zoomMe(zoomed);
}

QImage contourView::getPixstatsImage(){
    QImage psImage = QImage(ps->size(),QImage::Format_ARGB32 );
    QPainter p3(&psImage);
    ps->render(&p3);

    return psImage;
}

void contourView::setSurface(wavefront *wf){
    getPlot()->setSurface(wf);
    ps->setData(wf);
}

void contourView::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    QString txt = (zoomed)? tr("Restore to MainWindow") : tr("FullScreen");
    myMenu.addAction(txt,  this, SLOT(zoom()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}
ContourPlot *contourView::getPlot(){
    return ui->widget;
}

void contourView::on_doubleSpinBox_valueChanged(double arg1)
{
    ui->widget->showContoursChanged(arg1);
    if (arg1 == 0.){
        ui->fillContourCB->setChecked(true);
        on_fillContourCB_clicked(true);
    }

}


void contourView::on_pushButton_pressed()
{
    emit showAllContours();
}
#include <qwt_plot_histogram.h>
#include <opencv2/highgui/highgui.hpp>
cv::Mat orientationMap(const cv::Mat& mag, const cv::Mat& ori, double thresh = 1.0)
{
    cv::Mat oriMap = cv::Mat::zeros(ori.size(), CV_8UC3);
    cv::Vec3b red(0, 0, 255);
    cv::Vec3b cyan(255, 255, 0);
    cv::Vec3b green(0, 255, 0);
    cv::Vec3b yellow(0, 255, 255);
    for(int i = 0; i < mag.rows*mag.cols; i++)
    {
        float* magPixel = reinterpret_cast<float*>(mag.data + i*sizeof(float));
        if(*magPixel > thresh)
        {
            float* oriPixel = reinterpret_cast<float*>(ori.data + i*sizeof(float));
            cv::Vec3b* mapPixel = reinterpret_cast<cv::Vec3b*>(oriMap.data + i*3*sizeof(char));
            if(*oriPixel < 90.0)
                *mapPixel = red;
            else if(*oriPixel >= 90.0 && *oriPixel < 180.0)
                *mapPixel = cyan;
            else if(*oriPixel >= 180.0 && *oriPixel < 270.0)
                *mapPixel = green;
            else if(*oriPixel >= 270.0 && *oriPixel < 360.0)
                *mapPixel = yellow;
        }
    }

    return oriMap;
}
void contourView::on_histogram_clicked()
{
    ps->show();

}

void contourView::on_fillContourCB_clicked(bool checked)
{
    QSettings set;
    ui->widget->showSpectrogram(checked);

}

void contourView::on_showRuler_clicked(bool checked)
{
    QSettings set;
    set.setValue("contourShowRuler", checked);
    if (getPlot()->m_wf)
        setSurface(getPlot()->m_wf);
}
