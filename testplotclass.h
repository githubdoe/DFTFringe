#ifndef TESTPLOTCLASS_H
#define TESTPLOTCLASS_H
#include <qwt_plot.h>
#include <QObject>
#include <QString>
class testPlotClass : public QwtPlot
{
    Q_OBJECT
public:
    explicit testPlotClass(QObject *parent = 0);
    void t(QString m);
signals:
    void waveSelected(QString);
public slots:
};

#endif // TESTPLOTCLASS_H
