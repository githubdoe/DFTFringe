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
#ifndef CONTOURPLOT_H
#define CONTOURPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <opencv2/opencv.hpp>
#include "wavefront.h"
#include "contourtools.h"
#include "usercolormapdlg.h"
#include "wavefront.h"
#include <qwt_picker.h>
#include <qwt_plot_picker.h>
#include <qwt_interval.h>

class MyZoomer;
class SpectrogramData: public QwtRasterData
{
public:
    SpectrogramData();
    wavefront *m_wf;
    void setSurface(wavefront *surface);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // keep compatibility with newer version of QWT used in QT6
    QwtInterval interval(Qt::Axis axis) const override;
    void setInterval(Qt::Axis axis, const QwtInterval &interval);
#endif
    virtual double value( double x, double y ) const override;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // keep compatibility with newer version of QWT used in QT6
private:
    QwtInterval m_xInterval;
    QwtInterval m_yInterval;
    QwtInterval m_zInterval;
#endif
};
class ContourPlot: public QwtPlot
{
    Q_OBJECT
    QwtPlotPicker* picker_;
    MyZoomer* tracker_;
    double m_lastAngle;

public:
    QwtPlotSpectrogram *d_spectrogram;
    wavefront* m_wf;
    ContourTools *m_tools;
    static bool m_useMiddleOffset;
    static int m_colorMapNdx;
    static double m_zOffset;
    static double m_waveRange;
    double contourRange;
    static QString m_zRangeMode;
    ContourPlot(QWidget * = NULL, ContourTools *tools  = 0, bool minimal = false);
    void setSurface(wavefront * mat);
    void applyZeroOffset(bool useMiddle);
    void setZRange();
    void setColorMap(int ndx);
    void setTool(ContourTools* tool);
    bool m_autoInterval;
    bool m_minimal;
    bool m_linkProfile;
    QPen m_rulerPen;
    int m_radialDeg;
    bool m_do_fill;


signals:
    void setMinMaxValues(double,double);
    void setWaveRange(double);
    void newContourRange(double);
    void sigPointSelected(QPointF );

public Q_SLOTS:
  void selected(QPointF pos);


public Q_SLOTS:
    void showContour( bool on );
    void showSpectrogram(bool on );
    void setAlpha( int );
    void ContourMapColorChanged(int);
    void showContoursChanged(double);
    void contourZeroOffsetChanged(const QString & val);
    void contourColorRangeChanged(const QString &arg1);
    void contourWaveRangeChanged(double);
    void on_line_color_changed(QColor);
    void contourFillChanged(int);
    void newDisplayErrorRange(double min, double max);
    void drawProfileLine(const double ang);
#ifndef QT_NO_PRINTER
    void printPlot();
#endif

private:
    void drawCanvas(QPainter* p);
    void initPlot();


    QColor m_contourPen;

    void ruler();
    double m_min;
    double m_max;

protected:

};

#endif // CONTOURPLOT_H
