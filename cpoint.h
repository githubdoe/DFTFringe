#ifndef CPOINT_H
#define CPOINT_H
#include <QtGui>

class CPoint // aka "control point"
        : public QPointF
{
public:
    CPoint(double _x, double _y);
    CPoint(double _x, double _y, double bez_dist);
    double lx,ly,rx,ry; // left and right bezier control points
    //double slope;
    //double distLeft;
    //double distRight;
    //double factor;  // saves time in calculating L and R points.
    QPointF getLeft(); // get bezier control point left of and related to this point
    QPointF getRight(); // get bezier control point right of and related to this point
    void setLeft(double x, double y, double graph_ratio); // set position of left control point (changes slope but not distance of right point as well)
    void setRight(double x, double y, double graph_ratio);
    void setX(qreal _x);
    void setY(qreal _y);
};

#endif // CPOINT_H
