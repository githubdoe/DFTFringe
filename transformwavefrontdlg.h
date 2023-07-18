#ifndef TRANSFORMWAVEFRONTDLG_H
#define TRANSFORMWAVEFRONTDLG_H

#include <QDialog>
#include "wavefront.h"
namespace Ui {
class TransformWaveFrontDlg;
}

class TransformWaveFrontDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TransformWaveFrontDlg(QWidget *parent = 0);

    ~TransformWaveFrontDlg();

signals:
    void resizeW(int newSize);
    void flipLR();
    void flipV();
    void changeWavelength(double wavelength);

private slots:
    void on_ChangeWaveLength_clicked();

    void on_flipLR_clicked();

    void on_Resize_clicked();

    void on_flipVertical_clicked();

private:
    Ui::TransformWaveFrontDlg *ui;

};

#endif // TRANSFORMWAVEFRONTDLG_H
