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
#ifndef OGLVIEW_H
#define OGLVIEW_H

#include <QWidget>
#include "glwidget.h"
#include "contourtools.h"
#include "oglcontrols.h"
#include "surfaceanalysistools.h"
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QColor>

class OGLView : public QWidget
{
    Q_OBJECT
public:
    explicit OGLView(QWidget *parent = 0, ContourTools *m_tool = 0,
             surfaceAnalysisTools *surfTools = 0);
    GLWidget *m_gl;
    QCheckBox *fillCB;
    QPushButton *lightingPb;
    QSpinBox *vscale;
    QDoubleSpinBox *backWallScaleSB;
    QPushButton *orthoPb;
    QPushButton *backgroundPb;
    QColor m_background;

signals:
    void showAll3d(GLWidget *);
public slots:
    void showAll();
    void setBackground();

};

#endif // OGLVIEW_H
