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
#ifndef INTENSITYPLOT_H
#define INTENSITYPLOT_H
#include <QWidget>
#include <qwt_plot.h>
#include <opencv2/opencv.hpp>

class intensityPlot:  public QwtPlot
{
Q_OBJECT
public:
    intensityPlot(QWidget *parent = NULL);
    virtual ~intensityPlot() {}
    void setSurface(cv::Mat imgMat);
    virtual void resizeEvent( QResizeEvent * );
    void showRed(bool);
    void showBlue(bool);
    void showGreen(bool);
    double m_waveRange;
    virtual bool eventFilter( QObject *, QEvent * );
    cv::Mat m_img;
    cv::Mat planes[3];
public slots:
    void angleChanged(double a);


private:
    void populate();
    void updateGradient();
    QPoint startPos;
    double m_angle;
    bool m_showRed;
    bool m_showGreen;
    bool m_showBlue;

};




#endif // INTENSITYPLOT_H
