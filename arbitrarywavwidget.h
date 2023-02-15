#ifndef ARBITRARYWAVWIDGET_H
#define ARBITRARYWAVWIDGET_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#include "cpoint.h"

class ArbitraryWavWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ArbitraryWavWidget(QWidget *parent = nullptr);
    ~ArbitraryWavWidget();
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;
    QList<CPoint> pts;
    double mirror_null;
    enum e_units {mm, cm, in};
    e_units ww_unit = in;
    //struct ipnt {int x; int y;};
    //struct dpnt {double x; double y;};

    // these next 4 functions: transx translates distance in mm to position in our graph and back
    //                         transy translates wavelength to position in our graph and back
    int transx(double x);
    int transy(double y);
    double transx(int x);
    double transy(int y);
    double transRatio(); // y/x scaling
    void setMode(int _mode);
    void prepare(int size);
    double getValue(double rho); // return a wave height value for this diameter (rho = radius from 0.0 to 1.0)
    void showPrepare(); // for testing only
    void setRadius(double radius);
    void setYscale(double value);
    void saveJson();
    void readJson();
private:
    int height;
    int width;
    int graph_left; // where actual lines are being drawn - this is also pixels reserved for y axis labels
    int graph_height; // pixels reserved for actual graph - height-graph_height is reserved for x axis labels
    int pos_y0; // position of graph_height/2
    int pos_edge; // position in graph of right edge of mirror (near right edge of graph
    double wave_height; // .25 means the graph only goes up to +.25 waves and down to -.25 waves
    void sortPoints();
    bool bDragging;
    bool bDraggingBezierPoint;
    int dragging_point_index;
    bool bDragging_bez_left;
    bool bDissuadeOverhangs = true;
    bool bDrawCalculatedPoints = false; // this is for testing purposes only - to examine the data created in prepare()
    int mode=0;  // 0=bezier 1=cubic 2=quadratic
    const double bez_distance_ratio = 15; // take radius of mirror, divide by this number and that's the distance out that the initial bezier control points appear
    double * wf_array=0;
    int wf_array_size=0;
    double mirror_radius; // mm

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent * event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    int findPoint(QPoint p1);
    void doCurve(QPainter &painter, int left_point_x, int left_point_y, int right_point_x, int right_point_y);
    void fixOverhangs(int index);
    bool testInflections(int index); // test inflection slopes for bez to the right of this cpoint and return true if inflections changed
    int radius_to_index(double r);
    double rho_to_index(double r);
    double index_to_radius(int i); // used only for testing

signals:
    void yScaleChanged(double scale);

};

#endif // ARBITRARYWAVWIDGET_H
