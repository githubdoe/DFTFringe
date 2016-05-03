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
#ifndef SURFACEPROPERTIESDLG_H
#define SURFACEPROPERTIESDLG_H

#include <QDialog>

namespace Ui {
class surfacePropertiesDlg;
}

class surfacePropertiesDlg : public QDialog
{
    Q_OBJECT

public:
    explicit surfacePropertiesDlg(QWidget *parent = 0);
    ~surfacePropertiesDlg();
signals:
    void xLightMoved(int value);
    void yLightMoved(int value);
    void zLightMoved(int value);
    void ambientMoved(int value);
    void diffuseMoved(int value);
    void specularMoved(int value);
    void surfaceDiffuseMoved(int value);
    void surfaceSpecularMoved(int value);
    void surfaceAmbientMoved(int value);
    void surfaceShineMoved(int value);
    void surfaceEmitionMove(int value);
    void silk();
    void glass();
    void metalFoil();
    void matt();
    void red(int);
    void green(int);
    void blue(int);

private slots:
    void on_specular_valueChanged(int value);

    void on_surfaceAmbient_valueChanged(int value);

    void on_surfaceDiffuse_valueChanged(int value);

    void on_surfaceSpecular_valueChanged(int value);

    void on_metalFoil_clicked(bool checked);

    void on_glass_clicked(bool checked);

    void on_matt_clicked(bool checked);

    void on_surfaceShine_valueChanged(int value);

    void on_surfaceEmission_valueChanged(int value);

    void on_buttonBox_accepted();

    void on_ambientLight_valueChanged(int value);

    void on_xLight_valueChanged(int value);

    void on_yLight_valueChanged(int value);

    void on_zLight_valueChanged(int value);

    void on_diffuse_valueChanged(int value);

    void on_Satin_clicked(bool checked);

    void on_plastic_clicked(bool checked);

    void on_red_valueChanged(int value);

    void on_green_valueChanged(int value);

    void on_blue_valueChanged(int value);

private:
    Ui::surfacePropertiesDlg *ui;
};

#endif // SURFACEPROPERTIESDLG_H
