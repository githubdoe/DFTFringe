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
#include "zernikeprocess.h"
#include <opencv2/opencv.hpp>
#include <cmath>
#include "mainwindow.h"
#include <QDebug>
#include "surfaceanalysistools.h"
#include "simigramdlg.h"
#include "settings2.h"
#include "myutils.h"
//#include "arbitrarywavefrontdlg.h"
#include "userdrawnprofiledlg.h"
std::vector<bool> zernEnables;
std::vector<double> zNulls;
double BestSC = -1.;
int Zw[] = {								/*  n    */
    1,									  //    0
    4,4,3,								  //	1
    6,6,8,8,5,  //8						  //	2
    8,8,10,10,12,12,7, // 15					3
    10,10,12,12,14,14,16,16,9,  //24			4
    12,12,14,14,16,16,18,18,20,20,11,  //35		5
    14,14,16,16,18,18,20,20,22,22,24,24,13  // 48
};



//
//	Copyright (C) 2019 Michael Peck <mpeck1 -at- ix.netcom.com>
//
//	License: MIT <https://opensource.org/licenses/MIT>
//

// Fill a matrix with Zernike polynomial values



// [[Rcpp::export]]


cv::Mat zpmCx(QVector<double> rho, QVector<double> theta, int maxorder) {

  int m, n, n0, mmax = maxorder/2;
  int i, imax = rho.size();
  int order, nm, nm1mm1, nm1mp1, nm2m;
  int ncol = (mmax+1)*(mmax+1);
  double a0;
  double cosmtheta[mmax], sinmtheta[mmax];
  cv::Mat_<double>  zm(imax, ncol);

  //do some rudimentary error checking

  //if (rho.length() != theta.length()) stop("Numeric vectors must be same length");
  //if ((maxorder % 2) != 0) stop("maxorder must be even");

  //good enough


  for (i=0; i<imax; i++) {

    //cache values of cos and sin
    cosmtheta[0] = std::cos(theta[i]);
    sinmtheta[0] = std::sin(theta[i]);
    for (m=1; m<mmax; m++) {
      cosmtheta[m] = cosmtheta[m-1]*cosmtheta[0] - sinmtheta[m-1]*sinmtheta[0];
      sinmtheta[m] = sinmtheta[m-1]*cosmtheta[0] + cosmtheta[m-1]*sinmtheta[0];
    }

    zm(i, 0) = 1.0;                     //piston term
    zm(i, 3) = 2. * rho[i] * rho[i] - 1.; //defocus

    // now fill in columns with m=n for n>0

    for (m=1; m <= mmax; m++) {
      zm(i, m*m) = rho[i] * zm(i, (m-1)*(m-1));
    }

    // non-symmetric terms

    for (order=4; order<=maxorder; order+=2) {
      for (m=order/2-1; m>0; m--) {
        n=order-m;
        nm = order*order/4 + n - m;
        nm1mm1 = (order-2)*(order-2)/4 + n - m;
        nm1mp1 = nm - 2;
        nm2m = nm1mm1 - 2;
        zm(i, nm) = rho[i]*(zm(i, nm1mm1) + zm(i, nm1mp1)) - zm(i, nm2m);
      }

      // m=0 (symmetric) term
      nm = order*order/4 + order;
      nm1mp1 = nm-2;
      nm2m = (order-2)*(order-2)/4+order-2;
      zm(i, nm) = 2.*rho[i]*zm(i, nm1mp1) - zm(i, nm2m);
    }

    // now multiply each column by normalizing factor and cos, sin

    n0 = 1;
    for (order=2; order <= maxorder; order+=2) {
      for(m=order/2; m>0; m--) {
        n=order-m;
        a0 = sqrt(2.*(n+1));
        zm(i, n0+1) = a0*sinmtheta[m-1]*zm(i, n0);
        zm(i, n0) *= a0*cosmtheta[m-1];
        n0 += 2;
      }
      n = order;
      zm(i, n0) *= sqrt(n+1.);
      n0++;
    }
  }
  return zm;
}
/*
Public Function ZernikeW(n As Integer) As Double
' N is Zernike number as given by James Wyant
' routine calculates the inverse of the weight in the variance computation
*/
int ZernikeW(int n)
{
    return(Zw[n]);
}

long fact( int n1, int n2)
{
    int f = n1;
    for (int i = n1-1; i > n2; --i)
    {
        f *= i;
    }
    return f;

}
void gauss_jordan(int n, double* Am, double* Bm)
{
    /*
Private Sub GJ(n As Integer)
Dim indxc(50) As Integer, indxr(50) As Integer, ipiv(50) As Integer
Dim i As Integer, icol As Integer, irow As Integer, j As Integer
Dim k As Integer, l As Integer, ll As Integer
Dim big As Double, dum As Double, pivinv As Double, temp As Double

'solves the equation b=ax
   'N is matrix order
   'on entry, Bm(1 to N) is the left-hand-side column vector
   'on exit, Bm(1 to N) is the solution vector, x
   'on entry, Am(1 to N, 1 to N) is the matrix coefficients
   'on exit, Am is the inverse matrix
 */
    int* ipiv = new int[n];
    int* indxr = new int[n];
    int* indxc = new int[n];
    double big;
    int irow = 0;
    int icol = 0;
    double pivinv;

    for(int  j = 0; j < n; ++j)
    {
        ipiv[j] = 0;
    }

    for(int i = 0; i < n; ++i)
    {
        big = 0.;
        for(int  j = 0; j < n; ++j)
        {
            if ((ipiv[j] != 1))
            {
                int row_ndx = j * n;
                for(int k = 0; k < n; ++k)
                {
                    if (ipiv[k] == 0)
                    {
                        int ndx = row_ndx + k;
                        if (fabs(Am[ndx]) >= big)
                        {
                            big = fabs(Am[ndx]);
                            irow = j;
                            icol = k;
                        }
                    }
                }

            }
        }
        ++ipiv[icol];
        if (irow != icol)
        {
            for(int l = 0; l < n; ++l)
            {
                int irow_ndx = irow * n + l;
                int icol_ndx = icol * n + l;
                double temp = Am[irow_ndx];
                Am[irow_ndx] = Am[icol_ndx];
                Am[icol_ndx] = temp;
            }
            double temp = Bm[irow];
            Bm[irow] = Bm[icol];
            Bm[icol] = temp;
        }
        indxr[i] = irow;
        indxc[i] = icol;
        pivinv = 1. / Am[icol * n + icol];
        Am[icol * n + icol] = 1.;
        for(int l = 0; l < n; ++l)
        {
            int ndx = icol * n + l;
            Am[ndx] *= pivinv;
        }
        Bm[icol] *= pivinv;
        for(int ll = 0; ll < n; ++ll)
        {
            if (ll != icol)
            {
                int ll_ndx = ll * n;
                double dum = Am[ll_ndx + icol];
                Am[ll_ndx + icol] = 0.;
                for( int l = 0; l < n; ++l)
                {
                    int ndx = ll * n + l;
                    Am[ndx] = Am[ndx] - Am[icol * n +  l] * dum;
                }
                Bm[ll] = Bm[ll] - Bm[icol] * dum;
            }
        }
    }
    for(int l = n-1; l>= 0; --l)
    {
        if (indxr[l] != indxc[l])
        {
            for(int k = 0; k < n; ++k)
            {
                double temp = Am[k * n + indxr[l]];
                Am[k * n + indxr[l]] = Am[k * n + indxc[l]];
                Am[k * n + indxc[l]] = temp;
            }
        }
    }
    delete[] ipiv;
    delete[] indxr;
    delete[] indxc;
}
/*
Public Function Zernike(n As Integer, X As Double, Y As Double) As Double
' N is Zernike number as given by James Wyant
*/
zernikePolar *zernikePolar::m_instance = 0;
zernikePolar *zernikePolar::get_Instance(){
    if (m_instance == 0){
        m_instance = new zernikePolar;
    }
    return m_instance;
}

