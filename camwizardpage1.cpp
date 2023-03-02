#include "camwizardpage1.h"
#include "ui_camwizardpage1.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <QImageReader>
#include <QFileDialog>
#include <vector>
#include <QDebug>
#include "camcalibrationreviewdlg.h"
#include <sstream>
#include <QMessageBox>
#include <QMenu>
#include <QSettings>
#include <QInputDialog>
#include "settings2.h"
#include <opencv2/core/types_c.h>
#include <opencv2/core/core_c.h>


using namespace cv;
namespace cv
{
    using std::vector;
}
CamWizardPage1::CamWizardPage1(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::CamWizardPage1)
{
    ui->setupUi(this);
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    QSettings set;
    Pattern p = (Pattern)(set.value("camCalibratePattern", CIRCLES_GRID).toInt());
    ui->columns->blockSignals(true);
    ui->useCircleGrid->blockSignals(true);
    ui->rows->blockSignals(true);
    ui->radioButton_2->blockSignals(true);

    ui->useCircleGrid->setChecked(p == CIRCLES_GRID);
    ui->radioButton_2->setChecked(p != CIRCLES_GRID);
    ui->rows->setValue(set.value("camCalibrateRows", 8).toInt());
    ui->columns->setValue(set.value("camCalibrateCols", 8).toInt());

    ui->columns->blockSignals(false);
    ui->useCircleGrid->blockSignals(false);
    ui->rows->blockSignals(false);
    ui->radioButton_2->blockSignals(false);

    cameraMatrix = Mat::zeros(3,3,CV_32F);
    distCoeffs = Mat::zeros(1,5,CV_32F);
    //resize( QSize(600, 489).expandedTo(minimumSizeHint()) );
}

CamWizardPage1::~CamWizardPage1()
{
    delete ui;
}

static void calcBoardCornerPositions(cv::Size boardSize, float squareSize, std::vector<cv::Point3f>& corners,
                                     Pattern patternType /*= Settings::CHESSBOARD*/)
{
    corners.clear();
    switch(patternType)
    {
    case CHESSBOARD:
    case CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; ++i )
            for( int j = 0; j < boardSize.width; ++j )
                corners.push_back(cv::Point3f(float( j*squareSize ), float( i*squareSize ), 0));
        break;

    case ASYMMETRIC_CIRCLES_GRID:
        for( int i = 0; i < boardSize.height; i++ )
            for( int j = 0; j < boardSize.width; j++ )
                corners.push_back(cv::Point3f(float((2*j + i % 2)*squareSize), float(i*squareSize), 0));
        break;
    default:
        break;
    }
}

double computeReprojectionErrors( const std::vector<std::vector<cv::Point3f> >& objectPoints,
                                         const std::vector<std::vector<cv::Point2f> >& imagePoints,
                                         const std::vector<cv::Mat>& rvecs, const std::vector<cv::Mat>& tvecs,
                                         const cv::Mat& cameraMatrix , const cv::Mat& distCoeffs,
                                         std::vector<float>& perViewErrors)
{
    std::vector<cv::Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    perViewErrors.resize(objectPoints.size());

    for( i = 0; i < (int)objectPoints.size(); ++i )
    {
        projectPoints( cv::Mat(objectPoints[i]), rvecs[i], tvecs[i], cameraMatrix,
                       distCoeffs, imagePoints2);
        err = cv::norm(cv::Mat(imagePoints[i]), cv::Mat(imagePoints2), CV_L2);

        int n = (int)objectPoints[i].size();
        perViewErrors[i] = (float) std::sqrt(err*err/n);
        totalErr        += err*err;
        totalPoints     += n;
    }

    return std::sqrt(totalErr/totalPoints);
}
bool CamWizardPage1::runCalibration( cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
                            std::vector<std::vector<cv::Point2f> > imagePoints, std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
                            std::vector<float>& reprojErrs,  double& totalAvgErr)
{

    cameraMatrix = cv::Mat::eye(3, 3, CV_64FC1);
    //if( s.flag & CV_CALIB_FIX_ASPECT_RATIO )
        cameraMatrix.at<double>(0,0) = 1.0;

    distCoeffs = cv::Mat::zeros(8, 1, CV_64FC1);

    cv::vector<cv::vector<cv::Point3f> > objectPoints(1);
    calcBoardCornerPositions( cv::Size(ui->columns->value(), ui->rows->value()), 1,
                              objectPoints[0], ui->useCircleGrid ->isChecked() ? CIRCLES_GRID   : CHESSBOARD);

    objectPoints.resize(imagePoints.size(),objectPoints[0]);

    //Find intrinsic and extrinsic camera parameters
    double rms = cv::calibrateCamera(objectPoints, imagePoints, imageSize, cameraMatrix,
                                 distCoeffs, rvecs, tvecs, cv::CALIB_ZERO_TANGENT_DIST|
                                 cv::CALIB_FIX_PRINCIPAL_POINT|cv::CALIB_FIX_ASPECT_RATIO|
                                 cv::CALIB_FIX_K4|cv::CALIB_FIX_K5);

    ui->Results->append( "\nRe-projection error reported by calibrateCamera: " + QString().number( rms) + "\n");

    bool ok = checkRange(cameraMatrix) && checkRange(distCoeffs);

    totalAvgErr = computeReprojectionErrors(objectPoints, imagePoints,
                                             rvecs, tvecs, cameraMatrix, distCoeffs, reprojErrs);

    return ok;
}
void CamWizardPage1::eraseItem()
{
    // If multiple selection is on, we need to erase all selected items
    for (int i = 0; i < ui->listWidget->selectedItems().size(); ++i) {
        // Get curent item on selected row
        QListWidgetItem *item = ui->listWidget->takeItem(ui->listWidget->currentRow());
        // And remove it
        delete item;
    }
}

