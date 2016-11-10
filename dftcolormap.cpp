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
#include "dftcolormap.h"
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
#include <QDebug>
QList<colorStop> dftColorMap::userStops;

void dftColorMap::setUserStops(QList<colorStop> &stops) {
    dftColorMap::userStops = stops;
}


dftColorMap::dftColorMap(int type, wavefront *wf, bool zeroBased, double errorMargin, double scale):
        QwtLinearColorMap( Qt::black, Qt::white ),m_wf(wf)
{

    switch (type) {
    case 3://Gray
        addColorStop(0.0,QColor(00,00,00));
        addColorStop(1.,QColor(255,255,255));
        break;
    case 6:
        for (double v = 0.; v <= .999; v += .1){
            double theta = 2. * M_PI * v;
            int b = scale * (141 + 110 * cos(theta));
            int g = scale * (111 + 110 * cos(theta + 4 * M_PI / 3.));
            int r = scale * (141 + 110 * cos(theta + 2 * M_PI / 3.));
            QColor c(r,g,b);
            addColorStop(v, c);
            //qDebug() << v << c;
        }
        break;
    case 5:
        foreach(colorStop s, dftColorMap::userStops){
            s.color.setRed(s.color.red() * scale);
            s.color.setGreen(s.color.green() * scale);
            s.color.setBlue(s.color.blue() * scale);
            addColorStop(s.pos,s.color);
        }
        break;
    case 4:// Exceeds error limit
    {
        // figure out where on this wavefront -.125 +.125 is when
        // wavefront is scaled from 0 to 1

        double range = 1.;
        double mean= .5;
        if (m_wf){
            mean = m_wf->mean;
            double zmin = m_wf->mean - 3 * m_wf->std;
            double zmax = m_wf->mean + 3 * m_wf->std;

            range = (zmax - zmin);
        }

        double sf = 1./range;
        double middle = (range * sf)/2.;
        double lowError =  middle - errorMargin * sf;
        lowError = fmax(.001, lowError);
        double highError = middle + errorMargin * sf;
        highError = fmin(.99,highError);

         if (zeroBased) {
            highError = 2 * errorMargin * sf;
            addColorStop(0,Qt::black);
        }
        else {
            addColorStop(0.f,Qt::cyan);
            addColorStop(lowError, Qt::blue);
            addColorStop(lowError + .001,QColor(50,50,50));
        }
        addColorStop(highError, QColor(200,200,200));
        addColorStop(highError+.001,QColor(255,0,0));

        break;
    }
    case 2://Vibrant
        addColorStop(0.f,Qt::cyan);
        addColorStop(.4, Qt::blue);
        addColorStop(.45,QColor(90,90,90));

    addColorStop(.5, QColor(150,150,150));
    addColorStop(.8,QColor(255,0,0));
    addColorStop(.99,Qt::yellow);
    /*
        addColorStop(0, QColor(0, 0, 50));
        addColorStop(0.15, QColor(20, 0, 120));
        addColorStop(0.33, QColor(200, 30, 140));
        addColorStop(0.6, QColor(255, 100, 0));
        addColorStop(0.85, QColor(255, 255, 40));
        addColorStop(1, QColor(255, 255, 255)); */
        break;
    case 1://Tropical
        double pos;
        pos = 1.0/12.0; addColorStop(pos, QColor(0,0,255));
        pos = 2.0/12.0;  addColorStop(pos, QColor(0,92,138));
        //pos = 3.0/12.0;  addColorStop(pos, QColor(35,234,170));
        pos = 4.0/12.0;  addColorStop(pos, QColor(0,136,136));
        pos = 5.0/12.0;  addColorStop(pos, QColor(0,109,25));
        pos = 6.0/12.0;  addColorStop(pos, QColor(100,180,100));
        pos = 7.0/12.0;  addColorStop(pos, QColor(66,185,45));
        pos = 8.0/12.0;  addColorStop(pos, QColor(155,168,91));
        pos = 9.0/12.0;  addColorStop(pos, QColor(188,188,0));
        pos = 10.0/12.0;  addColorStop(pos, QColor(255,189,0));
        pos = 11.0/12.0;  addColorStop(pos, QColor(189,0,0));
        pos = 1.;        addColorStop(pos, QColor(255,66,0));
        break;

    case 0://HotCold

        addColorStop(0,Qt::black);
        addColorStop( 0.15, Qt::blue );
        addColorStop(0.25, Qt::cyan);
        addColorStop(.75,QColor(160,160,160));
        addColorStop( 0.5, QColor(150,60,0) );
        addColorStop(.90, Qt::red);
        addColorStop(.99,Qt::yellow);
        break;
    }
}