void zernikePolar::init(double rho, double theta){
        rho2 = rho * rho;
        rho3 = pow(rho,3.);
        rho4 = pow(rho,4.);
        rho5 = pow(rho,5.);
        rho6 = pow(rho,6.);
        rho8 = pow(rho,8.);
        rho10 = pow(rho,10.);
        costheta = cos(theta);
        sintheta = sin(theta);
        cos2theta = cos(2. * theta);
        sin2theta = sin(2. * theta);
        cos3theta = cos(3. * theta);
        sin3theta = sin(3. * theta);
        cos4theta = cos(4. * theta);
        sin4theta = sin(4. * theta);
        cos5theta = cos(5. * theta);
        sin5theta = sin(5. * theta);
    }

double zernikePolar::zernike(int n, double rho, double theta){

    switch(n){
    case 0: return 1.;
        break;
    case 1: return rho * costheta;
        break;
    case 2: return rho * sintheta;
        break;
    case 3: return -1. + 2. * rho * rho;
        break;
    case 4: return rho2 * cos2theta;
        break;
    case 5: return rho2 * sin2theta;
        break;
    case 6: return rho * (-2. + 3. * rho2) * costheta;
        break;
    case 7: return rho * (-2. + 3. * rho2) * sintheta;
        break;
    case 8:
    {
        return 1. + rho2 * (-6 + 6. * rho2);
        break;
    }
    case 9: return rho * rho * rho * cos3theta;
        break;
    case 10: return rho * rho * rho * sin3theta;
        break;

    case 11:

        return rho2 * (-3 + 4 * rho2) * cos2theta;

        break;
    case 12:

        return rho2 * (-3 + 4 * rho2) * sin2theta ;

        break;

    case 13:
        return rho * (3. - 12. * rho2 + 10. * rho4) * costheta;
        break;
    case 14:
        return rho * (3. - 12. * rho2 + 10. * rho4) * sintheta;
        break;
    case 15:
        return -1 + 12 * rho2 - 30. * rho4 + 20. * rho6;
        break;
    case 16: return rho4 * cos4theta;
        break;
    case 17: return rho4 * sin4theta;
        break;
    case 18: return rho3 *( -4. + 5. * rho2) * cos3theta;
        break;
    case 19: return rho3 *( -4. + 5. * rho2) * sin3theta;
        break;
    case 20: return rho2 * (6. - 20. * rho2 + 15 * rho4)* cos2theta;
        break;
    case 21: return rho2 * (6. - 20. * rho2 + 15 * rho4)* sin2theta;
        break;
    case 22: return rho * (-4. + 30. * rho2 - 60. * rho4 + 35 * rho6)* costheta;
        break;
    case 23: return rho * (-4. + 30. * rho2 - 60. * rho4 + 35 * rho6)* sintheta;
        break;
    case 24: return 1. - 20. * rho2 + 90. *  rho4 - 140. * rho6 + 70. * rho8;
        break;
    case 25: return rho5 * cos5theta;
        break;
    case 26: return rho5 * sin5theta;
        break;
    case 27: return rho4 * (-5. + 6. * rho2) * cos4theta;
        break;
    case 28: return rho4 * (-5. + 6. * rho2) * sin4theta;
        break;
    case 29: return rho3 * (10. - 30. * rho2 + 21. * rho4) * cos3theta;
        break;
    case 30: return rho3 * (10. - 30. * rho2 + 21. * rho4) * sin3theta;
        break;
    case 31: return rho2 *(-10. + 60. * rho2 - 105. * rho4 + 56. * rho6) * cos2theta;
        break;
    case 32: return rho2 *(-10. + 60. * rho2 - 105. * rho4 + 56. * rho6) * sin2theta;
        break;
    case 33: return rho * (5. - 60. * rho2 + 210 * rho4 -280. * rho6 + 126. * rho8) * costheta;
        break;
    case 34: return rho * (5. - 60. * rho2 + 210 * rho4 -280. * rho6 + 126. * rho8) * sintheta;
        break;
    case 35: return -1 + 30. * rho2 -210 * rho4 + 560. * rho6 - 630 * rho8 + 252. * rho10;
        break;
    case 36: return rho6 * cos(6. * theta);
        break;
    case 37: return rho6 * sin(6. * theta);
        break;
    case 38: return rho5 * (-6. + 7 * rho2) * cos5theta;
        break;
    case 39: return rho5 * (-6. + 7 * rho2) * sin5theta;
        break;
    case 40: return rho4 * (15. -42. * rho2 + 28. * rho4) * cos4theta;
        break;
    case 41: return rho4 * (15. -42. * rho2 + 28. * rho4) * sin4theta;
        break;
    case 42: return rho3 * (-20 + 105. * rho2 - 168. * rho4 + 84 * rho6) * cos3theta;
        break;
    case 43: return rho3 * (-20. + 105. * rho2 - 168. * rho4 + 84. * rho6) * sin3theta;
        break;
    case 44: return rho2 * (15. - 140. * rho2 + 420. * rho4 - 504. * rho6 +  210. * rho8) * cos2theta;
        break;
    case 45: return rho2 * (15. - 140. * rho2 + 420. * rho4 - 504. * rho6 +  210. * rho8) * sin2theta;
        break;
    case 46: return rho *(-6. + 105 * rho2 - 560. * rho4 + 1260. * rho6 -1260. * rho8 +462. * rho10) * costheta;
        break;
    case 47: return rho *(-6. + 105 * rho2 - 560. * rho4 + 1260. * rho6 -1260. * rho8 +462. * rho10) * sintheta;
        break;
    case 48: return 1. - 42. * rho2 + 420. * rho4 - 1680. * rho6 + 3150. * rho8 -2772. * rho10 + 924. * pow(rho,12.);
        break;

    }
    return 0.;


}


