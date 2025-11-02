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
#include <opencv2/opencv.hpp>
#include <QRadioButton>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include "percentcorrectiondlg.h"
namespace Ui {
class ProfilePlot;
}

class ProfilePlot : public QWidget
{
    Q_OBJECT

public:
    percentCorrectionDlg *m_pcdlg;
    QwtPlot *m_plot;
    wavefront* m_wf;
    ProfilePlot( QWidget *parent = NULL, ContourTools* tools = 0 );
    ~ProfilePlot();
    QVector<wavefront*> *wfs;
    void setSurface(wavefront * wf);
    virtual void resizeEvent( QResizeEvent * );
    QPolygonF createProfile(double units, wavefront *wf, bool allowOffset = true);
    QPolygonF createAverageProfile(double umnits, wavefront *wf, bool removeNull);
    ContourTools *m_tools;
    double m_waveRange;
    QCheckBox *showNmCB;
    QCheckBox *showSurfaceCB;
    QRadioButton *OneOnly;
    QRadioButton *Show16;
    QRadioButton *ShowAll;
    double m_showSurface;
    double m_showNm;
    bool zoomed;
    bool m_showSlopeError;


    double slopeLimitArcSec;
    void setDefocusValue(double val);
    void setDefocusWaveFront( const cv::Mat_<double> &wf);
signals:
    void zoomMe(bool);
    void profileAngleChanged(const double ang);

public slots:
    void wheelEvent(QWheelEvent *event);
    void setWavefronts(QVector<wavefront*> *wf);
    void angleChanged(double a);
    void newDisplayErrorRange(double min, double max);
    void zeroOffsetChanged(const QString&);
    void showOne();
    void show16();
    void showAll();
    void showNm(bool);
    void showSurface(bool);
    void zoom();
    void showContextMenu(QPoint pos);
    void showSlope(bool);
    void slopeLimit(double);
    void contourPointSelected(QPointF pos);
    void populate();
    void showCorrection();
    void make_correction_graph();
    void showXPercent();
    void showXInches();
    void showXMM();
    //QPolygonF createZernProfile(wavefront *wf);
private:
    enum class ProfileType {
        SHOW_ONE = 0,
        SHOW_16 = 1,
        SHOW_ALL = 2
    };

    void updateGradient();
    void saveXscaleSettings();
    bool dragging;
    QPoint startPos;
    QString offsetType;
    QwtCompass *compass;
    QCheckBox *showSlopeError;

    QDoubleSpinBox *slopeLimitSB;
    double m_defocusValue;

    bool m_displayPercent = false;
    bool m_displayInches = false;
private:

    ProfileType type;


    Ui::ProfilePlot *ui;
    bool m_defocus_mode;
    cv::Mat_<double> m_defocus_wavefront;
};

#endif // PROFILEPLOT_H

