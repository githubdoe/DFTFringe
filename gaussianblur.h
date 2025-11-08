#include <opencv2/opencv.hpp>

/*
    cx = wf->m_outside.m_center.rx();
    cy = wf->m_outside.m_center.ry();
    double rad = wf->m_outside.m_radius-2;
    */


void CropGaussianBlur(cv::Mat in, cv::Mat out, int kernelSize, int centerx, int centery, int radius);