double Zernike(int n, double X, double Y)
{


    static double X2 = 0., X3 = 0., X4 = 0.;
    static double Y2 = 0., Y3 = 0., Y4 = 0.;
    static double R2 = 0.;
    static double last_x = 0.;
    static double last_y = 0.;

    if (last_x != X || last_y != Y)
    {
        X2 = X * X;
        X3 = X2 * X;
        X4 = X2 * X2;
        last_x = X;
        Y2 = Y * Y;
        Y3 = Y2 * Y;
        Y4 = Y2 * Y2;
        R2 = X2 +Y2;


        last_y =Y;
    }

    double d;
    switch(n)
    {
    case 0:
        //	return(.01 * sin(3 * M2PI * sqrt(R2)));
        return(1.);
        break;
    case 1:
        return(X);
        break;
    case 2:
        return(Y);
        break;
    case 3:
        return(-1. + 2. * R2);
        break;
    case 4:
        return(X2 - Y2);
        break;
    case 5:
        return(2. * X * Y);
        break;
    case 6:
        return(-2. * X + 3. * X * R2);
        break;
    case 7:
        return(-2. * Y + 3. * Y * R2);
        break;
    case 8:
        return(1.+ R2 * (-6. + 6. * R2));
        break;
    case 9:
        return(X3 - 3. * X * Y2);
        break;
    case 10:
        return(3. * X2 * Y - Y3);
        break;
    case 11:
        return(-3. * X2 + 4.* X4 + 3. * Y2- 4. * Y4);
        break;
    case 12:
        return 2. * X * Y*(-3. + 4. * R2);
        break;
    case 13:
        return X * (3. + R2 * (-12. + 10. * R2));
        break;
    case 14:
        return Y * (3. + R2 * (-12. + 10. * R2));
        break;
    case 15:		//2nd spherical
        return(-1. + R2 * (12. + R2* (-30. + 20. * R2)));
        break;
    case 16:
        return(X4 - 6. * X2 * Y2 + Y4);
        break;
    case 17:
        return 4. * X * Y * (X2- Y2);
        break;
    case 18:
        return X * (5. * X4 + 3. * Y2 *(4.-5.* Y2) - 2 * X2 * ( 2. + 5. * Y2));
        break;
    case 19:
        return Y * (15. * X4 + 4. * Y2  - 5. * Y4 + 2. * X2 * (-6. + 5. * Y2 ));
        break;
    case 20:
        return (X2 - Y2)*(6. + R2 * (-20. + 15.* R2));
        break;
    case 21:
        return       2. * X*Y*(6. + R2 * (-20.  + 15. * R2));
        break;
    case 22:
        return X * (-4. + R2 * (30. + R2 * (-60. + 35. * R2)));
        break;
    case 23:
        return Y * (-4. + R2 * (30. + R2 * (-60. + 35. * R2)));
        break;
    case 24:  //3Rd spherical
        return(1. + R2 * (-20. + R2 * (90. + R2 * (-140. + 70. * R2))));
        break;
    case 25:
        return X *( X4 - 10. * X2 * Y2 + 5. * Y4);
        break;
    case 26:
        d = Y*(5. * X4 - 10. * X2* Y2 + Y4);
        break;
    case 27:
        d = 6.* X4 * X2 - (30. * X2 * Y2) * (-1. + Y2) + Y4 * (-5. + 6. * Y2) - 5* X4 *(1.+6. * Y2);
        break;
    case 28:
        d = X*(-20. *  X2 * Y + 20. * Y3 + 24. * X2 * Y * R2 -	24 * Y3 * R2);
        break;
    case 29:
        d = X*( 10. * X2 - 30. * Y2 + R2 *(-30. * X2 + 90.* Y2 + R2 * (21.*X2 - 63.* Y2)));
        break;
    case 30:
        d = Y*(-10. * Y2 + 30. * X2 + R2 * (30. * Y2   - 90.* X2 + R2 * (-21.* Y2 + 63.* X2)));
        break;
    case 31:
        d = (-10. + R2 * (60. + R2* (-105. + 56. * R2)))* (X2 - Y2);
        break;
    case 32:
        d =  X*Y*(-20. + R2 * (120. + R2* (- 210. + 112.* R2)));
        break;
    case 33:
        d = X*(5. + R2 * (-60. + R2*(210. + R2 *(-280. + R2 * 126))));
        break;
    case 34:
        d = Y*(5. +R2 * (-60. + R2 * (210. +R2 * (-280. + 126.* R2))));
        break;
    case 35: //4 spherical
        d = -1. + R2 * (30. + R2 * (-210.  + R2 * (560. +R2 * (-630. + 252.* R2))));
        break;
    case 36:
        d = X4 * X2 - 15. * X4 * Y2 + 15. * X2 * Y4 - Y4*Y2;
        break;
    case 37:
        d = 6. * X4 * X * Y - 20. * X3 * Y3 + 6. * X * Y4*Y;
        break;
    case 38:
        d = -6. * X4 * X + 60. * X3 * Y2 - 30. * X * Y4 + 7. * X4 * X * R2 - 70. * X3 * Y2 * R2 +
            35. * X * Y4 * R2;
        break;
    case 39: // Spherical 5
        d = 1. + R2* (-42. + R2 * (420. + R2 *(-1680. + R2 * (3150.+ R2 * (-2772. + 924. * R2)))));
        break;
    case 40:	// spherical 6
        {

        d = -1. + R2 * (56. +R2 * (-756. + R2 * (4200. + R2 * ( -11550.  + R2 * (16632. +R2 * (-12012.  +3432.* R2))))));
        }
        break;

    case 41:	// spherical 7

        d = 1. + R2 * (-72. + R2 * (1260. +R2*(-9240. + R2 * (34650. +R2 * (-72072. + R2 * (84084.
            +R2 * (-51480. + 12870. * R2)))))));
        //d = zpr(8,0,R);
        break;

    default:
        return(0.);
    }
    return d;


}



zernikeProcess *zernikeProcess::m_Instance = NULL;
zernikeProcess *zernikeProcess::get_Instance(){
    if (m_Instance == NULL){
        m_Instance = new zernikeProcess();
    }
    return m_Instance;
}

zernikeProcess::zernikeProcess(QObject *parent) :
    QObject(parent),m_gridRowSize(200), m_maxOrder(0),m_needsInit(true),m_dirty_zerns(true)
{
    md = mirrorDlg::get_Instance();;
    QSettings set;
    setMaxOrder(set.value("Zern maxOrder", 12).toInt());
}

