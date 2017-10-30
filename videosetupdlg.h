#ifndef VIDEOSETUPDLG_H
#define VIDEOSETUPDLG_H

#include <QDialog>
#include <QImage>

namespace Ui {
class videoSetupDlg;
}

class videoSetupDlg : public QDialog
{
    Q_OBJECT

public:
    explicit videoSetupDlg(QImage img, QWidget *parent = 0);
    ~videoSetupDlg();
    QImage m_img;
    int fps();
    int width();
    int height();
    double angleStep();
    QSize m_imageSize;
private slots:

    void on_buttonBox_accepted();
    void on_fullSize_clicked(bool checked);

private:
    Ui::videoSetupDlg *ui;
};

#endif // VIDEOSETUPDLG_H
