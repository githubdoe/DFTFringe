// written by gr@gr5.org
// gaussian blur but pays attention to borders - specifically the outer radius

#include <opencv2/opencv.hpp>

/*
    cx = wf->m_outside.m_center.rx();
    cy = wf->m_outside.m_center.ry();
    double rad = wf->m_outside.m_radius-2;
    */


void CropGaussianBlur(cv::Mat in, cv::Mat out, int kernelSize, int centerx, int centery, int radius)
{
    // it's okay if in and out are the same materix as we do calculations in a single row buffer

    // make sure kernelSize is positive and odd
    if (kernelSize < 1) kernelSize=1;
    if (kernelSize %2 == 0) kernelSize++;

    int rad2 = (radius)*(radius);
    int kernel_radius = (kernelSize-1)/2;

    cv::Mat kernel = cv::getGaussianKernel(kernelSize,0);

    // we will be using this 1 dimensional kernal matrix to do a 1 dimensional gaussian that blurrs horizontal pixels 
    // together but ignores vertical neighbors.  Then we will start over and do the vertical blurring.  Gaussian blur lets
    // you do this (separate vertical and horizontal blurring steps) which saves a LOT of time even with 3x3 blurring kernels

     int size = in.cols;
    double * onerow = new double[size];
    bool * mask = new bool[size];
    bool some_points_masked=false;

    for( int y=0; y<in.rows; y++){
        int y2 = (y-centery)*(y-centery);
        for(int x=0; x<size; x++){
            // first make the mask - true means inside the circle
            int x2 = (x-centerx)*(x-centerx);
            mask[x] = ((x2+y2) < rad2);
        }
        for( int x=0; x<size; x++){
            if (mask[x] == false) {
                // outside radius
                onerow[x] = in.at<double>(y,x);
                continue;
            }

            // check to see if all the points needed to create the gaussian blur for this pixel are available
            some_points_masked=false;
            int i = x-kernel_radius;
            if (i<0 || i>= size || mask[i]==false)
                some_points_masked=true;
            else {
                i = x+kernel_radius;
                if (i<0 || i>= size || mask[i]==false)
                    some_points_masked=true;
            }

            if (some_points_masked) {
                double kernel_sum=0;
                double sum=0;
                int kernel_index=0;
                for(int i = x-kernel_radius; i<=x+kernel_radius; i++) {
                    if (i>=0 && i< size && mask[i]) {
                        sum+= kernel.at<double>(kernel_index)*in.at<double>(y,i);
                        kernel_sum += kernel.at<double>(kernel_index);
                    }
                    kernel_index++;
                }
                onerow[x] = sum/kernel_sum;

            } else {
                // all points available
                double avg=0;
                int kernel_index=0;
                for(int i = x-kernel_radius; i<=x+kernel_radius; i++)
                    avg+= kernel.at<double>(kernel_index++)*in.at<double>(y,i);
                onerow[x] = avg;
            }
        }

        // copy points to out matrix

        for( int x=0; x<size; x++)
            out.at<double>(y,x) = onerow[x];
    }
    delete[] onerow;
    delete[] mask;

    // now we do the vertical gaussian

    size = in.rows;
    double * onecol = new double[size];
    mask = new bool[size];

    for( int x=0; x<out.cols; x++){
        int x2 = (x-centerx)*(x-centerx);
        for( int y=0; y<size; y++){
            // first make the mask - true means inside the circle
            int y2 = (y-centery)*(y-centery);
            mask[y] = ((x2+y2) < rad2);
        }
        for( int y=0; y<size; y++){
            if (mask[y] == false) {
                // outside radius
                onecol[y] = out.at<double>(y,x);
                continue;
            }

            // check to see if all the points needed to create the gaussian blur for this pixel are available
            some_points_masked=false;
            int i = y-kernel_radius;
            if (i<0 || i>= size || mask[i]==false)
                some_points_masked=true;
            else {
                i = y+kernel_radius;
                if (i<0 || i>= size || mask[i]==false)
                    some_points_masked=true;
            }

            if (some_points_masked) {
                double kernel_sum=0;
                double sum=0;
                int kernel_index=0;
                for(int i = y-kernel_radius; i<=y+kernel_radius; i++) {
                    if (i>=0 && i< size && mask[i]) {
                        sum+= kernel.at<double>(kernel_index) * out.at<double>(i,x);
                        kernel_sum += kernel.at<double>(kernel_index);
                    }
                    kernel_index++;
                }
                onecol[y] = sum/kernel_sum;

            } else {
                // all points available
                double avg=0;
                int kernel_index=0;
                for(int i = y-kernel_radius; i<=y+kernel_radius; i++)
                    avg+= kernel.at<double>(kernel_index++) * out.at<double>(i,x);
                onecol[y] = avg;
            }
        }

        // copy points to out matrix

        for(int y=0; y<size; y++)
            out.at<double>(y,x) = onecol[y];
    }

    delete[] onecol;
    delete[] mask;

}
