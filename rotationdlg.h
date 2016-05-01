#ifndef ROTATIONDLG_H
#define ROTATIONDLG_H

#include <QDialog>

namespace Ui {
class RotationDlg;
}

class RotationDlg : public QDialog
{
    Q_OBJECT

public:
    explicit RotationDlg( QList<int> list, QWidget *parent = 0);
    ~RotationDlg();
signals:

    void rotateTheseSig(double, QList<int>);
private slots:
    void on_buttonBox_accepted();

private:
    Ui::RotationDlg *ui;
    QList<int> list;
};

#endif // ROTATIONDLG_H
