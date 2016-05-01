#ifndef SUBTRACTWAVEFRONATSDLG_H
#define SUBTRACTWAVEFRONATSDLG_H

#include <QDialog>
#include <QListWidget>
namespace Ui {
class subtractWavefronatsDlg;
}

class subtractWavefronatsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit subtractWavefronatsDlg(QList<QString> list, QWidget *parent = 0);
    ~subtractWavefronatsDlg();
    int getSelected();

private:
    Ui::subtractWavefronatsDlg *ui;
};

#endif // SUBTRACTWAVEFRONATSDLG_H
