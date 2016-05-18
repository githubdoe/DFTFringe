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

#ifndef GLWIDGET_H
#define GLWIDGET_H
#include <QObject>
#include <QGLWidget>
#include <QVector3D>
#include <vector>
#include <QCheckBox>
#include "wavefront.h"
#include "contourtools.h"
#include "dftcolormap.h"
#include "oglcontrols.h"
#include "surfaceanalysistools.h"
#include "surfacepropertiesdlg.h"
#define RADIUS 400
class CQuad
{
public:
    QVector3D i;
    QVector3D j;
    QVector3D k;
    QVector3D n;
    CQuad() { QVector3D p;  i = j = k = n =  p;}
    CQuad( QVector3D p1, QVector3D p2, QVector3D p3, QVector3D p4): i(p1), j(p2), k(p3), n(p4)
    {};
    CQuad(const CQuad& q)
    {
        *this = q;
    }
    CQuad& operator=(const CQuad& q)
    {
        i = q.i;
        j = q.j;
        k = q.k;
        n= q.n;
        return *this;
    }
};

class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(QWidget *parent = 0, ContourTools *m_tool = 0,
             surfaceAnalysisTools *surfTools = 0);
    ~GLWidget();
    ContourTools* m_tools;

    surfaceAnalysisTools* m_surfTools;
    surfacePropertiesDlg *m_surfProperties;
    void setSurface(wavefront * wf);
    int xRotation() const { return xRot; }
    int yRotation() const { return yRot; }
    int zRotation() const { return zRot; }
    int m_LightParam[12];
    int m_red;
    int m_green;
    int m_blue;
    QCheckBox FillCB;
    QColor m_background;
    void setBackground(QColor c);

public slots:
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void xlightChanged(int value);
    void ylightChanged(int value);
    void zlightChanged(int value);
    void ambientChanged(int value);
    void diffuseChanged(int value);
    void specularChanged(int value);
    void surfaceAmbient(int value);
    void surfaceDiffuse(int value);
    void surfaceSpecular(int value);
    void surfaceShine(int value);
    void surfaceEmission(int value);
    // from countour tools
    void showContoursChanged(double);
    void contourZeroOffsetChanged(const QString &);
    void contourColorRangeChanged(const QString &arg1);
    void contourIntervalChanged(double);
    void contourWaveRangeChanged(double);
    void setRed(int);
    void setGreen(int);
    void setBlue(int);
    void colorMapChanged(int);
    void openLightingDlg();
signals:
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent (QWheelEvent *e);

private slots:
    void fillGridChanged(bool);
    void resolutionChanged(int);
    void ogheightMagValue(int);
    void backWallScale(double v);
    void setMinMaxValues(double, double);
    void perspectiveChanged(bool);

private:

    void normalizeAngle(int *angle);
    void make_surface(void);
    void make_color(double & y, double& r, double& g, double& b);
    void make_surface_from_doubles();
    void DrawScene();
    void SetLight();
    void setZranges();

    wavefront *m_wf;

    int xRot;
    int yRot;
    int zRot;
    double m_edge_mask_offset;
    QString m_zRangeMode;
    double m_profile_scale_setting;
    double m_Vscale;
    double m_max_y;
    double m_min_y;
    double m_fRangeY;
    double m_fRangeX;
    double m_fRangeZ;
    double m_zTrans;
    double m_xTrans;
    double m_yTrans;
    double m_zoomFactor;
    int m_resolutionPercent;
    bool m_GB_enabled;
    int m_gbValue;

    std::vector< CQuad> m_quads;
    std::vector<CQuad> m_grids;
    std::vector<QVector3D> m_vert_profile;
    std::vector<QVector3D> m_horz_profile;

    bool m_use_grey;
    GLenum	m_FillMode;

    QPoint lastPos;
    bool m_draw_profiles_on_3d;
    bool m_list_good;
    bool m_dirty_surface;
    bool m_draw_grid_overlay;
    double m_BackWall_Scale;
    bool m_ortho;
    bool m_flip_y_view;
    bool m_flip_x_view;
    dftColorMap* m_colorMap;


};
/*

Name	Ambient	Diffuse	Specular	Shininess
emerald	0.0215	0.1745	0.0215,	0.07568	0.61424	0.07568	0.633	0.727811	0.633	0.6
jade	0.135	0.2225	0.1575	0.54	0.89	0.63	0.316228	0.316228	0.316228	0.1
obsidian	0.05375	0.05	0.06625	0.18275	0.17	0.22525	0.332741	0.328634	0.346435	0.3
pearl	0.25	0.20725	0.20725	1	0.829	0.829	0.296648	0.296648	0.296648	0.088
ruby	0.1745	0.01175	0.01175	0.61424	0.04136	0.04136	0.727811	0.626959	0.626959	0.6
turquoise	0.1	0.18725	0.1745	0.396	0.74151	0.69102	0.297254	0.30829	0.306678	0.1
brass	0.329412	0.223529	0.027451	0.780392	0.568627	0.113725	0.992157	0.941176	0.807843	0.21794872
bronze	0.2125	0.1275	0.054	0.714	0.4284	0.18144	0.393548	0.271906	0.166721	0.2
chrome	0.25	0.25	0.25	0.4	0.4	0.4	0.774597	0.774597	0.774597	0.6
copper	0.19125	0.0735	0.0225	0.7038	0.27048	0.0828	0.256777	0.137622	0.086014	0.1
gold	0.24725	0.1995	0.0745	0.75164	0.60648	0.22648	0.628281	0.555802	0.366065	0.4
silver	0.19225	0.19225	0.19225	0.50754	0.50754	0.50754	0.508273	0.508273	0.508273	0.4
black plastic	0.0	0.0	0.0	0.01	0.01	0.01	0.50	0.50	0.50	.25
cyan plastic	0.0	0.1	0.06	0.0	0.50980392	0.50980392	0.50196078	0.50196078	0.50196078	.25
green plastic	0.0	0.0	0.0	0.1	0.35	0.1	0.45	0.55	0.45	.25
red plastic	0.0	0.0	0.0	0.5	0.0	0.0	0.7	0.6	0.6	.25
white plastic	0.0	0.0	0.0	0.55	0.55	0.55	0.70	0.70	0.70	.25
yellow plastic	0.0	0.0	0.0	0.5	0.5	0.0	0.60	0.60	0.50	.25
black rubber	0.02	0.02	0.02	0.01	0.01	0.01	0.4	0.4	0.4	.078125
cyan rubber	0.0	0.05	0.05	0.4	0.5	0.5	0.04	0.7	0.7	.078125
green rubber	0.0	0.05	0.0	0.4	0.5	0.4	0.04	0.7	0.04	.078125
red rubber	0.05	0.0	0.0	0.5	0.4	0.4	0.7	0.04	0.04	.078125
white rubber	0.05	0.05	0.05	0.5	0.5	0.5	0.7	0.7	0.7	.078125
yellow rubber	0.05	0.05	0.0	0.5	0.5	0.4	0.7	0.7	0.04	.078125
*/
#endif // GLWIDGET_H

