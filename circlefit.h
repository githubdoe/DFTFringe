#ifndef CIRCLEFIT_H
#define CIRCLEFIT_H
#include <iostream>
#include <cmath>
#include <limits>
#include <iomanip>
#include <cstdlib>

/************************************************************************
            DECLARATION OF THE CLASS CIRCLE
************************************************************************/
// Class for Circle
// A circle has 7 fields:
//     a, b, r (of type reals), the circle parameters
//     s (of type reals), the estimate of sigma (standard deviation)
//     g (of type reals), the norm of the gradient of the objective function
//     i and j (of type int), the iteration counters (outer and inner, respectively)

class Circle
{
public:

    // The fields of a Circle
    double a, b, r, s, g, Gx, Gy;
    int i, j;

    // constructors
    Circle();
    Circle(double aa, double bb, double rr);

    // routines
    void print(void);

    // no destructor we didn't allocate memory by hand.
};


class CircleData
{
public:

    int n;
    double *X;		//space is allocated in the constructors
    double *Y;		//space is allocated in the constructors
    double meanX, meanY;

    // constructors
    CircleData();
    CircleData(int N);
    CircleData(int N, const double X[], const double Y[]);

    // routines
    void means(void);
    void center(void);
    void scale(void);
    void print(void);

    // destructors
    ~CircleData();
};


//   next define some frequently used constants:

const double One=1.0,Two=2.0,Three=3.0,Four=4.0;


//   next define some frequently used functions:

template<typename T>
inline T SQR(T t) { return t*t; };

Circle CircleFitByHyper (CircleData& data);
Circle CircleFitByKasa (CircleData& data);
Circle CircleFitByPratt (CircleData& data);
Circle CircleFitByTaubin (CircleData& data);
int CircleFitByLevenbergMarquardtFull (const CircleData& data, const Circle& circleIni, double LambdaIni, Circle& circle);

#endif // CIRCLEFIT_H
