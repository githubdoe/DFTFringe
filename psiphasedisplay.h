#ifndef PSIPHASEDISPLAY_H
#define PSIPHASEDISPLAY_H

#include <QDialog>
#include "opencv/cv.h"

namespace Ui {
class PSIphaseDisplay;
}

class PSIphaseDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit PSIphaseDisplay( QVector<double> phases, QWidget *parent = 0);
    ~PSIphaseDisplay();

private:
    Ui::PSIphaseDisplay *ui;
    void plot(QVector<double> phases);
};

#endif // PSIPHASEDISPLAY_H
