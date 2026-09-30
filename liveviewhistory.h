#pragma once

#include <QWidget>
#include <QVector>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
QT_CHARTS_USE_NAMESPACE
#endif

class liveViewHistory : public QWidget {
    Q_OBJECT
public:
    explicit liveViewHistory(QWidget *parent = nullptr);
    void addSample(double rawRms, double rawSa);
    void showBestFit(bool show);

private slots:
    void onResetClicked();

private:
    double computeRunningAverage(const QVector<double> &data, int windowSize);

    QChart *chart;
    QChartView *chartView;
    QLineSeries *rmsSeries;
    QLineSeries *saSeries;
    QValueAxis *axisX;
    QValueAxis *axisY_RMS;
    QValueAxis *axisY_SA;

    QVector<double> rawRmsData;
    QVector<double> rawSaData;

    QDateTime firstSampleTime; // Tracks the start time reference
};
