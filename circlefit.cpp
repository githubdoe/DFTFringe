#include "circleutils.h"
#include "circle.h"
double pythag(double a, double b)
{
    double absa=abs(a),	absb=abs(b);
    if (absa > absb) return absa*sqrt(One+SQR(absb/absa));
    else return (absb == 0.0 ? 0.0 : absb*sqrt(One+SQR(absa/absb)));
}


/************************************************************************
            BODY OF THE MEMBER ROUTINES
************************************************************************/
// Default constructor

Circle::Circle()
{
    a=0.; b=0.; r=1.; s=0.; i=0; j=0;
}

// Constructor with assignment of the circle parameters only

Circle::Circle(double aa, double bb, double rr)
{
    a=aa; b=bb; r=rr;
}

// Printing routine

void Circle::print(void)
{
    cout << endl;
    cout << setprecision(10) << "center (" <<a <<","<< b <<")  radius "
         << r << "  sigma " << s << "  gradient " << g << "  iter "<< i << "  inner " << j << endl;
}

//****************** Sigma ************************************
//
//   estimate of Sigma = square root of RSS divided by N
//   gives the root-mean-square error of the geometric circle fit


//
//						 data.h
//

/************************************************************************
            DECLARATION OF THE CLASS DATA
************************************************************************/
// Class for Data
// A data has 5 fields:
//       n (of type int), the number of data points
//       X and Y (arrays of type double), arrays of x- and y-coordinates
//       meanX and meanY (of type double), coordinates of the centroid (x and y sample means)




/************************************************************************
            BODY OF THE MEMBER ROUTINES
************************************************************************/
// Default constructor
CircleData::CircleData()
{
    n=0;
    X = new double[n];
    Y = new double[n];
    for (int i=0; i<n; i++)
    {
        X[i]=0.;
        Y[i]=0.;
    }
}

// Constructor with assignment of the field N
CircleData::CircleData(int N)
{
    n=N;
    X = new double[n];
    Y = new double[n];

    for (int i=0; i<n; i++)
    {
        X[i]=0.;
        Y[i]=0.;
    }
}

// Constructor with assignment of each field
CircleData::CircleData(int N, double dataX[], double dataY[])
{
    n=N;
    X = new double[n];
    Y = new double[n];

    for (int i=0; i<n; i++)
    {
        X[i]=dataX[i];
        Y[i]=dataY[i];
    }
}

// Routine that computes the x- and y- sample means (the coordinates of the centeroid)

void CircleData::means(void)
{
    meanX=0.; meanY=0.;

    for (int i=0; i<n; i++)
    {
        meanX += X[i];
        meanY += Y[i];
    }
    meanX /= n;
    meanY /= n;
}

// Routine that centers the data set (shifts the coordinates to the centeroid)

void CircleData::center(void)
{
    double sX=0.,sY=0.;
    int i;

    for (i=0; i<n; i++)
    {
        sX += X[i];
        sY += Y[i];
    }
    sX /= n;
    sY /= n;

    for (i=0; i<n; i++)
    {
        X[i] -= sX;
        Y[i] -= sY;
    }
    meanX = 0.;
    meanY = 0.;
}

// Routine that scales the coordinates (makes them of order one)

void CircleData::scale(void)
{
    double sXX=0.,sYY=0.,scaling;
    int i;

    for (i=0; i<n; i++)
    {
        sXX += X[i]*X[i];
        sYY += Y[i]*Y[i];
    }
    scaling = sqrt((sXX+sYY)/n/Two);

    for (i=0; i<n; i++)
    {
        X[i] /= scaling;
        Y[i] /= scaling;
    }
}



// Destructor
CircleData::~CircleData()
{
    delete[] X;
        delete[] Y;
}

double Sigma (CircleData& data, Circle& circle)
{
    double sum=0.,dx,dy;

    for (int i=0; i<data.n; i++)
    {
        dx = data.X[i] - circle.a;
        dy = data.Y[i] - circle.b;
        sum += SQR(sqrt(dx*dx+dy*dy) - circle.r);
    }
    return sqrt(sum/data.n);
}

