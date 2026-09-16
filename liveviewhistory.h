#ifndef LIVEVIEWHISTORY_H
#define LIVEVIEWHISTORY_H

#include <QWidget>
#include <QVector>

namespace QtCharts {
    class QChart;
    class QChartView;
    class QLineSeries;
    class QValueAxis;
}

class liveViewHistory : public QWidget {
    Q_OBJECT
public:
    explicit liveViewHistory(QWidget *parent = nullptr);
    ~liveViewHistory() = default;

    void addSample(double rawRms, double rawSa);
    void showBestFit(bool show);

private slots:
    void onResetClicked();

private:
    double computeRunningAverage(const QVector<double> &data, int windowSize);

    QtCharts::QChart *chart;
    QtCharts::QChartView *chartView;
    QtCharts::QLineSeries *rmsSeries;
    QtCharts::QLineSeries *saSeries;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY_RMS;
    QtCharts::QValueAxis *axisY_SA;

    QVector<double> rawRmsData;
    QVector<double> rawSaData;
};

#endif // LIVEVIEWHISTORY_H
