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
#include <opencv2/opencv.hpp>
#include "Circleoutline.h"
#include <QPointF>
class wavefront
{
public:
    wavefront();
    ~wavefront();
    wavefront( const wavefront &wf); // copy constructor doing deep copy of cv::Mat
    wavefront( wavefront && ) = delete;	// move constructor, deleted because unused
    wavefront& operator=( const wavefront & ) = default; // copy operator not doing deep copy of cv::mat
    wavefront& operator=(wavefront &&) = delete; // move operator, deleted because unused

    cv::Mat_<double> data;
    cv::Mat_<double> nulledData;
    cv::Mat_<uint8_t> mask;
    cv::Mat_<double> workData;
    cv::Mat_<uint8_t> workMask;
    std::vector<double> InputZerns;
    double gaussian_diameter;
    bool wasSmoothed;
    bool useSANull;
    double GBSmoothingValue;

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
    QVector<std::vector<cv::Point> > regions;
    bool regions_have_been_expanded;


};

#endif // WAVEFRONT_H