// compute zernikes from unwrapped surface
#define SAMPLE_WIDTH 1
void zernikeProcess::unwrap_to_zernikes(wavefront &wf, int zterms){

    int nx = wf.data.cols;
    int ny = wf.data.rows;

    cv::Mat surface = wf.data;

    //if (!m_dirty_zerns)
        //return;

    Settings2 &settings = *Settings2::getInstance();
    if (wf.m_inside.m_radius > wf.m_outside.m_radius * settings.m_general->getObs()) {
        initGrid(wf, 12);
        ZernFitWavefront(wf);
        for (int z = 1; z < m_norms.size(); ++z){
            wf.InputZerns[z] *= m_norms[z];
        }
        return;
    }
    /*
    'calculate LSF matrix elements
    */


    bool useSvd = false;
    if (settings.m_general->useSVD()){
        useSvd = true;
    }

    cv::Mat_<double> A;//(count,Z_TERMS);
    cv::Mat_<double> B;//(count,1);
    cv::Mat_<double> X(zterms,1);
    int count = 0;
    if (useSvd){
        count = cv::countNonZero(wf.workMask);
        A = cv::Mat_<double>::zeros(count,zterms);
        B = cv::Mat_<double>::zeros(count,1);
    }
    else {
        A = cv::Mat_<double>::zeros(zterms,zterms);
        B = cv::Mat_<double>::zeros(zterms,1);
    }


    //calculate LSF right hand side
    int step = SAMPLE_WIDTH;

    while ((nx/step) > 100)
    {
        ++step;
    }

    double delta = 1./(wf.m_outside.m_radius);
    zernikePolar &zpolar = *zernikePolar::get_Instance();
    int sampleCnt = 0;
    for(int y = 0; y < ny; y += step) //for each point on the surface
    {
        //((MainWindow*)parent())->progBar->setValue(100 * y/ny);
        for(int x = 0; x < nx; x += step)
        {
            double ux = (x -wf.m_outside.m_center.x()) * delta;
            double uy = (y -wf.m_outside.m_center.y()) * delta;
            double rho = sqrt(ux * ux + uy * uy);

            if ( rho <= 1. && (wf.mask.at<uchar>(y,x) != 0) && wf.data.at<double>(y,x) != 0.0){
                double theta = atan2(uy,ux);
                zpolar.init(rho, theta);
                for ( int i = 0; i < zterms; ++i)
                {

                    if (useSvd){
                        double t = zpolar.zernike(i, rho, theta);
                        A(sampleCnt,i) = t;
                    }
                    else {

                        double t = zpolar.zernike(i, rho, theta);
                        for (int j = 0; j < zterms; ++j)
                        {
                            //Am[ndx] = Am[ndx] + t * zpolar.zernike(j, rho, theta);
                            A(i,j) +=  t * zpolar.zernike(j, rho, theta);
                        }
                        // FN is the OPD at (Xn,Yn)
                        //Bm[i] = Bm[i] + surface.at<double>(y,x) * t;
                        B(i) +=     surface.at<double>(y,x) * t;
                    }

                }
                if (useSvd){
                    B(sampleCnt++) = surface.at<double>(y,x);
                    if (sampleCnt > count){
                        QMessageBox::warning(0,"Critical Error", QString("Zernike computation sampleCnt > count %1 %2").arg(sampleCnt).arg(count));
                        return ;
                    }
                }
            }
        }
    }

    cv::solve(A,B,X,(useSvd)? DECOMP_SVD : DECOMP_LU);
    if (settings.m_general->showConditionNumbers()){
        cv::Mat Ai;

        double conditionNumber = 1./cv::invert(A,Ai,DECOMP_SVD);
        double c2 = cv::norm(A,NORM_L2) * cv::norm(Ai,NORM_L2);
        emit statusBarUpdate(QString(" Zernike LSF matrix Condition Numbers %1 %2").arg(conditionNumber, 6, 'f', 3).arg(c2, 6, 'f', 3),1);
    }
    wf.InputZerns = std::vector<double>(zterms,0);
    for (int z = 0; z < X.rows; ++z){
        wf.InputZerns[z] = X(z);
    }


}

cv::Mat zernikeProcess::null_unwrapped(wavefront&wf, std::vector<double> zerns, std::vector<bool> enables,
                                       int start_term, int last_term)
{

    int nx = wf.data.cols;
    int ny = wf.data.rows;

    cv::Mat unwrapped = wf.data.clone();

    double scz8 = md->z8 * md->cc;
    Settings2 &settings = *Settings2::getInstance();

    if (wf.m_inside.m_radius > wf.m_outside.m_radius * settings.m_general->getObs()) {
        double E = wf.m_inside.m_radius/wf.m_outside.m_radius;
        double f = (1 - E * E);
        f *= f;
        scz8 =  scz8*f;
    }
    if (!md->doNull || !wf.useSANull){
        scz8 = 0.;
    }
    double midx = wf.m_outside.m_center.rx();
    double midy = wf.m_outside.m_center.ry();
    double rad = wf.m_outside.m_radius;
    cv::Mat mask = cv::Mat::zeros(wf.data.size(), CV_8UC1);
    uchar val = 0xff;
    fillCircle(mask, midx,midy, rad + 2, &val);
    cv::Mat nulled = cv::Mat::zeros(wf.data.size(),numType);
    if (wf.m_inside.m_radius > 0){
        uchar val = 0;
        fillCircle(mask, wf.m_inside.m_center.x(), wf.m_inside.m_center.y(), wf.m_inside.m_radius-2, &val);
    }

    bool doDefocus = surfaceAnalysisTools::get_Instance()->m_useDefocus;
    double defocus = 0;
    if (doDefocus){
        defocus = surfaceAnalysisTools::get_Instance()->m_defocus;

    }

    double ux,uy,sz,nz;
    double rho,theta;
    zernikePolar &zpolar = *zernikePolar::get_Instance();
    for(int  y = 0; y < ny; ++y)
    {
        for(int x = 0; x < nx; ++x)
        {

            if (mask.at<uint8_t>(y,x) != 0 && wf.data.at<double>(y,x) != 0.0)
            {
                ux = (double)(x - midx)/rad;
                uy = (double)(y - midy)/rad;
                rho = sqrt(ux * ux + uy * uy);
                if (rho > 1.){
                    continue;
                }

                theta = atan2(uy,ux);
                zpolar.init(rho,theta);

                sz = unwrapped.at<double>(y,x);
                nz = 0;

                if (last_term > 7)
                {
                    if (md->doNull && enables[8])
                        nz -= scz8 * zpolar.zernike(8,rho, theta);
                }

                for (int z = start_term; z < Z_TERMS; ++z)
                {
                    if ((z == 3) & doDefocus){
                        nz += defocus * zpolar.zernike(z,rho, theta);
                        nz -= zerns[z] * zpolar.zernike(z,rho,theta);

                    }
                    else
                    if (!enables[z])
                        nz -= zerns[z] * zpolar.zernike(z,rho, theta);

                }

                nulled.at<double>(y,x) = sz +nz;
            }
        }
    }

    //generate_image_from_doubles(nulled_image, nx,ny, CString(L"Nulled"),true);

    return nulled;
}

