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
#include "contourview.h"
#include "ui_contourview.h"
#include <QMenu>

contourView::contourView(QWidget *parent, ContourTools *tools) :
    QWidget(parent),
    ui(new Ui::contourView), zoomed(false)
{
    ui->setupUi(this);
    ui->widget->setTool(tools);
    connect(ui->widget, SIGNAL(newContourRange(double)), ui->doubleSpinBox , SLOT(setValue(double)));
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showContextMenu(QPoint)));
}

contourView::~contourView()
{
    delete ui;
}
void contourView::zoom(){
    zoomed = !zoomed;
    emit zoomMe(zoomed);
}

void contourView::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    QString txt = (zoomed)? tr("Restore to MainWindow") : tr("FullScreen");
    myMenu.addAction(txt,  this, SLOT(zoom()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}
ContourPlot *contourView::getPlot(){
    return ui->widget;
}

void contourView::on_doubleSpinBox_valueChanged(double arg1)
{
    ui->widget->contourIntervalChanged(arg1);
}


void contourView::on_pushButton_pressed()
{
    emit showAllContours();
}
