#ifndef ASTIGPOLARGRAPH_H
#define ASTIGPOLARGRAPH_H

#include <QDialog>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QtCharts/QAreaSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QPolarChart>
#include "wavefront.h"
QT_CHARTS_USE_NAMESPACE
namespace Ui {
class astigPolargraph;
}
class astigSample{

public:
    QString m_name;
    double m_xastig;
    double m_yastig;
    astigSample(QString name, double xastig, double yastig): m_name(name), m_xastig(xastig), m_yastig(yastig){};
};

class astigPolargraph : public QDialog
{
    Q_OBJECT

public:
    explicit astigPolargraph(QList<astigSample> list,QWidget *parent = nullptr);
    ~astigPolargraph();

private slots:
    void on_waveFrontTable_cellDoubleClicked(int row, int column);

private:
    Ui::astigPolargraph *ui;
    QPolarChart *chart;
};

#endif // ASTIGPOLARGRAPH_H
