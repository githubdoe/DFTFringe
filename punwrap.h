#ifndef PUNWRAP_H
#define PUNWRAP_H
#include "opencv/cv.h"
#include "opencv/highgui.h"
void unwrap(double *pphase, double *unwrapped, char *mask, int nx, int ny);


#define BORDER      0x1
#define UNWRAPPED   0x2

#define AVOID (BORDER | UNWRAPPED)
#endif
