#ifndef CAMWIZARDPAGE1_H
#define CAMWIZARDPAGE1_H

#include <QWizardPage>
#include <QStringList>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
namespace Ui {
class CamWizardPage1;
}
enum Pattern { NOT_EXISTING, CHESSBOARD, CIRCLES_GRID, ASYMMETRIC_CIRCLES_GRID };
class CamWizardPage1 : public QWizardPage
{
    Q_OBJECT

public:
    explicit CamWizardPage1(QWidget *parent = 0);
    ~CamWizardPage1();
    QStringList fileList;
    bool runCalibration(cv::Size& imageSize, cv::Mat& cameraMatrix, cv::Mat& distCoeffs,
                                std::vector<std::vector<cv::Point2f> > imagePoints, std::vector<cv::Mat>& rvecs, std::vector<cv::Mat>& tvecs,
                                std::vector<float>& reprojErrs,  double& totalAvgErr);

    bool runCalibrationAndSave(cv::Size imageSize, cv::Mat&  cameraMatrix,
                                               cv::Mat& distCoeffs,std::vector<std::vector<cv::Point2f> > imagePoints );
    Ui::CamWizardPage1 *ui;
    cv::Mat m_coeffs;
private slots:
    void on_compute_clicked();
    void on_pushButton_clicked();
    void showContextMenu(const QPoint &pos);
    void eraseItem();
    void on_saveParams_clicked();
    void on_rows_valueChanged(int arg1);
    void on_columns_valueChanged(int arg1);
    void on_useCircleGrid_clicked(bool checked);
    void on_radioButton_2_clicked(bool checked);


    void on_GenerateImage_clicked();

    void on_pushButton_2_clicked();



    void on_currentLens_clicked();

private:
    void saveSettings();
    cv::Mat cameraMatrix, distCoeffs;
void showResuslts();
};

#endif // CAMWIZARDPAGE1_H
