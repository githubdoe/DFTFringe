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
#include "settingsigram.h"
#include "ui_settingsigram.h"
#include <QSettings>
#include <QColorDialog>
#include <QPen>
#include <QPainter>
static inline QString colorButtonStyleSheet(const QColor &bgColor)
{
    if (bgColor.isValid()) {
        QString rc = QLatin1String("border: 2px solid black; border-radius: 2px; background:");
        rc += bgColor.name();
        return rc;
    }
    return QLatin1String("border: 2px dotted black; border-radius: 2px;");
}
settingsIGram::settingsIGram(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsIGram)
{
    ui->setupUi(this);
    QSettings set;
    edgeColor = QColor(set.value("igramEdgeLineColor", "white").toString());
    ui->edgeLineColorPb->setStyleSheet(colorButtonStyleSheet(edgeColor));
    centerColor = QColor(set.value("igramCenterLineColor", "yellow").toString());
    ui->centerLineColorPb->setStyleSheet(colorButtonStyleSheet(centerColor));
    ui->opacitySB->setValue(set.value("igramLineOpacity", 65.).toDouble());
    edgeWidth = set.value("igramEdgeWidth",3).toInt();
    centerWidth = set.value("igramCenterWidth",3).toInt();
    ui->spinBox->setValue(edgeWidth);
    ui->centerSpinBox->setValue(centerWidth);

    connect(ui->buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), SLOT(on_buttonBox_accepted()));
    ui->styleCB->setEditable(false);
    ui->styleCB->setIconSize(QSize(80,14));
    ui->styleCB->setMinimumWidth(80);

    for (int aaa = Qt::SolidLine; aaa < Qt::CustomDashLine; aaa++)
    {
        QPixmap pix(80,14);
        pix.fill(Qt::white);

        QBrush brush(Qt::black);
        QPen pen(Qt::black ,2.5,(Qt::PenStyle)aaa);

        QPainter painter(&pix);
        painter.setPen(pen);
        painter.drawLine(2,7,78,7);

        ui->styleCB->addItem(QIcon(pix),"");
    }
    int style = set.value("igramLineStyle", 1).toInt();
    ui->styleCB->setCurrentIndex(style-1);
}

settingsIGram::~settingsIGram()
{
    delete ui;
}


void settingsIGram::on_edgeLineColorPb_clicked()
{
    edgeColor = QColorDialog::getColor(edgeColor, 0);
    ui->edgeLineColorPb->setStyleSheet(colorButtonStyleSheet(edgeColor));
}

void settingsIGram::on_spinBox_valueChanged(int arg1)
{
    edgeWidth = arg1;
}

void settingsIGram::on_centerSpinBox_valueChanged(int arg1)
{
    centerWidth = arg1;
}

void settingsIGram::on_centerLineColorPb_clicked()
{
    centerColor = QColorDialog::getColor(centerColor, 0);
    ui->centerLineColorPb->setStyleSheet(colorButtonStyleSheet(centerColor));
}

void settingsIGram::on_buttonBox_accepted()
{
    QSettings set;
    set.setValue("igramCenterLineColor", centerColor.name());
    set.setValue("igramEdgeLineColor", edgeColor.name());
    set.setValue("igramEdgeWidth", edgeWidth);
    set.setValue("igramCenterWidth", centerWidth);
    set.setValue("igramLineOpacity", ui->opacitySB->value());
    set.setValue("igramLineStyle", ui->styleCB->currentIndex() + 1);
    emit igramLinesChanged(edgeWidth,centerWidth, edgeColor, centerColor, ui->opacitySB->value(),
                           ui->styleCB->currentIndex()+1);
}
