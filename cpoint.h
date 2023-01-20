#ifndef CPOINT_H
#define CPOINT_H
#include <QtGui>
#include <QJsonObject>
class CPoint // aka "control point"
        : public QPointF
{
    // A CPoint consists of what you see as a point in the gui: a point that the curve passes through
    // and also a point to the left and one to the right.  All 3 points are connected by a straight gray
    // line in the gui.  Beziers on the other hand are *between* two cpoints and made up of 4 points.
public:
    CPoint(double _x, double _y);
    CPoint(double _x, double _y, double bez_dist);
    CPoint(QJsonObject &obj);
    double lx,ly,rx,ry; // left and right bezier control points
    void save();  // save all our data in secondary storage
    void restore(); // restore all our data from secondary storage
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
    void toJson(QJsonObject &obj);
    int inflection1; // can be -1,0,1 Slope of inflection point or 0 if indeterminate.  For bezier to the right of this cpoint.
    int inflection2; // slope of second inflection point for bezier to the right of this cpoint
private:
    double x1,x2,x3,y1,y2,y3; // temporary secondary/backup storage of all 3 points
};

#endif // CPOINT_H
