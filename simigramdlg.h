#ifndef SIMIGRAMDLG_H
#define SIMIGRAMDLG_H

#include <QDialog>

namespace Ui {
class simIgramDlg;
}

class simIgramDlg : public QDialog
{
    Q_OBJECT

public:
    explicit simIgramDlg(QWidget *parent = 0);
    ~simIgramDlg();
    double xtilt;
    double ytilt;
    double defocus;
    double correction;
    double xastig;
    double yastig;
    double star;
    double ring;
    double noise;
    int zernNdx;
    double zernValue;
    int size;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::simIgramDlg *ui;
};

#endif // SIMIGRAMDLG_H
