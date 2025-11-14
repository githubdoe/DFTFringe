#include <QDebug>
#include <opencv2/opencv.hpp>
#include "Circleoutline.h"

#if defined(_WIN32)
#include <Windows.h>
#define WIDTH 12
#define DIV 1048576ull

long showmem(const QString & /*title*/){

    //qDebug () << title;
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
         return statex.ullAvailVirtual/DIV;
    qDebug() <<QString("%1% in use.").arg(statex.dwMemoryLoad, WIDTH);

     qDebug() <<QString("%1 free  MB of physical memory.").arg(
              statex.ullAvailPhys/DIV, WIDTH);


     qDebug() <<QString("%1 free  MB of paging file.").arg(
              statex.ullAvailPageFile/DIV, WIDTH);
     qDebug() <<QString("%1 total MB of virtual memory.").arg(
              statex.ullTotalVirtual/DIV, WIDTH);
     qDebug() <<QString("%1 free  MB of virtual memory.").arg(
              statex.ullAvailVirtual/DIV, WIDTH);

     return statex.ullAvailVirtual/DIV;
}
#else
int showmem(const QString&){return 1000;}
#endif



/*
void debugshow(const cv::Mat &m) {
    // PLEASE DON'T DELETE
    // outputs do debug stream several key areas of a matrix - mainly helps figure out where the edge of the data is
    int height = m.rows;
    int width = m.cols;

    int y=height/2;
    int count_zeros=0;
    int data_index=0;
    double data[6];
    for(int x=0;x<width;x++) {
        double val = m.at<double>(y,x);
        if (val == 0.0) {
            count_zeros++;
            continue;
        }
        data[data_index++]=val;
        if (data_index>=5)
            break;
    }

    spdlog::get("logger")->debug("middle zeros: {} data {} {} {} {} {}", count_zeros, data[0], data[1], data[2], data[3], data[4]  );

    y=height/3;
    count_zeros=0;
    data_index=0;
    for(int x=0;x<width;x++) {
        double val = m.at<double>(y,x);
        if (val == 0.0) {
            count_zeros++;
            continue;
        }
        data[data_index++]=val;
        if (data_index>=5)
            break;
    }

    y--;
    count_zeros=0;
    data_index=0;
    for(int x=0;x<width;x++) {
        double val = m.at<double>(y,x);
        if (val == 0.0) {
            count_zeros++;
            continue;
        }
        data[data_index++]=val;
        if (data_index>=5)
            break;
    }

    spdlog::get("logger")->debug("third0 zeros: {} data {} {} {} {} {}", count_zeros, data[0], data[1], data[2], data[3], data[4]  );

    y--;
    count_zeros=0;
    data_index=0;
    for(int x=0;x<width;x++) {
        double val = m.at<double>(y,x);
        if (val == 0.0) {
            count_zeros++;
            continue;
        }
        data[data_index++]=val;
        if (data_index>=5)
            break;
    }

    spdlog::get("logger")->debug("third1 zeros: {} data {} {} {} {} {}", count_zeros, data[0], data[1], data[2], data[3], data[4]  );

    //
    // right side
    //

    y = height/3;
    count_zeros=0;
    for(int x=width;x>0;x--) {
        double val = m.at<double>(y,x);
        if (val == 0.0) {
            count_zeros++;
            continue;
        }
        else break;
    }

    spdlog::get("logger")->debug("third0 right side zeros: {} ", count_zeros);

    y--;
    count_zeros=0;
    for(int x=width;x>0;x--) {
        double val = m.at<double>(y,x);
        if (val == 0.0) {
            count_zeros++;
            continue;
        }
        else break;
    }

    spdlog::get("logger")->debug("third1 right side zeros: {} ", count_zeros);

    y--;
    count_zeros=0;
    for(int x=width;x>0;x--) {
        double val = m.at<double>(y,x);
        if (val == 0.0) {
            count_zeros++;
            continue;
        }
        else break;
    }

    spdlog::get("logger")->debug("third2 right side zeros: {} ", count_zeros);

    //
    // inside circle
    //

    y=height/2;
    int x=width/2;
    int min,max;
    while(x>0 && m.at<double>(y,x) == 0.0)x--;
    min = x;
    x=width/2;
    while(x<width && m.at<double>(y,x) == 0.0)x++;
    max = x;
    spdlog::get("logger")->debug("middle left/right side zeros: {} {} ", min, max);

    y=height/3;
    x=width/2;
    while(x>0 && m.at<double>(y,x) == 0.0)x--;
    min = x;
    x=width/2;
    while(x<width && m.at<double>(y,x) == 0.0)x++;
    max = x;
    spdlog::get("logger")->debug("third0 left/right side zeros: {} {} ", min, max);

    y--;
    x=width/2;
    while(x>0 && m.at<double>(y,x) == 0.0)x--;
    min = x;
    x=width/2;
    while(x<width && m.at<double>(y,x) == 0.0)x++;
    max = x;
    spdlog::get("logger")->debug("third1 left/right side zeros: {} {} ", min, max);

    y--;
    x=width/2;
    while(x>0 && m.at<double>(y,x) == 0.0)x--;
    min = x;
    x=width/2;
    while(x<width && m.at<double>(y,x) == 0.0)x++;
    max = x;
    spdlog::get("logger")->debug("third2 left/right side zeros: {} {} ", min, max);


}

*/


