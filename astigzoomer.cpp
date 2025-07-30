#include "astigzoomer.h"
#include <qwt_scale_map.h>
#include <qwt_plot.h>
#include <qwt_plot_marker.h>
#include <QtMath>
#include <QPen>

astigZoomer::astigZoomer( QWidget *canvas, QwtPlot * plot ):
    QwtPlotZoomer( canvas ),mPlot(plot)
{
    setRubberBandPen( QPen( Qt::lightGray ) );
    setTrackerMode( QwtPlotPicker::AlwaysOn );
}


QwtText astigZoomer::trackerTextF( const QPointF &p ) const
{

    QString s("");
    const QwtScaleMap xMap = mPlot->canvasMap(2);
    const QwtScaleMap yMap = mPlot->canvasMap(QwtPlot::yLeft);

    const double cx = xMap.transform( p.x());
    const double cy = yMap.transform(p.y());

    const QwtPlotItemList& itmList = mPlot->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
          it != itmList.end(); ++it )
    {
        if (( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker){
            QwtPlotMarker *mark = static_cast<QwtPlotMarker *>( *it );
            QPointF pm = mark->value();

            double delx = cx - xMap.transform(pm.x());
            double dely = cy - yMap.transform(pm.y());
            double r = qSqrt(delx * delx + dely * dely);
            if (r < 4) {
                s = mark->title().text();
                break;
            }
        }
    }

    QwtText text( s );
    text.setColor( Qt::black );
    text.setFont(QFont("Arial",12));

    QColor c = rubberBandPen().color();
    text.setBorderPen( QPen( c ) );
    text.setBorderRadius( 6 );
    c.setAlpha( 170 );
    text.setBackgroundBrush( c );

    return text;
}
