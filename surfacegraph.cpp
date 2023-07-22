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

#include "surfacegraph.h"

#include <QtDataVisualization/QValue3DAxis>
#include <QtDataVisualization/Q3DTheme>
#include <opencv2/imgproc.hpp>
#include <QtCore/qmath.h>
#include <QLabel>
#include <QSettings>
#include <QDebug>
using namespace QtDataVisualization;

int sampleCountX = 50;
int sampleCountZ = 50;

const float sampleMin = -8.0f;
const float sampleMax = 8.0f;
#include <QOpenGLFunctions>
SurfaceGraph::SurfaceGraph(Q3DSurface *surface)
    : m_graph(surface),m_axisMaxSliderXValue(100),m_axisMinSliderXValue(0), m_wf(0), m_colorRange(.56)
{
    m_xGridMin = 0;
    m_graph->setAxisX(new QValue3DAxis);
    m_graph->setAxisY(new QValue3DAxis);
    m_graph->setAxisZ(new QValue3DAxis);

    m_graph->scene()->activeCamera()->setCameraPreset(Q3DCamera::CameraPresetFrontHigh);


    m_mirrorSurfaceProxy = new QSurfaceDataProxy();
    m_wavefrontSeries = new QSurface3DSeries(m_mirrorSurfaceProxy);
    QSettings set;
   m_colorLabelProxy = new QSurfaceDataProxy();
   m_colorLabelSeries = new QSurface3DSeries(m_colorLabelProxy);
   m_yGridHeight = maxHeightSelections[(set.value("yMaxNdx", 8).toInt())];

    m_graph->activeTheme()->setType(Q3DTheme::Theme(3));
    QFont font = m_graph->activeTheme()->font();
    font.setPointSize(20);
    m_graph->activeTheme()->setFont(font);
    m_graph->activeTheme()->setAmbientLightStrength(.5);
    m_graph->scene()->activeLight()->setPosition(QVector3D(600,0,1));
    m_graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);
    //m_graph->activeTheme()->setGridLineColor(Qt::white);

    m_xLight = set.value("xSurfLightParam", 67).toInt();
    m_zLight = set.value("zSurfLightParam", 71).toInt();
    m_yLight = set.value("ySurfLightParam", 99).toInt();

    m_ambient = set.value("ambientLightParam", 71).toInt();
    m_gradientScale = set.value("scaleYSurfParam", 25).toDouble() * .1;
    m_colorMap = new dftColorMap(set.value("colorMapType",0).toInt(),0,false,.125,.7);
    m_yoffsetValue = 0.;
    m_yshiftAuto = set.value("yShiftAuto", 1).toBool();
    enableSqrtSinModel(true);
    fillSurfaceProxy();

}

SurfaceGraph::~SurfaceGraph()
{
    delete m_graph;
}



