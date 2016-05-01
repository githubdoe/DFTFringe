#ifndef CONTOURPLOT_H
#define CONTOURPLOT_H

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include "opencv/cv.h"
#include "wavefront.h"
#include "contourtools.h"
#include "usercolormapdlg.h"
#include "wavefront.h"

class SpectrogramData: public QwtRasterData
{
public:
    SpectrogramData();
    wavefront *m_wf;
    void setSurface(wavefront *surface);

    virtual double value( double x, double y ) const;

};
class ContourPlot: public QwtPlot
{
    Q_OBJECT

public:

    wavefront* m_wf;
    ContourTools *m_tools;
    bool m_useMiddleOffset;
    int m_colorMapNdx;
    double m_zOffset;
    double m_waveRange;
    double contourRange;
    QString m_zRangeMode;
    ContourPlot(QWidget * = NULL, ContourTools *tools  = 0, bool minimal = false);
    void setSurface(wavefront * mat);
    void applyZeroOffset(bool useMiddle);
    void setZRange();
    void setColorMap(int ndx);
    void setTool(ContourTools* tool);
    bool m_autoInterval;
    bool m_minimal;
signals:
    void setMinMaxValues(double,double);
    void setWaveRange(double);
    void newContourRange(double);

public Q_SLOTS:
    void showContour( bool on );
    void showSpectrogram( int on );
    void setAlpha( int );


    void ContourMapColorChanged(int);
    void showContoursChanged(double);
    void contourZeroOffsetChanged(const QString & val);
    void contourColorRangeChanged(const QString &arg1);
    void contourIntervalChanged(double);
    void contourWaveRangeChanged(double);
    void on_line_color_changed(QColor);
    void contourFillChanged(int);
#ifndef QT_NO_PRINTER
    void printPlot();
#endif

private:
    void drawCanvas(QPainter* p);
    void initPlot();

    QwtPlotSpectrogram *d_spectrogram;
    QColor m_contourPen;
protected:

};

#endif // CONTOURPLOT_H
