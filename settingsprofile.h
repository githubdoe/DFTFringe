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
#ifndef SETTINGSPROFILE_H
#define SETTINGSPROFILE_H

#include <QDialog>

namespace Ui {
class settingsProfile;
}

class settingsProfile : public QDialog
{
    Q_OBJECT

public:
    explicit settingsProfile(QWidget *parent = 0);
    ~settingsProfile();
    void setColor(int num);
    QColor getColor(int num);
private slots:
    void on_pushButton_1_pressed();

    void on_pushButton_2_pressed();

    void on_pushButton_9_pressed();

    void on_pushButton_8_pressed();

    void on_pushButton_7_pressed();

    void on_pushButton_6_pressed();

    void on_pushButton_5_pressed();

    void on_pushButton_4_pressed();

    void on_pushButton_3_pressed();

private:
    Ui::settingsProfile *ui;
};

#endif // SETTINGSPROFILE_H
