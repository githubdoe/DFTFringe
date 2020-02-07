#ifndef PSIPHASEDISPLAY_H
#define PSIPHASEDISPLAY_H

#include <QDialog>

namespace Ui {
class PSIphaseDisplay;
}

class PSIphaseDisplay : public QDialog
{
    Q_OBJECT

public:
    explicit PSIphaseDisplay(QWidget *parent = 0);
    ~PSIphaseDisplay();

private:
    Ui::PSIphaseDisplay *ui;
};

#endif // PSIPHASEDISPLAY_H