void SurfaceGraph::enableSqrtSinModel(bool enable)
{
    if (enable) {
        //! [3]
        m_wavefrontSeries->setDrawMode(QSurface3DSeries::DrawSurface);
        m_wavefrontSeries->setFlatShadingEnabled(true);

        m_colorLabelSeries->setDrawMode(QSurface3DSeries::DrawSurface);
        m_colorLabelSeries->setFlatShadingEnabled(true);
        m_graph->axisX()->setLabelFormat("%.2f");
        m_graph->axisZ()->setLabelFormat("%.2f");
        m_graph->axisX()->setRange(sampleMin, sampleMax);
        m_graph->axisY()->setRange(0.0f, m_yGridHeight);
        m_graph->axisZ()->setRange(sampleMin, sampleMax);
        qDebug() << "sampleMin" << sampleMin << sampleMax;
        m_graph->axisX()->setLabelAutoRotation(30);
        m_graph->axisY()->setLabelAutoRotation(90);
        m_graph->axisZ()->setLabelAutoRotation(30);

        m_graph->addSeries(m_wavefrontSeries);
        m_graph->addSeries(m_colorLabelSeries);
        m_graph->setSelectionMode(QAbstract3DGraph::SelectionItemAndRow
                                                                 | QAbstract3DGraph::SelectionSlice);
        //! [3]

        //! [8]
        // Reset range sliders for Sqrt&Sin
        m_rangeMinX = sampleMin;
        m_rangeMinZ = sampleMin;
        m_stepX = (sampleMax - sampleMin) / float(sampleCountX - 1);
        m_stepZ = (sampleMax - sampleMin) / float(sampleCountZ - 1);

        m_graph->axisX()->setTitle("X axis");
        m_graph->axisX()->setTitleVisible(true);
        m_graph->axisZ()->setTitle("Y axis");
        m_graph->axisY()->setTitleVisible(true);
        m_graph->axisZ()->setTitleVisible(true);
        m_graph->axisY()->setTitle("Waves at 550nm");

        setGreenToRedGradient();
        m_graph->scene()->activeLight()->setAutoPosition(false);
        m_graph->activeTheme()->setAmbientLightStrength(m_ambient/100.);
        m_graph->scene()->activeLight()->setPosition(QVector3D((m_xLight - 50) * .1,
                                                               (m_zLight - 50) * .1,
                                                               (m_yLight - 50) * .1));
       /* m_axisMinSliderX->setMaximum(sampleCountX - 2);
        m_axisMinSliderX->setValue(0);
        m_axisMaxSliderX->setMaximum(sampleCountX - 1);
        m_axisMaxSliderX->setValue(sampleCountX - 1);
        m_axisMinSliderZ->setMaximum(sampleCountZ - 2);
        m_axisMinSliderZ->setValue(0);
        m_axisMaxSliderZ->setMaximum(sampleCountZ - 1);
        m_axisMaxSliderZ->setValue(sampleCountZ - 1);
        */
        //! [8]
    }
}
void SurfaceGraph::setColorMap(int ndx){

    m_colorMap = new dftColorMap(ndx,m_wf, false );
    m_colorStops = m_colorMap->userStops;

    if (m_wf)
        setSurface(m_wf);
}
double pround(double x, double precision)
{
    if (x == 0.)
        return x;
    int ex = floor((log10(abs(x))) - precision) + 1;
    double div = pow(10, ex);

    return floor(x / div + 0.5) * div;
}

void SurfaceGraph::setGraphColors(){
    QFont fnt = m_graph->property("font").value<QFont>();
    QFontMetrics fm(fnt);
    int widthf=fm.horizontalAdvance("XXXXXX");

    QLinearGradient gr;
    int height = m_graph->height();
    int width = widthf + 60;
    int border = 40;
    QLinearGradient grl(0, 0, 1, height - 2 * border);
    QVector< double> pos = m_colorMap->colorStops();
    m_gradientScale =  m_maxHeight/m_yGridHeight;
    if (m_gradientScale > 1.) {
        m_gradientScale = 1.;

    }
    gr.setSpread(QGradient::RepeatSpread);
    foreach (double p , pos){
        QColor c = m_colorMap->rgb(QwtInterval(0.,1.), p);
       //qDebug() << "gr"<< p << p * m_gradientScale << m_gradientScale;
        gr.setColorAt(p * m_gradientScale, c);
        grl.setColorAt(1-p,c);
    }

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    m_graph->seriesList().at(1)->setBaseGradient(gr);
    m_graph->seriesList().at(1)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);

    QPixmap pm(width, height);
    pm.fill(Qt::transparent);
    QPainter pmp(&pm);
    pmp.setBrush(QBrush(grl));
    pmp.setPen(Qt::NoPen);
    pmp.drawRect(5, border, 35, height - 2 * border);
    pmp.setPen(Qt::black);
    int step = (height - 2 * border) / 5;
    for (int i = 0; i < 6; i++) {
        int yPos = height -i * step -border;
        pmp.drawLine(border, yPos, 55, yPos);
        pmp.drawText(border/2 + 40, yPos + 2, QString("%1 ").arg(pround(i/5. * m_maxHeight,1.5),0,'f',2));
    }

    m_legend->setPixmap(pm);

}