void zernikeProcess::fillVoid(wavefront &wf){
    // fill in "ignore regions" - interpolate using all our zernike terms
    double ux,uy;
    double rho,theta;

    zernikePolar &zpolar = *zernikePolar::get_Instance();

    if (wf.regions.size() > 0){
        int x = wf.regions[0][0].x;
        int y = wf.mask.rows - wf.regions[0][0].y;
        int startx = x;
        int endx = x;
        int starty = y;
        int endy = y;
        for (int n = 0; n < wf.regions.size(); ++n){

            for (std::size_t i = 0; i < wf.regions[n].size(); ++i){
                int x = wf.regions[n][i].x;
                int y = wf.mask.rows - wf.regions[n][i].y;
                startx = fmin(startx, x);
                endx = fmax(endx, x);
                starty = fmin(starty, y);
                endy = fmax(endy, y);
            }

            startx-= 2;
            endx += 2;
            starty -=2;
            endy +=2;
            double midx = wf.m_outside.m_center.x();
            double midy = wf.m_outside.m_center.y();
            double rad = wf.m_outside.m_radius;
            for (int y = starty; y < endy; ++y){
                for (int x = startx; x < endx; ++x){
                    if (x < 0 || y < 0 || x >= wf.data.cols || y >= wf.data.rows){
                        continue;
                    }
                    if (wf.mask.at<uint8_t>(y,x) == 0){
                        ux = (double)(x - midx)/rad;
                        uy = (double)(y - midy)/rad;
                        rho = sqrt(ux * ux + uy * uy);
                        theta = atan2(uy,ux);
                        zpolar.init(rho,theta);
                        double v = 0.;

                        for (int z = 0; z < m_norms.size(); ++z){
                            v += wf.InputZerns[z] * zpolar.zernike(z,rho, theta);
                        }
                        wf.data.at<double>(y,x) = v;
                    }
                }
            }
        }
    }
    if (wf.m_inside.isValid()) {
        // now also fill in region near central mask border - the central obstruction.
        // So we just fill in slightly outward and all the way to the center (in case we are averaging with another
        // wavefront later where the center obstruction is in a different position or missing)
        // Outward we go only 1 pixel in case a rotated wavefront has an unmasked pixel set to zero (maybe we should do the fill before rotating?)


        // outer radius of area to fill in
        double radius_outer_fill = wf.m_inside.m_radius+5; // go out a few pixels

        double in_midx = wf.m_inside.m_center.x();
        double in_midy = wf.m_inside.m_center.y();


        int startx = in_midx - radius_outer_fill;
        int endx   = in_midx + radius_outer_fill;
        int starty = in_midy - radius_outer_fill;
        int endy   = in_midy + radius_outer_fill;

        startx-= 2;
        endx += 2;
        starty -=2;
        endy +=2;
        double midx = wf.m_outside.m_center.x();
        double midy = wf.m_outside.m_center.y();
        double rad = wf.m_outside.m_radius;

        //showData("fill void mask",wf.mask);

        for (int y = starty; y < endy; ++y){
            for (int x = startx; x < endx; ++x){
                if (x < 0 || y < 0 || x >= wf.data.cols || y >= wf.data.rows){
                    continue;
                }

                if (wf.mask.at<uint8_t>(y,x) == 0){
                    ux = (double)(x - midx)/rad;
                    uy = (double)(y - midy)/rad;
                    rho = sqrt(ux * ux + uy * uy);
                    theta = atan2(uy,ux);
                    zpolar.init(rho,theta);
                    double v = 0.;

                    for (int z = 0; z < m_norms.size(); ++z){
                        v += wf.InputZerns[z] * zpolar.zernike(z,rho, theta);
                    }
                    wf.data.at<double>(y,x) = v;
                }
            }
        }
    }
}
int zernikeProcess::getNumberOfTerms(){
    int terms = m_maxOrder/2+1;

    return terms * terms;
}
void spectral_color(double &R,double &G,double &B,double wavelength)
// RGB <0,1> <- lambda l <400,700> [nm]
{ 
    R=0.0; 
    G=0.0; 
    B=0.0; 
    double gamma = .8;
    double attenuation = 1;

    if (wavelength >= 380 && wavelength <= 440){
        attenuation = 0.3 + 0.7 * (wavelength - 380) / (440 - 380);
        R = pow(((-(wavelength - 440) / (440 - 380)) * attenuation),gamma);
        G = 0.0;
        B = pow(1.0 * attenuation, gamma);
    }
    else if( wavelength >= 440 && wavelength <= 490){
        R = 0.0;
        G = pow(((wavelength - 440) / (490 - 440)), gamma);
        B = 1.0;
    }
    else if (wavelength >= 490 && wavelength <= 510){
        R = 0.0;
        G = 1.0;
        B = pow((-(wavelength - 510) / (510 - 490)), gamma);
    }
    else if (wavelength >= 510 && wavelength <= 580){
        R = pow(((wavelength - 510) / (580 - 510)), gamma);
        G = 1.0;
        B = 0.0;
    }
    else if (wavelength >= 580 && wavelength <= 645){
        R = 1.0;
        G = pow((-(wavelength - 645) / (645 - 580)), gamma);
        B = 0.0;
    }
    else if (wavelength >= 645 && wavelength <= 750){
        attenuation = 0.3 + 0.7 * (750 - wavelength) / (750 - 645);
        R = pow((1.0 * attenuation), gamma);
        G = 0.0;
        B = 0.0;
    }
    else{
        R = .5;
        G = .5;
        B = .5;
    }
    qDebug() << R << G << B;
/*    if ((l>=400.0)&&(l<410.0)) {
        t=(l-400.0)/(410.0-400.0);
        r= +(0.33*t)-(0.20*t*t);
        g = 0.
        b = 1.) * t;
    }
    else if ((l>=410.0)&&(l<475.0)) {
        t=(l-410.0)/(475.0-410.0);
        r=0.14 -(0.13*t*t); }
    else if ((l>=545.0)&&(l<595.0)) {
        t=(l-545.0)/(595.0-545.0);
        r= +(1.98*t)-( t*t); }
    else if ((l>=595.0)&&(l<650.0)) {
        t=(l-595.0)/(650.0-595.0);
        r=0.98+(0.06*t)-(0.40*t*t);
    }
    else if ((l>=650.0)&&(l<700.0)) {
        t=(l-650.0)/(700.0-650.0);
        r=0.65-(0.84*t)+(0.20*t*t);
    }
    if ((l>=415.0)&&(l<475.0)) {
        t=(l-415.0)/(475.0-415.0);
        g= +(0.80*t*t);
    }
    else if ((l>=475.0)&&(l<590.0)) { t=(l-475.0)/(590.0-475.0);
        g=0.8 +(0.76*t)-(0.80*t*t);
    }
    else if ((l>=585.0)&&(l<639.0)) {
        t=(l-585.0)/(639.0-585.0);
        g=0.84-(0.84*t) ;
    } if ((l>=400.0)&&(l<475.0)) {
        t=(l-400.0)/(475.0-400.0);
        b= +(2.20*t)-(1.50*t*t);
    } else if ((l>=475.0)&&(l<560.0)) {
        t=(l-475.0)/(560.0-475.0);
        b=0.7 -( t)+(0.30*t*t);
    }
    */
}