void CamWizardPage1::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = ui->listWidget->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

bool CamWizardPage1::runCalibrationAndSave(cv::Size imageSize, cv::Mat&  cameraMatrix, cv::Mat& distCoeffs,std::vector<std::vector<cv::Point2f> > imagePoints )
{
    std::vector<cv::Mat> rvecs, tvecs;
    std::vector<float> reprojErrs;
    double totalAvgErr = 0;

    bool ok = runCalibration(imageSize, cameraMatrix, distCoeffs, imagePoints, rvecs, tvecs,
                             reprojErrs, totalAvgErr);
    if (!ok){
        QString msg = QString().sprintf("Calibration failed. avg re projection error = %lf", totalAvgErr);

       QMessageBox::warning(0,"warning", msg);
       return !ok;
    }
    ui->Results->append(QString().sprintf("total average error %lf\n",totalAvgErr));
    ui->Results->append(ok ? QString("Calibration succeeded") : QString("Calibration failed"));

    return ok;
}







#include <QTextStream>
bool xfindChessboardCorners( InputArray _image, Size patternSize,
                            OutputArray corners, int flags );
QTextStream out(stdout);
void CamWizardPage1::on_compute_clicked()
{
    //APS-C (23.7 x 15.5 mm)  D40 sensor
    // 3008 pixels wide
    //focal_pixel = (focal_mm / sensor_width_mm) * image_width_in_pixels

    if (ui->listWidget->count() == 0){
        QMessageBox::warning(0,"warning", "First add some calibration images.");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->Results->clear();
    cv::Size imageSize;
    std::vector<std::vector<cv::Point2f> > imagePoints;

    std::vector<cv::Point2f> pointBuf;
    std::vector<cv::Mat> images;
    std::vector<cv::Mat> keypoints;
    for (int i = 0; i < ui->listWidget->count(); i++) {
        cv::Mat view;
        QString fileName = ui->listWidget->item(i)->text();
        view = cv::imread(fileName.toStdString().c_str());
        cv::Mat raw = view.clone();

        imageSize = view.size();  // Format input image.
        images.push_back(view.clone());
        cv::Size size(ui->columns->value(),ui->rows->value());
        bool found = false;
        if (ui->useCircleGrid->isChecked()){
            cv::Mat gray;
            cvtColor(view, gray, cv::COLOR_BGR2GRAY);
            //cv::threshold(gray,gray, 200, 255, cv::THRESH_BINARY);
            cv::Mat dial;
            cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,
                                                         cv::Size( 3, 3 ));
            cv::Mat erod;
            cv::erode(gray, erod, element);
            for (int i = 1; i < 30; ++i){
                cv::Mat element2 = cv::getStructuringElement( cv::MORPH_RECT,
                                                              cv::Size( 2 * i + 1, 2 * i + 1));

                cv::dilate(erod, dial, element2);

                found = cv::findCirclesGrid( dial, size, pointBuf );

                if (found)
                    break;
                if ( i == 1){
                    cv::SimpleBlobDetector detector;

                    // Detect blobs.
                    std::vector<cv::KeyPoint> keypoints;
                    detector.detect( dial, keypoints);

                    int horz = 1;
                    int vert = 1;
                    for (int ndx = 1; ndx < keypoints.size(); ++ndx){
                        float del = keypoints[ndx-1].pt.x - keypoints[ndx].pt.x;
                        if (del > 0) {
                            ++horz;
                        }
                        else {

                            horz = 1;
                            ++vert;
                        }
                    }

                }
            }
            if (!found){
                view = dial.clone();

            }
        }
        else {
            cv::Mat viewGray;
            cv::cvtColor(view, viewGray, cv::COLOR_BGR2GRAY);
//            IplImage _img = viewGray;

//            int c = cvCheckChessboard(&_img,size);
            Size deltas[] = {Size(0,0), Size(-1,0), Size(-1,-1), Size(-1,1),
                             Size(0,-1), Size(1,0), Size(1,-1),Size(1,1)};
            Size sizex;
            for (int i = 0; i < 8; ++i){
                sizex.width = size.width + deltas[i].width;
                sizex.height = size.height +  deltas[i].height;

                ui->Results->append(QString().sprintf("trying %dX%d", sizex.width,sizex.height));
                qApp->processEvents();
                pointBuf.clear();
                found = cv::findChessboardCorners( viewGray.clone(), sizex, pointBuf,
                cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FAST_CHECK | cv::CALIB_CB_NORMALIZE_IMAGE);
                if (found){
                    size = sizex;
                    ui->columns->setValue(size.width);
                    ui->rows->setValue(size.height);
                    ui->Results->append(" Found.\n");
                    break;
                }
                ui->Results->append(" Not Found.\n");
                qApp->processEvents();
//                vector<CBQuad *> quads;
//                vector<Point2f> corners;
//                int corner_count;
//                //me2FindChessboardCorners( viewGray.clone(), sizex, corners, &corner_count,
//                  //CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FAST_CHECK | CV_CALIB_CB_NORMALIZE_IMAGE);

            }
            // improve the found corners' coordinate accuracy for chessboard
            if (found){
                cv::cornerSubPix( viewGray, pointBuf, cv::Size(11,11),
                                  cv::Size(-1,-1), cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 30, 0.1 ));
            }
        }

        if (found){
            imagePoints.push_back(pointBuf);
            // Draw the corners.
            cv::Mat view2 = view.clone();
            view2.setTo(cv::Scalar(255,255,255));
            double alpha  = .1;
            double beta = ( 1.0 - alpha );

            drawChessboardCorners( view2, size, cv::Mat(pointBuf), found );
            cv::Mat dst;
            addWeighted( view, alpha, view2, beta, 0.0, dst);
            keypoints.push_back(dst.clone());
        }

        //imagemgic convert CirclesGrid2.jpg -distort barrel "0.06335 -0.18432 -0.13009" x.jpg

        if (!found){
            std::stringstream msg;
            msg << "Grid of " << ui->columns->value() << " X " << ui->rows->value() << " not found. Press any key.";
            ui->Results->append("Pattern not found. Calibration failed.");
            // Set up the detector with default parameters.
            cv::SimpleBlobDetector detector;

            cv::threshold(raw,view, 200, 255, cv::THRESH_BINARY);

            // Detect blobs.
            std::vector<cv::KeyPoint> keypoints;
            detector.detect( view, keypoints);

            // Draw detected blobs as red circles.
            // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
            cv::Mat im_with_keypoints;
            cv::drawKeypoints( view, keypoints, im_with_keypoints, cv::Scalar(0,0,255), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
            QApplication::restoreOverrideCursor();
            // Show blobs
            cv::imshow(msg.str().c_str(), im_with_keypoints );
            cv::waitKey();
            cv::destroyAllWindows();

        }


    }
    QApplication::restoreOverrideCursor();
    if (imagePoints.size() == 0){

        return;
    }


    runCalibrationAndSave( imageSize,  cameraMatrix, distCoeffs, imagePoints);


    cv::Mat rview, map1, map2;
//    cv::initUndistortRectifyMap(cameraMatrix, distCoeffs, cv::Mat(),
//        cv::getOptimalNewCameraMatrix(cameraMatrix, distCoeffs, imageSize, 1, imageSize, 0),
//        imageSize, CV_16SC2, map1, map2);

    std::vector<cv::Mat> corrected;
    for (int i = 0; i < ui->listWidget->count(); i++) {
        cv::Mat temp = images[i].clone();
        //cv::remap(temp, rview, map1, map2, cv::INTER_LINEAR);
        cv::undistort(temp, rview, cameraMatrix, distCoeffs);
        corrected.push_back(rview.clone());
    }
    CamCalibrationReviewDlg dlg( images,keypoints, corrected );
    dlg.exec();
    ui->saveParams->setEnabled(true);

    showResuslts();
}


