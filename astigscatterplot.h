#ifndef ASTIGSCATTERPLOT_H
#define ASTIGSCATTERPLOT_H
#include <qwt_plot.h>
#include <QPointF>
#include <QString>
#include <QVector>
#include <QObject>
class myPlotPicker;
class QwtPlotMarker;
class QwtPlotCurve;
class astigScatterPlot : public QwtPlot
{
    Q_OBJECT
    QwtPlotMarker *d_origin;
    QwtPlotCurve *d_curve;
    myPlotPicker *Picker;
    double m_max;
public:
    astigScatterPlot(QWidget *parent = 0);
    void addValue(QString name, QPointF p);
signals:
    void waveSeleted(QString m);
public slots:
    void selectedwave(QString);
};

#endif // ASTIGSCATTERPLOT_H
