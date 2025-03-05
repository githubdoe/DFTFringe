#ifndef PERCENTCORRECTIONDLG_H
#define PERCENTCORRECTIONDLG_H

#include <QDialog>
#include <QPolygonF>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>
#include <QTableWidgetItem>
#include <armadillo>
#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/qabstract3dseries.h>
#include <QVBoxLayout>
#include "graphmodifier.h"
#include "percentCorrectionSurface.h"
using namespace QtDataVisualization;

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
    arma::mat zoneZerns;
    QList<double> zoneCenter;
    QList<double> zoneedge;
    QList<QVector<double> > m_barData;
    QStringList m_seriesName;

    QPolygonF m_avg;
    int m_currentprofileIndex = 0;
    int m_currentSelectedZone = -1;

    QVector<surfaceData *>  surfs;
    QPolygonF makePercentages(surfaceData *);
    double getnormalSlope(double RoC, double radius, std::vector<double> Zernikes, double x, double null);
    double getZernSurface( double RoC, double MirrorRad, std::vector<double> Zernikes, double x, double null);
    double GetActualKE(double RoC, double MirrorRad, std::vector<double> Zernikes, double x);
    void  make_3DBarControls(QWidget *widget, QVBoxLayout *vlayout);
    Q3DBars *m_barGraph;
    GraphModifier *modifier;
    QBar3DSeries *m_series;
    QBarDataProxy *m_proxy;
public:
    int m_maxOrder = 12;
signals:
    void percent_plot_changed();
    void make_percent_correction();
protected:
    void closeEvent(QCloseEvent *event);
    void moveEvent(QMoveEvent *event);
public:
    explicit percentCorrectionDlg( QWidget *parent = nullptr);
    ~percentCorrectionDlg();
    std::vector<double> m_zerns;
    void setData(QVector< surfaceData *> );
    void plot();
    void plotProfile();
    void plotSlope();
    void updateZoneTable();
    QJsonDocument loadZonesFromJson(QString str);
    double GetActualKE(double RoC, double MirrorRad, std::vector<double> Zernikes, int zone);
private slots:
    void on_percentTable_itemChanged(QTableWidgetItem *item);

private slots:

    void on_minvalue_valueChanged(double arg1);

    void on_maxvalue_valueChanged(double arg1);

    void on_numberOfZones_valueChanged(int arg1);

    void on_help_clicked();

    void on_loadZones_clicked();

    void on_saveZones_clicked();

    void on_Generate_clicked();

    void on_maxOrder_valueChanged(int arg1);

    arma::mat makeZoneZerns(QList<double> centers);

    void on_correction_toggled(bool checked);

private:
    Ui::percentCorrectionDlg *ui;

    void makeZones();
    void saveSettings();

};

#endif // PERCENTCORRECTIONDLG_H
