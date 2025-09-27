#ifndef MYPLOTPICKER_H
#define MYPLOTPICKER_H

#include <QObject>
#include <QString>
#include <qwt_plot.h>
#include <QPoint>
#include <qwt_plot_picker.h>
#include <qwt_text.h>
class myPlotPicker : public QwtPlotPicker
{
    Q_OBJECT
public:
    explicit myPlotPicker( QwtPlot * plot);
    bool eventFilter( QObject *, QEvent * );
    virtual QwtText trackerTextF( const QPointF &pos ) const;
    void select(QPointF p);
    QString find(QPointF p)const;
    QwtPlot *m_plot;
signals:
    void mySelected(const QString&);
public slots:
};

#endif // MYPLOTPICKER_H
