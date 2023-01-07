#include "arbitrarywavwidget.h"
#include "bezier.h"


ArbitraryWavWidget::ArbitraryWavWidget(QWidget *parent)
    : QWidget{parent} {
    mirror_null = 0;
    mirror_radius = 12*25;  // 24 inch mirror by default
    wave_height=0.3;  // by default we start at +/- .25 waves of vertical scale
    ww_unit=in; // default inches
    double bez_dist = mirror_radius/2;
    pts.append(CPoint(0,0, bez_dist)); // mandatory
    //pts.append(CPoint(mirror_radius,0.125, bez_dist)); // optional starting points
    // debugging test pts[1].setLeft(pts[1].x()-3, pts[1].y()-.3,0.1);
    bDragging=false;
    bDraggingBevierPoint = false;

    // the following are just initial values that will not let the transx() and transy() functions get a divide by zero
    pos_y0=1;
    graph_height=2;
    graph_left = 1;
    pos_edge = 2;
}

void ArbitraryWavWidget::setRadius(double radius) {
    if (radius == mirror_radius)
        return;

    mirror_radius = radius;
    double bez_dist = mirror_radius/2;
    pts.clear();
    pts.append(CPoint(0,0, bez_dist)); // mandatory
    //pts.append(CPoint(mirror_radius,0.125, bez_dist)); // optional starting points
    bDragging=false;
    bDraggingBevierPoint = false;

}

void ArbitraryWavWidget::showPrepare() {
    // for testing - display the prepared data in this graph widget
    bDrawCalculatedPoints=true;
    update(); // redraw
}

void ArbitraryWavWidget::setMode(int _mode) {
    mode = _mode;
    update(); // redraw
    }

ArbitraryWavWidget::~ArbitraryWavWidget(){
    pts.empty();
    qDebug() << "wav widget destructor";
}

bool comparePoints(const QPointF &a, const QPointF &b) {
    return a.x() < b.x();
}

void ArbitraryWavWidget::sortPoints() {
     std::sort(pts.begin(), pts.end(), comparePoints);
}

QSize ArbitraryWavWidget::sizeHint() const {
    return QSize(1400,400);
}
QSize ArbitraryWavWidget::minimumSizeHint() const {
    return QSize(200,100);

}

void ArbitraryWavWidget::resizeEvent(QResizeEvent * /*event*/) {

}

// these next 4 functions: transx translates distance in mm to position in our graph and back
//                         transy translates wavelength to position in our graph and back
int ArbitraryWavWidget::transx(double r){
    return (r/mirror_radius)*(pos_edge-graph_left) + graph_left;
}
int ArbitraryWavWidget::transy(double y){
    // -y because positive pixels is in the negative y direction (more positive pixels towards bottom of graph)
    return -y/wave_height*(graph_height-pos_y0) + pos_y0;
}
double ArbitraryWavWidget::transx(int x){
    return (x-graph_left)*mirror_radius/(pos_edge-graph_left);
}
double ArbitraryWavWidget::transy(int y){
    return (y-pos_y0)*(-wave_height)/(graph_height-pos_y0);
}

double ArbitraryWavWidget::transRatio(){
    return fabs((wave_height)/(graph_height-pos_y0) / (mirror_radius/(pos_edge-graph_left)));
}

int ArbitraryWavWidget::findPoint(QPoint p1) {
    for(int i=0; i<pts.size(); i++) {
        QPointF p2 = pts.at(i);
        if ( (abs(p1.x() - transx(p2.x())) < 8) &&
             (abs(p1.y() - transy(p2.y())) < 8) ) {
            return i;
        }
    }
    return -1; // not found
}

void ArbitraryWavWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        dragging_point_index = findPoint(event->pos());
        if (dragging_point_index >=0) {
            bDragging=true;
            return;
        }
        if (mode == 0) {
            // we are in bezier mode.  Search those control points now...
            //dragging_bev_index = findBev(event->pos());
            for(int i=0; i<pts.size(); i++) {
                QPoint const &p1 = event->pos();
                CPoint main_point = pts.at(i);

                QPointF p2 = main_point.getLeft();
                if ( (abs(p1.x() - transx(p2.x())) < 8) &&
                     (abs(p1.y() - transy(p2.y())) < 8) ) {
                    // found it
                    dragging_point_index = i;
                    bDragging_bev_left = true;
                    bDraggingBevierPoint=true;
                    return;
                }

                p2 = main_point.getRight();
                if ( (abs(p1.x() - transx(p2.x())) < 8) &&
                     (abs(p1.y() - transy(p2.y())) < 8) ) {
                    // found it
                    dragging_point_index = i;
                    bDragging_bev_left = false; // it's the right point
                    bDraggingBevierPoint=true;
                    return;
                }
            }
        }
        // let's create a new point here
        QPoint ip = event->pos();
        CPoint newp(transx((int)ip.x()), transy((int)ip.y()), mirror_radius/bez_distance_ratio);
        pts.append(newp);
        sortPoints();
        dragging_point_index = findPoint(event->pos());
        // default handles to half way in X of nearest two points
        if (dragging_point_index>0) {
            // left handle of new point
            CPoint &myPoint = pts[dragging_point_index];
            CPoint &leftPoint = pts[dragging_point_index-1];
            double desired_length = fabs(myPoint.x() - leftPoint.x()) / 2;
            myPoint.lx = myPoint.x() - desired_length;
            // right handle of prev point
            leftPoint.setRight(leftPoint.x()+desired_length, leftPoint.ry,transRatio());
        }
        if (dragging_point_index == pts.size()-1) {
            // added point is last point in list
            CPoint &myPoint = pts[dragging_point_index];
            myPoint.rx = myPoint.x() + 10; // last point - just make handle 10mm out.  Always
        } else {
            // more points to the right
            CPoint &myPoint = pts[dragging_point_index];
            CPoint &nextPoint = pts[dragging_point_index+1];
            double desired_length = fabs(myPoint.x() - nextPoint.x()) / 2;
            myPoint.rx = myPoint.x() + desired_length;
            nextPoint.setLeft(nextPoint.x() - desired_length, nextPoint.ly, transRatio());
        }

        if (dragging_point_index>=0)
            bDragging=true; // hopefully it always gets here
        update();
    } else {
        if (event->button() == Qt::RightButton) {
            // delete point (if mouse over point)
            int i = findPoint(event->pos());
            if (i>0) // can't delete point 0 - it's permanent
                pts.removeAt(i);
            update();
        }
    }
}
void ArbitraryWavWidget::mouseMoveEvent(QMouseEvent *event) {
    if (bDragging == false && bDraggingBevierPoint == false )
        return;

    if ( (event->buttons() & Qt::LeftButton) == 0)
        return;

    // update position of point
    QPoint ip = event->pos();

    if (bDragging) {
        pts[dragging_point_index].setX(transx(ip.x()));
        pts[dragging_point_index].setY(transy(ip.y()));

        if (dragging_point_index == 0)
            pts[dragging_point_index].setX(0); // this point always at mirror center and can't be moved sideways
        if (ip.x() > width)
            pts[dragging_point_index].setX(transx(width)); // don't allow points to go outside graph area on right side
        if (ip.x() < graph_left)
            pts[dragging_point_index].setX(0); // don't allow points to go outside graph area on left side


        //qDebug() << "x " << ip.x() << " y " << ip.y() << " tx " << transx(ip.x()) << " ty " << transy(ip.y());
        sortPoints();// this messes up which point is being dragged so re-locate the point
        int i = findPoint(event->pos());
        if (i>=0) {
            dragging_point_index=i;
            fixOverhangs(i);
        }
    }
    if (bDraggingBevierPoint) {
        double x = transx(ip.x());
        double y = transy(ip.y());
        if (dragging_point_index == 0)
            y = pts[0].y(); // first point (pts[0]) bez control point forced to be horizontal
        if (bDragging_bev_left)
            pts[dragging_point_index].setLeft(x,y,transRatio());
        else
            pts[dragging_point_index].setRight(x,y,transRatio());
        fixOverhangs(dragging_point_index);
    }

    update(); // redraw
}

