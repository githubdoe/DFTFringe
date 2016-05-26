/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
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
    bool zoomed;
signals:
    void lineSpacingChanged(double);
    void showAllContours();
    void zoomMe(bool);
private slots:
    void on_doubleSpinBox_valueChanged(double arg1);
    void showContextMenu(const QPoint &pos);
    void on_pushButton_pressed();
    void zoom();

private:
    Ui::contourView *ui;

};

#endif // CONTOURVIEW_H
