#ifndef METRICSDISPLAY_H
#define METRICSDISPLAY_H

#include <QDockWidget>
#include <QTableView>
#include <QLabel>
#include "zernikedlg.h"
namespace Ui {
class metricsDisplay;
}

class metricsDisplay : public QDockWidget
{
    Q_OBJECT
    static metricsDisplay *m_instance;
public:
    explicit metricsDisplay(QWidget *parent = 0);
    ~metricsDisplay();
    static metricsDisplay *get_instance(QWidget * parent = 0);
    QTableView *getZernView();
    QLabel *mDiam;
    QLabel *mROC;
    QLabel *mRMS;
    QLabel *mStrehl;
    QLabel *mCC;
    QTableView* mvalues;
    ZernTableModel * tableModel;
    void setName(QString name);
    void setWavePerFringe(double val);
private slots:
    void on_recomputePB_clicked();
    void on_DisableAll_clicked();

signals:
    void recomputeZerns();
private:
    Ui::metricsDisplay *ui;
};

#endif // METRICSDISPLAY_H
