#include "showaliasdlg.h"
#include "ui_showaliasdlg.h"
#include <QPixmap>
#include <QtWidgets>
#include "opencv2/imgproc/imgproc.hpp"
#include <QSettings>
class ImageViewer : public QWidget {
    QPixmap m_pixmap;
    QRectF m_rect;
    QPointF m_reference;
    QPointF m_delta;
    qreal m_scale = 1.0;
    void paintEvent(QPaintEvent *) override {
        QPainter p{this};
        p.translate(rect().center());
        p.scale(m_scale, m_scale);
        p.translate(m_delta);
        p.drawPixmap(m_rect.topLeft(), m_pixmap);
    }
    void mousePressEvent(QMouseEvent *event) override {
        m_reference = event->pos();
        qApp->setOverrideCursor(Qt::ClosedHandCursor);
        setMouseTracking(true);
    }
    void mouseMoveEvent(QMouseEvent *event) override {
        m_delta += (event->pos() - m_reference) * 1.0/m_scale;
        m_reference = event->pos();
        update();
    }
    void mouseReleaseEvent(QMouseEvent *) override {
        qApp->restoreOverrideCursor();
        setMouseTracking(false);
    }
    void wheelEvent(QWheelEvent * e) override{
        int del = e->delta()/120;
        if (del < 0)
            scale(.8);
        else scale(1.2);
    }

public:
    void setImage(cv::Mat mat){
        cv::Mat rgb;
        cv::cvtColor(mat,rgb, cv::COLOR_GRAY2RGB);
        QImage img= QImage((uchar*) rgb.data, rgb.cols, rgb.rows, rgb.step,
                            QImage::Format_RGB888).copy();
        m_pixmap = QPixmap::fromImage(img);
        m_rect = m_pixmap.rect();
        m_rect.translate(-m_rect.center());
        update();
    }
    void scale(qreal s) {
        m_scale *= s;
        update();
    }
    //QSize sizeHint() const override { return {400, 400}; }
};
showAliasDlg::showAliasDlg(int size, QWidget *parent) :
    QDialog(parent),resizedValue(size),    ui(new Ui::showAliasDlg)
{

    ui->setupUi(this);
    m_contrast = 10.;
    ui->resizedSB->blockSignals(true);
    ui->resizedSB->setValue(size);

    ui->resizedSB->blockSignals(false);
    originalView = new ImageViewer;
    ui->originalSscroleArea->setWidget(originalView);
    resizedView = new ImageViewer;
    ui->resizedScrollArea->setWidget(resizedView);
    resizedValue = size;

}

showAliasDlg::~showAliasDlg()
{
    delete ui;
}
 void showAliasDlg::setImage(QImage &img){

     m_img = img.copy();
     ui->resizedSB->setMaximum(img.width());
     contrast();

     ui->originalLb->setText( QString().sprintf("Original %dx%d", img.width(),img.height()));
     downSize();

 }
void showAliasDlg::contrast(){
    QImage temp = m_img.copy();
    cv::Mat res(temp.height(),temp.width(),CV_8UC4,(uchar*)temp.bits(),temp.bytesPerLine());

    cv::cvtColor(res,gray, cv::COLOR_RGBA2GRAY);

    for( int y = 0; y < gray.rows; y++ ) {
        for( int x = 0; x < gray.cols; x++ ) {
            gray.at<uchar>(y,x) = cv::saturate_cast<uchar>( m_contrast*( gray.at<uchar>(y,x) ));
        }
    }
    originalView->setImage(gray);
}
void showAliasDlg::downSize(){
   cv::Mat small = gray.clone();


   cv::resize(small, small, cv::Size(resizedValue,resizedValue) , CV_INTER_NN);
   cv::resize(small,small, cv::Size(gray.cols, gray.rows), CV_INTER_NN);
   //cv::GaussianBlur(small,small, cv::Size(5, 5),0,0);
   resizedView->setImage(small);
}
void showAliasDlg::on_contrastSlider_sliderMoved(int position)
{
    m_contrast = 20. * position/100.;
    contrast();
    downSize();
}

void showAliasDlg::on_resizedSB_valueChanged(int arg1)
{
    resizedValue = arg1;
    downSize();
}


void showAliasDlg::on_origZommIn_pressed()
{
    originalView->scale(1.2);
}

void showAliasDlg::on_origZoomOut_pressed()
{
    originalView->scale(.8);
}

void showAliasDlg::on_resizedZoomIn_pressed()
{
    resizedView->scale(1.2);
}

void showAliasDlg::on_resizedZoomOut_pressed()
{
    resizedView->scale(.8);
}

void showAliasDlg::on_pushButton_2_pressed()
{
    QString msg("This dialaog shows the difference from the full size igram to the size the DFT will resize it too if needed.\n You can Zoom in with"
                " the scroll wheel and pan with the left mouse button.\n\n There is a trade off between resolution and memory usage.\n"
                "You can reduce the size of the wavefront if you reduce the size of the igram.  However that might cause the fringes to be"
                "too narrow.\n  Inspect the resized igram to make sure the fringes stay at least 3 pixels wide everwhere.");
    QMessageBox::information(this, "Show Resized Interferogram Help", msg);
}
