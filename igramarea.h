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
#ifndef IGRAMAREA_H
#define IGRAMAREA_H


/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <QColor>
#include <QImage>
#include <QPoint>
#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QLabel>
#include <qscrollarea>
#include "Circleoutline.h"
#include <list>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "dftthumb.h"
#include <QTimer>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QScrollArea>
#include "settingsigram.h"

extern QScrollArea *gscrollArea;
enum {OutSideOutline, CenterOutline, PolyArea};
enum zoomMode {NORMALZOOM, EDGEZOOM};
class regionEditTools;
class outlinePair{
public:
    QImage m_image;
    CircleOutline m_outline;
    outlinePair(QImage img, CircleOutline outline): m_image(img), m_outline(outline){}
};

class undoStack {
    std::list<outlinePair> m_stack;
    std::list<outlinePair> m_redo;

public:
    undoStack(){};
    void push(QImage img, CircleOutline outline);
    outlinePair  current(){return m_stack.back();};
    outlinePair undo();
    outlinePair redo();
    void clear();

};



class IgramArea : public QWidget
{
    Q_OBJECT

public:
    IgramArea(QWidget *parent = 0, void *mwp = 0 );
    QMediaPlayer *player;
    QMediaPlaylist *playlist;
    QVideoWidget *videoWidget;
    void *m_mw;
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName, const char *fileFormat);
    void setPenColor(const QColor &newColor);
    void setPenWidth(int newWidth);
    bool isModified() const { return modified; }
    QColor penColor() const { return edgePenColor; }
    int penWidth() const { return edgePenWidth; }
    QString lastPath;
    bool m_doGamma;
    double m_gammaValue;
    void crop();
    void saveOutlines();
    void deleteOutline();
    void readOutlines();

    void SideOutLineActive(bool checked);
    void CenterOutlineActive(bool checked);
    void PolyAreaActive(bool checked);
    void nextStep();
    bool sideOutlineIsActive;
    void save();

    void doGamma(double gammaV);
    QImage igramImage;
    QString m_filename;
    CircleOutline m_outside;
    CircleOutline m_center;
    void hideOutline(bool checked);
    bool m_hideOutlines;
    void loadOutlineFile(QString filename);
    void undo();
    void redo();
    void writeOutlines(QString fileName);
    QString makeOutlineName();
    void shiftoutline(QPointF p);
    void setZoomMode(zoomMode mode);


public slots:
    void gammaChanged(bool, double);
    void generateSimIgram();
    void clearImage();
    void dftReady(QImage img);
    void outlineTimerTimeout();
    void shiftUp();
    void shiftDown();
    void shiftRight();
    void shiftLeft();
    void zoomIn();
    void zoomOut();
    void igramOutlineParmsChanged(outlineParms parms);
    void increase( int i = 1);
    void decrease(int i = 1);
    void zoomFull();
    void toggleHideOutline();
    void edgeMode();
signals:
    void enableShiftButtons(bool);
    void statusBarUpdate(QString);
    void selectDFTab();
    void upateColorChannels(QImage);
    void showTab(int);
    void dftCenterFilter(double);
    void imageSize(QString);
    void doDFT();
protected:
    bool eventFilter(QObject *object, QEvent *event);
    void wheelEvent(QWheelEvent * event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void computeZoomBounds();
    QPointF calculateZoomedPt(QPointF p);
    void DrawZoomed(void);
    void DrawSimIgram(void);
private:
    QPushButton *m_demo;

    void drawBoundary();
    void resizeImage();
    void createActions();
    void zoom(int del, QPointF zoompt);
    bool modified;
    bool scribbling;
    bool regionMode;
    bool verticalTracking;
    int edgePenWidth;
    int centerPenWidth;
    double opacity;
    double m_lastGamma;
    bool needToConvertBGR;
    QColor centerPenColor;
    QColor edgePenColor;
    bool m_autoSaveOutline;
    void deleteRegions();

public:
    QImage igramDisplay;
    QVector<std::vector<cv::Point> > m_polygons;
    int polyndx;
    regionEditTools *m_regionEdit;
    void syncRegions();
private:
    QImage m_withOutlines;
    QPointF m_OutterP1;
    QPointF m_OutterP2;
    QPointF m_innerP1;
    QPointF m_innerP2;
    QPointF lastPoint;
    QPointF zoompt;

    undoStack m_outsideHist;
    undoStack m_centerHist;
    QAction *fitToWindowAct;
    double scale;
    double fitScale;
    int outterPcount;
    int innerPcount;

    int lineStyle;


    int zoomIndex;
    bool dragMode;
    QPointF dragStart;
    int crop_dx;
    int crop_dy;
    int cropTotalDx, cropTotalDy;
    double cropScale;
    dftThumb *m_dftThumb;
    QTimer *m_outlineTimer;
    bool hasBeenCropped;
    bool m_edgeMode;
    int m_zoomBoxWidth;
    zoomMode m_zoomMode;
    void increaseRegion(int n, double scale);
public:
   int m_current_boundry;
public slots:
   void addregion();
   void deleteregion(int);
   void selectRegion(int);
   // m_mw(mw),QWidget(parent),scale(1.),outterPcount(0), innerPcount(0), zoomFactor(0.),m_current_boundry(OutSideOutline),
      //zoomIndex(0),dragMode(false),m_hideOutlines(false),cropTotalDx(0), cropTotalDy(0), hasBeenCropped(false)
};
//! [0]

#endif