void CropGaussianBlur(cv::Mat in, cv::Mat out, int kernelSize, const CircleOutline &outside, const CircleOutline &center)
{
    // Warning - this function is extremely sensitive to getting the mask off by even just a few pixels out of all 360k or so.  The mask
    // must be perfect to every last pixel.  If the outer mask zeros some valid pixels we get a turned edge.  Very visible in the 3d view.
    // if it's off the other way you will see some pixels at level 0 outside of the wavefront.  Either error messes up foucault and
    // ronchi simulations

    // Right now we shrink outer outline by 2 pixels and enlarge inner outline by 1 pixel. As far as I can tell, this is an arbitrary
    // hack.  These values should at a minimum be in some constant somewhere or better, part of the outline class or maybe we should
    // get rid of these alltogether?  For now, know that if you mess with it one one place in DFTFringe, you have to mess with it
    // in other places as well.


    // first we make a mask - with ones where the data is and zeros where there is no data.  So if 0 is black it will be a white donut
    // using outside and center outlines (often center outline has radius zero so then a disk instead of a donut)
    // we also zero out the input data which probably is already zeroed but we can't take that chance particularly if there were wavefronts averaged
    // because the average feature fills in the center and outside the outer outline.

    const int height = in.rows;
    const int width = in.cols;
    cv::Mat mask = cv::Mat::ones(height,width,CV_64F);

    //
    // outer circle of mask
    //

    double cx = outside.m_center.x();
    double cy = outside.m_center.y();
    double radius2 = (outside.m_radius-2)*(outside.m_radius-2);
    for (int x=0; x<width; x++) {
        double dx = (cx-x)*(cx-x);
        for (int y=0; y<width; y++) {
            double dy = (cy-y)*(cy-y);
            if ((dx+dy) >= radius2) {
                mask.at<double>(y,x) = 1e-5; // can't use zero or we get divide by zero issues later (in cells we don't care about)
                in.at<double>(y,x) = 0;
            }
        }
    }

    //
    // inner circle of mask
    //

    cx = center.m_center.x();
    cy = center.m_center.y();
    radius2 = (center.m_radius+1)*(center.m_radius+1);
    if (center.m_radius > 0) {
        for (int x=0; x<width; x++) {
            double dx = (cx-x)*(cx-x);
            for (int y=0; y<width; y++) {
                double dy = (cy-y)*(cy-y);
                if ((dx+dy) < radius2) {
                    mask.at<double>(y,x) = 1e-5; // can't use zero or we get divide by zero issues later (in cells we don't care about)
                    in.at<double>(y,x) = 0;
                }
            }
        }
    }



    // these next 3 lines of code are brilliant and from julien asking chatgpt but I get it completely - if someone wants an explanation
    // I'll have to draw some pictures...  Although I try to explain with words farther below

    cv::GaussianBlur(in,out,cv::Size(kernelSize,kernelSize),0);
    cv::GaussianBlur(mask,mask,cv::Size(kernelSize,kernelSize),0);
    out = out / mask; // corrects for the zeroed pixels that are outside the circle (were set to zero and were in the outer mask area or center area for annular wavefronts)

    //
    // now zero out points outside of mask
    //

    //
    // outer circle of mask
    //


    cx = outside.m_center.x();
    cy = outside.m_center.y();
    radius2 = (outside.m_radius-2)*(outside.m_radius-2);

    for (int x=0; x<width; x++) {
        double dx2 = (cx-x)*(cx-x);
        for (int y=0; y<width; y++) {
            double dy2 = (cy-y)*(cy-y);
            if ((dx2+dy2) >= radius2) {
                out.at<double>(y,x) = 0;
            }
        }
    }

    //
    // inner circle of mask
    //

    cx = center.m_center.x();
    cy = center.m_center.y();
    radius2 = (center.m_radius+1)*(center.m_radius+1);
    if (center.m_radius > 0) {
        for (int x=0; x<width; x++) {
            double dx = (cx-x)*(cx-x);
            for (int y=0; y<width; y++) {
                double dy = (cy-y)*(cy-y);
                if ((dx+dy) < radius2) {
                    out.at<double>(y,x) = 0;
                }
            }
        }
    }

    // here's an attempt at an explanation of the 3 "magic" lines of code.  When you are getting the gaussian blur of a pixel near the edge you are adding
    // up a bunch of points convolved (multiplied) by the kernel shape.  But the pixels outside the circle are all zero and because the kernel is
    // normalized you normally don't have to divide by the sum of the kernel values. but you don't want to use the kernel values for points outside
    // the circle (and you sort of haven't used them because they are all multiplied by zero) but in this case you need to divide by the kernel
    // values that fall inside the circle.  So we make a mask and blur it the same amount and this gives us the proper weighting.  Points far from
    // the edge will all be set to 1 so it won't affect the weighting.  Points near the edge will have a smaller weighting value.  aka normalizing
    // value aka sum of kernel points.


}
