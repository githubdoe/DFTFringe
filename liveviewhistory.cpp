#include "liveviewhistory.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QDateTime>
#include <algorithm>
#include <cmath>
liveViewHistory::liveViewHistory(QWidget *parent) : QWidget(parent) {
    // Force the widget's background color to match the dark theme edge-to-edge
    setStyleSheet("background-color: #2D2D30; color: #DCDCDC;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4); // Small clean padding inside splitter
    mainLayout->setSpacing(4);

    // Setup Chart
    chart = new QChart();
    chart->setTitle(QString("Trend"));
    chart->setMargins(QMargins(0, 0, 0, 0));
    // Configure Legend Appearance for High Contrast
        chart->legend()->setVisible(true);
        chart->legend()->setAlignment(Qt::AlignTop);
        chart->legend()->setLabelBrush(QBrush(QColor(240, 240, 240))); // Bright off-white for maximum legibility
        chart->legend()->setBackgroundVisible(false);
    // Dark Theme Backgrounds
    chart->setBackgroundBrush(QBrush(QColor(45, 45, 48)));
    chart->setPlotAreaBackgroundBrush(QBrush(QColor(30, 30, 30)));
    chart->setPlotAreaBackgroundVisible(true);
    chart->setTitleBrush(QBrush(QColor(220, 220, 220)));

    // Series Setup
    rmsSeries = new QLineSeries();
    rmsSeries->setName("Avg RMS");

    QPen rmsPen(QColor(51, 181, 229));
    rmsPen.setWidth(2);
    rmsSeries->setPen(rmsPen);

    saSeries = new QLineSeries();
    saSeries->setName(m_itemName);
    QPen saPen(QColor(255, 187, 51));
    saPen.setWidth(2);
    saSeries->setPen(saPen);

    chart->addSeries(rmsSeries);
    chart->addSeries(saSeries);

    // Axes Setup
    axisX = new QValueAxis();
    axisX->setTitleText("minutes");
    axisX->setRange(0, 1);
    chart->addAxis(axisX, Qt::AlignBottom);
    rmsSeries->attachAxis(axisX);
    saSeries->attachAxis(axisX);

    axisY_RMS = new QValueAxis();
    axisY_RMS->setTitleText("Avg RMS");
    axisY_RMS->setTickCount(5);
    axisY_RMS->setRange(0, 1);

    chart->addAxis(axisY_RMS, Qt::AlignLeft);
    rmsSeries->attachAxis(axisY_RMS);

    axisY_SA = new QValueAxis();
    axisY_SA->setTitleText(m_itemName);
    axisY_SA->setTickCount(5);
    axisY_SA->setRange(-100, 100);
    chart->addAxis(axisY_SA, Qt::AlignRight);
    saSeries->attachAxis(axisY_SA);

    // Style Axes
    QColor axisColor(180, 180, 180);
    QColor gridColor(60, 60, 60);

    auto styleAxis = [axisColor, gridColor](QValueAxis *axis) {
        axis->setLabelsColor(axisColor);
        axis->setTitleBrush(QBrush(axisColor));
        axis->setLinePen(QPen(axisColor));
        axis->setGridLinePen(QPen(gridColor, 1, Qt::DashLine));
    };

    styleAxis(axisX);
    styleAxis(axisY_RMS);
    styleAxis(axisY_SA);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setFrameShape(QFrame::NoFrame);
    mainLayout->addWidget(chartView);

    // Reset Control Button & Connection
    QPushButton *resetBtn = new QPushButton("Reset Data", this);
    resetBtn->setStyleSheet("background-color: #3E3E42; color: white; border: 1px solid #555; padding: 5px;");
    connect(resetBtn, &QPushButton::clicked, this, &liveViewHistory::onResetClicked);
    mainLayout->addWidget(resetBtn);

    resize(800, 500);
}

void liveViewHistory::addSample(double rawRms, double rawSa) {
    QDateTime currentTime = QDateTime::currentDateTime();

    if (!firstSampleTime.isValid()) {
        firstSampleTime = currentTime;
    }

    double elapsedMinutes = static_cast<double>(firstSampleTime.msecsTo(currentTime)) / 60000.0;

    rawRmsData.append(rawRms);
    rawSaData.append(rawSa);

    double avgRms = computeRunningAverage(rawRmsData, 5);
    double avgSa = computeRunningAverage(rawSaData, 10);

    rmsSeries->append(elapsedMinutes, avgRms);
    saSeries->append(elapsedMinutes, avgSa); // fixed syntax

    axisX->setRange(0, std::max(1.0, elapsedMinutes));

    // --- Rounded RMS Axis Range ---
    double maxRms = 0.001; // prevent zero bounds
    for (const auto &point : rmsSeries->points()) {
        if (point.y() > maxRms) maxRms = point.y();
    }

    // Snap maxRms to a clean upper bound (e.g., multiples of 0.05 or 0.1)
    double targetRmsMax = maxRms * 1.15;
    double rmsUpper = std::ceil(targetRmsMax * 20.0) / 20.0; // Snaps to nearest 0.05
    if (rmsUpper < 0.05) rmsUpper = 0.05;
    axisY_RMS->setRange(0, rmsUpper);

    // --- Rounded SA / Zernike Axis Range ---
    if (!saSeries->points().isEmpty()) {
        double minSa = saSeries->points().at(0).y();
        double maxSa = saSeries->points().at(0).y();
        for (const auto &point : saSeries->points()) {
            if (point.y() < minSa) minSa = point.y();
            if (point.y() > maxSa) maxSa = point.y();
        }

        double span = std::max(0.01, maxSa - minSa);
        double padding = span * 0.15;

        // Snap min/max to clean decimals
        double saMin = std::floor((minSa - padding) * 10.0) / 10.0;
        double saMax = std::ceil((maxSa + padding) * 10.0) / 10.0;
        if (saMin == saMax) { saMin -= 1.0; saMax += 1.0; }

        axisY_SA->setRange(saMin, saMax);
    }
}

void liveViewHistory::setItem(QString name) {
    m_itemName = name; // ensure member variable updates if you track it
    saSeries->setName(name);
    axisY_SA->setTitleText(QString("live %1").arg(name));

    if (name == "Best Fit Conic"){
        axisY_SA->setRange(-2.0, 2.0); // Clean bounds
    }
    else{
        axisY_SA->setRange(-10.0, 10.0); // Clean bounds
    }
    onResetClicked();
}

void liveViewHistory::onResetClicked() {
    rawRmsData.clear();
    rawSaData.clear();
    rmsSeries->clear();
    saSeries->clear();
    firstSampleTime = QDateTime(); // Reset start time so the next sample acts as the new epoch
    axisX->setRange(0, 1);
    axisY_RMS->setRange(0, 1);
    axisY_SA->setRange(-100, 100);
}

double liveViewHistory::computeRunningAverage(const QVector<double> &data, int windowSize) {
    int start = std::max(0, (int)(data.size() - windowSize));
    int count = data.size() - start;
    double sum = 0.0;
    for (int i = start; i < data.size(); ++i) {
        sum += data[i];
    }
    return count > 0 ? sum / count : 0.0;
}
