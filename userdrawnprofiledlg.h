#ifndef USERDRAWNPROFILEDLG_H
#define USERDRAWNPROFILEDLG_H

#include <QDialog>
#include <arbitrarywavwidget.h>
namespace Ui {
class UserDrawnProfileDlg;
}

class UserDrawnProfileDlg : public QDialog
{
    Q_OBJECT
protected:
    static UserDrawnProfileDlg *m_instance;
public:
    explicit UserDrawnProfileDlg(QWidget *parent = nullptr);
    ~UserDrawnProfileDlg();
    static UserDrawnProfileDlg* get_instance();
    double getValue(double rho);
    void prepare(int size);
    void setDiameter(double _mirror_diam);
    bool bOkPressed = false;
private slots:
    void on_cmbUnits_currentIndexChanged(int index);

    void on_load_clicked();

    void on_save_clicked();

    void on_buttonBox_accepted();

    void on_helpButton_clicked();

    void on_buttonBox_rejected();

    void yScaleChanged(double value);

    void on_yScale_valueChanged(double arg1);

private:
    Ui::UserDrawnProfileDlg *ui;

};

#endif // USERDRAWNPROFILEDLG_H
