#ifndef ASTIGZOOMER_H
#define ASTIGZOOMER_H

#include <qwt_plot_zoomer.h>
#include <qwt_text.h>
class QwtPlot;
class astigZoomer: public QwtPlotZoomer
{
    Q_OBJECT
public:
    QwtPlot * mPlot;
    explicit astigZoomer( QWidget *canvas, QwtPlot * plot );


protected:
    QwtText trackerTextF( const QPointF &p ) const;

};

#endif // ASTIGZOOMER_H
