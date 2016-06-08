#ifndef BATHASTIGDLG_H
#define BATHASTIGDLG_H

#include <QDialog>

namespace Ui {
class bathAstigDlg;
}

class bathAstigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit bathAstigDlg(QWidget *parent = 0);
    ~bathAstigDlg();

private slots:

    void on_diamSb_valueChanged(double arg1);

    void on_rocsb_valueChanged(double arg1);

    void on_sepSb_valueChanged(double arg1);

    void on_lambdaSb_valueChanged(double arg1);

private:
    Ui::bathAstigDlg *ui;
    void compute();
};

#endif // BATHASTIGDLG_H
