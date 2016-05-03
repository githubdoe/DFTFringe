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
#include "dftthumb.h"
#include "ui_dftthumb.h"
#include <QDebug>
dftThumb::dftThumb(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dftThumb)
{
    ui->setupUi(this);
    ui->label->setScaledContents( true );
    //ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void dftThumb::setImage(QImage img){
    m_img = img.scaled(300, 300, Qt::KeepAspectRatio);;

    ui->label->setPixmap(QPixmap::fromImage(m_img));//.scaled(labelSize, Qt::KeepAspectRatio)));

}

dftThumb::~dftThumb()
{
    delete ui;
}