QString wft = "c:\\TEMP\\Simulated_Wavefront.wft";
void SurfaceGraph::fillSurfaceProxy() {

    if (m_wf == 0) return;
    double diam = m_wf->diameter;
    cv::Mat data;
    m_wf->workData.copyTo(data);
    double min;
    min = m_wf->min;
    double mean = m_wf->mean;
    double std = m_wf->std;


    if (!m_yshiftAuto){
        min = m_yoffsetValue;
    }
    else {
        min = mean -  1.5 * std;
    }
    m_maxHeight = 10 * m_colorRange * std;

    data = data - min;



    double mmperstep = (diam/2)/(m_wf->m_outside.m_radius);

    int steps = 1;
    sampleCountZ = data.rows;
    sampleCountX = data.cols;
    QSurfaceDataArray *dataArray = new QSurfaceDataArray;
    dataArray->reserve(sampleCountZ/steps);
    for (int i = 0 ; i < sampleCountZ ; i += steps) {
        QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX/steps);
        float z = ((i - sampleCountZ/2.) * mmperstep);
        int index = 0;
        for (int j = 0; j < sampleCountX; j += steps) {
            float x = ((j - sampleCountX/2.) * mmperstep);

            float y = data.at<double>(i,j);
            if ((m_wf->workMask.at<uint8_t>(i,j)) != 255){
                y = -.15;
            }
            (*newRow)[index++].setPosition(QVector3D(x, y, z));
        }
        *dataArray << newRow;
    }

    m_mirrorSurfaceProxy->resetArray(dataArray);
    m_graph->axisX()->setRange((-diam/2.-5), (diam/2.) +5);
    m_graph->axisZ()->setRange((-diam/2.)-5, (diam/2.)+5);


    QSurfaceDataArray *colorLabelArray = new QSurfaceDataArray;
    colorLabelArray->reserve(2);
    QSurfaceDataRow *row1 = new QSurfaceDataRow(2);
    (*row1)[0].setPosition(QVector3D( diam/2.-5,  0, diam/2. +5));
    (*row1)[1].setPosition(QVector3D(diam/2 +5,  0, diam/2. +5));
    *colorLabelArray << row1;

    QSurfaceDataRow *row2 = new QSurfaceDataRow(2);
    (*row2)[0].setPosition(QVector3D( diam/2. -5,  m_maxHeight, diam/2 +5));
    (*row2)[1].setPosition(QVector3D(diam/2+5,  m_maxHeight, diam/2 +5));
    *colorLabelArray << row2;

    m_colorLabelProxy->resetArray(colorLabelArray);


    setGraphColors();
}
void SurfaceGraph::range(int val){
    m_colorRange = .01 * val;
    setSurface(m_wf);
    qDebug() << "range"<< m_colorRange;
}

void SurfaceGraph::autoRange(){
    m_colorRange = .52;
    fillSurfaceProxy();
}

void SurfaceGraph::setSurface(wavefront *wf){
        m_wf = wf;
    fillSurfaceProxy();


}
void SurfaceGraph::yshiftAutoChanged(bool b){
    m_yshiftAuto = b;
    setSurface(m_wf);
}
void SurfaceGraph::yOffsetChanged(int val){
    m_yoffsetndx = val;
    if (m_wf == 0) return;


    double min,max;
    int id1,id2;

    cv::minMaxIdx(m_wf->workData, &min, &max, &id1, &id2, m_wf->workMask);

    m_yoffsetValue = -.02 * val * (max - min);

    emit yOffsetValue(m_yoffsetValue);
    setSurface(m_wf);
}

void SurfaceGraph::scaleY(int d){

    m_yGridHeight = maxHeightSelections[d];
    qDebug() << "yscale" << m_yGridHeight <<  m_gradientScale << d;
    m_graph->axisY()->setRange(0.0f, m_yGridHeight);
    setSurface(m_wf);
}
void SurfaceGraph::lightX(int d){
    float pos = (d - 50) * .1;
    m_xLight = d;
    QVector3D current = m_graph->scene()->activeLight()->position();
    m_graph->scene()->activeLight()->setPosition(QVector3D(pos,current.z(), current.y()));
    QSettings set;
    set.setValue("xSurfLightParam", d);
}
void SurfaceGraph::ambient(int d){
    m_ambient = d;
    m_graph->activeTheme()->setAmbientLightStrength(d/100.);
    QSettings set;
    set.setValue("ambientLightParam", d);
}

