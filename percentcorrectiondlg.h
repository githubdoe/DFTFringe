#ifndef PERCENTCORRECTIONDLG_H
#define PERCENTCORRECTIONDLG_H

#include <QDialog>
#include <QPolygonF>
#include <qwt_plot_curve.h>

namespace Ui {
class percentCorrectionDlg;
}

class percentCorrectionDlg : public QDialog
{
    Q_OBJECT
    int m_number_of_zones = 6;
    int m_size;
    double m_radius;
    double m_z8;
    double m_desiredZ8;
    double m_lambda_nm;
    double m_outputLambda;
    bool m_showZones;
public:
    explicit percentCorrectionDlg( QWidget *parent = nullptr);
    ~percentCorrectionDlg();
    QPolygonF m_avg;
    void plot(QPolygonF avg, double radius,double z8,
              double desiredZ8, double lambda_nm, double outputlampda);
    void replot();
private slots:

    void on_checkBox_clicked(bool checked);

    void on_zones_valueChanged(int arg1);

    void on_checkBox_2_clicked(bool checked);

private:
    Ui::percentCorrectionDlg *ui;
};

#endif // PERCENTCORRECTIONDLG_H
