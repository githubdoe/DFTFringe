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
#ifndef ZERNIKEPROCESS_H
#define ZERNIKEPROCESS_H

#include <QObject>
#include "wavefront.h"
#include "zernikedlg.h"
#include "mirrordlg.h"
#include "mainwindow.h"
extern std::vector<double> zNulls;
extern std::vector<bool> zernEnables;
extern int Zw[];
extern double BestSC;
double Zernike(int n, double x, double y);
double ZernikePolar(int n, double rho, double theta);
void gauss_jordan(int n, double* Am, double* Bm);
void ZernikeSmooth(Mat wf, Mat mask);
class zernikeProcess : public QObject
{
    Q_OBJECT
private:
    static zernikeProcess *m_Instance;
public:
    explicit zernikeProcess(QObject *parent = 0);
    static zernikeProcess *get_Instance();
    double unwrap_to_zernikes(wavefront &wf);
    cv::Mat null_unwrapped(wavefront&wf,  std::vector<double> zerns, std::vector<bool> enables,int start_term =0, int last_term = Z_TERMS);
    //double Wavefront(double x1, double y1, int Order);
    void unwrap_to_zernikes(zern_generator *zg, cv::Mat wf, cv::Mat mask);
    cv::Mat Z;
    cv::Mat inputZ;
    bool m_dirty_zerns;
    mirrorDlg *md;
    MainWindow *mw;

signals:

public slots:

};

#endif // ZERNIKEPROCESS_H
