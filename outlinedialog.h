#ifndef OUTLINEDIALOG_H
#define OUTLINEDIALOG_H

#include <QDialog>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv/cv.h"
#include "opencv/highgui.h"
namespace Ui {
class outlineDialog;
}

class outlineDialog : public QDialog
{
    Q_OBJECT
    int m_threshold;
    int m_blurrSize;
    double m_xoffset;
    double m_yoffset;
    double m_radiusOffset;
    double m_contrast;
    cv::Mat m_igram;
    bool m_findEdgePixels;
    bool m_find;
    bool m_hideOutline;
public:
    explicit outlineDialog(double x, double y, double rad, QWidget *parent = 0);
    ~outlineDialog();
    double m_x, m_y, m_rad;
    cv::Mat img;
    void setImage(cv::Mat img);
    void updateOutline();
    void updateDisplay(cv::Mat img);


private slots:
    void on_blurSlider_valueChanged(int value);

    void on_thresholdSlider_valueChanged(int value);

    void on_blurSB_valueChanged(int arg1);

    void on_contrastSlider_valueChanged(int value);

    void on_radiusoffset_valueChanged(double arg1);

    void on_showEdgePixelsCB_clicked(bool checked);

    void on_YOffsetSBar_valueChanged(int value);

    void on_XoffsetSBar_valueChanged(int value);

    void on_xoffsetSB_valueChanged(int arg1);

    void on_yoffsetSb_valueChanged(int arg1);

    void on_radiusSBar_valueChanged(int value);

    void accept();

    void on_pushButton_clicked();

    void on_checkBox_clicked(bool checked);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent * event);

    void shiftDown();
    void shiftUp();
    void shiftLeft();
    void shiftRight();
    void increase();
    void decrease();

    void on_outlineThickness_valueChanged(int arg1);

private:
    Ui::outlineDialog *ui;
    void hideSearchcontrole(bool hide);
    bool dragMode;
    QPointF lastPoint;
};

#endif // OUTLINEDIALOG_H
