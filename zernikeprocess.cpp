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
#include <opencv/cv.h>
#include <cmath>
#include "mainwindow.h"
#include <QDebug>
#include "surfaceanalysistools.h"
#include "simigramdlg.h"
#include "settings2.h"
#include "myutils.h"
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
    int irow;
    int icol;
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
    QObject(parent),m_dirty_zerns(true)
{
    md = mirrorDlg::get_Instance();;
}

// compute zernikes from unwrapped surface
#define SAMPLE_WIDTH 1
void zernikeProcess::unwrap_to_zernikes(wavefront &wf)
{
    int nx = wf.data.cols;
    int ny = wf.data.rows;

    cv::Mat surface = wf.data;

    if (!m_dirty_zerns)
        return;

    Z = cv::Mat(Z_TERMS,1,numType, 0.);

    /*
    'calculate LSF matrix elements
    */

    Settings2 &settings = *Settings2::getInstance();
    bool useSvd = false;
    if (settings.m_general->useSVD()){
        useSvd = true;
    }

    cv::Mat_<double> A;//(count,Z_TERMS);
    cv::Mat_<double> B;//(count,1);
    cv::Mat_<double> X(Z_TERMS,1);
    int count = 0;
    if (useSvd){
        count = cv::countNonZero(wf.workMask);
        A = cv::Mat_<double>::zeros(count,Z_TERMS);
        B = cv::Mat_<double>::zeros(count,1);
    }
    else {
        A = cv::Mat_<double>::zeros(Z_TERMS,Z_TERMS);
        B = cv::Mat_<double>::zeros(Z_TERMS,1);
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
                for ( int i = 0; i < Z_TERMS; ++i)
                {

                    if (useSvd){
                        double t = zpolar.zernike(i, rho, theta);
                        A(sampleCnt,i) = t;
                    }
                    else {

                        double t = zpolar.zernike(i, rho, theta);
                        for (int j = 0; j < Z_TERMS; ++j)
                        {
                            //Am[ndx] = Am[ndx] + t * zpolar.zernike(j, rho, theta);
                            A(i,j) +=  t * zpolar.zernike(j, rho, theta);
                        }
                        // FN is the OPD at (Xn,Yn)
                        //Bm[i] = Bm[i] + surface.at<double>(y,x) * t;
                        B(i) += surface.at<double>(y,x) * t;
                    }

                }
                if (useSvd){
                    B(sampleCnt++) = surface.at<double>(y,x);
                    if (sampleCnt > count){
                        QMessageBox::warning(0,"Critical Error", QString().sprintf("Zernike computation sampleCnt > count %d %d",sampleCnt,count));
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
        emit statusBarUpdate(QString().sprintf(" Zernike LSF matrix Condition Numbers %6.3lf %6.3lf", conditionNumber, c2),1);
    }
    wf.InputZerns = std::vector<double>(Z_TERMS,0);
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
    if (doDefocus)
        defocus = surfaceAnalysisTools::get_Instance()->m_defocus;
    double ux,uy,sz,nz;
    double rho,theta;
    zernikePolar &zpolar = *zernikePolar::get_Instance();
    for(int  y = 0; y < ny; ++y)
    {
        for(int x = 0; x < nx; ++x)
        {

            if (mask.at<bool>(y,x) != 0 && wf.data.at<double>(y,x) != 0.0)
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
                    if ((z == 3) & doDefocus)
                        nz += defocus * zpolar.zernike(z,rho, theta);

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
    double ux,uy;
    double rho,theta;

    zernikePolar &zpolar = *zernikePolar::get_Instance();

    if (wf.regions.size() > 0){
        int startx = 9999;
        int endx = 0;
        int starty = 9999;
        int endy = 0;
        for (int n = 0; n < wf.regions.size(); ++n){
            for (int i = 0; i < wf.regions[n].size(); ++i){
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
                    if (wf.mask.at<bool>(y,x) == 0.){
                        ux = (double)(x - midx)/rad;
                        uy = (double)(y - midy)/rad;
                        rho = sqrt(ux * ux + uy * uy);
                        theta = atan2(uy,ux);
                        zpolar.init(rho,theta);
                        double v = 0.;

                        for (int z = 0; z < Z_TERMS; ++z){
                            v += wf.InputZerns[z] * zpolar.zernike(z,rho, theta);
                        }

                        wf.data.at<double>(y,x) = v;
                    }
                }
            }
        }
    }
}

cv::Mat makeSurfaceFromZerns(int border, bool doColor){
    simIgramDlg &dlg = *simIgramDlg::get_instance();
    int wx = dlg.size + 2 *  border;
    int wy = wx;
    double rad = (double)(wx-1)/2.;
    double xcen = rad,ycen = rad;
    rad -= border;
    cv::Mat result = cv::Mat::zeros(wx,wx, (doColor)? CV_8UC4: numType);

    double rho;
    double spacing = 1.;
    mirrorDlg *md = mirrorDlg::get_Instance();
    zernikePolar &zpolar = *zernikePolar::get_Instance();
    for (int y = 0; y <  wx; ++y)
    {
        double uy = (double)(y - (ycen)) / rad;
        for (int x = 0; x < wy; ++x)
        {
            double ux = (double)(x - (xcen )) /rad;
            rho = sqrt(ux * ux + uy * uy);

            if (rho <= 1.)
            {
                double theta = atan2(uy,ux);
                zpolar.init(rho,theta);
                double S1 =
                        dlg.star * cos(10.  *  theta) +
                        dlg.ring * cos(10 * 2. * rho);

                for (unsigned int z = 0; z < dlg.zernikes.size(); ++z){
                    double val = dlg.zernikes[z];
                    if (z == 8){
                       val = (dlg.doCorrection) ? md->cc * md->z8 * val * .01 : val;
                    }
                    S1 +=  val * zpolar.zernike(z,rho,theta);
                }

                if (doColor){
                    int iv = cos(spacing *2 * M_PI * S1) * 100 + 120;
                    result.at<Vec4b>(y,x)[2] = iv;
                    result.at<Vec4b>(y,x)[3] = 255;
                }
                else {
                    if (S1 == 0.0) S1 += .0000001;
                    result.at<double>(y,x) = S1;
                }
            }
            else    // outside mirror outline
            {
                if (doColor){
                    result.at<Vec4b>(y,x) = Vec4f(0.,00.,100.,0);
                }
                else {
                    result.at<double>(y,x) = 0 ;
                }
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
            if (mask.at<bool>(x,y))
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
