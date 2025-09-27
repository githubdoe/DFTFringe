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
#include "renamewavefrontdlg.h"
#include "ui_renamewavefrontdlg.h"

renameWavefrontDlg::renameWavefrontDlg(const QString &name, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::renameWavefrontDlg)
{
    ui->setupUi(this);
    ui->name->setText(name);
}

renameWavefrontDlg::~renameWavefrontDlg()
{
    delete ui;
}
QString renameWavefrontDlg::name(){
    return ui->name->text();
}
