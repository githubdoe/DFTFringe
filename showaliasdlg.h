#ifndef SHOWALIASDLG_H
#define SHOWALIASDLG_H

#include <QDialog>
#include <QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

namespace Ui {
class showAliasDlg;
}
class ImageViewer;
class showAliasDlg : public QDialog
{
    Q_OBJECT

public:
    explicit showAliasDlg(int size,QWidget *parent = 0);
    ~showAliasDlg();
    int resizedValue;
    double m_contrast;
    void setImage(QImage &img);
    void downSize();

private slots:
    void on_contrastSlider_sliderMoved(int position);

    void on_resizedSB_valueChanged(int arg1);

    void on_origZommIn_pressed();

    void on_origZoomOut_pressed();

    void on_resizedZoomIn_pressed();

    void on_resizedZoomOut_pressed();

    void on_pushButton_2_pressed();

private:
    Ui::showAliasDlg *ui;
    ImageViewer *originalView;
    ImageViewer *resizedView;
    ImageViewer *diffView;
    cv::Mat gray;
    cv::Mat downSized;
    cv::Mat diff;
    QImage m_img;
    void contrast();
};

#endif // SHOWALIASDLG_H
