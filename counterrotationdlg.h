#ifndef COUNTERROTATIONDLG_H
#define COUNTERROTATIONDLG_H

#include <QDialog>

namespace Ui {
class CounterRotationDlg;
}

class CounterRotationDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CounterRotationDlg( QString fn, double rot, bool dir, QWidget *parent = 0);
    ~CounterRotationDlg();
    QString getRotation();
    bool isClockwise();

private:
    Ui::CounterRotationDlg *ui;
};

#endif // COUNTERROTATIONDLG_H