Circle CircleFitByHyper (CircleData& data)
/*
      Circle fit to a given set of data points (in 2D)

      This is an algebraic fit based on the journal article

      A. Al-Sharadqah and N. Chernov, "Error analysis for circle fitting algorithms",
      Electronic Journal of Statistics, Vol. 3, pages 886-911, (2009)

      It is an algebraic circle fit with "hyperaccuracy" (with zero essential bias).
      The term "hyperaccuracy" first appeared in papers by Kenichi Kanatani around 2006

      Input:  data     - the class of data (contains the given points):

          data.n   - the number of data points
          data.X[] - the array of X-coordinates
          data.Y[] - the array of Y-coordinates

     Output:
               circle - parameters of the fitting circle:

           circle.a - the X-coordinate of the center of the fitting circle
           circle.b - the Y-coordinate of the center of the fitting circle
           circle.r - the radius of the fitting circle
           circle.s - the root mean square error (the estimate of sigma)
           circle.j - the total number of iterations

     This method combines the Pratt and Taubin fits to eliminate the essential bias.

     It works well whether data points are sampled along an entire circle or
     along a small arc.

     Its statistical accuracy is theoretically higher than that of the Pratt fit
     and Taubin fit, but practically they all return almost identical circles
     (unlike the Kasa fit that may be grossly inaccurate).

     It provides a very good initial guess for a subsequent geometric fit.

       Nikolai Chernov  (September 2012)

*/
{
    int i,iter,IterMAX=99;

    double Xi,Yi,Zi;
    double Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Cov_xy,Var_z;
    double A0,A1,A2,A22;
    double Dy,xnew,x,ynew,y;
    double DET,Xcenter,Ycenter;

    Circle circle;

    data.means();   // Compute x- and y- sample means (via a function in the class "data")

//     computing moments

    Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;

    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;

        Mxy += Xi*Yi;
        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
        Mzz += Zi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;
    Mzz /= data.n;

//    computing the coefficients of the characteristic polynomial

    Mz = Mxx + Myy;
    Cov_xy = Mxx*Myy - Mxy*Mxy;
    Var_z = Mzz - Mz*Mz;

    A2 = Four*Cov_xy - Three*Mz*Mz - Mzz;
    A1 = Var_z*Mz + Four*Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
    A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
    A22 = A2 + A2;

//    finding the root of the characteristic polynomial
//    using Newton's method starting at x=0
//     (it is guaranteed to converge to the right root)

    for (x=0.,y=A0,iter=0; iter<IterMAX; iter++)  // usually, 4-6 iterations are enough
    {
        Dy = A1 + x*(A22 + 16.*x*x);
        xnew = x - y/Dy;
        if ((xnew == x)||(!isfinite(xnew))) break;
        ynew = A0 + xnew*(A1 + xnew*(A2 + Four*xnew*xnew));
        if (abs(ynew)>=abs(y))  break;
        x = xnew;  y = ynew;
    }

//    computing paramters of the fitting circle

    DET = x*x - x*Mz + Cov_xy;
    Xcenter = (Mxz*(Myy - x) - Myz*Mxy)/DET/Two;
    Ycenter = (Myz*(Mxx - x) - Mxz*Mxy)/DET/Two;

//       assembling the output

    circle.a = Xcenter + data.meanX;
    circle.b = Ycenter + data.meanY;
    circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz - x - x);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = iter;  //  return the number of iterations, too

    return circle;
}

