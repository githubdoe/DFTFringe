#ifndef RONCHICOMPAREDIALOG_H
#define RONCHICOMPAREDIALOG_H

#include <QDialog>
#include <QImage>
#include <QLabel>
#include <QSlider>
#include <opencv2/opencv.hpp>

class RonchiCompareDialog : public QDialog {
    Q_OBJECT

public:
    explicit RonchiCompareDialog(const QImage& img1, const QString& name1, const QImage &img2,
                                                      const QString& name2, QWidget* parent);
    virtual ~RonchiCompareDialog();

private slots:
    void updateOverlay(int val);
    void onSaveClicked();

private:
    cv::Mat qImageToMat(const QImage& image);

    QImage m_q1;
    QImage m_q2;
    cv::Mat m_currentMat;

    QLabel* m_displayLabel;
    QLabel* m_baseLabel;
    QLabel* m_compLabel;
};

#endif // RONCHICOMPAREDIALOG_H
