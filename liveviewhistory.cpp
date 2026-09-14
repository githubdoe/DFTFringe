#include "liveviewhistory.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <algorithm>

liveViewHistory::liveViewHistory(QWidget *parent) : QDialog(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Setup Chart
    chart = new QtCharts::QChart();
    chart->setTitle("Trend (RMS Averagee & Live SA)");

    rmsSeries = new QtCharts::QLineSeries();
    rmsSeries->setName("Avg RMS");
    saSeries = new QtCharts::QLineSeries();
    saSeries->setName("SA");

    chart->addSeries(rmsSeries);
    chart->addSeries(saSeries);

    // Axes
    axisX = new QtCharts::QValueAxis();
    axisX->setTitleText("seconds)");
    axisX->setRange(0, 1);
    chart->addAxis(axisX, Qt::AlignBottom);
    rmsSeries->attachAxis(axisX);
    saSeries->attachAxis(axisX);

    axisY_RMS = new QtCharts::QValueAxis();
    axisY_RMS->setTitleText("Avg RMS");
    axisY_RMS->setRange(0, 1);
    chart->addAxis(axisY_RMS, Qt::AlignLeft);
    rmsSeries->attachAxis(axisY_RMS);

    axisY_SA = new QtCharts::QValueAxis();
    axisY_SA->setTitleText("live SA");
    axisY_SA->setRange(-100, 100);
    chart->addAxis(axisY_SA, Qt::AlignRight);
    saSeries->attachAxis(axisY_SA);

    chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    mainLayout->addWidget(chartView);

    // Reset Control
    QPushButton *resetBtn = new QPushButton("Reset Data", this);
    connect(resetBtn, &QPushButton::clicked, this, &liveViewHistory::onResetClicked);
    mainLayout->addWidget(resetBtn);

    resize(800, 500);
}

void liveViewHistory::addSample(double rawRms, double rawSa) {
    rawRmsData.append(rawRms);
    rawSaData.append(rawSa);

    // Compute running average (window size of 5 samples)
    double avgRms = computeRunningAverage(rawRmsData, 5);
    double avgSa = computeRunningAverage(rawSaData, 5);

    int sampleIndex = rawRmsData.size();
    rmsSeries->append(sampleIndex, avgRms);
    saSeries->append(sampleIndex, avgSa);

    // Update X-axis to include the complete set of samples
    axisX->setRange(1, std::max(2, sampleIndex));

    // Auto-scale Y-axes dynamically with headroom
    double maxRms = 0.0;
    for (const auto &point : rmsSeries->points()) {
        if (point.y() > maxRms) maxRms = point.y();
    }
    axisY_RMS->setRange(0, std::max(.0, maxRms * 1.1));

    if (!saSeries->points().isEmpty()) {
        double minSa = saSeries->points().at(0).y();
        double maxSa = saSeries->points().at(0).y();
        for (const auto &point : saSeries->points()) {
            if (point.y() < minSa) minSa = point.y();
            if (point.y() > maxSa) maxSa = point.y();
        }
        double saSpan = maxSa - minSa;
        double saPadding = saSpan == 0 ? 10.0 : saSpan * 0.1;
        axisY_SA->setRange(minSa - saPadding, maxSa + saPadding);
    }
}

void liveViewHistory::onResetClicked() {
    rawRmsData.clear();
    rawSaData.clear();
    rmsSeries->clear();
    saSeries->clear();
    axisX->setRange(0, 1);
    axisY_RMS->setRange(0, 1);
    axisY_SA->setRange(-100, 100);
}

double liveViewHistory::computeRunningAverage(const QVector<double> &data, int windowSize) {
    int start = std::max(0, data.size() - windowSize);
    int count = data.size() - start;
    double sum = 0.0;
    for (int i = start; i < data.size(); ++i) {
        sum += data[i];
    }
    return count > 0 ? sum / count : 0.0;
}
