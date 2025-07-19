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

#ifndef ZERNIKEPOLAR_H
#define ZERNIKEPOLAR_H

#include <QObject>

class zernikePolar : public QObject
{
    Q_OBJECT
public:
    explicit zernikePolar(){};
    static zernikePolar *get_Instance();  //TODO check if we really need a singleton here
    void init(double rho, double theta, size_t nbTerms = 48);
    double zernike(size_t z);
private:
     static zernikePolar *m_instance;
     double rho2;
     double rho3;
     double rho4;
     double rho5;
     double rho6;
     double rho8;
     double rho10;
     double costheta;
     double sintheta;
     double cos2theta;
     double sin2theta;
     double cos3theta;
     double sin3theta;
     double cos4theta;
     double sin4theta;
     double cos5theta;
     double sin5theta;
     size_t m_nbTermsComputed;
     double zernTerms[49]; // TODO if no singleton, use correct length
};


#endif