void SurfaceGraph::lightZ(int d){
        float pos = (d - 50) * .1;
        m_zLight = d;
        QVector3D current = m_graph->scene()->activeLight()->position();
        m_graph->scene()->activeLight()->setPosition(QVector3D(current.x(),pos,current.y()));
        QSettings set;
        set.setValue("zSurfLightParam", d);
}

void SurfaceGraph::lightY(int d){
    m_yLight = d;
    float pos = (d - 50) * .1;
    QVector3D current = m_graph->scene()->activeLight()->position();
    m_graph->scene()->activeLight()->setPosition(QVector3D(current.x(),current.z(),pos));
    QSettings set;
    set.setValue("ySurfLightParam", d);
}
void SurfaceGraph::adjustXMin(int min)
{

    m_xGridMin = min;
    setSurface(m_wf);
    return;
    m_axisMinSliderXValue = min;
    int cols = m_wf->workData.cols;
    double minX = -cols * .501 * m_mmPerPixel + .01 * min * cols * m_mmPerPixel;
    //float minX =
//qDebug() << minX << min;
    int maxX = cols * .501 * m_mmPerPixel;
//    if (min >= max) {
//        max = min + 1;
//        m_axisMaxSliderX->setValue(max);
//    }

qDebug() << "range" << minX << maxX;
    m_graph->axisX()->setRange(minX, maxX);
}

void SurfaceGraph::adjustXMax(int max)
{
    float maxX = m_stepX * float(max) + m_rangeMinX;

    int min = m_axisMinSliderX->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderX->setValue(min);
    }
    float minX = m_stepX * min + m_rangeMinX;

    setAxisXRange(minX, maxX);
}

void SurfaceGraph::adjustZMin(int min)
{
    float minZ = m_stepZ * float(min) + m_rangeMinZ;

    int max = m_axisMaxSliderZ->value();
    if (min >= max) {
        max = min + 1;
        m_axisMaxSliderZ->setValue(max);
    }
    float maxZ = m_stepZ * max + m_rangeMinZ;

    setAxisZRange(minZ, maxZ);
}

void SurfaceGraph::adjustZMax(int max)
{
    float maxX = m_stepZ * float(max) + m_rangeMinZ;

    int min = m_axisMinSliderZ->value();
    if (max <= min) {
        min = max - 1;
        m_axisMinSliderZ->setValue(min);
    }
    float minX = m_stepZ * min + m_rangeMinZ;

    setAxisZRange(minX, maxX);
}

//! [5]
void SurfaceGraph::setAxisXRange(float min, float max)
{
    m_graph->axisX()->setRange(min, max);
}

void SurfaceGraph::setAxisZRange(float min, float max)
{
    m_graph->axisZ()->setRange(min, max);
}
//! [5]

//! [6]
void SurfaceGraph::changeTheme(int theme)
{
    m_graph->activeTheme()->setType(Q3DTheme::Theme(theme));
}
//! [6]

void SurfaceGraph::setBlackToYellowGradient()
{
    //! [7]
    QLinearGradient gr;
    gr.setColorAt(0.12, Qt::blue);

    gr.setColorAt(.3 , Qt::cyan);
    gr.setColorAt(0.45 , QColor(156,83,43));
    gr.setColorAt(.7 , Qt::gray);
    gr.setColorAt(.9  , Qt::red);
    gr.setColorAt(.95 , Qt::white);
    qDebug() << "gggg"<< gr.stops();
    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    //! [7]
}

void SurfaceGraph::setGreenToRedGradient()
{
    QLinearGradient gr;
    gr.setColorAt(0.0, Qt::darkGreen);
    gr.setColorAt(0.2/2.5, Qt::yellow);
    gr.setColorAt(0.7/2.5, Qt::darkRed);
    gr.setColorAt(1.0/2.5, Qt::red);

    m_graph->seriesList().at(0)->setBaseGradient(gr);
    m_graph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
}
QSize SurfaceGraph::Size(){
    return  m_graph->size();
}
QImage SurfaceGraph::render(int width, int height){
    QSize orgsize = m_graph->size();
    if (orgsize.width() < width){
        qDebug() << "m_graph size" << orgsize << width << height;
        m_graph->resize(width, height);
    }
    QImage result = m_graph->renderToImage(0, QSize(width,height));
    m_graph->resize(orgsize);
    return result;
}
