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
#include "contourtools.h"
#include "ui_contourtools.h"
#include <QColorDialog>
#include <QPalette>
#include <QSettings>
#include <QDebug>
#include <QSettings>
ContourTools::ContourTools(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::ContourTools)
{
    ui->setupUi(this);
    ui->ColorMapCB->addItem(tr("Hot cold"));
    ui->ColorMapCB->addItem(tr("Tropical"));
    ui->ColorMapCB->addItem(tr("Vibrant"));
    ui->ColorMapCB->addItem(tr("Gray"));
    ui->ColorMapCB->addItem(tr(" > Error limits"));
    ui->ColorMapCB->addItem(tr("user map"));
    ui->ColorMapCB->addItem(tr("OpenFringe"));

    ui->colorRangeCB->addItem(tr("Auto"));
    ui->colorRangeCB->addItem(tr("Min/Max"));
    ui->colorRangeCB->addItem(tr("Fractions of Wave"));

    QSettings s;
    int ndx = s.value("colorMap ndx", 1).toInt();
    ui->ColorMapCB->setCurrentIndex(ndx);
    ui->zeroOffsetCB->addItem(tr("Middle"));
    ui->zeroOffsetCB->addItem(tr("Minimum"));

    ui->errorRangeSpin->setSingleStep(.01);
    ui->errorRangeSpin->setMinimum(.01);
    ui->errorRangeSpin->setDecimals(3);
    QSettings settings;
    QColor lineColor = QColor(settings.value("ContourLineColor", "white").toString());
    QPalette pal = ui->lineColorDisplay->palette();
    pal.setColor(QPalette::Window,lineColor);
    ui->lineColorDisplay->setPalette(pal);
    ui->lineColorDisplay->setAutoFillBackground( true );
    m_minmaxEditTimer = new QTimer(this);
    connect(m_minmaxEditTimer, SIGNAL(timeout()),this, SLOT(updateMinMax()));

}


ContourTools::~ContourTools()
{
    delete ui;
}

void ContourTools::connectTo(QWidget *view){
    connect(this, SIGNAL(ContourMapColorChanged(int)), view, SLOT(ContourMapColorChanged(int)));
    connect(this, SIGNAL(contourZeroOffsetChanged(const QString &)),
            view, SLOT(contourZeroOffsetChanged(const QString &)));
    connect(this, SIGNAL(contourColorRangeChanged(const QString &)),
            view, SLOT(contourColorRangeChanged(const QString &)));
    connect(view, SIGNAL(setMinMaxValues(double,double)),this, SLOT(setMinMaxValues(double,double)));
    connect(this, SIGNAL(contourWaveRangeChanged(double)),view, SLOT(contourWaveRangeChanged(double)));
    connect(view, SIGNAL(setWaveRange(double)),this, SLOT(setWaveRange(double)));
    connect(this, SIGNAL(lineColorChanged(QColor)), view, SLOT(on_line_color_changed(QColor)));
    connect(this, SIGNAL(newDisplayErrorRange(double,double)), view, SLOT(newDisplayErrorRange(double,double)));
    enablTools(false);
}

void ContourTools::enablTools(bool b){
    QList<QWidget *> widgets = findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        widget->setEnabled(b);
    }
}
void ContourTools::setWaveRange(double val){
    ui->errorRangeSpin->blockSignals(true);
    ui->errorRangeSpin->setValue(val);
    ui->errorRangeSpin->blockSignals(false);
}

void ContourTools::setMinMaxValues(double min, double max){
    m_min = min;
    m_max = max;
    ui->minSB->blockSignals(true);
    ui->maxSB->blockSignals(true);
    ui->minSB->setValue(min);
    ui->maxSB->setValue(max);
    ui->maxSB->blockSignals(false);
    ui->minSB->blockSignals(false);

    emit newDisplayErrorRange(min,max);
}

void ContourTools::on_ColorMapCB_activated(int index)
{
    QSettings set;
    set.setValue("colorMap ndx",index);
    emit ContourMapColorChanged(index);
}



void ContourTools::on_zeroOffsetCB_activated(const QString &arg1)
{
    emit contourZeroOffsetChanged(arg1);
}

void ContourTools::on_colorRangeCB_activated(const QString &arg1)
{
    ui->errorRangeSpin->setEnabled(arg1 == "Fractions of Wave");
    emit contourColorRangeChanged(arg1);
}

void ContourTools::on_errorRangeSpin_valueChanged(double arg1)
{

    emit contourWaveRangeChanged(arg1);
}

void ContourTools::on_LineColorBtn_clicked()
{
    QColor color = QColorDialog::getColor(Qt::white);
    QPalette pal = ui->lineColorDisplay->palette();
    pal.setColor(QPalette::Window,color);
    ui->lineColorDisplay->setPalette(pal);
    ui->lineColorDisplay->setAutoFillBackground( true );
    emit lineColorChanged(color);
}



//called when timer for edit expires.
void ContourTools::updateMinMax(){
    emit newDisplayErrorRange(m_min,m_max);
}

void ContourTools::on_maxSB_valueChanged(double arg1)
{
    m_max = arg1;
    ui->errorRangeSpin->blockSignals(true);
    ui->errorRangeSpin->setValue(m_max-m_min);
    ui->errorRangeSpin->blockSignals(false);
    m_minmaxEditTimer->start(1000);
}

void ContourTools::on_minSB_valueChanged(double arg1)
{
    m_min = arg1;
    ui->errorRangeSpin->blockSignals(true);
    ui->errorRangeSpin->setValue(m_max-m_min);
    ui->errorRangeSpin->blockSignals(false);
    m_minmaxEditTimer->start(1000);
}
