#ifndef PERCENTCORRECTIONDLG_H
#define PERCENTCORRECTIONDLG_H

#include <QDialog>
#include <QPolygonF>
#include <qwt_plot_curve.h>
#include <qwt_plot_marker.h>

namespace Ui {
class percentCorrectionDlg;
}
class ZonePicker: public QObject
{
    Q_OBJECT
    QList<double> *m_zonelist;
    bool moving = false;
    bool onazone = false;

public:
    bool m_editing = false;
    QwtPlotMarker *d_lastFound = NULL;
    double m_radius;
    ZonePicker( QwtPlot *plot, QList<double> *zones );
    virtual bool eventFilter( QObject *, QEvent * );

    virtual bool event( QEvent * );
    void edit(bool b);
    void reset();
Q_SIGNALS:
    void zoneMoved(int zone, double rad);
    void zoneSelected(int ndx);

private:
    QwtPlotMarker *findItem(double x, double tolerance, QwtPlotItem::RttiValues type);
    void select( const QPoint & );
    void move( const QPoint & );
    void release();
    void highlight(QwtPlotMarker *m, bool flag);
    QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotMarker *d_selectedMarker;

    int d_selectedZone;

};
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
    QList<double> zoneCenter;
    QList<double> zoneedge;
    QList<int> indexOfZoneCenter;
    ZonePicker *zonePicker;
    int m_currentSelectedZone = -1;
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

    void zoneMoved(int ndx, double value);

    void zoneSelected(int ndx);

    void on_loadZones_clicked();

    void on_saveZones_clicked();

    void on_editEnable_toggled(bool checked);

    void on_zoneValue_valueChanged(double arg1);



private:
    Ui::percentCorrectionDlg *ui;

    void makeZones();
    void saveSettings();

};

#endif // PERCENTCORRECTIONDLG_H
