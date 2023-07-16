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
#include "pixelstats.h"
namespace Ui {
class contourView;
}

class contourView : public QWidget
{
    Q_OBJECT
    enum ruler{RULERNONE,RULERMETRIC,RULERINCH};
public:
    explicit contourView(QWidget *parent = 0, ContourTools *tools = 0);
    ~contourView();
    ContourPlot *getPlot();
    void setSurface(wavefront * wf);
    bool zoomed;
    QImage getPixstatsImage();
    pixelStats *getPixelstats(){ return ps;}
signals:
    void lineSpacingChanged(double);
    void showAllContours();
    void zoomMe(bool);
private slots:
    void on_doubleSpinBox_valueChanged(double arg1);
    void showContextMenu(const QPoint &pos);
    void on_pushButton_pressed();
    void zoom();

    void on_histogram_clicked();

    void on_fillContourCB_clicked(bool checked);

    void on_showRuler_clicked(bool checked);

    void updateRuler();
    void on_LinkProfileCB_clicked(bool checked);

private:
    Ui::contourView *ui;
    ContourTools* tools;
    pixelStats *ps;
    ruler rulerSetting;

};

#endif // CONTOURVIEW_H
