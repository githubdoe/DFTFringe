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
#include "vortexdebug.h"
#include "ui_vortexdebug.h"

vortexDebug::vortexDebug(QWidget *parent) :
    QDockWidget(parent),  m_showInput(false), m_showFdom(false),
    m_showFdom2(false), m_showFdom3(false), m_showOrientation(false),
    m_showWrapped(false), m_showUnwrapped(false), m_smooth(9.),
    ui(new Ui::vortexDebug)
{
    ui->setupUi(this);
}

vortexDebug::~vortexDebug()
{
    delete ui;
}

void vortexDebug::on_showInput_clicked(bool checked)
{
    m_showInput = checked;
}

void vortexDebug::on_smooth_valueChanged(int value)
{
    m_smooth = value;
    ui->label->setText(QString("%1").arg(value));

}

void vortexDebug::on_showFdom_clicked(bool checked)
{
    m_showFdom = checked;
}

void vortexDebug::on_showFdom2_clicked(bool checked)
{
    m_showFdom2 = checked;
}

void vortexDebug::on_showFdom3_clicked(bool checked)
{
    m_showFdom3 = checked;
}

void vortexDebug::on_showOrientation_clicked(bool checked)
{
    m_showOrientation = checked;
}

void vortexDebug::on_showWrapped_clicked(bool checked)
{
    m_showWrapped = checked;
}

void vortexDebug::on_showUnwrapped_clicked(bool checked)
{
    m_showUnwrapped = checked;
}
