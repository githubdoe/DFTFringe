/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "oglcontrols.h"
#include "ui_oglcontrols.h"
#include <QSettings>
OGLControls::OGLControls(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::OGLControls)
{
    ui->setupUi(this);
    enablTools(false);
    QSettings s;
    bool v = s.value("oglFill", true).toBool();
    ui->checkBox->setChecked(v);
}

OGLControls::~OGLControls()
{
    delete ui;
}


void OGLControls::on_doPerspective_clicked()
{

}

void OGLControls::on_checkBox_clicked(bool checked)
{
    QSettings s;
    s.setValue("oglFill", true);
    emit fillGridChanged(checked);
}

void OGLControls::enablTools(bool b){
    QList<QWidget *> widgets = findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        widget->setEnabled(b);
    }
}

void OGLControls::on_spinBox_valueChanged(int arg1)
{
    emit resolutionChanged(arg1);
}

void OGLControls::on_surfaceMagnification_valueChanged(int arg1)
{
    emit ogheightMagValue(arg1);
}
