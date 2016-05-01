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



signals:

public slots:


};

#endif // OGLVIEW_H
