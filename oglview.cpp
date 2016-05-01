#include "oglview.h"
#include <QtCore>
#include <QLayout>

OGLView::OGLView(QWidget *parent, ContourTools *m_tool,
                 surfaceAnalysisTools *surfTools) :
    QWidget(parent)
{
    QSettings s;
    m_gl = new GLWidget(this, m_tool, surfTools);
    QHBoxLayout *lh = new QHBoxLayout();
    QVBoxLayout *lv = new QVBoxLayout();
    lv->addLayout(lh);

    fillCB = new QCheckBox("Fill",this);
    lh->addWidget(fillCB);

    bool v = s.value("oglFill", true).toBool();
    fillCB->setChecked(v);
    connect(fillCB, SIGNAL(clicked(bool)), m_gl, SLOT(fillGridChanged(bool)));

    lightingPb = new QPushButton("Lighting",this);
    lh->addWidget(lightingPb);
    connect(lightingPb, SIGNAL(clicked()), m_gl, SLOT(openLightingDlg()));

    QLabel *lb1 = new QLabel("Vertical Scale:",this);
    lb1->setMaximumHeight(10);
    lh->addWidget(lb1);

    vscale = new QSpinBox(this);
    vscale->setSingleStep(10);
    vscale->setMaximum(1000);
    vscale->setValue(200);
    connect(vscale, SIGNAL(valueChanged(int)), m_gl, SLOT(ogheightMagValue(int)));
    lh->addWidget(vscale);

    QLabel *lb2 = new QLabel("BackWall Scale:");
    lh->addWidget(lb2);
    backWallScaleSB = new QDoubleSpinBox(this);
    backWallScaleSB->setDecimals(3);
    backWallScaleSB->setValue(.125);
    backWallScaleSB->setMinimum(.001);
    backWallScaleSB->setSingleStep(.005);

    lh->addWidget(backWallScaleSB);
    connect(backWallScaleSB, SIGNAL(valueChanged(double)), m_gl, SLOT(backWallScale(double)));
    QLabel *lb3 = new QLabel("Waves",this);
    lb3->setMaximumHeight(10);
    lh->addWidget(lb3);


    lh->addStretch();
    lv->addWidget(m_gl);
    setLayout(lv);
}
