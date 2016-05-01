#ifndef WFTEXAMINER_H
#define WFTEXAMINER_H

#include <QDialog>
#include <qwt_plot.h>
#include "wavefront.h"
#include <qwt_plot_curve.h>
namespace Ui {
class wftExaminer;
}

class wftExaminer : public QDialog
{
    Q_OBJECT

public:
    explicit wftExaminer(wavefront *wf, QWidget *parent = 0);
    ~wftExaminer();
    wavefront *m_wf;
    QwtPlot * m_Pl;
    int m_y;
    QwtPlotCurve *curve;
    QwtPlotCurve *maskCurve;
    void setupPlot();
private slots:
    void on_spinBox_valueChanged(int arg1);

private:
    Ui::wftExaminer *ui;
};

#endif // WFTEXAMINER_H
