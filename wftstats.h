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
class wftStats
{
public:
    wftStats(mirrorDlg *md);
    void showStats();
    void computeWftStats(    QVector<wavefront*> wavefronts, int ndx);
    void computeZernStats(int ndx);
    void computeWftRunningAvg(QVector<wavefront *> wavefronts, int ndx);
    void makeWftPlot(QVector<wavefront *> &wavefronts, int ndx, StatsDlg *statsDlg);
    void makeZernPlot();
    void makeHistoPlot();
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
};

#endif // WFTSTATS_H
