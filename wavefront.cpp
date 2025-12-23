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
    gaussian_diameter(0.),useSANull(true),m_origin(WavefrontOrigin::Unknown),m_manuallyInverted(false),dirtyZerns(true),regions_have_been_expanded(false)
{
}

wavefront::~wavefront()
{

    data.release();
    mask.release();
    workData.release();
    workMask.release();
    InputZerns.clear();
    nulledData.release();

}
wavefront::wavefront( const wavefront &wf): 
    data(wf.data.clone()),
    nulledData(wf.nulledData.clone()),
    mask(wf.mask.clone()),
    workData(wf.workData.clone()),
    workMask(wf.workMask.clone()),
    InputZerns(wf.InputZerns),
    gaussian_diameter(wf.gaussian_diameter),
    wasSmoothed(wf.wasSmoothed),
    useSANull(wf.useSANull),
    GBSmoothingValue(wf.GBSmoothingValue),
    m_origin(wf.m_origin),
    m_manuallyInverted(wf.m_manuallyInverted),
    lambda(wf.lambda),
    m_outside(wf.m_outside),
    m_inside(wf.m_inside),
    diameter(wf.diameter),
    roc(wf.roc),
    min(wf.min),
    max(wf.max),
    std(wf.std),
    mean(wf.mean),
    dirtyZerns(wf.dirtyZerns)
{}

