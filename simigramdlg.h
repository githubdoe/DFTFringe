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
#ifndef SIMIGRAMDLG_H
#define SIMIGRAMDLG_H

#include <QDialog>

namespace Ui {
class simIgramDlg;
}

class simIgramDlg : public QDialog
{
    Q_OBJECT

public:
    explicit simIgramDlg(QWidget *parent = 0);
    ~simIgramDlg();
    static simIgramDlg*get_instance();
    double xtilt;
    double ytilt;
    double defocus;
    double correction;
    double xastig;
    double yastig;
    double star;
    double ring;
    double noise;
    int zernNdx;
    double zernValue;
    int size;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::simIgramDlg *ui;
    static simIgramDlg *m_instance;
};

#endif // SIMIGRAMDLG_H
