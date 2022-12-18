#ifndef EDGEPLOT_H
#define EDGEPLOT_H

#include <QDialog>
#include <QtCharts>
namespace Ui {
class EdgePlot;
}

class EdgePlot : public QDialog
{
    Q_OBJECT

public:
    explicit EdgePlot(QWidget *parent = nullptr);
    ~EdgePlot();
    double m_percent;
    double m_height;
    double m_mag;
    double m_sharp;
private slots:
    void on_height_valueChanged(int value);

    void on_percentRho_valueChanged(int value);

    void on_power_valueChanged(int value);


private:
    Ui::EdgePlot *ui;

    QChart *m_chart;
    QLineSeries *m_data;
    QLineSeries *m_marker;
    int m_breakPoint;
    void Update();
};

#endif // EDGEPLOT_H
