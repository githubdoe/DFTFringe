#ifndef VORTEX_H
#define VORTEX_H
#include <opencv/cv.h>

void vortex(cv::Mat &dft, const char *ext,
           double low, double smooth,
           double *phase, const char *what);
#endif // VORTEX_H
