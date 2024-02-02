#ifndef WAVEFRONTAVERAGEFILTERDLG_H
#define WAVEFRONTAVERAGEFILTERDLG_H

#include <QDialog>

namespace Ui {
class wavefrontAverageFilterDlg;
}

class wavefrontAverageFilterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit wavefrontAverageFilterDlg(QWidget *parent = 0);
    ~wavefrontAverageFilterDlg();
    double rms();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::wavefrontAverageFilterDlg *ui;
};

#endif // WAVEFRONTAVERAGEFILTERDLG_H
