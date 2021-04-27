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
#include "settingsprofile.h"
#include "ui_settingsprofile.h"
#include <QSettings>
#include <QColorDialog>
#include <QWidget>
static inline QString colorButtonStyleSheet(const QColor &bgColor)
{
    if (bgColor.isValid()) {
        QString rc = QLatin1String("border: 2px solid black; border-radius: 2px; background:");
        rc += bgColor.name();
        return rc;
    }
    return QLatin1String("border: 2px dotted black; border-radius: 2px;");
}
settingsProfile::settingsProfile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsProfile)
{
    ui->setupUi(this);
    QString name;
    QSettings set;
    ui->pushButton_1->setStyleSheet(colorButtonStyleSheet(set.value("profile color pushButton_1", QColor(0,0,0).name()).toString()));
    for (int i = 2 ; i < 10; ++i){
        name = QString().sprintf("pushButton_%d",i);
        QPushButton *btn = findChild<QPushButton *>(name);
        QColor color = QColor(Qt::GlobalColor( 7 + i%7 ) );
        color = QColor(set.value("profile color "+name, color).toString());
        btn->setStyleSheet(colorButtonStyleSheet(color));
    }
}
QColor settingsProfile::getColor(int num){
    QString name = QString().sprintf("pushButton_%d",1 + num%7);
    QPushButton *btn = findChild<QPushButton *>(name);
    return btn->palette().color(QPalette::Background);
}

void settingsProfile::setColor(int num){
    QString name = QString().sprintf("pushButton_%d",num);
    QPushButton *btn = findChild<QPushButton *>(name);
    QColor color = QColorDialog::getColor( btn->palette().color(QPalette::Background));
    btn->setStyleSheet(colorButtonStyleSheet(color));
    QSettings set;
    set.setValue("profile color "+name, color.name());
}
int settingsProfile::lineWidth(){
    return ui->lineWidth->value();
}
int settingsProfile::selectedWidth(){
    return ui->selectedWidth->value();
}

settingsProfile::~settingsProfile()
{
    delete ui;
}

void settingsProfile::on_pushButton_1_pressed()
{
    setColor(1);
}

void settingsProfile::on_pushButton_2_pressed()
{
    setColor(2);
}

void settingsProfile::on_pushButton_9_pressed()
{
    setColor(9);
}

void settingsProfile::on_pushButton_8_pressed()
{
    setColor(8);
}

void settingsProfile::on_pushButton_7_pressed()
{
    setColor(7);
}

void settingsProfile::on_pushButton_6_pressed()
{
    setColor(6);
}

void settingsProfile::on_pushButton_5_pressed()
{
    setColor(5);
}

void settingsProfile::on_pushButton_4_pressed()
{
    setColor(4);
}

void settingsProfile::on_pushButton_3_pressed()
{
    setColor(3);
}


