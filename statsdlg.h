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
#ifndef STATSDLG_H
#define STATSDLG_H

#include <QDialog>
#include "qwt_plot.h"
namespace Ui {
class StatsDlg;
}

class StatsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDlg(QWidget *parent = 0);
    ~StatsDlg();
    QwtPlot *m_wavefrontPlot;
    QwtPlot *m_zernPlot;
    bool doAstig;
    bool doSA;
    bool doComa;
    bool saveCsv;
    bool savePDF;
    bool saveJpg;
    bool doInputs;
    bool doZernGroup;
    int zernFrom;
    int zernTo;
    bool showFileNames;
    bool removeOutliers;
    QColor m_background;
    bool removeAboveLimit;
    double RMSLimitValue;

private slots:
    void on_buttonBox_accepted();

    void on_bakcgroundpb_clicked();

private:
    Ui::StatsDlg *ui;
};

#endif // STATSDLG_H
