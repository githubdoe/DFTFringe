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
#include "subtractwavefronatsdlg.h"
#include "ui_subtractwavefronatsdlg.h"

subtractWavefronatsDlg::subtractWavefronatsDlg(QList<QString> list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::subtractWavefronatsDlg)
{
    ui->setupUi(this);
    ui->listWidget->addItems(list);
}

subtractWavefronatsDlg::~subtractWavefronatsDlg()
{
    delete ui;
}
int subtractWavefronatsDlg::getSelected(){
    QList<QListWidgetItem *> l = ui->listWidget->selectedItems();
    if (l.size() > 0)
        return ui->listWidget->row(l[0]);
    return -1;

}