cv::Mat zernikeProcess::makeSurfaceFromZerns(int border, bool doColor){
    simIgramDlg &dlg = *simIgramDlg::get_instance();
    double obs = .01 * dlg.getObs();
    int wx = dlg.size + 2 *  border;
    double rad = (double)(wx-1)/2.;
    rad -= border;
    cv::Mat result = cv::Mat::ones(wx,wx, (doColor)? CV_8UC4: numType);



    initGrid(wx, rad, (wx-1)/2, (wx-1)/2, m_maxOrder, 0);
    double spacing = 1.;
    mirrorDlg *md = mirrorDlg::get_Instance();
    qDebug() << "fringe spacing" << md->fringeSpacing;
    double r,g,b;
    spectral_color(r,g,b, md->lambda);
    if (doColor) {
        result =  Vec4f(0.,125. * .5 * g, 125 * r, 125. * b);
    }


    UserDrawnProfileDlg * dlg_arbitrary = UserDrawnProfileDlg::get_instance();
    if (dlg.m_doArbitrary)
        dlg_arbitrary->prepare(dlg.size);


    for (std::size_t i = 0; i < m_zerns.n_rows; ++i)
    {

        double rho = m_rhoTheta.row(0)(i);
        double theta = m_rhoTheta.row(1)(i);
        double S1 =
                dlg.star * cos(dlg.m_star_arms  *  theta) +
                dlg.ring * cos (dlg.m_ring_count * 2 * M_PI * rho);


        if (dlg.m_doEdge){
            double edge = dlg.m_edgeRadius;
            if (rho > edge){
                double p =  (rho - edge)/(1-edge);
                double v = pow(p,dlg.m_edgeSharp);
                double k = -v * dlg.m_edgeMag;
                S1 += k;
            }

        }

        if (dlg.m_doArbitrary)
            S1 += dlg_arbitrary->getValue(rho);



        for (unsigned int z = 0; z < dlg.zernikes.size(); ++z){
            double val = dlg.zernikes[z];
            if (z == 8){
                val = (dlg.doCorrection && md->doNull) ? md->cc * md->z8 * val * .01 : val;
            }
            S1 +=  val * m_zerns(i,z)/((doColor) ? md->fringeSpacing: 1.);

            int x =  m_col[i];
            int y =  m_row[i];
            if (doColor){

                if (rho < obs)
                    continue;
                int iv = cos(spacing *2 * M_PI * S1) * 100 + 120;
                result.at<Vec4b>(y,x)[0] = iv * b;
                result.at<Vec4b>(y,x)[1] = iv * g;
                result.at<Vec4b>(y,x)[2] = iv * r;
            }
            else {
                if (S1 == 0.0) S1 += .0000001;
                //if (rho < .5) S1 = 0.;
                result.at<double>(y,x) = S1;
            }
        }

    }

    return result;
}
#define TSIZE 450	// number of points in zern generator
void ZernikeSmooth(cv::Mat wf, cv::Mat mask)
{
    // given a wavefront generate arbitrary number of zernikes from it.  Then create wavefront from the zernikies.
    double *m = (double *)(wf.data);


    int size = wf.cols;
    int step = (double)size/200.;	// 200 samples across the wavefront
    double delta = (double)TSIZE/size;	// scale factor from wavefront to zernike generator

    double* tmp = new double[TSIZE * TSIZE];
    memset(tmp, 0,sizeof(double) * TSIZE * TSIZE);

    zern_generator zg(TSIZE);
    zg.set_spec(6);
    zg.zpoly_list();



    //'calculate LSF matrix elements

    int terms = zg.get_terms_cnt();
    int am_size = terms* terms;
    double* Am = new double[am_size];
    double* Bm = new double[terms];
    for (int  i = 0; i < am_size; ++i)
    {
        Am[i] = 0.;
    }
    for (int i = 0; i < terms; ++i)
        Bm[i] = 0;

    //calculate LSF right hand side
    for(int y = 0; y < size; y += step) //for each point on the surface
    {
        for(int x = 0; x < size; x += step)
        {
            int xx = x* delta;
            int yy = y * delta;
            if (mask.at<uint8_t>(x,y))
            {
                int sndx = x + y* size;

                tmp[xx+yy * TSIZE] = m[sndx];
                for (int  i = 0; i < terms; ++i)
                {
                    double ipoly = zg.get_zpoly(i,xx,yy);
                    int dy = i * terms;
                    for (int j = 0; j < terms; ++j)
                    {
                        int ndx = j + dy;
                        Am[ndx] = Am[ndx] +
                                ipoly * zg.get_zpoly(j, xx, yy);
                    }
                    Bm[i] = Bm[i] + m[sndx] * ipoly;

                }

            }
        }

    }
    // compute coefficients
    gauss_jordan (terms, Am, Bm);

    zg.set_zcoefs(Bm);

    for (int i = 0; i < terms; ++i){
        qDebug() << i << " " << Bm[i];
    }
    delete[] Am;
    delete[] Bm;
    delete[] tmp;
}


arma::mat zernikeProcess::rhotheta( int width, double radius, double cx, double cy,
                                   const wavefront *wf){
    bool useMask = false;
    if (wf != 0){
        useMask = true;
        radius = wf->m_outside.m_radius;
        width = wf->data.rows;
        cx = wf->m_outside.m_center.x();
        cy = wf->m_outside.m_center.y();
    }
    int rows = width;

    vector<double> rhov;
    vector<double> thetav;
     m_row.clear();
     m_col.clear();

    for (int y = 0; y < rows; ++y){
        double uy = (y -cy)/radius;

        QApplication::processEvents();
        for (int x = 0; x< rows; ++x){

            double ux = (x -cx)/radius;
            double rho = sqrt( ux * ux + uy * uy);
            double theta = atan2(uy,ux);
            if ( rho <= 1. && ((useMask) ?  (wf->mask.at<uchar>(y,x) != 0): true)
                 && ((wf == 0)? true:wf->data.at<double>(y,x) != 0.0)) {

                rhov.push_back(rho);
                thetav.push_back(theta);
                 m_row.push_back(y);
                 m_col.push_back(x);

            }

        }
    }
    arma::rowvec r(rhov);
    arma::rowvec t(thetav);

   return arma::join_cols(r,t);
}


// Fill a matrix witha Zernike polynomial values
arma::mat zernikeProcess::zpmC(arma::rowvec rho, arma::rowvec theta, int maxorder) {

    int m, n, n0, mmax = maxorder/2;
    unsigned int i, imax = rho.size();
    int order, nm, nm1mm1, nm1mp1, nm2m;
    int ncol = (mmax+1)*(mmax+1);
    double cosmtheta[mmax], sinmtheta[mmax];
    arma::mat zm(imax, ncol);

    m_norms[0] = 1;
    for (i=0; i<imax; i++) {

      //cache values of cos and sin
      cosmtheta[0] = std::cos(theta[i]);
      sinmtheta[0] = std::sin(theta[i]);
      for (m=1; m<mmax; m++) {
        cosmtheta[m] = cosmtheta[m-1]*cosmtheta[0] - sinmtheta[m-1]*sinmtheta[0];
        sinmtheta[m] = sinmtheta[m-1]*cosmtheta[0] + cosmtheta[m-1]*sinmtheta[0];
      }

      zm(i, 0) = 1.0;                     //piston term
      zm(i, 3) = 2. * rho[i] * rho[i] - 1.; //defocus

      // now fill in columns with m=n for n>0

      for (m=1; m <= mmax; m++) {
        zm(i, m*m) = rho[i] * zm(i, (m-1)*(m-1));
      }

      // non-symmetric terms

      for (order=4; order<=maxorder; order+=2) {
        for (m=order/2-1; m>0; m--) {
          n=order-m;
          nm = order*order/4 + n - m;
          nm1mm1 = (order-2)*(order-2)/4 + n - m;
          nm1mp1 = nm - 2;
          nm2m = nm1mm1 - 2;
          zm(i, nm) = rho[i]*(zm(i, nm1mm1) + zm(i, nm1mp1)) - zm(i, nm2m);
        }

        // m=0 (symmetric) term
        nm = order*order/4 + order;
        nm1mp1 = nm-2;
        nm2m = (order-2)*(order-2)/4+order-2;
        zm(i, nm) = 2.*rho[i]*zm(i, nm1mp1) - zm(i, nm2m);
      }

      // now multiply each column by normalizing factor and cos, sin

      n0 = 1;
      for (order=2; order <= maxorder; order+=2) {
          for(m=order/2; n0 < ncol && m>0; m--) {
              n=order-m;
              if (n0 < m_norms.size()-2){
                  m_norms[n0+1] = m_norms[n0];
                  zm(i, n0+1) = sinmtheta[m-1]*zm(i, n0);
              }
              zm(i, n0) *= cosmtheta[m-1];
              n0 += 2;
          }
          n = order;
          if (n0 < ncol) {
             m_norms[n0]= sqrt(n+1.);
          }
        n0++;
      }
    }

    return zm;
}

