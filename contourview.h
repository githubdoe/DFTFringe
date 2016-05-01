#ifndef CONTOURVIEW_H
#define CONTOURVIEW_H

#include <QWidget>
#include "contourtools.h"
#include "contourplot.h"
namespace Ui {
class contourView;
}

class contourView : public QWidget
{
    Q_OBJECT

public:
    explicit contourView(QWidget *parent = 0, ContourTools *tools = 0);
    ~contourView();
    ContourPlot *getPlot();
signals:
    void lineSpacingChanged(double);
    void showAllContours();
private slots:
    void on_doubleSpinBox_valueChanged(double arg1);

    void on_pushButton_pressed();

private:
    Ui::contourView *ui;
};

#endif // CONTOURVIEW_H
