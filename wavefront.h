#ifndef WAVEFRONT_H
#define WAVEFRONT_H
#include "opencv/cv.h"
#include "circleoutline.h"

class wavefront
{
public:
    wavefront();
    ~wavefront();
    cv::Mat data;
    cv::Mat smoothedData;
    cv::Mat nulledData;
    cv::Mat mask;
    cv::Mat workData;
    cv::Mat workMask;
    cv::Mat zernSurface;
    std::vector<double> InputZerns;
    uint lastZernEnableUpdate;
    double contourDelta;
    double gaussian_diameter;
    bool useGBSmoothing;
    int GBSmoothingValue;

    QString name;
    double lambda;
    CircleOutline m_outside;
    CircleOutline m_inside;
    double diameter;

    double roc;
    double min;
    double max;
    double std;
    double mean;
    bool dirtyZerns;
    bool computed;

};

#endif // WAVEFRONT_H
