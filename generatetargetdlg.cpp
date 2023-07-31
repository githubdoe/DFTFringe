#include "generatetargetdlg.h"
#include "ui_generatetargetdlg.h"
#include <QDesktopWidget>
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDebug>
#include <QSettings>
using namespace cv;
using namespace std;

generateTargetDlg::generateTargetDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::generateTargetDlg), m_units(INCHES)
{
    ui->setupUi(this);
    m_dpix = qApp->desktop()->physicalDpiX();
    m_dpiy = qApp->desktop()->physicalDpiY();
    ui->printerGroup->hide();
    QSettings set;
    ui->rows->setValue(set.value("target rows", 10).toInt());
}

generateTargetDlg::~generateTargetDlg()
{
    delete ui;

}

void generateTargetDlg::on_generate_clicked()
{
    QSettings set;
    set.setValue("target rows", ui->rows->value());
    int blockSize = m_dpix/2;
    int hsize;
    int vsize;

    if (ui->fullScreen->isChecked()){
        // make sure sizes are even
       hsize = qApp->desktop()->width();
       hsize/=2;
       hsize *= 2;

       vsize = qApp->desktop()->height();
       vsize /=2;
       vsize *= 2;



    }
    else {
        double mul = 1.;
        switch (m_units){
        case MM:
            mul = 96./25.4;
            break;
        case INCHES:
            mul = 96;
            break;
        case PIXELS:

            break;
        }

        hsize = ui->width->value() * mul;
        vsize = ui->height->value() * mul;
    }
    blockSize = min(hsize,vsize) / (ui->rows->value());
    int blocks = hsize/blockSize;
    hsize = blocks * blockSize;
    blocks = vsize/blockSize;
    vsize = blocks * blockSize;


    Mat chessBoard(vsize,hsize,CV_8UC1,Scalar(255));
    unsigned char color=0;

    int vcnt = 0;
    int hcnt = 0;
    int lastx = hsize - (ui->useChecker->isChecked() ? blockSize : .5 * blockSize);
    int lasty = vsize - (ui->useChecker->isChecked() ? blockSize : .5 * blockSize);
    for(int i=blockSize/2;   i<= lasty; i=i+blockSize){
        ++vcnt;
        hcnt = 0;
        for(int j= blockSize/2;    j <= lastx; j=j+blockSize){
            ++hcnt;
            if (ui->useChecker->isChecked()){
                 Mat ROI=chessBoard(Rect(j,i,blockSize,blockSize));
                 ROI.setTo(Scalar::all(color));
                 color=~color;
            }
            else if (ui->lineGridRb->isChecked()){
                line(chessBoard, Point(j,i),Point(hsize-blockSize/2, i),Scalar(0,0,0),2);
                line(chessBoard, Point(j,blockSize/2), Point(j,vsize-blockSize/2), Scalar(0,0,0),2);
            }
            else {
                circle(chessBoard, Point(j ,i), blockSize/3, Scalar(0,0,0), -1);
            }
        }
        if ( (hcnt %2) == 0)
           color=~color;
    }

    if (ui->useChecker->isChecked()){
        --hcnt;
        --vcnt;
    }
    putText(chessBoard, QString("%1 X %2").arg(hcnt).arg(vcnt).toStdString().c_str(),
            Point(40,20),1,1,cv::Scalar(100, 100,100));
    //pyrDown(chessBoard,chessBoard);
    imshow("Grid pattern", chessBoard);
    waitKey(1);

/*  debug to see contours of grid
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    /// Find contours

    findContours( chessBoard.clone(), contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
    RNG rng(12345);
    /// Draw contours
    Mat drawing = Mat::zeros( chessBoard.size(), CV_8UC3 );
    for( int i = 0; i< contours.size(); i++ )
       {
         Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
         drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
       }

    /// Show in a window
    namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
    waitKey(1);
*/
}

void generateTargetDlg::on_printed_clicked(bool checked)
{
    if (!checked)
        ui->printerGroup->hide();
    else
        ui->printerGroup->show();
    on_generate_clicked();
}

void generateTargetDlg::on_fullScreen_clicked(bool checked)
{
    if (checked)
        ui->printerGroup->hide();
    else
        ui->printerGroup->show();
    on_generate_clicked();
}



void generateTargetDlg::on_rows_valueChanged(int /*arg1*/)
{
    on_generate_clicked();
}

void generateTargetDlg::on_useChecker_toggled(bool /*checked*/)
{
    on_generate_clicked();
}

void generateTargetDlg::on_lineGridRb_clicked()
{
    on_generate_clicked();
}

void generateTargetDlg::on_usePixels_clicked(bool /*checked*/)
{
    double mul = 1.;
    switch (m_units){
    case MM:
        mul = 96./25.4;
        break;

    case INCHES:
        mul = 96.;
        break;
    case PIXELS:
        break;
    }

    ui->width->setValue((int)(ui->width->value() * mul));
    ui->height->setValue((int)(ui->height->value() * mul));

    m_units = PIXELS;
}

void generateTargetDlg::on_useInches_clicked(bool /*checked*/)
{
    double mul = 1.;
    switch (m_units){
    case MM:
        mul = 1./25.4;
        break;
    case INCHES:
        break;
    case PIXELS:
        mul = 1./96.;
        break;
    }
    ui->width->setValue(ui->width->value() * mul);
    ui->height->setValue(ui->height->value() * mul);

     m_units = INCHES;

}

void generateTargetDlg::on_useMm_clicked(bool /*checked*/)
{
    double mul = 1.;
    switch (m_units){
    case MM:
        break;
    case INCHES:
        mul = 25.4;
        break;
    case PIXELS:
        mul = 25.4/96.;
        break;
    }

    ui->width->setValue(ui->width->value() * mul);
    ui->height->setValue(ui->height->value() * mul);
    m_units = MM;
}
