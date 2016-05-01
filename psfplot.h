#ifndef PSFPLOT_H
#define PSFPLOT_H

#include <QWidget>
#include <qwt_plot.h>
#include "wavefront.h"
#include <opencv/cv.h>
#include <qwt_plot_legenditem.h>
#include <qwt_legend.h>
#include <qwt_plot_legenditem.h>
namespace Ui {
class psfPlot;
}

class psfPlot : public QwtPlot
{
    Q_OBJECT

public:
    explicit psfPlot(QWidget *parent = 0);
    ~psfPlot();
    void setData(cv::Mat wf, QString label, QPen color);
    void clear();

private:
    Ui::psfPlot *ui;

};

#endif // PSFPLOT_H
