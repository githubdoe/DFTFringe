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
#include "armadillo"
#include <stdlib.h>

//TODO remove this if possible and use a class. It's undocumented, difficult to undesrstand and maintain
extern std::vector<bool> zernEnables;
//double zernike(int n, double x, double y);
//void gauss_jordan(int n, double* Am, double* Bm);


void dumpArma(const arma::mat &mm, const QString &title = "",
            QVector<QString> colHeading = QVector<QString>(0),
            QVector<QString> RowLable = QVector<QString>(0));

class zernikeProcess : public QObject
{
    Q_OBJECT
private:
    static zernikeProcess *m_Instance;
    int m_gridRowSize;
    double m_radius;
    int m_maxOrder;
    double m_obsPercent;


    bool m_needsInit;

public:
    arma::mat m_rhoTheta;
    bool m_lastusedAnnulus;
    explicit zernikeProcess(QObject *parent = 0);
    static zernikeProcess *get_Instance();
    void unwrap_to_zernikes(wavefront &wf, int zterms = Z_TERMS);
    cv::Mat null_unwrapped(wavefront&wf,  std::vector<double> zerns, std::vector<bool> enables,int start_term =0, int last_term = Z_TERMS);
    std::vector<double> ZernFitWavefront( wavefront &wf);
    void initGrid(wavefront &wf, int maxOrder);
    void initGrid(int width, double radius, double cx, double cy, int maxOrder, double inside = 0);
    void unwrap_to_zernikes(zern_generator *zg, cv::Mat wf, cv::Mat mask);
    cv::Mat makeSurfaceFromZerns(int border, bool doColor);

    arma::mat rhotheta( int width, double radius, double cx, double cy, const wavefront *wf = 0);

    arma::mat zpmC(arma::rowvec rho, arma::rowvec theta, int maxorder);
    void fillVoid(wavefront &wf);
    void setMaxOrder(int n);
    int getNumberOfTerms();
    cv::Mat Z;
    cv::Mat inputZ;
    bool m_dirty_zerns;
    bool m_bDontProcessEvents;

    mirrorDlg *md;
    MainWindow *mw;
    arma::mat m_zerns;
    QVector<double> m_norms;
    std::vector<int> m_row;
    std::vector<int> m_col;
signals:
void statusBarUpdate(QString, int);
public slots:

};


#endif // ZERNIKEPROCESS_H
