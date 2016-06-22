/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#ifndef WAVEFRONT_H
#define WAVEFRONT_H
#include "opencv/cv.h"
#include "circleoutline.h"

class wavefront
{
public:
    wavefront();
    ~wavefront();
    wavefront(const wavefront &wf);
    cv::Mat_<double> data;
    cv::Mat_<double> nulledData;
    cv::Mat_<bool> mask;
    cv::Mat_<double> workData;
    cv::Mat_<bool> workMask;
    std::vector<double> InputZerns;
    double gaussian_diameter;
    bool wasSmoothed;
    bool useSANull;
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


};

#endif // WAVEFRONT_H
