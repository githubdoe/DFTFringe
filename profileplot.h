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
#ifndef PROFILEPLOT_H
#define PROFILEPLOT_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <qwt_plot.h>
#include "wavefront.h"
#include "contourtools.h"
#include <qwt_compass.h>
#include <qwt_dial.h>
#include "opencv2/core.hpp"
#include <QRadioButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
namespace Ui {
class ProfilePlot;
}

class ProfilePlot : public QWidget
{
    Q_OBJECT

public:
    QwtPlot *m_plot;
    wavefront* m_wf;
    ProfilePlot( QWidget *parent = NULL, ContourTools* tools = 0 );
    QVector<wavefront*> *wfs;
    void setSurface(wavefront * wf);
    virtual void resizeEvent( QResizeEvent * );
    QPolygonF createProfile(double units, wavefront *wf);
    ContourTools *m_tools;
    double m_waveRange;
    virtual bool eventFilter( QObject *, QEvent * );
    QCheckBox *showNmCB;
    QCheckBox *showSurfaceCB;
    QRadioButton *OneOnly;
    QRadioButton *Show16;
    QRadioButton *ShowAll;
    int type;
    double m_showSurface;
    double m_showNm;
    bool zoomed;
    bool m_showSlopeError;
    double slopeLimitArcSec;
    void setDefocusValue(double val);
    void setDefocusWaveFront( cv::Mat_<double> wf);
signals:
    void zoomMe(bool);
    void profileAngleChanged(const double ang);

public slots:
    void setWavefronts(QVector<wavefront*> *wf);
    void angleChanged(double a);
    void newDisplayErrorRange(double min, double max);
    void zeroOffsetChanged(QString);
    void showOne();
    void show16();
    void showAll();
    void showNm(bool);
    void showSurface(bool);
    void zoom();
    void showContextMenu(const QPoint &pos);
    void showSlope(bool);
    void slopeLimit(double);
    void contourPointSelected(const QPointF &pos);

private:
    void populate();
    void updateGradient();
    bool dragging;
    QPoint startPos;
    QString offsetType;
    QwtCompass *compass;
    QCheckBox *showSlopeError;
    QDoubleSpinBox *slopeLimitSB;
    double m_defocusValue;

private:
    Ui::ProfilePlot *ui;
    bool m_defocus_mode;
    cv::Mat_<double> m_defocus_wavefront;
};

#endif // PROFILEPLOT_H

