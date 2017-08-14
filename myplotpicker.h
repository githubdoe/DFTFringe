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
    void select(const QPointF &p);
    QString find(const QPointF &p)const;
    QwtPlot *m_plot;
signals:
    void selected(const QString);
public slots:
};

#endif // MYPLOTPICKER_H
