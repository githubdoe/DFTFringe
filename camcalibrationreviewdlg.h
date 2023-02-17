#ifndef CAMCALIBRATIONREVIEWDLG_H
#define CAMCALIBRATIONREVIEWDLG_H

#include <QDialog>
#include <vector>
#include <opencv2/opencv.hpp>

namespace Ui {
class CamCalibrationReviewDlg;
}

class CamCalibrationReviewDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CamCalibrationReviewDlg(    std::vector<cv::Mat> images,    std::vector<cv::Mat> keypoints, std::vector<cv::Mat> corrected,QWidget *parent = 0);
    ~CamCalibrationReviewDlg();
    virtual void resizeEvent( QResizeEvent * );
private slots:
    void on_verticalScrollBar_valueChanged(int value);

    void on_Overlay_pressed();

    void on_Overlay_released();

private:
    int m_ndx;
    std::vector<cv::Mat> m_images;
    std::vector<cv::Mat> m_keypoints;
    std::vector<cv::Mat> m_corrected;
    Ui::CamCalibrationReviewDlg *ui;
    bool m_overlay;
    int m_viewType;

    void showImage(int ndx);
};

#endif // CAMCALIBRATIONREVIEWDLG_H