arma::mat zernikeProcess::zapmC(const arma::rowvec& rho, const arma::rowvec& theta, const int& maxorder) {

  unsigned int nrow = rho.size();
  int mmax = maxorder/2;
  int ncol = (mmax+1)*(mmax+1);
  int i, j, m, nj;
  double zpnorm = std::sqrt((double) nrow);
  arma::mat zm(nrow, ncol), annzm(nrow, ncol);

  //do some rudimentary error checking

//  if (rho.size() != theta.size()) stop("Numeric vectors must be same length");
//  if ((maxorder % 2) != 0) stop("maxorder must be even");

  //good enough

  zm = zpmC(rho, theta, maxorder);

  // for each azimuthal index m find the column indexes
  // of the zp matrix having that value of m. That
  // subset is what we need to orthogonalize.
  // Note this is done separately for "sine" and "cosine" components.

  nj = maxorder/2 + 1;
  for (m=0; m<mmax; m++) {
    arma::uvec jsin(nj);
    arma::uvec jcos(nj);
    arma::mat Q(nrow, nj);
    arma::mat R(nj, nj);
    arma::vec sgn(nj);
    for (i=0; i<nj; i++) {
      jcos(i) = (m+i+1)*(m+i+1) - 2*m -1;
      jsin(i) = jcos(i) + 1;
    }

    qr_econ(Q, R, zm.cols(jcos));
    sgn = sign(R.diag());

    annzm.cols(jcos) = zpnorm * Q * diagmat(sgn);
    if (m > 0) {
      qr_econ(Q, R, zm.cols(jsin));
      sgn = sign(R.diag());
      annzm.cols(jsin) = zpnorm * Q * diagmat(sgn);
    }
    --nj;
  }

  //  highest order only has one term

  j = mmax*mmax;

  annzm.col(j) = zm.col(j) * zpnorm / norm(zm.col(j));
  annzm.col(j+1) = zm.col(j+1) * zpnorm / norm(zm.col(j+1));

  return annzm;
}
void dumpArma(arma::mat mm, QString title = "", QVector<QString> colHeading = QVector<QString>(0),
              QVector<QString> RowLable = QVector<QString>(0)){
    arma::mat theMat = mm;
    QString transposed(" ");


    QString log;

    QString s;
    QTextStream out(&s);

    out << "<!DOCTYPE html><html><body><H1>" << transposed <<  title << mm.n_rows <<"X" << mm.n_cols << "</H1>";

    log.append(s);
    if (!colHeading.empty()){
        log.append("<table><tr>");
        for (int c = 0; c < colHeading.size(); ++c){
            log.append("<th>"+colHeading[c] + "</th>");
        }
        log.append("</tr>\n");
    }

    for (std::size_t row = 0; row < theMat.n_rows; ++row){
        log.append("<tr> <td> ");
        if (!RowLable.empty()){

            if (row < static_cast<std::size_t>(RowLable.size())){
                log.append(QString("<b>%1</b>").arg(RowLable[row]));
            }

        }
        log.append("</td>");

        for (int c = 0; c< 10; ++c){
            log.append(QString("<td style=\"text-align:center\" width=\"150\">%1</td>").arg(theMat(row,c), 6, 'f', 5));
        }
        log.append("</tr>\n");

    }

    log.append("</table></body></html>");
    QTextEdit *display = new QTextEdit();
    display->resize(2500,1000);
    display->insertHtml(log);

    display->show();

}
void zernikeProcess::initGrid(int width, double radius, double cx, double cy, int maxOrder,
                              double insideRad){
    // if grid or maxOrder is different then update values.
    double obsPercent = 0.;
    if (radius != 0){
        obsPercent = (double)insideRad/radius;
    }

    setMaxOrder(maxOrder);
    if ( m_needsInit || width != m_gridRowSize ||
            radius != m_radius ||
            maxOrder != m_maxOrder ||
         obsPercent != m_obsPercent){
        m_gridRowSize = width;
        m_radius = radius;
        m_obsPercent = obsPercent;
        m_rhoTheta = rhotheta(width, radius, cx, cy);

        if (obsPercent <= 0.) {
            m_zerns = zpmC(m_rhoTheta.row(0), m_rhoTheta.row(1), maxOrder);
//            for (int i = 0; i < m_zerns.n_rows; ++i){
//                if (m_row[i] == 300){
//                    qDebug()<<  "0obs" << i <<  m_row[i] << m_col [i] << m_zerns(i,1) ;
//                }
//            }
        }
        else {
            m_zerns = zapmC(m_rhoTheta.row(0), m_rhoTheta.row(1), maxOrder);
            arma::mat m_zernsaa = zpmC(m_rhoTheta.row(0), m_rhoTheta.row(1), maxOrder);

        }

    }
    m_needsInit = false;
    return;
}

// create the rho theta vectors and the Zernike values.
void zernikeProcess::initGrid(wavefront &wf, int maxOrder){
    // if grid or maxOrder is different then update values.
    initGrid(wf.data.rows, wf.m_outside.m_radius, wf.m_outside.m_center.x(),
             wf.m_outside.m_center.y(), maxOrder, wf.m_inside.m_radius);

    return;
}
void zernikeProcess::setMaxOrder(int n){

    if (m_maxOrder != n) {
        m_maxOrder = n;
        int cnt = n/2 + 1;
        cnt *= cnt;
        m_norms.resize(cnt);

        zernEnables.resize(cnt, true);
        m_needsInit = true;
    }
}


