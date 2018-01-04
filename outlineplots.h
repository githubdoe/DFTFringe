#ifndef OUTLINEPLOTS_H
#define OUTLINEPLOTS_H

#include <QDockWidget>
#include "Qwt_plot.h"
namespace Ui {
class outlinePlots;
}

class outlinePlots : public QDockWidget
{
    Q_OBJECT

public:
    explicit outlinePlots(QWidget *parent = 0);
    ~outlinePlots();
    QwtPlot *getPLot(int id);
private:
    Ui::outlinePlots *ui;
};

#endif // OUTLINEPLOTS_H