Circle CircleFitByKasa (CircleData &data)
/*
      Circle fit to a given set of data points (in 2D)

      This is an algebraic fit, disovered and rediscovered by many people.
      One of the earliest publications is due to Kasa:

      I. Kasa, "A curve fitting procedure and its error analysis",
      IEEE Trans. Inst. Meas., Vol. 25, pages 8-14, (1976)

      Input:  data     - the class of data (contains the given points):

          data.n   - the number of data points
          data.X[] - the array of X-coordinates
          data.Y[] - the array of Y-coordinates

     Output:
               circle - parameters of the fitting circle:

           circle.a - the X-coordinate of the center of the fitting circle
           circle.b - the Y-coordinate of the center of the fitting circle
           circle.r - the radius of the fitting circle
           circle.s - the root mean square error (the estimate of sigma)
           circle.j - the total number of iterations

     The method is based on the minimization of the function

                 F = sum [(x-a)^2 + (y-b)^2 - R^2]^2

     This is perhaps the simplest and fastest circle fit.

     It works well when data points are sampled along an entire circle
     or a large part of it (at least half circle).

     It does not work well when data points are sampled along a small arc
     of a circle. In that case the method is heavily biased, it returns
     circles that are too often too small.

     It is the oldest algebraic circle fit (first published in 1972?).
     For 20-30 years it has been the most popular circle fit, at least
     until the more robust Pratt fit (1987) and Taubin fit (1991) were invented.

       Nikolai Chernov  (September 2012)
*/
{
    int i;

    double Xi,Yi,Zi;
    double Mxy,Mxx,Myy,Mxz,Myz;
    double B,C,G11,G12,G22,D1,D2;

    Circle circle;

    data.means();   // Compute x- and y- sample means (via a function in the class "data")

//     computing moments

    Mxx=Myy=Mxy=Mxz=Myz=0.;

    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;

        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxy += Xi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;

//    solving system of equations by Cholesky factorization

    G11 = sqrt(Mxx);
    G12 = Mxy/G11;
    G22 = sqrt(Myy - G12*G12);

    D1 = Mxz/G11;
    D2 = (Myz - D1*G12)/G22;

//    computing paramters of the fitting circle

    C = D2/G22/Two;
    B = (D1 - G12*C)/G11/Two;

//       assembling the output

    circle.a = B + data.meanX;
    circle.b = C + data.meanY;
    circle.r = sqrt(B*B + C*C + Mxx + Myy);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = 0;

    return circle;
}

Circle CircleFitByPratt (CircleData &data)
/*
      Circle fit to a given set of data points (in 2D)

      This is an algebraic fit, due to Pratt, based on the journal article

      V. Pratt, "Direct least-squares fitting of algebraic surfaces",
      Computer Graphics, Vol. 21, pages 145-152 (1987)

      Input:  data     - the class of data (contains the given points):

          data.n   - the number of data points
          data.X[] - the array of X-coordinates
          data.Y[] - the array of Y-coordinates

     Output:
               circle - parameters of the fitting circle:

           circle.a - the X-coordinate of the center of the fitting circle
           circle.b - the Y-coordinate of the center of the fitting circle
           circle.r - the radius of the fitting circle
           circle.s - the root mean square error (the estimate of sigma)
           circle.j - the total number of iterations

     The method is based on the minimization of the function

                 F = sum [(x-a)^2 + (y-b)^2 - R^2]^2 / R^2

     This method is more balanced than the simple Kasa fit.

     It works well whether data points are sampled along an entire circle or
     along a small arc.

     It still has a small bias and its statistical accuracy is slightly
     lower than that of the geometric fit (minimizing geometric distances).

     It provides a good initial guess for a subsequent geometric fit.

       Nikolai Chernov  (September 2012)

*/
{
    int i,iter,IterMAX=99;

    double Xi,Yi,Zi;
    double Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Cov_xy,Var_z;
    double A0,A1,A2,A22;
    double Dy,xnew,x,ynew,y;
    double DET,Xcenter,Ycenter;

    Circle circle;

    data.means();   // Compute x- and y- sample means (via a function in the class "data")

//     computing moments

    Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;

    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;

        Mxy += Xi*Yi;
        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
        Mzz += Zi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;
    Mzz /= data.n;

//    computing coefficients of the characteristic polynomial

    Mz = Mxx + Myy;
    Cov_xy = Mxx*Myy - Mxy*Mxy;
    Var_z = Mzz - Mz*Mz;

    A2 = Four*Cov_xy - Three*Mz*Mz - Mzz;
    A1 = Var_z*Mz + Four*Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
    A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
    A22 = A2 + A2;

//    finding the root of the characteristic polynomial
//    using Newton's method starting at x=0
//     (it is guaranteed to converge to the right root)

    for (x=0.,y=A0,iter=0; iter<IterMAX; iter++)  // usually, 4-6 iterations are enough
    {
        Dy = A1 + x*(A22 + 16.*x*x);
        xnew = x - y/Dy;
        if ((xnew == x)||(!isfinite(xnew))) break;
        ynew = A0 + xnew*(A1 + xnew*(A2 + Four*xnew*xnew));
        if (abs(ynew)>=abs(y))  break;
        x = xnew;  y = ynew;
    }

//    computing paramters of the fitting circle

    DET = x*x - x*Mz + Cov_xy;
    Xcenter = (Mxz*(Myy - x) - Myz*Mxy)/DET/Two;
    Ycenter = (Myz*(Mxx - x) - Mxz*Mxy)/DET/Two;

//       assembling the output

    circle.a = Xcenter + data.meanX;
    circle.b = Ycenter + data.meanY;
    circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz + x + x);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = iter;  //  return the number of iterations, too

    return circle;
}