void CamWizardPage1::on_pushButton_clicked()
{

    QFileDialog dialog(this, tr("Select calibartion images"));
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);


    if (dialog.exec()){
        ui->listWidget->addItems(dialog.selectedFiles());
    }
}
void CamWizardPage1::saveSettings(){
    QSettings set;
    set.setValue("camCalibratePattern", ui->useCircleGrid->isChecked() ? CIRCLES_GRID : CHESSBOARD);
    set.setValue("camCalibrateRows", ui->rows->value());
    set.setValue("camCalibrateCols", ui->columns->value());

}

void CamWizardPage1::on_saveParams_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("Lens name"),
                                         tr("Lens name:"), QLineEdit::Normal,
                                         "lens210", &ok);
    if (ok && !text.isEmpty()){
        std::stringstream msg;
        msg << text.toStdString() << ",";
        msg << ui->K1->text().toStdString().c_str() << ",";
        msg << ui->K2->text().toStdString().c_str() << ",";
        msg << ui->P1->text().toStdString().c_str() << ",";
        msg << ui->P2->text().toStdString().c_str() << ",";
        msg << ui->K3->text().toStdString().c_str() << ",";
        msg << ui->xFv->text().toStdString().c_str() << ",";
        msg << ui->XFc->text().toStdString().c_str() << ",";
        msg << ui->YFc->text().toStdString().c_str() << std::endl;
        QString parms(msg.str().c_str());
        Settings2::getInstance()->m_igram->updateLenses(parms);
    }


}

