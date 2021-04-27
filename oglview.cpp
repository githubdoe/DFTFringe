/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "oglview.h"
#include <QtCore>
#include <QLayout>
#include <QColorDialog>
#include <QShortcut>
#include <QMenu>
#include <QFileDialog>
#include <QSettings>
#include <QSpinBox>
#include "videosetupdlg.h"
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include "surfacegraph.h"
#include <QMessageBox>
#include <QGroupBox>

using namespace QtDataVisualization;
OGLView::OGLView(QWidget *parent, ContourTools *m_tool,
                 surfaceAnalysisTools *surfTools) :
    QWidget(parent), m_spinRate(5)
{

    QSettings s;
    Q3DSurface *graph = new Q3DSurface();
    m_surface = new SurfaceGraph(graph);
    QWidget *container = QWidget::createWindowContainer(graph);

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return;
    }
    container->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    container->setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_controls = new Surface3dControlsDlg(m_surface);

    QHBoxLayout *topH = new QHBoxLayout();
    QHBoxLayout *lh = new QHBoxLayout();
    QVBoxLayout *lv = new QVBoxLayout();

    lv->addLayout(lh);

    QVBoxLayout *rightcontrols = new QVBoxLayout();

    QLabel *colorLedgend = new QLabel(this);
    //colorLedgend->setPixmap(pm);
    m_surface->setLegend(colorLedgend);

    QGroupBox *heightMapGroupBox = new QGroupBox(QStringLiteral("Waves 550nm"));
    //heightMapGroupBox->setStyleSheet({"background: grey;"});
    QVBoxLayout *colorMapVBox = new QVBoxLayout;
    colorMapVBox->addWidget(colorLedgend);
    heightMapGroupBox->setLayout(colorMapVBox);


    rightcontrols->addWidget(heightMapGroupBox);
    lightingPb = new QPushButton("3D Controls",this);
    lh->addWidget(lightingPb);
    connect(lightingPb, SIGNAL(pressed()), this,SLOT(openLightDlg()));

    connect(m_tool,SIGNAL(ContourMapColorChanged(int)), m_surface, SLOT(setColorMap(int)));

    lh->addStretch();
    lv->addWidget(container);
    topH->addLayout(lv,10);
    topH->addLayout(rightcontrols,1);
    setLayout(topH);

}
OGLView::~OGLView(){
    m_controls->close();
    delete m_controls;
}
void OGLView::openLightDlg(){
    m_controls->show();
}
QSize OGLView::sizeHint() const{
    return QSize(300,300);
}


