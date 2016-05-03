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
#ifndef WAVEFRONTSTATS_H
#define WAVEFRONTSTATS_H
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_renderer.h>
#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>
class WaveFrontStats : public QwtPlot
{
public:
    WaveFrontStats(QWidget *parent);
    void setData(QPolygonF wftPoints, QPolygonF avgPoints);
    void saveImage();
    void savePdf();
    void setup();
private:
    QwtPlotCurve *curve1;
    QwtPlotCurve* curve2;
    QPolygonF wftPoints;
    QwtPlot * m_plot;
};

#endif // WAVEFRONTSTATS_H