void CamWizardPage1::on_rows_valueChanged(int arg1)
{
    saveSettings();
}

void CamWizardPage1::on_columns_valueChanged(int arg1)
{
    saveSettings();
}

void CamWizardPage1::on_useCircleGrid_clicked(bool checked)
{
    saveSettings();
}

void CamWizardPage1::on_radioButton_2_clicked(bool checked)
{
    saveSettings();
}



#include "generatetargetdlg.h"
void CamWizardPage1::on_GenerateImage_clicked()
{
   generateTargetDlg dlg;
   dlg.exec();
}
#include <QDesktopServices>
void CamWizardPage1::on_pushButton_2_clicked()
{
    QString link = qApp->applicationDirPath() + "/res/Help/lensDistort.html";
    QDesktopServices::openUrl(QUrl(link));
}

void CamWizardPage1::showResuslts()
{
    QStringList parms;
    ui->xFv->setText(QString().number(cameraMatrix.at<double>(0,0)));
    ui->yFv->setText(QString().number(cameraMatrix.at<double>(1,1)));
    ui->XFc->setText(QString().number(cameraMatrix.at<double>(0,2)));
    ui->YFc->setText(QString().number(cameraMatrix.at<double>(1,2)));

    ui->K1->setText(QString().number(distCoeffs.at<double>(0,0)));
    ui->K2->setText(QString().number(distCoeffs.at<double>(0,1)));
    ui->P1->setText(QString().number(distCoeffs.at<double>(0,2)));
    ui->P2->setText(QString().number(distCoeffs.at<double>(0,3)));
    ui->K3->setText(QString().number(distCoeffs.at<double>(0,4)));

}

void CamWizardPage1::on_currentLens_clicked()
{
    QStringList parms = Settings2::getInstance()->m_igram->m_lenseParms;
    ui->xFv->setText(parms[6]);
    ui->yFv->setText(parms[6]);
    ui->XFc->setText(parms[7]);
    ui->YFc->setText(parms[8]);

    ui->K1->setText(parms[1]);
    ui->K2->setText(parms[2]);
    ui->P1->setText(parms[3]);
    ui->P2->setText(parms[4]);
    ui->K3->setText(parms[5]);
}
