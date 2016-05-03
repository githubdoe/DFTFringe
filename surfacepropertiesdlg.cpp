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
#include "surfacepropertiesdlg.h"
#include "ui_surfacepropertiesdlg.h"
#include <qsettings.h>
#include <qdebug.h>
surfacePropertiesDlg::surfacePropertiesDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::surfacePropertiesDlg)
{
    ui->setupUi(this);
    QSettings settings;
    ui->xLight->setValue(settings.value("xlight", -20).toInt());
    ui->yLight->setValue(settings.value("ylight", -20).toInt());
    ui->zLight->setValue(settings.value("zlight", -20).toInt());
    ui->ambientLight->setValue(settings.value("ambientLight", -20).toInt());
    ui->diffuse->setValue(settings.value("diffuseLight",0).toInt());
    ui->specular->setValue(settings.value("specularLight", 0).toInt());
    ui->surfaceAmbient->setValue(settings.value("surfaceAmbient", 0).toInt());
    ui->surfaceDiffuse->setValue(settings.value("surfaceDiffuse", 0).toInt());
    ui->surfaceSpecular->setValue(settings.value("surfaceSpecular",0).toInt());
    ui->surfaceShine->setValue(settings.value("surfaceShine",0).toInt());
}

surfacePropertiesDlg::~surfacePropertiesDlg()
{
    delete ui;
}


void surfacePropertiesDlg::on_xLight_valueChanged(int value)
{
    QSettings set;
    set.setValue("xLight", value);
    emit xLightMoved(value);
}

void surfacePropertiesDlg::on_yLight_valueChanged(int value)
{
    QSettings set;
    set.setValue("yLight", value);
    emit yLightMoved(value);
}

void surfacePropertiesDlg::on_zLight_valueChanged(int value)
{
    QSettings set;
    set.setValue("zLight", value);
    emit zLightMoved(value);
}
void surfacePropertiesDlg::on_ambientLight_valueChanged(int value)
{
    QSettings set;
    set.setValue("ambientLight", value);
    emit ambientMoved(value);
}

void surfacePropertiesDlg::on_diffuse_valueChanged(int value)
{
    QSettings set;
    set.setValue("diffuseLight", value);
    emit diffuseMoved(value);
}

void surfacePropertiesDlg::on_specular_valueChanged(int value)
{
    QSettings set;
    set.setValue("specularLight", value);
    emit specularMoved(value);
}

void surfacePropertiesDlg::on_surfaceAmbient_valueChanged(int value)
{
    QSettings set;
    set.setValue("surfaceAmbient", value);
    emit surfaceAmbientMoved(value);
}

void surfacePropertiesDlg::on_surfaceDiffuse_valueChanged(int value)
{
    QSettings set;
    set.setValue("surfaceDiffuse", value);
    emit surfaceDiffuseMoved(value);
}

void surfacePropertiesDlg::on_surfaceSpecular_valueChanged(int value)
{
    QSettings set;
    set.setValue("surfaceSpucular", value);
    emit surfaceSpecularMoved(value);
}

void surfacePropertiesDlg::on_surfaceShine_valueChanged(int value)
{
    QSettings set;
    set.setValue("surfaceShine", value);
    emit surfaceShineMoved(value);
}


void surfacePropertiesDlg::on_surfaceEmission_valueChanged(int value)
{
    emit surfaceEmitionMove(value);
}



void surfacePropertiesDlg::on_metalFoil_clicked(bool checked)
{
    if (checked){
        ui->xLight->setValue(-38);
        ui->yLight->setValue(500);
        ui->zLight->setValue(-6);
        ui->ambientLight->setValue(9);
        ui->diffuse->setValue(50);
        ui->specular->setValue(100);
        ui->surfaceDiffuse->setValue(0);
        ui->surfaceSpecular->setValue(75);
        ui->surfaceShine->setValue(25);
        update();
    }
}

void surfacePropertiesDlg::on_glass_clicked(bool checked)
{
    if (checked)
    {
        ui->xLight->setValue(-16);
        ui->yLight->setValue(269);
        ui->zLight->setValue(-6);
        ui->ambientLight->setValue(14);
        ui->diffuse->setValue(52);
        ui->specular->setValue(100);
        ui->surfaceDiffuse->setValue(44);
        ui->surfaceSpecular->setValue(79);
        ui->surfaceShine->setValue(104);
        update();
    }
}

void surfacePropertiesDlg::on_matt_clicked(bool checked)
{
    if (checked){
        ui->xLight->setValue(-25);
        ui->yLight->setValue(40);
        ui->zLight->setValue(4);
        ui->ambientLight->setValue(23);
        ui->diffuse->setValue(52);
        ui->specular->setValue(29);
        ui->surfaceDiffuse->setValue(99);
        ui->surfaceSpecular->setValue(15);
        ui->surfaceShine->setValue(128);
        update();
    }
}



void surfacePropertiesDlg::on_buttonBox_accepted()
{
    qDebug() << "Params";
    qDebug() << ui->xLight->value() << " " << ui->yLight->value() << " " << ui->zLight->value();
    qDebug() <<"ambient light" << ui->ambientLight->value() << " diff " << ui->diffuse->value() << " spec " << ui->specular->value();
    qDebug() << "sur amb " << ui->surfaceAmbient->value() << " surdiff " << ui->surfaceDiffuse->value() << " sur spec" << ui->surfaceSpecular->value();
    qDebug() << ui->surfaceShine->value();
}



void surfacePropertiesDlg::on_Satin_clicked(bool checked)
{    if (checked){
        ui->xLight->setValue(-26);
        ui->yLight->setValue(432);
        ui->zLight->setValue(-18);
        ui->ambientLight->setValue(17);
        ui->diffuse->setValue(0);
        ui->specular->setValue(74);
        ui->surfaceDiffuse->setValue(94);
        ui->surfaceSpecular->setValue(61);
        ui->surfaceShine->setValue(30);
        update();
    }

}

void surfacePropertiesDlg::on_plastic_clicked(bool checked)
{
    if (checked){
        ui->xLight->setValue(-25);
        ui->yLight->setValue(462);
        ui->zLight->setValue(4);
        ui->ambientLight->setValue(28);
        ui->diffuse->setValue(0);
        ui->specular->setValue(100);
        ui->surfaceDiffuse->setValue(0);
        ui->surfaceSpecular->setValue(61);
        ui->surfaceShine->setValue(48);
        update();
    }
}


void surfacePropertiesDlg::on_red_valueChanged(int value)
{
    emit red(value);
}

void surfacePropertiesDlg::on_green_valueChanged(int value)
{
    emit green(value);
}

void surfacePropertiesDlg::on_blue_valueChanged(int value)
{
    emit blue(value);
}
