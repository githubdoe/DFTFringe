#pragma once

#include <QWidget>
#include <QVector>
#include <QDateTime>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

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

    QtCharts::QChart *chart;
    QtCharts::QChartView *chartView;
    QtCharts::QLineSeries *rmsSeries;
    QtCharts::QLineSeries *saSeries;
    QtCharts::QValueAxis *axisX;
    QtCharts::QValueAxis *axisY_RMS;
    QtCharts::QValueAxis *axisY_SA;

    QVector<double> rawRmsData;
    QVector<double> rawSaData;

    QDateTime firstSampleTime; // Tracks the start time reference
};