Circle CircleFitByTaubin (CircleData& data)
/*
      Circle fit to a given set of data points (in 2D)

      This is an algebraic fit, due to Taubin, based on the journal article

      G. Taubin, "Estimation Of Planar Curves, Surfaces And Nonplanar
                  Space Curves Defined By Implicit Equations, With
                  Applications To Edge And Range Image Segmentation",
                  IEEE Trans. PAMI, Vol. 13, pages 1115-1138, (1991)

      Input:  data     - the class of data (contains the given points):

          data.n   - the number of data points
          data.X[] - the array of X-coordinates
          data.Y[] - the array of Y-coordinates

     Output:
               circle - parameters of the fitting circle:

           circle.a - the X-coordinate of the center of the fitting circle
           circle.b - the Y-coordinate of the center of the fitting circle
           circle.r - the radius of the fitting circle
           circle.s - the root mean square error (the estimate of sigma)
           circle.j - the total number of iterations

     The method is based on the minimization of the function

                  sum [(x-a)^2 + (y-b)^2 - R^2]^2
              F = -------------------------------
                      sum [(x-a)^2 + (y-b)^2]

     This method is more balanced than the simple Kasa fit.

     It works well whether data points are sampled along an entire circle or
     along a small arc.

     It still has a small bias and its statistical accuracy is slightly
     lower than that of the geometric fit (minimizing geometric distances),
     but slightly higher than that of the very similar Pratt fit.
     Besides, the Taubin fit is slightly simpler than the Pratt fit

     It provides a very good initial guess for a subsequent geometric fit.

       Nikolai Chernov  (September 2012)

*/
{
    int i,iter,IterMAX=99;

    double Xi,Yi,Zi;
    double Mz,Mxy,Mxx,Myy,Mxz,Myz,Mzz,Cov_xy,Var_z;
    double A0,A1,A2,A22,A3,A33;
    double Dy,xnew,x,ynew,y;
    double DET,Xcenter,Ycenter;

    Circle circle;

    data.means();   // Compute x- and y- sample means (via a function in the class "data")

//     computing moments

    Mxx=Myy=Mxy=Mxz=Myz=Mzz=0.;

    for (i=0; i<data.n; i++)
    {
        Xi = data.X[i] - data.meanX;   //  centered x-coordinates
        Yi = data.Y[i] - data.meanY;   //  centered y-coordinates
        Zi = Xi*Xi + Yi*Yi;

        Mxy += Xi*Yi;
        Mxx += Xi*Xi;
        Myy += Yi*Yi;
        Mxz += Xi*Zi;
        Myz += Yi*Zi;
        Mzz += Zi*Zi;
    }
    Mxx /= data.n;
    Myy /= data.n;
    Mxy /= data.n;
    Mxz /= data.n;
    Myz /= data.n;
    Mzz /= data.n;

//      computing coefficients of the characteristic polynomial

    Mz = Mxx + Myy;
    Cov_xy = Mxx*Myy - Mxy*Mxy;
    Var_z = Mzz - Mz*Mz;
    A3 = Four*Mz;
    A2 = -Three*Mz*Mz - Mzz;
    A1 = Var_z*Mz + Four*Cov_xy*Mz - Mxz*Mxz - Myz*Myz;
    A0 = Mxz*(Mxz*Myy - Myz*Mxy) + Myz*(Myz*Mxx - Mxz*Mxy) - Var_z*Cov_xy;
    A22 = A2 + A2;
    A33 = A3 + A3 + A3;

//    finding the root of the characteristic polynomial
//    using Newton's method starting at x=0
//     (it is guaranteed to converge to the right root)

    for (x=0.,y=A0,iter=0; iter<IterMAX; iter++)  // usually, 4-6 iterations are enough
    {
            Dy = A1 + x*(A22 + A33*x);
        xnew = x - y/Dy;
        if ((xnew == x)||(!isfinite(xnew))) break;
        ynew = A0 + xnew*(A1 + xnew*(A2 + xnew*A3));
        if (abs(ynew)>=abs(y))  break;
        x = xnew;  y = ynew;
    }

//       computing paramters of the fitting circle

    DET = x*x - x*Mz + Cov_xy;
    Xcenter = (Mxz*(Myy - x) - Myz*Mxy)/DET/Two;
    Ycenter = (Myz*(Mxx - x) - Mxz*Mxy)/DET/Two;

//       assembling the output

    circle.a = Xcenter + data.meanX;
    circle.b = Ycenter + data.meanY;
    circle.r = sqrt(Xcenter*Xcenter + Ycenter*Ycenter + Mz);
    circle.s = Sigma(data,circle);
    circle.i = 0;
    circle.j = iter;  //  return the number of iterations, too

    return circle;
}


