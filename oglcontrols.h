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
#ifndef OGLCONTROLS_H
#define OGLCONTROLS_H

#include <QDockWidget>

namespace Ui {
class OGLControls;
}

class OGLControls : public QDockWidget
{
    Q_OBJECT

public:
    explicit OGLControls(QWidget *parent = 0);
    ~OGLControls();
    void enablTools(bool b);
signals:
    void resolutionChanged(int);
    void fillGridChanged(bool);
    void ogheightMagValue(int);
private slots:

    void on_doPerspective_clicked();

    void on_checkBox_clicked(bool checked);

    void on_spinBox_valueChanged(int arg1);

    void on_surfaceMagnification_valueChanged(int arg1);

private:
    Ui::OGLControls *ui;
};

#endif // OGLCONTROLS_H
