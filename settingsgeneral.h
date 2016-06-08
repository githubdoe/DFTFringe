#ifndef SETTINGSGENERAL_H
#define SETTINGSGENERAL_H

#include <QDialog>

namespace Ui {
class settingsGeneral;
}

class settingsGeneral : public QDialog
{
    Q_OBJECT

public:
    explicit settingsGeneral(QWidget *parent = 0);
    ~settingsGeneral();
    bool useRMS();

private:
    Ui::settingsGeneral *ui;
};

#endif // SETTINGSGENERAL_H
