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
#ifndef WFTSTATS_H
#define WFTSTATS_H
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_renderer.h>
#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_scale_engine.h>
#include "zernikes.h"
#include "mirrordlg.h"
#include <opencv/cv.h>
#include "wavefront.h"
#include "statsdlg.h"
#include <QObject>

class wftStats : public QObject
{
   Q_OBJECT
public:
    wftStats(mirrorDlg *md);
    void showStats();
    void computeWftStats(    QVector<wavefront*> wavefronts, int ndx);
    void computeZernStats(int ndx);
    void computeWftRunningAvg(QVector<wavefront *> wavefronts, int ndx);
    QwtPlot *makeWftPlot(QVector<wavefront *> &wavefronts, int ndx, StatsDlg *statsDlg);
    QwtPlot *makeZernPlot();
    QwtPlot *makeHistoPlot();
    void makeCsv();

    QList<int> outliers;
    double IQ;
private:
    QwtPlot *wftPlot;
    QwtPlot *zernPlot;
    QwtPlot *wftHistogram;
    QPolygonF wftPoints;

    QPolygonF avgPoints;
    QPolygonF astigs;
    QPolygonF spherical;
    QPolygonF sphericaRunningAvg;
    QPolygonF outliersInner;
    QPolygonF outliersOuter;
    QPolygonF inrange;
    QVector<int> trueNdx;
    double wftsMedian;

    mirrorDlg *md;
    cv::Mat m_Zerns;
    QVector<QwtIntervalSample> zernMinMax;
    QVector<QwtIntervalSample> zernStd;
    QVector<QString> zNames;
    QVector< QVector < double> > zerns;  //each row has the zerns for that sample
    QVector<int> m_hist;
signals:

public slots:
};

#endif // WFTSTATS_H
