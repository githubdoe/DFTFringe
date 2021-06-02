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
#include "contourtools.h"
#include "surfaceanalysistools.h"
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QColor>
#include "surfacegraph.h"
#include "surface3dcontrolsdlg.h"
#include <QCheckBox>
class OGLView : public QWidget
{
    Q_OBJECT
    Surface3dControlsDlg *m_controls;
public:
    explicit OGLView(QWidget *parent = 0, ContourTools *m_tool = 0,
             surfaceAnalysisTools *surfTools = 0);
    ~OGLView();
    SurfaceGraph *m_surface;
    QCheckBox *fillCB;
    QPushButton *lightingPb;
    QSpinBox *vscale;
    QDoubleSpinBox *backWallScaleSB;
    QPushButton *orthoPb;
    QPushButton *backgroundPb;
    QColor m_background;
    QWidget *m_container;

    QSize sizeHint() const;
    int m_spinRate;

public slots:

    void openLightDlg();
    void saveImage();
    void showSelected();

};

#endif // OGLVIEW_H
