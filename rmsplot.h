#ifndef RMSPLOT_H
#define RMSPLOT_H
#include <QVector>
#include <QString>
#include <QPointF>
#include <qwt_plot.h>
#include <QString>
#include <QObject>

class QwtPlotCurve;
class astigZoomer;
class myPlotPicker;
class rmsPlot : public QwtPlot
{
    Q_OBJECT
    QwtPlotCurve *d_curve;
    myPlotPicker *Picker;
    QVector<QPointF> points;
    double m_max;
public:
    explicit rmsPlot(QWidget *parent = 0);

public:
    void addValue(QString name, QPointF p);
private slots:
    void selectedwave(QString m);
signals:
    void waveSeleted(QString);
};

#endif // RMSPLOT_H