std::vector<double>  zernikeProcess::ZernFitWavefront(wavefront &wf){
    initGrid(wf, m_maxOrder);

    int ztermCnt = m_zerns.n_cols;

    cv::Mat surface = wf.data;

    Z = cv::Mat(ztermCnt,1,numType, 0.);

    /*
    'calculate LSF matrix elements
    */

    cv::Mat_<double> A;//(count,Z_TERMS);
    cv::Mat_<double> B;//(count,1);
    cv::Mat_<double> X(ztermCnt,1);

    A = cv::Mat_<double>::zeros(ztermCnt,ztermCnt);
    B = cv::Mat_<double>::zeros(ztermCnt,1);

    //calculate LSF right hand side

    QProgressDialog *prg = new QProgressDialog;
    prg->setWindowTitle(QString("fitting %1 samples to %2 zernike terms").arg(m_rhoTheta.n_cols).arg(getNumberOfTerms()));
    prg->setMaximum( m_rhoTheta.n_cols);
    prg->setValue(0);
    prg->show();
    prg->resize(1000,50);
    for (std::size_t i = 0; i < m_rhoTheta.n_cols; ++i) { // for each sample point
            double rho = m_rhoTheta.row(0)(i);


            if (i%5000 == 0) {
                prg->setValue(i);
                QApplication::processEvents();
//                if (prg->wasCanceled()){
//                    throw 42;
//                }
            }
            if ( rho <= 1. && (wf.mask.at<uchar>( m_row[i], m_col[i]) != 0)){

                for ( int zi = 0; zi < ztermCnt; ++zi)
                {
                    double t = m_zerns(i,zi);
                    //if (zi == 0)
                       // qDebug() << "z0" << t << "rho:"<<m_rhoTheta.row(0)(i);
                    for (int zj = 0; zj < ztermCnt; ++zj)
                    {
                        //Am[ndx] = Am[ndx] + t * zpolar.zernike(j, rho, theta);
                        A(zi,zj) +=  t * m_zerns(i,zj);
                    }
                    // FN is the OPD at (Xn,Yn)
                    //Bm[i] = Bm[i] + surface.at<double>(y,x) * t;
                    B(zi) +=     surface.at<double>( m_row[i], m_col[i]) * t;
                }

            }
        }


    cv::solve(A,B,X, DECOMP_QR);

    wf.InputZerns = std::vector<double>(ztermCnt,0);
    for (std::size_t z = 0;  z < static_cast<std::size_t>(X.rows); ++z){
       if (z < wf.InputZerns.size()){
            //qDebug() << z << X(z) << wf.InputZerns[z] << (QString("%1").arg(X(z) - wf.InputZerns[z], 6, 'f', 4)).toDouble();
        }
       else {
           //qDebug() << z << X(z);
       }
       wf.InputZerns[z] = X(z);
    }

    delete prg;
    std::vector<double> r;
    X.col(0).copyTo(r);
    return r;
}
void make3DPsf(cv::Mat surface){

    QWidget *plotWindow = new QWidget;
    Q3DSurface *m_PSF_3Dgraph;
    m_PSF_3Dgraph = new Q3DSurface();
    QWidget *container = QWidget::createWindowContainer(m_PSF_3Dgraph);
    QHBoxLayout *hLayout = new QHBoxLayout();
    QVBoxLayout *vLayout = new QVBoxLayout();
    hLayout->addWidget(container, 1);
    hLayout->addLayout(vLayout);
    plotWindow->setLayout(hLayout);

    cv::Mat data = surface.clone();//(cv::Rect(start,start,nx/2,nx/2));



    double xmin,xmax;
    cv::minMaxLoc(data, &xmin,&xmax);

    data/=xmax;

    QSurfaceDataProxy *m_sqrtSinProxy = new QSurfaceDataProxy();
    QSurface3DSeries *m_sqrtSinSeries = new QSurface3DSeries(m_sqrtSinProxy);

    //draw surfaced
    {

        int sampleCountX = data.size[1];
        int sampleCountZ = data.size[0];
        int width = sampleCountX;
        QSurfaceDataArray *dataArray = new QSurfaceDataArray;
        QSurfaceDataArray *dataArray2 = new QSurfaceDataArray;
        dataArray->reserve(sampleCountZ);
        dataArray2->reserve(sampleCountZ);
        for (int i = 0 ; i < sampleCountZ ; i++) {
            QSurfaceDataRow *newRow = new QSurfaceDataRow(sampleCountX);
            QSurfaceDataRow *backRow = new QSurfaceDataRow(sampleCountX);
            float z = -(sampleCountZ/2. - i);
            int index = 0;
            for (int j = 0; j < sampleCountX; j++) {
                float x = -(sampleCountX/2. -j);

                float y = data.at<double>(i,j);
                (*newRow)[index].setPosition(QVector3D(x, y, z));
                (*backRow)[index++].setPosition(QVector3D(x, y, width/2));
            }
            *dataArray << newRow;
            *dataArray2 << backRow;
        }

        QSurfaceDataProxy *m_sqrtSinProxy2 = new QSurfaceDataProxy();
        QSurface3DSeries *m_sqrtSinSeries2 = new QSurface3DSeries(m_sqrtSinProxy2);


        m_sqrtSinProxy->resetArray(dataArray);
        m_sqrtSinSeries->setDrawMode(QSurface3DSeries::DrawSurface);
        m_sqrtSinSeries->setFlatShadingEnabled(false);

        m_sqrtSinProxy2->resetArray(dataArray2);
        m_sqrtSinSeries2->setDrawMode(QSurface3DSeries::DrawSurfaceAndWireframe);
        m_sqrtSinSeries2->setFlatShadingEnabled(false);

        QLinearGradient gr;
        gr.setColorAt(0.0, Qt::darkGray);
        gr.setColorAt(0.01,Qt::cyan);
        gr.setColorAt(0.33, Qt::blue);
        gr.setColorAt(0.67, Qt::lightGray);
        gr.setColorAt(1.0, Qt::red);
    m_PSF_3Dgraph->activeTheme()->setType(Q3DTheme::Theme(3));



        m_PSF_3Dgraph->axisX()->setRange(-width/2, width/2);
        m_PSF_3Dgraph->axisZ()->setRange(-width/2, width/2);

        m_PSF_3Dgraph->addSeries(m_sqrtSinSeries);
        m_PSF_3Dgraph->addSeries(m_sqrtSinSeries2);
        m_PSF_3Dgraph->seriesList().at(0)->setBaseGradient(gr);
        m_PSF_3Dgraph->seriesList().at(0)->setColorStyle(Q3DTheme::ColorStyleRangeGradient);
    }
    plotWindow->show();
}
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "zernikesmoothingdlg.h"
using namespace cv;
void debugZernRoutines(wavefront &wf){
    int cols = 4;
    for (int i = 0; i < 10; ++i) {
        qDebug() << i << i/cols << i%cols;
    }
    return;

    zernikeProcess &zp = *zernikeProcess::get_Instance();

//    QFile inputFile("C:\\Users\\doeas\\Documents\\t.txt");
//    double xc = 487.;
//    double yc = 486.;
//    double rad = 463.;
//    if (inputFile.open(QIODevice::ReadOnly))
//    {
//        QTextStream in(&inputFile);
//        int row = 0;
//        vector<double> tmpRow;
//        vector< vector<double>> data;
//        while (!in.atEnd())
//        {
//            QString line = in.readLine();
//            QStringList fields = line.simplified().split(" ");
//            tmpRow.clear();
//            for (int i = 0; i < fields.size(); ++i){
//                if (fields[i].contains("NA")){
//                    tmpRow.push_back(0.0);
//                }
//                else {
//                    tmpRow.push_back(fields[i].toDouble());
//                }
//            }
//            data.push_back(tmpRow);
//        }
//        inputFile.close();
//        qDebug() << "data" << data.size() << data[10].size();
//        cv::Mat matd(data.size(),data[1].size(), numType,0.);
//        for(int i=0; i<matd.rows; ++i)
//            for(int j=0; j<matd.cols; ++j)
//                matd.at<double>(i, j) = data.at(i).at(j);
//        make3DPsf(matd);

//        sm->createSurfaceFromPhaseMap(matd,
//                                      CircleOutline(QPointF(xc,yc),rad),
//                                      CircleOutline(QPointF(0,0),0),
//                                      QString("Mikes wavefront"));




        for (int terms = 6; terms < 50; terms += 2) {

            zp.initGrid(wf, terms);
            zp.ZernFitWavefront(wf);
            qDebug() << "Max Order" << terms << wf.InputZerns[8]/(sqrt(5.));
        }


    }

