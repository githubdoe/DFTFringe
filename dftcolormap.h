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
#ifndef DFTCOLORMAP_H
#define DFTCOLORMAP_H
#include "wavefront.h"
#include <qwt_color_map.h>
#include "usercolormapdlg.h"
#include <QList>
class dftColorMap: public QwtLinearColorMap
{
public:
    dftColorMap(int type = 0, wavefront *wf= 0, bool zeroBased = true,
                double errorMargin = .125, double scale = 1.);
    void setRange(double low, double high);
    static void setUserStops(QList<colorStop> &stops);
    wavefront *m_wf;
    static QList<colorStop> userStops;
};
#endif // DFTCOLORMAP_H
