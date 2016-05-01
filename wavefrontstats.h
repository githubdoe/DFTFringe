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
