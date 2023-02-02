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
#ifndef PSFPLOT_H
#define PSFPLOT_H

#include <QWidget>
#include <qwt_plot.h>
#include "wavefront.h"
#include <opencv2/opencv.hpp>
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
    void setData(cv::Mat wf, QString label, QPen color, bool doLog);
    void clear();

private:
    Ui::psfPlot *ui;


};

#endif // PSFPLOT_H
