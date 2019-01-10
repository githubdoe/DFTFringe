#ifndef WAVEFRONTFILTERDLG_H
#define WAVEFRONTFILTERDLG_H

#include <QDialog>

namespace Ui {
class wavefrontFilterDlg;
}

class wavefrontFilterDlg : public QDialog
{
    Q_OBJECT

public:
    explicit wavefrontFilterDlg(QWidget *parent = 0);
    ~wavefrontFilterDlg();
    bool shouldFilterWavefront();
    bool shouldFilterFile();
    double rms();
    void addRMSValue(QString name, QPointF value );
    void addAstigValue(QString name, QPointF value);
    void plot();
    void setRemoveFileMode();
signals:
    void waveWasSelected(QString m);
private slots:
    void on_rmsValue_valueChanged(double arg1);
    void waveSelected(QString m);
    void on_beepEnable_clicked(bool checked);

    void on_remove_clicked(bool checked);

private:
    Ui::wavefrontFilterDlg *ui;
};

#endif // WAVEFRONTFILTERDLG_H
