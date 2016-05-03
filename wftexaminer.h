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
#ifndef WFTEXAMINER_H
#define WFTEXAMINER_H

#include <QDialog>
#include <qwt_plot.h>
#include "wavefront.h"
#include <qwt_plot_curve.h>
namespace Ui {
class wftExaminer;
}

class wftExaminer : public QDialog
{
    Q_OBJECT

public:
    explicit wftExaminer(wavefront *wf, QWidget *parent = 0);
    ~wftExaminer();
    wavefront *m_wf;
    QwtPlot * m_Pl;
    int m_y;
    QwtPlotCurve *curve;
    QwtPlotCurve *maskCurve;
    void setupPlot();
private slots:
    void on_spinBox_valueChanged(int arg1);

private:
    Ui::wftExaminer *ui;
};

#endif // WFTEXAMINER_H
