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
#include "wavefront.h"

wavefront::wavefront():
    zernEnablesApplied(),gaussian_diameter(0.),gbEnabled(false),gbValue(20.),wasSmoothed(false),useSANull(true),GBSmoothingValue(0.),m_origin(WavefrontOrigin::Unknown),m_manuallyInverted(false),dirtyZerns(true),regions_have_been_expanded(false)
{
}

wavefront wavefront::copyShallow() const
{
    wavefront out;
    
    // those are cv::mat objects. `=` operator will not copy the data, but will create a new header pointing to the same data. So it is a shallow copy.
    out.data = data;
    out.nulledData = nulledData;
    out.mask = mask;
    out.workData = workData;
    out.workMask = workMask;

    out.InputZerns = InputZerns;
    out.zernEnablesApplied = zernEnablesApplied;
    out.gaussian_diameter = gaussian_diameter;
    out.gbEnabled = gbEnabled;
    out.gbValue = gbValue;
    out.wasSmoothed = wasSmoothed;
    out.useSANull = useSANull;
    out.GBSmoothingValue = GBSmoothingValue;
    out.m_origin = m_origin;
    out.m_manuallyInverted = m_manuallyInverted;
    out.name = name;
    out.lambda = lambda;
    out.m_outside = m_outside;
    out.m_inside = m_inside;
    out.diameter = diameter;
    out.roc = roc;
    out.min = min;
    out.max = max;
    out.std = std;
    out.mean = mean;
    out.dirtyZerns = dirtyZerns;
    out.regions = regions;
    out.regions_have_been_expanded = regions_have_been_expanded;
    return out;
}

wavefront wavefront::copyDeep() const
{
    wavefront out;
    out.data = data.clone();
    out.nulledData = nulledData.clone();
    out.mask = mask.clone();
    out.workData = workData.clone();
    out.workMask = workMask.clone();
    out.InputZerns = InputZerns;
    out.zernEnablesApplied = zernEnablesApplied;
    out.gaussian_diameter = gaussian_diameter;
    out.gbEnabled = gbEnabled;
    out.gbValue = gbValue;
    out.wasSmoothed = wasSmoothed;
    out.useSANull = useSANull;
    out.GBSmoothingValue = GBSmoothingValue;
    out.m_origin = m_origin;
    out.m_manuallyInverted = m_manuallyInverted;
    out.name = name;
    out.lambda = lambda;
    out.m_outside = m_outside;
    out.m_inside = m_inside;
    out.diameter = diameter;
    out.roc = roc;
    out.min = min;
    out.max = max;
    out.std = std;
    out.mean = mean;
    out.dirtyZerns = dirtyZerns;
    out.regions = regions;
    out.regions_have_been_expanded = regions_have_been_expanded;
    return out;
}


