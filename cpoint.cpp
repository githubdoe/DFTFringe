#include "cpoint.h"

CPoint::CPoint(double _x, double _y) : CPoint(_x, _y, 10) // control point
{

}

CPoint::CPoint(double _x, double _y, double bez_dist) : QPointF(_x,_y) {
    // initialize bezier points just in case
    lx = _x - bez_dist;
    ly = _y;
    rx = _x + bez_dist;
    ry = _y;

}

void CPoint::setX(qreal _x) {
    double dx = _x - x();
    QPointF::setX(_x);

    // move bezier control points the same amount so they follow the main point
    lx += dx;
    rx +=dx;
}
void CPoint::setY(qreal _y) {
    double dy = _y - y();
    QPointF::setY(_y);

    // move bezier control points the same amount so they follow the main point
    ly += dy;
    ry +=dy;
}


QPointF CPoint::getLeft() {
    // get bezier control point left of and related to this point
    return QPointF(lx,ly);
}

QPointF CPoint::getRight() {
    // get bezier control point right of and related to this point
    return QPointF(rx,ry);
}
void CPoint::setLeft(double _x, double _y, double graph_ratio) {
    // set position of left control point (changes slope but not distance of right point as well)
    // graph_ratio is used so that when you move one handle, the other handle keeps it's length but this depends on the graphing ratio
    // because the graph is in units of mm on x axis and waves on y axis
    if (_x > x()-.1)
        _x = x()-.1; // point must be at least .1mm left of main point
    lx = _x;
    ly = _y;

    //
    // now update right point based on new slope
    //

    // calculate slope based on left point
    double dx = x()-lx;
    double dy = (y()-ly)/graph_ratio;
    if (dx==0)
        dx = 1; // 1mm
    double slope = dy/dx;

    // find distance to right point
    dx = x()-rx;
    dy = (y()-ry)/graph_ratio;
    double distRight = sqrt(dx*dx+dy*dy);


    double cos = 1/sqrt(1+slope*slope);
    rx = x() + distRight*cos;
    ry = y() + distRight*cos*slope*graph_ratio;
}
void CPoint::setRight(double _x, double _y, double graph_ratio) {
    // graph_ratio is used so that when you move one handle, the other handle keeps it's length but this depends on the graphing ratio
    // because the graph is in units of mm on x axis and waves on y axis
    if (_x < x()+.1)
        _x = x()+.1; // point must be at least .1mm right of main point

    rx = _x;
    ry = _y;

    //
    // now update left point based on new slope
    //

    // calculate slope based on right point
    double dx = x()-rx;
    double dy = (y()-ry)/graph_ratio;
    if (dx==0)
        dx = -1; // 1mm
    double slope = dy/dx;

    // find distance to left point
    dx = x()-lx;
    dy = (y()-ly)/graph_ratio;
    double distLeft = sqrt(dx*dx+dy*dy);
    double cos = 1/sqrt(1+slope*slope);
    lx = x() - distLeft*cos;
    ly = y() - distLeft*cos*slope*graph_ratio;
}
