#ifndef SHOWALLCONTOURSDLG_H
#define SHOWALLCONTOURSDLG_H

#include <QDialog>

namespace Ui {
class showAllContoursDlg;
}

class showAllContoursDlg : public QDialog
{
    Q_OBJECT

public:
    explicit showAllContoursDlg(QWidget *parent = 0);
    ~showAllContoursDlg();
    int getWidth();
    int getColumns();

private slots:
    void on_Width_valueChanged(int arg1);

    void on_columns_valueChanged(int arg1);

private:
    Ui::showAllContoursDlg *ui;
};

#endif // SHOWALLCONTOURSDLG_H
