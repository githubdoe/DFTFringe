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
#ifndef DFTTOOLS_H
#define DFTTOOLS_H

#include <QDockWidget>

namespace Ui {
class DFTTools;
}

class DFTTools : public QDockWidget
{
    Q_OBJECT

public:
    explicit DFTTools(QWidget *parent = 0);
    void connectTo(QWidget *view);
    ~DFTTools();
signals:
    void doDFT();
    void resizeIgram();
    void setFilter(double);
    void dftChannel(const QString&);
    void dftSizeChanged(const QString&);
    void dftSizeVal(int);
    void dftCenterFilter(double);
    void makeSurface();


private slots:

    void setCenterFilterValue(int);
    void on_colorChannelCB_activated(const QString &arg1);
    void on_filterAdjuster_valueChanged(double arg1);

    void on_pushButton_clicked();

    void on_computeDFT_pressed();

private:
    Ui::DFTTools *ui;
};

#endif // DFTTOOLS_H
