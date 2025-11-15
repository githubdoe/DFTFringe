#ifndef UTILS_H
#define UTILS_H
int showmem(const QString &title = "");
extern double outputLambda; //TODO nothing to do here. But there are many other occurences
#include <opencv2/opencv.hpp>
void CropGaussianBlur(const cv::Mat &in, cv::Mat &out, int kernelSize, const CircleOutline &outside, const CircleOutline &center);
//void debugshow(cv::Mat m);
#endif // UTILS_H