void ArbitraryWavWidget::fixOverhangs(int index) {
    if (bDissuadeOverhangs == false)
        return;
    if (index < 0 || index >= pts.size())
        return;

    if (index>0) {
        // left handle of point to fix
        CPoint &myPoint = pts[index];
        CPoint &leftPoint = pts[index-1];
        double max_combined_length = fabs(myPoint.x() - leftPoint.x()) * 2;
        double combined_length = myPoint.x()-myPoint.lx +
                                leftPoint.rx - leftPoint.x();
        if (combined_length > max_combined_length) {
            // scale back both bez handles in x distance
            double scaleback = max_combined_length/combined_length;
            myPoint.setLeft(myPoint.x() - (myPoint.x() - myPoint.lx)*scaleback, myPoint.ly, transRatio());
            leftPoint.setRight(leftPoint.x() + (leftPoint.rx-leftPoint.x())*scaleback, leftPoint.ry, transRatio());
        }
    }
    if (index == pts.size()-1) {
        // point is last point in list - don't mess with right handle
    } else {
        // more points to the right
        CPoint &myPoint = pts[index];
        CPoint &nextPoint = pts[index+1];
        double max_combined_length = fabs(nextPoint.x() - myPoint.x()) * 2;
        double combined_length = nextPoint.x()-nextPoint.lx +
                                myPoint.rx - myPoint.x();
        if (combined_length > max_combined_length) {
            // scale back both bez handles in x distance
            double scaleback = max_combined_length/combined_length;
            myPoint.setRight(myPoint.x() + (myPoint.rx - myPoint.x())*scaleback, myPoint.ry, transRatio());
            nextPoint.setLeft(nextPoint.x() - (nextPoint.x()-nextPoint.lx)*scaleback, nextPoint.ly, transRatio());
        }
    }



}

void ArbitraryWavWidget::mouseReleaseEvent(QMouseEvent * event) {
    bDragging = false;
    bDraggingBevierPoint = false;
    int index = findPoint(event->pos());
    fixOverhangs(index);
}

void ArbitraryWavWidget::wheelEvent(QWheelEvent *event) {
    //qDebug() << "wheel event" << event->angleDelta().y();
    if (event->angleDelta().y() > 0) {
        // scroll up.  zoom in.
        wave_height /= 1.1;
    }
    if (event->angleDelta().y() < 0) {
        // scroll down.  zoom out.
        wave_height *= 1.1;
    }
    update(); // redraw
    event->accept();
}

void ArbitraryWavWidget::doCurve(QPainter &painter, int left_point_x, int left_point_y, int right_point_x, int right_point_y) {
    // do a cubic double curve (an S curve) such that slope is zero at each end
    // Formula is basically in the form of y=jerk*x^3 for each of the 2 curves
    // I think of y=a*x^2 as a motion where a is acceleration.  For cubics the term is called
    // jerk (then snap, crackle, pop or sometimes snap is called jounce)

    double half_dist = (right_point_x - left_point_x)/2; // horizontal distance to fill with each curve
    double half_height = (right_point_y - left_point_y)/2; // vertical distance to fill with each curve
    // y=jerk*(x-left_point_x)^3+left_point_y
    double jerk = half_height/pow(half_dist,3);
    int ix_prev=0, iy_prev=0;
    bool bFirst;
    // left half of curve
    bFirst=true;
    for (int ix = left_point_x; ix <= left_point_x+half_dist; ix++) {
        double cubed = ix - left_point_x;
        cubed = cubed*cubed*cubed;
        int iy = jerk*cubed+left_point_y+0.5; // 0.5 is for rounding to nearest integer
        if (bFirst) {
            bFirst=false;
            painter.drawPoint(ix, iy);
        }
        else
            painter.drawLine(ix_prev,iy_prev,ix,iy);
        ix_prev=ix;
        iy_prev=iy;
    }
    // right half of curve
    for (int ix = left_point_x+half_dist+1; ix <= right_point_x; ix++) {
        double cubed = right_point_x - ix;
        cubed = cubed*cubed*cubed;
        int iy = right_point_y-jerk*cubed+0.5; // 0.5 is for rounding to nearest integer
        if (bFirst) {
            // seems impossible but possibly the earlier loop never had any points
            bFirst=false;
            painter.drawPoint(ix, iy);
        }
        else
            painter.drawLine(ix_prev,iy_prev,ix,iy);
        ix_prev=ix;
        iy_prev=iy;
    }

}

