#ifndef ARBITRARYWAVWIDGET_H
#define ARBITRARYWAVWIDGET_H

#include <QObject>
#include <QWidget>
#include <QtGui>
#include "CPoint.h"

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
    double mirror_radius; // mm
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
    bool bDraggingBevierPoint;
    int dragging_point_index;
    bool bDragging_bev_left;
    bool bDissuadeOverhangs = true;
    int mode=0;  // 0=bezier 1=cubic 2=quadratic
    const double bez_distance_ratio = 15; // take radius of mirror, divide by this number and that's the distance out that the initial bezier control points appear
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

signals:

};

#endif // ARBITRARYWAVWIDGET_H
