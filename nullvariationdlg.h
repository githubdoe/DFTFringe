#ifndef NULLVARIATIONDLG_H
#define NULLVARIATIONDLG_H

#include <QDialog>
#include <qwt_plot.h>
#include <QTimer>
namespace Ui {
class nullVariationDlg;
}

class nullVariationDlg : public QDialog
{
    Q_OBJECT

public:
    explicit nullVariationDlg(QWidget *parent = 0);
    ~nullVariationDlg();
    bool isMm;
    void calculate();
private slots:
    void on_useInch_clicked(bool checked);

    void on_useMm_clicked(bool checked);

    void on_roc_textChanged(const QString);

    void on_diameter_textChanged(const QString );

    void on_rocDelta_valueChanged(double val);

    void on_diameterDelta_valueChanged(double val);

    void on_pushButton_clicked();

    void on_ComputeSim_clicked();


    void on_sampleSizeSb_valueChanged(int arg1);

private:
    Ui::nullVariationDlg *ui;
    QwtPlot *histo_plot;
    QTimer m_guiTimer;
};

#endif // NULLVARIATIONDLG_H
