#ifndef PIXELSTATS_H
#define PIXELSTATS_H

#include <QWidget>
#include <opencv2/opencv.hpp>
#include "wavefront.h"
#include <qwt_plot.h>
#include <QScrollArea>
#include <qwt_plot_marker.h>

namespace Ui {
class pixelStats;
}

class pixelStats : public QWidget
{
    Q_OBJECT

public:
    explicit pixelStats(QWidget *parent = 0);
    ~pixelStats();
    void setData(const wavefront *w);
private slots:
    void bounds_valueChanged();

    void on_fit_clicked(bool checked);

    void on_radioButton_2_clicked();

    void on_minmaxloc_clicked(bool checked);

    void on_arcSecs_valueChanged(double arg1);

private:
    Ui::pixelStats *ui;
    const wavefront *m_wf;
    cv::Mat mask;
    void updateHisto();
    void updateSurface();
    QScrollArea *scrollArea;
    double slopeLimitArcSec;
};

class QPoint;
class QCustomEvent;

class CanvasPicker: public QObject
{
    Q_OBJECT
public:
    CanvasPicker( QwtPlot *plot );
    virtual bool eventFilter( QObject *, QEvent * );

    virtual bool event( QEvent * );
signals:
    void markerMoved();

private:
    void select( QPoint  );
    void move( QPoint  );
    void release();

    void shiftCurveCursor();

    QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotMarker *d_selectedMarker;

};
#endif // PIXELSTATS_H
