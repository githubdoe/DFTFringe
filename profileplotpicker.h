#ifndef PROFILEPLOTPICKER_H
#define PROFILEPLOTPICKER_H
#include <QPoint>
#include <qobject.h>


class QCustomEvent;
class QwtPlot;
class QwtPlotCurve;

class profilePlotPicker: public QObject
{
    Q_OBJECT
    QPoint m_lastMousePos;
public:
    profilePlotPicker( QwtPlot *plot );
    virtual bool eventFilter( QObject *, QEvent * );

    virtual bool event( QEvent * );

private:

    void select( QPoint  );
    void move( QPoint  );
    void moveBy( int dx, int dy );

    void release();


    QwtPlot *plot();
    const QwtPlot *plot() const;

    QwtPlotCurve *d_selectedCurve;
    int d_selectedPoint;
};
#endif
