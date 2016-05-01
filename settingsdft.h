#ifndef SETTINGSDFT_H
#define SETTINGSDFT_H

#include <QDialog>

namespace Ui {
class settingsDFT;
}

class settingsDFT : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDFT(QWidget *parent = 0);
    ~settingsDFT();
    bool showThumb();
    int DFTSize();
private slots:
    void on_DFTSizeSB_valueChanged(int arg1);

    void on_ShowDFTTHumbCB_clicked(bool checked);

private:
    Ui::settingsDFT *ui;
};

#endif // SETTINGSDFT_H