int CircleFitByLevenbergMarquardtFull (CircleData& data, Circle& circleIni, double LambdaIni, Circle& circle)
/*                                     <------------------ Input ------------------->  <-- Output -->

       Geometric circle fit to a given set of data points (in 2D)

       Input:  data     - the class of data (contains the given points):

           data.n   - the number of data points
           data.X[] - the array of X-coordinates
           data.Y[] - the array of Y-coordinates

               circleIni - parameters of the initial circle ("initial guess")

           circleIni.a - the X-coordinate of the center of the initial circle
           circleIni.b - the Y-coordinate of the center of the initial circle
           circleIni.r - the radius of the initial circle

           LambdaIni - the initial value of the control parameter "lambda"
                       for the Levenberg-Marquardt procedure
                       (common choice is a small positive number, e.g. 0.001)

       Output:
           integer function value is a code:
                      0:  normal termination, the best fitting circle is
                          successfully found
                      1:  the number of outer iterations exceeds the limit (99)
                          (indicator of a possible divergence)
                      2:  the number of inner iterations exceeds the limit (99)
                          (another indicator of a possible divergence)
                      3:  the coordinates of the center are too large
                          (a strong indicator of divergence)

           circle - parameters of the fitting circle ("best fit")

           circle.a - the X-coordinate of the center of the fitting circle
           circle.b - the Y-coordinate of the center of the fitting circle
           circle.r - the radius of the fitting circle
           circle.s - the root mean square error (the estimate of sigma)
           circle.i - the total number of outer iterations (updating the parameters)
           circle.j - the total number of inner iterations (adjusting lambda)

       Algorithm:  Levenberg-Marquardt running over the full parameter space (a,b,r)

       See a detailed description in Section 4.5 of the book by Nikolai Chernov:
       "Circular and linear regression: Fitting circles and lines by least squares"
       Chapman & Hall/CRC, Monographs on Statistics and Applied Probability, volume 117, 2010.

        Nikolai Chernov,  February 2014
*/
{
    int code,i,iter,inner,IterMAX=99;

    double factorUp=10.,factorDown=0.04,lambda,ParLimit=1.e+6;
    double dx,dy,ri,u,v;
    double Mu,Mv,Muu,Mvv,Muv,Mr,UUl,VVl,Nl,F1,F2,F3,dX,dY,dR;
    double epsilon=3.e-8;
    double G11,G22,G33,G12,G13,G23,D1,D2,D3;

    Circle Old,New;

//       starting with the given initial circle (initial guess)

    New = circleIni;

//       compute the root-mean-square error via function Sigma; see Utilities.cpp

    New.s = Sigma(data,New);

//       initializing lambda, iteration counters, and the exit code

    lambda = LambdaIni;
    iter = inner = code = 0;

NextIteration:

    Old = New;
    if (++iter > IterMAX) {code = 1;  goto enough;}

//       computing moments

    Mu=Mv=Muu=Mvv=Muv=Mr=0.;

    for (i=0; i<data.n; i++)
    {
        dx = data.X[i] - Old.a;
        dy = data.Y[i] - Old.b;
        ri = sqrt(dx*dx + dy*dy);
        u = dx/ri;
        v = dy/ri;
        Mu += u;
        Mv += v;
        Muu += u*u;
        Mvv += v*v;
        Muv += u*v;
        Mr += ri;
    }
    Mu /= data.n;
    Mv /= data.n;
    Muu /= data.n;
    Mvv /= data.n;
    Muv /= data.n;
    Mr /= data.n;

//       computing matrices

    F1 = Old.a + Old.r*Mu - data.meanX;
    F2 = Old.b + Old.r*Mv - data.meanY;
    F3 = Old.r - Mr;

    Old.g = New.g = sqrt(F1*F1 + F2*F2 + F3*F3);

try_again:

    UUl = Muu + lambda;
    VVl = Mvv + lambda;
    Nl = One + lambda;

//         Cholesly decomposition

    G11 = sqrt(UUl);
    G12 = Muv/G11;
    G13 = Mu/G11;
    G22 = sqrt(VVl - G12*G12);
    G23 = (Mv - G12*G13)/G22;
    G33 = sqrt(Nl - G13*G13 - G23*G23);

    D1 = F1/G11;
    D2 = (F2 - G12*D1)/G22;
    D3 = (F3 - G13*D1 - G23*D2)/G33;

    dR = D3/G33;
    dY = (D2 - G23*dR)/G22;
    dX = (D1 - G12*dY - G13*dR)/G11;

    if ((abs(dR)+abs(dX)+abs(dY))/(One+Old.r) < epsilon) goto enough;

//       updating the parameters

    New.a = Old.a - dX;
    New.b = Old.b - dY;

    if (abs(New.a)>ParLimit || abs(New.b)>ParLimit) {code = 3; goto enough;}

    New.r = Old.r - dR;

    if (New.r <= 0.)
    {
        lambda *= factorUp;
        if (++inner > IterMAX) {code = 2;  goto enough;}
        goto try_again;
    }

//       compute the root-mean-square error via function Sigma; see Utilities.cpp

    New.s = Sigma(data,New);

//       check if improvement is gained

    if (New.s < Old.s)    //   yes, improvement
    {
        lambda *= factorDown;
        goto NextIteration;
    }
    else                       //   no improvement
    {
        if (++inner > IterMAX) {code = 2;  goto enough;}
        lambda *= factorUp;
        goto try_again;
    }

    //       exit

enough:

    Old.i = iter;    // total number of outer iterations (updating the parameters)
    Old.j = inner;   // total number of inner iterations (adjusting lambda)

    circle = Old;

    return code;
}
