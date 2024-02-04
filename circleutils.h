#ifndef CIRCLEUTILS_H
#define CIRCLEUTILS_H
#include <iostream>
#include <cmath>
#include <limits>
#include <iomanip>
#include <cstdlib>
#include "circle.h"

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
#endif // CIRCLEUTILS_H
