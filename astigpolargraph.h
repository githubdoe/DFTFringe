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

namespace Ui {
class astigPolargraph;
}

class astigPolargraph : public QDialog
{
    Q_OBJECT

public:
    explicit astigPolargraph(QList<int> list,QWidget *parent = nullptr);
    ~astigPolargraph();

private:
    Ui::astigPolargraph *ui;
    QList<int> m_list;  // list index of selected wave fronts in surface manager's list.
};

#endif // ASTIGPOLARGRAPH_H
