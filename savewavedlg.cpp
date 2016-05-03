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
#include "savewavedlg.h"
#include "ui_savewavedlg.h"
#include <QFileDialog>
SaveWaveDlg::SaveWaveDlg(QString lastPath, QWidget *parent) :
     QDialog(parent),lastPath(lastPath),
    ui(new Ui::SaveWaveDlg)
{
    ui->setupUi(this);
    ui->lineEdit->setText(lastPath);
}

SaveWaveDlg::~SaveWaveDlg()
{
    delete ui;
}

void SaveWaveDlg::on_normalRb_clicked(bool)
{

}

void SaveWaveDlg::on_browsePB_clicked()
{
    fileName = QFileDialog::getSaveFileName(0,
                        tr("Write wave font file"), lastPath,
                        tr("wft (*.wft)"));
    if (fileName.isEmpty())
        return;
    ui->lineEdit->setText(fileName);
}
