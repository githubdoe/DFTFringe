#ifndef CONTOURRULERPARAMS_H
#define CONTOURRULERPARAMS_H

#include <QDialog>

namespace Ui {
class ContourRulerParams;
}

class ContourRulerParams : public QDialog
{
    Q_OBJECT

public:
    explicit ContourRulerParams(QWidget *parent = 0);
    ~ContourRulerParams();

private:
    Ui::ContourRulerParams *ui;
};

#endif // CONTOURRULERPARAMS_H
