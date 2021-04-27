/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Data Visualization module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SURFACEGRAPH_H
#define SURFACEGRAPH_H

#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include <QtWidgets/QSlider>
#include "wavefront.h"
#include "dftcolormap.h"
#include <QLabel>

using namespace QtDataVisualization;
extern double maxHeightSelections[];
class SurfaceGraph : public QObject
{
    Q_OBJECT
public:
    explicit SurfaceGraph(Q3DSurface *surface);
    ~SurfaceGraph();


    void enableSqrtSinModel(bool enable);

    //! [0]
    void toggleModeNone() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionNone); }
    void toggleModeItem() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItem); }
    void toggleModeSliceRow() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow
                                                          | QAbstract3DGraph::SelectionSlice); }
    void toggleModeSliceColumn() { m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndColumn
                                                             | QAbstract3DGraph::SelectionSlice); }
    //! [0]

    void setBlackToYellowGradient();
    void setGreenToRedGradient();

    void setAxisMinSliderX(QSlider *slider) { m_axisMinSliderX = slider; }
    void setAxisMaxSliderX(QSlider *slider) { m_axisMaxSliderX = slider; }
    void setAxisMinSliderZ(QSlider *slider) { m_axisMinSliderZ = slider; }
    void setAxisMaxSliderZ(QSlider *slider) { m_axisMaxSliderZ = slider; }

    void lightX(int d);
    void lightZ(int d);
    void lightY(int d);
    void ambient(int d);
    void scaleY(int d);

    void adjustXMin(int min);
    void adjustXMax(int max);
    void adjustZMin(int min);
    void adjustZMax(int max);
    void setSurface(wavefront *wf);
    void setGraphColors();
    void setLegend(QLabel * legend) {m_legend = legend;}
    QImage render();
public Q_SLOTS:
    void changeTheme(int theme);
    void setColorMap(int ndx);
    void yOffsetChanged(int val);
    void yshiftAutoChanged(bool b);
    void range(int val);

signals:
    void yOffsetValue(double val);
private:
    Q3DSurface *m_graph;
    QSurfaceDataProxy *m_mirrorSurfaceProxy;
    QSurfaceDataProxy *m_colorLabelProxy;
    QSurface3DSeries *m_wavefrontSeries;
    QSurface3DSeries *m_colorLabelSeries;

    QSlider *m_axisMinSliderX;
    QSlider *m_axisMaxSliderX;
    QSlider *m_axisMinSliderZ;
    QSlider *m_axisMaxSliderZ;
    float m_rangeMinX;
    float m_rangeMinZ;
    float m_stepX;
    float m_stepZ;
    float m_ambient;
    float m_xLight;
    float m_zLight;
    float m_yLight;
    double m_min;
    double m_max;
    double m_mmPerPixel;
    int m_axisMaxSliderXValue;
    int m_axisMinSliderXValue;
    int m_xGridMin;
    int m_yoffsetndx;
    double m_yoffsetValue;
    double m_maxHeight;
    bool m_yshiftAuto;

    int m_heightMapWidth;
    int m_heightMapHeight;
    double m_gradientScale;
    double m_yGridHeight;
    wavefront *m_wf;
    dftColorMap *m_colorMap;
    QList<colorStop> m_colorStops;

    void setAxisXRange(float min, float max);
    void setAxisZRange(float min, float max);
    void fillSurfaceProxy();
    QLabel * m_legend;
    double m_colorRange;
};

#endif // SURFACEGRAPH_H
