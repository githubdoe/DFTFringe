#include "unwraperrorsview.h"
#include "ui_unwraperrorsview.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QImageWriter>
#include <QApplication>
#include <QDesktopWidget>
using namespace cv;
unwrapErrorsView::unwrapErrorsView(const wavefront &wf, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::unwrapErrorsView), m_wf(wf)
{
    ui->setupUi(this);
    ui->view->setScaledContents(true);
    //ui->view->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
    createUnwrapErrors();
}
void unwrapErrorsView::createUnwrapErrors(){
    int cnt = 0;
    Mat errorView= Mat::zeros(m_wf.data.size(), CV_8UC1);
    for (int y = 0; y < m_wf.data.rows-1; ++y){
        for (int x = 0; x < m_wf.data.cols-1; ++x){
            if (m_wf.workMask.at<uint8_t>(y,x) != 0){
                double v1 = m_wf.data.at<double>(y,x);
                double v2 = m_wf.data.at<double>(y,x+1);
                double v3 = m_wf.data.at<double>(y+1,x);
                if ((fabs(v1 - v2) > .5) || (fabs(v1 - v3) > .5)){
                    errorView.at<char>(y,x) = 255;
                    ++cnt;
                }
            }
            else {
                errorView.at<char>(y,x) = 100;
            }

        }
    }

    ui->count->setText(QString().sprintf("%d",cnt));
    Mat xxx;
    flip(errorView, xxx, 0);

    cv::cvtColor(xxx,xxx, cv::COLOR_GRAY2RGB);
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = rec.height();

    double scale = (double)(height-200)/(double)xxx.rows;
    if (scale < 1.){
        cv::resize(xxx,xxx,Size(0,0), scale, scale);
    }
    QImage tmp = QImage((uchar*)xxx.data,
                        xxx.cols,
                        xxx.rows,
                        xxx.step,
                        QImage::Format_RGB888).copy();
    // set a scaled pixmap to a w x h window keeping its aspect ratio
    ui->view->resize(errorView.cols, errorView.rows);
    ui->view->setPixmap(QPixmap::fromImage(tmp));

}

unwrapErrorsView::~unwrapErrorsView()
{
    delete ui;
}

void unwrapErrorsView::on_save_clicked()
{
    QSettings set;
    QString path = set.value("mirrorConfigFile").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString dd = info.dir().absolutePath();
    const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList filter;
    if ( imageFormats.size() > 0 )
    {
        QString imageFilter( tr( "Images" ) );
        imageFilter += " (";
        for ( int i = 0; i < imageFormats.size(); i++ )
        {
            if ( i > 0 )
                imageFilter += " ";
            imageFilter += "*.";
            imageFilter += imageFormats[i];
        }
        imageFilter += ")";

        filter += imageFilter;
    }
    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save as Image"), dd + "//unwrapErrors.png",filter.join( ";;" ));


    if (fName.isEmpty())
        return;

    grab().save(fName);
}
