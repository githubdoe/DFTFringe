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
    int m_radialDeg();
    QColor m_rulerColor();

private slots:
    void on_radialAngleSB_valueChanged(int arg1);

    void on_setColorPB_clicked();
signals:
    void updateParms();
private:
    Ui::ContourRulerParams *ui;
};

#endif // CONTOURRULERPARAMS_H
