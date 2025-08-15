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
#ifndef RENAMEWAVEFRONTDLG_H
#define RENAMEWAVEFRONTDLG_H

#include <QDialog>

namespace Ui {
class renameWavefrontDlg;
}

class renameWavefrontDlg : public QDialog
{
    Q_OBJECT

public:
    explicit renameWavefrontDlg(const QString &name, QWidget *parent = 0);
    ~renameWavefrontDlg();
    QString name();

private:
    Ui::renameWavefrontDlg *ui;
};

#endif // RENAMEWAVEFRONTDLG_H
