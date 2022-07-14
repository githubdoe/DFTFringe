/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
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
    ZernTableModel * tableModel;
    void setName(QString name);
    void setWavePerFringe(double val, double lambda);
    void setOutputLambda(double val);
private:
    bool shouldEnableAll;
public slots:
   void resizeRows(int);
private slots:
    void on_recomputePB_clicked();
    void on_DisableAll_clicked();

    void on_sphericalPb_pressed();

signals:
    void recomputeZerns();
private:
    Ui::metricsDisplay *ui;
};

#endif // METRICSDISPLAY_H
