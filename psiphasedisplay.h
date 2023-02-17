#ifndef PSIPHASEDISPLAY_H
#define PSIPHASEDISPLAY_H

#include <QDialog>
#include <opencv2/opencv.hpp>

namespace Ui {
class PSIphaseDisplay;
}

class PSIphaseDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit PSIphaseDisplay( QVector<double> phases, QWidget *parent = 0);
    ~PSIphaseDisplay();
    void setPhases(QVector<double> phases, int iteration, double sdp);
    void useRadians(bool use);
private:
    Ui::PSIphaseDisplay *ui;
    void plot(QVector<double> phases, int iter, double sdp);
    bool m_useRadians;
};

#endif // PSIPHASEDISPLAY_H
