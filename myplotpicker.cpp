#include "myplotpicker.h"
#include <math.h>
#include <qevent.h>
#include <qwhatsthis.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_canvas.h>
#include <qdebug.h>
myPlotPicker::myPlotPicker(QwtPlot * plot) :
    QwtPlotPicker(plot->canvas()),m_plot(plot)
{
    qDebug() << "Picker";
    setTrackerMode( QwtPicker::AlwaysOn );
}

QwtText myPlotPicker::trackerTextF( const QPointF &pos ) const{

    QString s = find(pos);

    QwtText text( s );
    text.setColor( Qt::black );
    text.setFont(QFont("Arial",12));


    text.setBorderPen( QPen( Qt::black ) );
    text.setBorderRadius( 6 );

    text.setBackgroundBrush( QColor(Qt::lightGray) );

    return text;
}

bool myPlotPicker::eventFilter( QObject *object, QEvent *event )
{


    switch( event->type() )
    {


        case QEvent::MouseButtonPress:
        {

            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
            select(invTransform( mouseEvent->pos()) );
            return true;
        }

        default:
            break;
    }
    return QwtPlotPicker::eventFilter(object,event);
    return QObject::eventFilter( object, event );
}

void myPlotPicker::select(const QPointF &p){
    QString s = find(p);
    if (s == "")
        return;
    emit selected(s);
}
QString myPlotPicker::find(const QPointF &p)const{

    QString s("");
    const QwtScaleMap xMap = m_plot->canvasMap(2);
    const QwtScaleMap yMap = m_plot->canvasMap(QwtPlot::yLeft);

    const double cx = xMap.transform( p.x());
    const double cy = yMap.transform(p.y());

    const QwtPlotItemList& itmList = m_plot->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
          it != itmList.end(); ++it )
    {
        if (( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker){
            QwtPlotMarker *mark = static_cast<QwtPlotMarker *>( *it );
            QPointF pm = mark->value();

            double delx = cx - xMap.transform(pm.x());
            double dely = cy - yMap.transform(pm.y());
            double r = sqrt(delx * delx + dely * dely);
            if (r < 4) {
                s = mark->title().text();
                break;
            }
        }
    }

    return s;
}
