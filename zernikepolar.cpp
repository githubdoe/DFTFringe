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

#include "zernikepolar.h"
#include <cmath>
#include <QDebug>
#include "spdlog/spdlog.h"

zernikePolar::zernikePolar(double rho, double theta, size_t nbTerms) {
    // Having all terms computed at once here let's compiler optimize the code better

    if(nbTerms > 49){
        qWarning() << "zernikePolar::init: maxorder is limited to 49, setting to 49";
        nbTerms = 49;
    }
    
    m_nbTermsComputed = nbTerms;
    
    zernTerms[0] = 1.;
    
    const double rho2 = rho * rho;
    const double costheta = std::cos(theta);
    const double sintheta = std::sin(theta);
    const double cos2theta = std::cos(2. * theta);
    const double sin2theta = std::sin(2. * theta);
    zernTerms[1] = rho * costheta;
    zernTerms[2] = rho * sintheta;
    zernTerms[3] = -1. + 2. * rho2;
    zernTerms[4] = rho2 * cos2theta;
    zernTerms[5] = rho2 * sin2theta;
    zernTerms[6] = rho * (-2. + 3. * rho2) * costheta;
    zernTerms[7] = rho * (-2. + 3. * rho2) * sintheta;
    zernTerms[8] = 1. + rho2 * (-6 + 6. * rho2);

    double rho3;
    double rho4;
    double rho5;
    double rho6;
    double rho8;
    double cos3theta;
    double sin3theta;
    double cos4theta;
    double sin4theta;
    double rho10;
    double cos5theta;
    double sin5theta;

    // only compute what is actually needed
    // but to avoid complex code I use only 4 ranges
    if(nbTerms > 9)
    {
        rho3 = rho2 * rho;
        rho4 = rho3 * rho;
        rho5 = rho4 * rho;
        rho6 = rho5 * rho;
        rho8 = rho6 * rho2;
        cos3theta = std::cos(3. * theta);
        sin3theta = std::sin(3. * theta);
        cos4theta = std::cos(4. * theta);
        sin4theta = std::sin(4. * theta);

        zernTerms[9] = rho3 * cos3theta;
        zernTerms[10] = rho3 * sin3theta;
        zernTerms[11] = rho2 * (-3 + 4 * rho2) * cos2theta;
        zernTerms[12] = rho2 * (-3 + 4 * rho2) * sin2theta ;
        zernTerms[13] = rho * (3. - 12. * rho2 + 10. * rho4) * costheta;
        zernTerms[14] = rho * (3. - 12. * rho2 + 10. * rho4) * sintheta;
        zernTerms[15] = -1 + 12 * rho2 - 30. * rho4 + 20. * rho6;
        zernTerms[16] = rho4 * cos4theta;
        zernTerms[17] = rho4 * sin4theta;
        zernTerms[18] = rho3 *( -4. + 5. * rho2) * cos3theta;
        zernTerms[19] = rho3 *( -4. + 5. * rho2) * sin3theta;
        zernTerms[20] = rho2 * (6. - 20. * rho2 + 15 * rho4)* cos2theta;
        zernTerms[21] = rho2 * (6. - 20. * rho2 + 15 * rho4)* sin2theta;
        zernTerms[22] = rho * (-4. + 30. * rho2 - 60. * rho4 + 35 * rho6)* costheta;
        zernTerms[23] = rho * (-4. + 30. * rho2 - 60. * rho4 + 35 * rho6)* sintheta;
        zernTerms[24] = 1. - 20. * rho2 + 90. *  rho4 - 140. * rho6 + 70. * rho8;
    }

    if(nbTerms > 25) {
        rho10 = rho8 * rho2;
        cos5theta = std::cos(5. * theta);
        sin5theta = std::sin(5. * theta);

        zernTerms[25] = rho5 * cos5theta;
        zernTerms[26] = rho5 * sin5theta;
        zernTerms[27] = rho4 * (-5. + 6. * rho2) * cos4theta;
        zernTerms[28] = rho4 * (-5. + 6. * rho2) * sin4theta;
        zernTerms[29] = rho3 * (10. - 30. * rho2 + 21. * rho4) * cos3theta;
        zernTerms[30] = rho3 * (10. - 30. * rho2 + 21. * rho4) * sin3theta;
        zernTerms[31] = rho2 *(-10. + 60. * rho2 - 105. * rho4 + 56. * rho6) * cos2theta;
        zernTerms[32] = rho2 *(-10. + 60. * rho2 - 105. * rho4 + 56. * rho6) * sin2theta;
        zernTerms[33] = rho * (5. - 60. * rho2 + 210 * rho4 -280. * rho6 + 126. * rho8) * costheta;
        zernTerms[34] = rho * (5. - 60. * rho2 + 210 * rho4 -280. * rho6 + 126. * rho8) * sintheta;
        zernTerms[35] = -1 + 30. * rho2 -210 * rho4 + 560. * rho6 - 630 * rho8 + 252. * rho10;
    }

    if(nbTerms > 36)
    {
        zernTerms[36] = rho6 * std::cos(6. * theta);
        zernTerms[37] = rho6 * std::sin(6. * theta);
        zernTerms[38] = rho5 * (-6. + 7 * rho2) * cos5theta;
        zernTerms[39] = rho5 * (-6. + 7 * rho2) * sin5theta;
        zernTerms[40] = rho4 * (15. -42. * rho2 + 28. * rho4) * cos4theta;
        zernTerms[41] = rho4 * (15. -42. * rho2 + 28. * rho4) * sin4theta;
        zernTerms[42] = rho3 * (-20 + 105. * rho2 - 168. * rho4 + 84 * rho6) * cos3theta;
        zernTerms[43] = rho3 * (-20. + 105. * rho2 - 168. * rho4 + 84. * rho6) * sin3theta;
        zernTerms[44] = rho2 * (15. - 140. * rho2 + 420. * rho4 - 504. * rho6 +  210. * rho8) * cos2theta;
        zernTerms[45] = rho2 * (15. - 140. * rho2 + 420. * rho4 - 504. * rho6 +  210. * rho8) * sin2theta;
        zernTerms[46] = rho *(-6. + 105 * rho2 - 560. * rho4 + 1260. * rho6 -1260. * rho8 +462. * rho10) * costheta;
        zernTerms[47] = rho *(-6. + 105 * rho2 - 560. * rho4 + 1260. * rho6 -1260. * rho8 +462. * rho10) * sintheta;
        zernTerms[48] = 1. - 42. * rho2 + 420. * rho4 - 1680. * rho6 + 3150. * rho8 -2772. * rho10 + 924. * rho10 * rho2;
    }
}

double zernikePolar::zernike(size_t n){
    if(n < m_nbTermsComputed) {
        return zernTerms[n];
    }
    else
    {
        spdlog::get("logger")->critical("zernikePolar() Zernike order exceeds maximum computed.");
        throw std::out_of_range("Zernike order exceeds maximum computed order");
        return 0.;
    }
}