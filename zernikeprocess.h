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
extern std::vector<bool> zernEnables;
extern int Zw[];
extern double BestSC;
double zernike(int n, double x, double y);
void gauss_jordan(int n, double* Am, double* Bm);
void ZernikeSmooth(cv::Mat wf, cv::Mat mask);

typedef struct  {
    std::vector<bool> enables;
    std::vector<double> norms;
    int maxOrder;
    int noOfTerms;
    std::vector<int> rowIndex;
    std::vector<int> colIndex;
    arma::mat zernsArSample;
    arma::mat rhoTheta;
    int widthOfMatrix;
    double radius;
    double centerX;
    double centerY;
    double insideRadius;
    bool valid;
} zernikeData;


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
    arma::mat zapmC(const arma::rowvec& rho, const arma::rowvec& theta, const int& maxorder=12);
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

class zernikePolar : public QObject
{
    Q_OBJECT
public:
    explicit zernikePolar(){};
    static zernikePolar *get_Instance();
    void init(double rho, double theta);
    double zernike(int z, double rho, double theta);
private:
     static zernikePolar *m_instance;
     double rho2;
     double rho3;
     double rho4;
     double rho5;
     double rho6;
     double rho8;
     double rho10;
     double costheta;
     double sintheta;
     double cos2theta;
     double sin2theta;
     double cos3theta;
     double sin3theta;
     double cos4theta;
     double sin4theta;
     double cos5theta;
     double sin5theta;
};

void debugZernRoutines();

#endif // ZERNIKEPROCESS_H
