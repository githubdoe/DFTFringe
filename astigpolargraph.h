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
#include <QTableWidgetItem>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // keep compatibility with Qt5
    QT_CHARTS_USE_NAMESPACE
#endif

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

signals:
    void waveSeleted(QString);
private slots:
    void tooltip(QPointF point, bool state);
    void clicked(QPointF point);
    void on_waveFrontTable_itemClicked(QTableWidgetItem *item);

private:
    int findClosestPoint(const QPointF clickedPoint );
    Ui::astigPolargraph *ui;
    QPolarChart *chart;
    QList<astigSample> m_list;
public:
        void hideHoverHelp();
};

#endif // ASTIGPOLARGRAPH_H
