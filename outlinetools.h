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
#ifndef OUTLINETOOLS_H
#define OUTLINETOOLS_H

#include <QDockWidget>

namespace Ui {
class OutlineTools;
}

class OutlineTools : public QDockWidget
{
    Q_OBJECT

public:
    explicit OutlineTools(QWidget *parent = 0);
    ~OutlineTools();

private slots:

    void on_minus_clicked();

    void on_pluss_clicked();

private:
    Ui::OutlineTools *ui;
};

#endif // OUTLINETOOLS_H
