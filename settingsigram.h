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
#ifndef SETTINGSIGRAM_H
#define SETTINGSIGRAM_H

#include <QDialog>

namespace Ui {
class settingsIGram;
}

class settingsIGram : public QDialog
{
    Q_OBJECT

public:
    explicit settingsIGram(QWidget *parent = 0);
    ~settingsIGram();

signals:
    void igramLinesChanged(int,int,QColor,QColor, double, int, int);
private slots:
    void on_edgeLineColorPb_clicked();

    void on_spinBox_valueChanged(int arg1);

    void on_centerSpinBox_valueChanged(int arg1);

    void on_centerLineColorPb_clicked();

    void on_buttonBox_accepted();

private:
    Ui::settingsIGram *ui;
    QColor edgeColor;
    QColor centerColor;
    int edgeWidth;
    int centerWidth;
};

#endif // SETTINGSIGRAM_H
