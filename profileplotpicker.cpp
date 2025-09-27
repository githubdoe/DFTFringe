#include "profileplotpicker.h"
#include <qapplication.h>
#include <qevent.h>
#include <qpainter.h>
#include <qwt_plot.h>
#include <qwt_symbol.h>
#include <qwt_scale_map.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_directpainter.h>

profilePlotPicker::profilePlotPicker( QwtPlot *plot ):
    QObject( plot ),
    d_selectedCurve( NULL ),
    d_selectedPoint( -1 )
{
    QwtPlotCanvas *canvas = qobject_cast<QwtPlotCanvas *>( plot->canvas() );
    canvas->installEventFilter( this );

    // We want the focus, but no focus rect. The
    canvas->setFocusPolicy( Qt::StrongFocus );
#ifndef QT_NO_CURSOR
    canvas->setCursor( Qt::PointingHandCursor );
#endif
    canvas->setFocusIndicator( QwtPlotCanvas::ItemFocusIndicator );
    canvas->setFocus();

}

QwtPlot *profilePlotPicker::plot()
{
    return qobject_cast<QwtPlot *>( parent() );
}

const QwtPlot *profilePlotPicker::plot() const
{
    return qobject_cast<const QwtPlot *>( parent() );
}

bool profilePlotPicker::event( QEvent *ev )
{
    if ( ev->type() == QEvent::User )
    {

        return true;
    }
    return QObject::event( ev );
}

bool profilePlotPicker::eventFilter( QObject *object, QEvent *event )
{
    if ( plot() == NULL || object != plot()->canvas() )
        return false;

    switch( event->type() )
    {

        case QEvent::FocusIn:
        {

            break;
        }
        case QEvent::FocusOut:
        {

            break;
        }
        case QEvent::Paint:
        {
            QApplication::postEvent( this, new QEvent( QEvent::User ) );
            break;
        }
        case QEvent::MouseButtonPress:
        {
            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
            m_lastMousePos = mouseEvent->pos();
            select( mouseEvent->pos() );

            return true;
        }
        case QEvent::MouseMove:
        {

            const QMouseEvent *mouseEvent = static_cast<QMouseEvent *>( event );
            move( mouseEvent->pos() );
            m_lastMousePos = mouseEvent->pos();
            return true;
        }

        default:
            break;
    }

    return QObject::eventFilter( object, event );
}

// Select the point at a position. If there is no point
// deselect the selected point

void profilePlotPicker::select( QPoint pos )
{
    QwtPlotCurve *curve = NULL;
    double dist = 10e10;
    int index = -1;

    const QwtPlotItemList& itmList = plot()->itemList();
    for ( QwtPlotItemIterator it = itmList.begin();
        it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );

            double d;
            int idx = c->closestPoint( pos, &d );
            if ( d < dist )
            {
                curve = c;
                index = idx;
                dist = d;

            }
        }
    }

        d_selectedCurve = curve;
        d_selectedPoint = index;
}


// Move the selected point
void profilePlotPicker::move( QPoint pos )
{
    qDebug() << "move pos" << pos;
    if ( !d_selectedCurve )
        return;
qDebug() << "Move" << plot()->invTransform(d_selectedCurve->yAxis(), pos.y());
    double last = plot()->invTransform( d_selectedCurve->yAxis(), m_lastMousePos.y() );
    double current = plot()->invTransform( d_selectedCurve->yAxis(), pos.y() );
    double delta = current - last;

    QVector<double> xData( d_selectedCurve->dataSize() );
    QVector<double> yData( d_selectedCurve->dataSize() );

    for ( int i = 0;
        i < static_cast<int>( d_selectedCurve->dataSize() ); i++ )
    {
        xData[i] = d_selectedCurve->sample(i).x();
        yData[i] = d_selectedCurve->sample(i).y() + delta;
    }
    d_selectedCurve->setSamples( xData, yData );

    /*
       Enable QwtPlotCanvas::ImmediatePaint, so that the canvas has been
       updated before we paint the cursor on it.
     */
    QwtPlotCanvas *plotCanvas =
        qobject_cast<QwtPlotCanvas *>( plot()->canvas() );

    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, true );
    plot()->replot();
    plotCanvas->setPaintAttribute( QwtPlotCanvas::ImmediatePaint, false );


}




