#ifndef PREFSDLG_H
#define PREFSDLG_H

#include <QDialog>

namespace Ui {
class prefsDlg;
}

class prefsDlg : public QDialog
{
    Q_OBJECT
signals:
    void saveGeometry();

public:
    explicit prefsDlg(QWidget *parent = 0);
    ~prefsDlg();

private slots:
    void on_pushButton_clicked();

private:
    Ui::prefsDlg *ui;
};

#endif // PREFSDLG_H
