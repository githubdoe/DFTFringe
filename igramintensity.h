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
#ifndef IGRAMINTENSITY_H
#define IGRAMINTENSITY_H

#include <QDialog>
#include <QImage>
namespace Ui {
class igramIntensity;
}

class igramIntensity : public QDialog
{
    Q_OBJECT

public:
    explicit igramIntensity(QWidget *parent = 0);
    ~igramIntensity();
    void setIgram(QImage img);
private slots:
    void on_showRed_clicked(bool checked);

    void on_showBlue_clicked(bool checked);

    void on_showGreen_clicked(bool checked);

    void on_pushButton_clicked();

    void on_checkBox_clicked(bool checked);

private:
    Ui::igramIntensity *ui;

};

#endif // IGRAMINTENSITY_H
