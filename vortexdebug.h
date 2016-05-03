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
#ifndef VORTEXDEBUG_H
#define VORTEXDEBUG_H

#include <QDockWidget>

namespace Ui {
class vortexDebug;
}

class vortexDebug : public QDockWidget
{
    Q_OBJECT

public:
    explicit vortexDebug(QWidget *parent = 0);
    ~vortexDebug();
    bool m_showInput;
    bool m_showFdom;
    bool m_showFdom2;
    bool m_showFdom3;
    bool m_showOrientation;
    bool m_showWrapped;
    bool m_showUnwrapped;
    int m_smooth;
private slots:
    void on_showInput_clicked(bool checked);

    void on_smooth_valueChanged(int value);

    void on_showFdom_clicked(bool checked);

    void on_showFdom2_clicked(bool checked);

    void on_showFdom3_clicked(bool checked);

    void on_showOrientation_clicked(bool checked);

    void on_showWrapped_clicked(bool checked);

    void on_showUnwrapped_clicked(bool checked);

private:
    Ui::vortexDebug *ui;

};

#endif // VORTEXDEBUG_H
