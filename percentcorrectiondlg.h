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
    int m_number_of_zones = 5;
    int m_size;
    double m_radius;
    double m_z8;
    double m_desiredZ8;
    double m_lambda_nm;
    double m_outputLambda;
    bool m_showZones;
    double m_roc;
    double m_roc_offset = 0.;
signals:
    void percent_plot_changed();
protected:
    void closeEvent(QCloseEvent *event);
    void moveEvent(QMoveEvent *event);
public:
    explicit percentCorrectionDlg( QWidget *parent = nullptr);
    ~percentCorrectionDlg();
    QPolygonF m_avg;
    void plot(QPolygonF avg, double radius,double roc, double z8,
              double desiredZ8, double lambda_nm, double outputlampda,
              QColor penColor, bool addToPlot = false);
    void replot(QColor penColor = Qt::blue, bool addToPlot = false);
private slots:

    void on_minvalue_valueChanged(double arg1);

    void on_maxvalue_valueChanged(double arg1);

    void on_numberOfZones_valueChanged(int arg1);

    void on_help_clicked();

    void on_plotType_currentTextChanged(const QString &arg1);

    void on_testbox_valueChanged(double arg1);

    void on_del_valueChanged(int arg1);

    void on_avgwidth_valueChanged(int arg1);

private:
    Ui::percentCorrectionDlg *ui;
};

#endif // PERCENTCORRECTIONDLG_H
