#ifndef DEFOCUSDLG_H
#define DEFOCUSDLG_H

#include <QDialog>

namespace Ui {
class defocusDlg;
}

class defocusDlg : public QDialog
{
    Q_OBJECT
    double m_defocusInmm;

public:
    explicit defocusDlg(QWidget *parent = nullptr);
    ~defocusDlg();
protected:
    void closeEvent(QCloseEvent *event);
    void moveEvent(QMoveEvent *event);
signals:
    void defocus(double def);
private slots:
    void on_multiplier_valueChanged(int arg1);

    void on_defocusVal_valueChanged(double arg1);

    void on_spinValue_valueChanged(double arg1);

    void on_defocusSlider_valueChanged(int value);

    void on_pushButton_clicked();

private:
    Ui::defocusDlg *ui;
};

#endif // DEFOCUSDLG_H
