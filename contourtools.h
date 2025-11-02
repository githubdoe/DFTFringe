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
#ifndef CONTOURTOOLS_H
#define CONTOURTOOLS_H

#include <QDockWidget>
#include <QTimer>

class ContourPlot;

namespace Ui {
class ContourTools;
}

class ContourTools : public QDockWidget
{
    Q_OBJECT

public:
    explicit ContourTools(QWidget *parent = 0);
    ~ContourTools();
    void connectTo(ContourPlot *view);
    double m_interval;
    void enablTools(bool b);
    double m_min;
    double m_max;

signals:
    void ContourMapColorChanged(int);
    void showContoursChanged(double);
    void showContourLines(bool);
    void contourZeroOffsetChanged(const QString &);
    void contourColorRangeChanged(const QString &);
    void contourIntervalChanged(double);
    void contourWaveRangeChanged(double);
    void newDisplayErrorRange(double, double);
    void lineColorChanged(QColor);
    void fillChanged(int );
public slots:
   void setMinMaxValues(double min,double max);
private slots:
    void updateMinMax();

    void setWaveRange(double val);
    void on_ColorMapCB_activated(int index);

    void on_zeroOffsetCB_textActivated(const QString &arg1);

    void on_colorRangeCB_textActivated(const QString &arg1);

    void on_errorRangeSpin_valueChanged(double arg1);

    void on_LineColorBtn_clicked();

    void on_maxSB_valueChanged(double arg1);

    void on_minSB_valueChanged(double arg1);

private:
    Ui::ContourTools *ui;
    QTimer *m_minmaxEditTimer;
};

#endif // CONTOURTOOLS_H
