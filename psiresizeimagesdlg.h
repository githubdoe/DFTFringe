#ifndef PSIRESIZEIMAGESDLG_H
#define PSIRESIZEIMAGESDLG_H

#include <QDialog>

namespace Ui {
class PSIResizeImagesDlg;
}

class PSIResizeImagesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit PSIResizeImagesDlg(int newSize, int max, QWidget *parent = nullptr);
    ~PSIResizeImagesDlg();
    int getSize();
private slots:
    void on_m_resize_valueChanged(int arg1);

private:
    Ui::PSIResizeImagesDlg *ui;
};

#endif // PSIRESIZEIMAGESDLG_H