void ArbitraryWavWidget::paintEvent(QPaintEvent * /*event*/) {


    QPainter painter(this);
    QPen grayPen(Qt::gray, 1, Qt::SolidLine);
    QPen blackPen(Qt::black, 1, Qt::SolidLine);
    QPen bluePen(Qt::blue, 1.5, Qt::SolidLine);
    QFont font = QFont("Arial",10);
    painter.setFont(font);

    //
    // calculate some "constants" that we need to know: the size of the graph, the size of the margins and so on.
    // Note that most of these are also used in transx() and transy() functions which are called on mouse
    // operations as well as this function paintEvent().  That way we can think of and store most
    // points as (radius, wave_height) and let the 4 transx(), transy() functions translate to and from pixel positions
    //

    QSize sz = size();
    height = sz.height();
    width = sz.width();

    QFontMetrics fm(font);
    const int y_axis_font_width = fm.horizontalAdvance("-1.125");
    const int x_axis_font_width = fm.horizontalAdvance("9999 mm");
    const int font_height = fm.height();
    graph_left = y_axis_font_width+3; // space needed left of graph for vertical axis labels plus some extra pixels
    graph_height = height
            - (font_height+2); // space needed below graph for x axis labels
    pos_y0 = graph_height/2;
    pos_edge = width - (width-graph_left)*0.05; // .05 means 5% reserved to go beyond edge of mirror

    // draw border
    painter.drawLine(0,0,0,height-1);
    painter.drawLine(0,height-1,width-1,height-1);
    painter.drawLine(width-1,height-1,width-1,0);
    painter.drawLine(width-1,0,0,0);

    // graph grid
    double y=0;
    int iy;
    // show horizontal lines.  First determine spacing.
    double vertical_spacing=0.125; // 1/8 wave - default
    if (wave_height/.125 > 12)
        vertical_spacing=1;
    else if (wave_height/.125 > 6)
        vertical_spacing=0.25;



    while(y < wave_height) {
        if (y==0)
            painter.setPen(blackPen);
        else
            painter.setPen(grayPen);

        //
        // upper horizontal lines (+y) and lables
        //

        iy = transy(y);
        painter.drawLine(graph_left, iy, pos_edge, iy);

        // text
        QRect rect1(2,iy-font_height/2, graph_left, font_height);
        painter.drawText(rect1,QString{ "%1" }.arg(y, 0, 'f', 3));

        //
        // now do lower horizontal lines (-y) and labels (y=0 line drawn twice)
        //

        iy = transy(-y);
        painter.drawLine(graph_left, iy, pos_edge, iy);

        // text
        QRect rect2(2,iy-font_height/2, graph_left, font_height);
        painter.drawText(rect2,QString{ "%1" }.arg(-y, 0, 'f', 3));

        y += vertical_spacing;
    }

    // determine frequency of vertical lines
    double spacing=1; // always in mm
    switch(ww_unit)
    {
        case in:
            spacing=25.4;
            break;
        case cm:
        case mm:
            spacing=10;
            break;
    }

    // now reduce the number of vertical lines - we want at least 150 pixels between lines
    int pix_dist = transx(spacing);
    int count=1;
    while (pix_dist < 150) { // number here is minimum pixels per vertical line
        count++;
        pix_dist = transx(spacing*count);
    }
    spacing *= count;
    qDebug() << "spacing: " << spacing;

    // left edge of graph
    int ix;
    ix = pos_edge;
    painter.drawLine(ix,0,ix,graph_height);


    double x=0;
    while(x <= mirror_radius) {
        if (x==0)
            painter.setPen(blackPen);
        else
            painter.setPen(grayPen);
        ix = transx(x);
        painter.drawLine(ix,0,ix,graph_height);

        // text
        QString s;
        switch (ww_unit) {
        case mm:
            s = QString{ "%1 mm"}.arg(x, 0, 'f', 0);
            break;
        case cm:
            s = QString{ "%1 cm"}.arg(x/10, 0, 'f', 0);
            break;
        case in:
        default:
            s = QString{ "%1 in"}.arg(x/25.4, 0, 'f', 1);
            break;
        }

        QRect rect2(ix-x_axis_font_width/2, graph_height+1, x_axis_font_width, font_height);
        painter.drawText(rect2, Qt::AlignCenter, s);

        x+=spacing;
    }

    // draw the control points
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    painter.setPen(grayPen);
    for(int i=0; i<pts.size(); i++) {
        CPoint p = pts.at(i);
        if (this->mode==0) {
            // bezier mode - draw the control points
            //left
            QPointF pbez = p.getLeft();
            if (i>0) {
                // don't draw left of first point - that control point not used
                painter.drawLine(transx(p.x()), transy(p.y()), transx(pbez.x()), transy(pbez.y()) );
                painter.drawRect(transx(pbez.x())-3, transy(pbez.y())-2, 4, 4);
            }
            //right
            pbez = p.getRight();
            painter.drawLine(transx(p.x()), transy(p.y()), transx(pbez.x()), transy(pbez.y()) );
            painter.drawRect(transx(pbez.x())-3, transy(pbez.y())-2, 4, 4);
        }
        // do the main point after the bez lines
        painter.drawEllipse(transx(p.x())-5, transy(p.y())-4, 8, 8);  // 10,10 is the width/height of the circle;  -5 is because this function tells where corner goes
    }


    // draw the curves
    painter.setPen(bluePen);

    if (this->mode == 1) {
        // cubic mode
        QPointF leftPoint = pts.at(0);
        int left_point_x = transx(leftPoint.x());
        int left_point_y = transy(leftPoint.y());
        int right_point_x=0, right_point_y=0;
        for(int i=1; i<pts.size(); i++) {
            QPointF p = pts.at(i);
            right_point_x = transx(p.x());
            right_point_y = transy(p.y());

            doCurve(painter, left_point_x, left_point_y, right_point_x, right_point_y);

            left_point_x=right_point_x;
            left_point_y=right_point_y;
        }

        if (right_point_x < transx(mirror_radius)) {
            doCurve(painter, left_point_x, left_point_y, width, left_point_y);
        }
    } else if (this->mode==0) {
        // bezier mode
        Bezier::Point bez_pts[4];
        for(int i=1; i<pts.size(); i++) { // start at 1 and we will grab 2 points at a time
            CPoint p1 = pts.at(i-1);
            CPoint p4 = pts.at(i);
            QPointF p2 = p1.getRight();
            QPointF p3 = p4.getLeft();

            bez_pts[0]= Bezier::Point(p1.x(), p1.y());
            bez_pts[1]= Bezier::Point(p2.x(), p2.y());
            bez_pts[2]= Bezier::Point(p3.x(), p3.y());
            bez_pts[3]= Bezier::Point(p4.x(), p4.y());

            Bezier::Bezier<3> bez(bez_pts,4);
            Bezier::Point p_on_bez1, p_on_bez2;
            p_on_bez1 = bez.valueAt(0);
            const double t_increment = .01;
            for (double t=t_increment; t<=1; t+=t_increment) {
                p_on_bez2 = bez.valueAt(t);
                if (p_on_bez1.x > mirror_radius)
                    painter.setPen(grayPen);
                painter.drawLine(transx(p_on_bez1.x),transy(p_on_bez1.y),transx(p_on_bez2.x),transy(p_on_bez2.y));
                p_on_bez1 = p_on_bez2;
            }
        }

    }


    if (bDrawCalculatedPoints) {
        // for testing purposes - draw the prepare() data
        QPen redPen(Qt::red, 1.5, Qt::SolidLine);
        painter.setPen(redPen);
        for (int i=0; i<wf_array_size; i++) {
            int ix = transx(index_to_radius(i));
            int iy = transy(wf_array[i]);
            painter.drawPoint(ix,iy);
        }
    }


}
int ArbitraryWavWidget::radius_to_index(double r) {
    if (mirror_radius==0)
        return 0;
    return (int)(r/mirror_radius*(wf_array_size-1)+0.5);
}

