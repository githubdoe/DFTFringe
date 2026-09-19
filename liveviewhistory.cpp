#include "liveviewhistory.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QChart>
#include <QtCharts/QValueAxis>
#include <QDateTime>
#include <algorithm>

liveViewHistory::liveViewHistory(QWidget *parent) : QWidget(parent) {
    // Force the widget's background color to match the dark theme edge-to-edge
    setStyleSheet("background-color: #2D2D30; color: #DCDCDC;");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 4); // Small clean padding inside splitter
    mainLayout->setSpacing(4);

    // Setup Chart
    chart = new QtCharts::QChart();
    chart->setTitle("Trend (Avg RMS & Live best Fit conic)");
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
    rmsSeries = new QtCharts::QLineSeries();
    rmsSeries->setName("Avg RMS");
    QPen rmsPen(QColor(51, 181, 229));
    rmsPen.setWidth(2);
    rmsSeries->setPen(rmsPen);

    saSeries = new QtCharts::QLineSeries();
    saSeries->setName("Live Best Conic");
    QPen saPen(QColor(255, 187, 51));
    saPen.setWidth(2);
    saSeries->setPen(saPen);

    chart->addSeries(rmsSeries);
    chart->addSeries(saSeries);

    // Axes Setup
    axisX = new QtCharts::QValueAxis();
    axisX->setTitleText("minutes");
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
    axisY_SA->setTitleText("live Best Conic");
    axisY_SA->setRange(-100, 100);
    chart->addAxis(axisY_SA, Qt::AlignRight);
    saSeries->attachAxis(axisY_SA);

    // Style Axes
    QColor axisColor(180, 180, 180);
    QColor gridColor(60, 60, 60);

    auto styleAxis = [axisColor, gridColor](QtCharts::QValueAxis *axis) {
        axis->setLabelsColor(axisColor);
        axis->setTitleBrush(QBrush(axisColor));
        axis->setLinePen(QPen(axisColor));
        axis->setGridLinePen(QPen(gridColor, 1, Qt::DashLine));
    };

    styleAxis(axisX);
    styleAxis(axisY_RMS);
    styleAxis(axisY_SA);

    chartView = new QtCharts::QChartView(chart);
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

    // Save the time of the first sample upon reset or start
    if (!firstSampleTime.isValid()) {
        firstSampleTime = currentTime;
    }

    // Calculate time delta in minutes from the first sample
    double elapsedMinutes = static_cast<double>(firstSampleTime.msecsTo(currentTime)) / 60000.0;

    rawRmsData.append(rawRms);
    rawSaData.append(rawSa);

    double avgRms = computeRunningAverage(rawRmsData, 5);
    double avgSa = computeRunningAverage(rawSaData, 5);

    rmsSeries->append(elapsedMinutes, avgRms);
    saSeries->append(elapsedMinutes, avgSa);

    // Dynamically adjust X-axis range based on elapsed time (at least 1 minute range)
    axisX->setRange(0, std::max(1.0, elapsedMinutes));

    double maxRms = 0.0;
    for (const auto &point : rmsSeries->points()) {
        if (point.y() > maxRms) maxRms = point.y();
    }
    axisY_RMS->setRange(0, std::max(.04, maxRms * 1.1));

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

void liveViewHistory::showBestFit(bool show) {
    saSeries->setName((show)? "Best Fit" : "Live SA");
    axisY_SA->setTitleText((show)? "Live Best Fit": "live SA");
    if (show){
        axisY_SA->setRange(-2, 2);
    }
    else{
        axisY_SA->setRange(-100, 100);
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
    int start = std::max(0, data.size() - windowSize);
    int count = data.size() - start;
    double sum = 0.0;
    for (int i = start; i < data.size(); ++i) {
        sum += data[i];
    }
    return count > 0 ? sum / count : 0.0;
}