double ArbitraryWavWidget::rho_to_index(double r) {
    // rho is radius but value goes from 0 to 1.00
    // return value is a double on purpose!  This is important
    double index =  (r*(wf_array_size-1)+0.5);
    if (index < 0) return 0;
    //if (index >= wf_array_size) return wf_array_size-1;
    return index;
}

double ArbitraryWavWidget::index_to_radius(int i) { // used only for testing
    if (wf_array_size==0)
        return 0;
    return mirror_radius*i/wf_array_size;
}
void ArbitraryWavWidget::prepare(int size) {
    // create an array of doubles of size "size" and fill it with our wave heights for our drawn shape.
    // The index into this array will be the radius.
    if (size != wf_array_size || wf_array == 0) {
        // create array
        if (wf_array !=0)
            delete wf_array;
        wf_array_size = size;
        wf_array = new double[size];
    }

    const double empty_val = -1000000;
    for (int i=0; i< wf_array_size; ++i)
        wf_array[i]= empty_val; // this is to keep track of which elements have been set to something

    Bezier::Point bez_pts[4];
    for(int i=1; i<pts.size(); i++) { // start at 1 and we will grab 2 points at a time
        CPoint p1 = pts.at(i-1);
        CPoint p4 = pts.at(i);
        QPointF p2 = p1.getRight();
        QPointF p3 = p4.getLeft();

        bez_pts[0]= Bezier::Point(p1.x(), p1.y());
        bez_pts[1]= Bezier::Point(p2.x(), p2.y());
        bez_pts[2]= Bezier::Point(p3.x(), p3.y());
        bez_pts[3]= Bezier::Point(p4.x(), p4.y());

        Bezier::Bezier<3> bez(bez_pts,4);
        const double t_increment = .005;
        for (double t=0; t<=(1+t_increment/2); t+=t_increment) {
            Bezier::Point p = bez.valueAt(t);
            int index = radius_to_index(p.x);
            if (index<0 || index >= wf_array_size)
                continue;
            wf_array[index] = p.y;
        }
    }
    
    // fill in leading points with first filled value
    int first_valid_entry;
    for(first_valid_entry=0; first_valid_entry<wf_array_size; first_valid_entry++)
        if (wf_array[first_valid_entry] != empty_val)
            break;
    for (int i=0; i < first_valid_entry; i++)
        wf_array[i] = wf_array[first_valid_entry];
    
    // now fill in trailing points with last filled value
    int last_valid_entry;
    for(last_valid_entry=wf_array_size-1; last_valid_entry>=0; last_valid_entry--)
        if (wf_array[last_valid_entry] != empty_val)
            break;
    for (int i=last_valid_entry+1; i < wf_array_size; i++)
        wf_array[i] = wf_array[last_valid_entry];
    
    // now fill in gaps with interpolated values
    for (int i=0; i<wf_array_size; i++) {
        if (wf_array[i] != empty_val)
            continue;
        // found an empty starting at "i"
        int j;
        for (j=i; j<wf_array_size; j++)
            if (wf_array[j] != empty_val)
                break;
        i--;
        // okay i is first valid, j is valid.  everything between is invalid.
        for (int k=i+1; k<j; k++) {
            double portion = 1.0*(k-i)/(j-i);  // portion is value from 0 to 1.00 where 0 means
                                         // use all of point at [i] and 1 means use all of
                                         // point at [j] and .5 means use half of each
            wf_array[k] = (1-portion)*wf_array[i] + portion*wf_array[j];
        }// done with this section.  Filled in.
        i=j;
    }
}
double ArbitraryWavWidget::getValue(double rho) { // rho is the radius of our mirror from 0.0 to 1.0 where 0 is the center and 1.0 is the edge
    // using the array created in prepare() find the nearest entries in our array on either side of rho and return a linear interpolated wave height
    double index = rho_to_index(rho); // returned value is a double - we need to get the values on either side of this one
    int i1 = floor(index);
    int i2 = i1 + 1;
    if (i2 >= (wf_array_size-1)) {
        i2 = wf_array_size-1;
        i1 = i2 - 1;
    }
    double p1 = wf_array[i1];
    double p2 = wf_array[i2];
    double portion = index-i1;
    return p1*(1-portion)+p2*portion; // this formula will work fine even if index isn't between i1 and i2.  It will interpolate outwards.
}
