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

#include <math.h>
#include "zernikes.h"
#include <QDebug>
#include <QString>
#include "zernikeprocess.h"

void dump_matrix (double *a, int nrows, int ncols,const char *desc)
{
    int i, j;
    qDebug() << "###############";
    qDebug() << QString("%1").arg(desc);
    qDebug() << "###############";
    qDebug() << QString("%1, %2\n").arg(nrows).arg(ncols);
    for (i=0; i < nrows; i++)
    {
        for (j=0; j < ncols; j++)
        {
            qDebug() << QString("%1 ").arg(a[I(i, j, ncols)], 0, 'f');
        }
    }
}


double computeRMS(int term,  double val){

    return val /sqrt( Zw[term]);
}

zern_spec::zern_spec(int order)
{
   /* Zernike terms ordered as in Wyant and Creath. */
    m_nterms = (order+1)*(order+1);
    m_n = new int[m_nterms];
    m_m = new int[m_nterms];
    m_s = new unsigned char[m_nterms];

    int i = 0;
    for (int n=0; n <= order; n++) {
        for (int m=n; m > 0; m--) {
            for (int k=1; k <= 2; k++) {
                m_n[i] = n; m_m[i] = m; m_s[i] = k; i++;
            }
        }
        m_n[i] = n; m_m[i] = 0; m_s[i] = 0; i++;
    }

}

void zern_spec::dump(void)
{
    int i;
    qDebug() <<  "############";
    qDebug() <<  "zernike spec";
    qDebug() <<  "############";
    qDebug() <<  "       n   m   s";
    qDebug() <<  "      ----------";
    for (i=0; i < m_nterms; i++) {
        qDebug() <<  QString("%1: %2 %3 %4").arg(i, 3).arg(m_n[i], 3).arg(m_m[i], 3).arg(m_s[i], 3);
    }
}

inline
int rix (int n, int m)
{
    /* Turn (n, m) into single zero-based index for radial polynomials. */
    return ((n+1)*(n+2)/2 - m - 1);
}
zern_generator::zern_generator(int size)
:m_size(size), m_rho(0), m_theta(0), m_zpoly(0), m_zcoef(0)
{
    fill_rho();
    fill_theta();
    m_spec = new zern_spec(6);
}
zern_generator::~zern_generator()
{
    if (m_zpoly)
        delete[] m_zpoly;
    if (m_rho)
        delete[] m_rho;
    if (m_theta)
        delete[] m_theta;
    if (m_spec)
        delete m_spec;
}
void zern_generator::fill_rho ()
{

    double delta;
    int i, j;
    delta = 2.0 /  (m_size-1);
    if (m_rho)
        delete[] m_rho;
    m_rho = new double[m_size * m_size];

    for (i=0; i < m_size; i++)
    {
        double x = -1.0 + delta * i;
        for (j=0; j < m_size; j++)
        {
            double y = -1.0 + delta * j;
            m_rho[I(i, j, m_size)] = sqrt (pow (x, 2) + pow (y, 2));
        }
    }
}

void zern_generator::fill_theta ()
{
    double delta;
    int i, j;
    delta = 2.0 /  (m_size-1);
    if (m_theta)
        delete[] m_theta;

    m_theta = new double[m_size * m_size];

    for (i=0; i < m_size; i++)
    {
        double x = -1. + delta * i;
        for (j=0; j < m_size; j++)
        {
            double y = -1. + delta * j;
            m_theta[I(i, j, m_size)] = atan2 (x, y);
        }
    }
}

void zern_generator::set_spec(int order)
{
    if (m_spec)
        delete m_spec;
    m_spec = new zern_spec(order);
}
double *rpoly_list (double *rho, int npoints, int nmax)
{
    /* Evaluate and return a matrix of Zernike radial polynomials over the
       specified values of rho and up to the specified maximum n value. */
    int n, m, nterms, j, i;
    double *r;
    nterms = (nmax+1)*(nmax+2)/2;
    r = new double[nterms*npoints];// (sizeof (double), nterms*npoints);
    for (n=0; n <= nmax; n++)
        for (m=n; m >= 0; m--) {
            j = rix (n, m);
            if (n == m) {
                for (i=0; i < npoints; i++)
                    r[I(i, j, nterms)] = pow (rho[i], m);
            }
            else {
                for (i=0; i < npoints; i++)
                    r[I(i, j, nterms)] =
                        ((2*n-m)*rho[i]*r[I(i, j-1, nterms)]
                         - n*r[I(i, j-n-1, nterms)]) / (n-m);
            }
        }
    return (r);
}

void zern_generator::zpoly_list(void)
{
    int npoints = m_size * m_size;
    /* Evaluate and return a matrix of zernike polynomials.  The columns
       are ordered according to the specification s. */
    double *r, *ct, *st, *p;
    int i, j, maxn, maxm, rterms;

    /* These should really look for the max values in s: */
    maxn = maxm = m_spec->m_n[m_spec->m_nterms-1];

    r = rpoly_list (m_rho, npoints, maxn);
    rterms = (maxn+1)*(maxn+2)/2;

    ct = new double[npoints*maxm];// *)calloc (sizeof (double), npoints*maxm);
    st = new double[npoints*maxm];//(double *)calloc (sizeof (double), npoints*maxm);
    if (m_zpoly)
        delete[] m_zpoly;

    m_zpoly = new double[npoints*m_spec->m_nterms];//(double *)calloc (sizeof (double), npoints*s->nterms);

    /* Cache sin and cos of multiples of theta for efficiency. */
    for (i=0; i < npoints; i++)
        for (j=0; j < maxm; j++) {
            ct[I(i, j, maxm)] = cos ((j+1) * m_theta[i]);
            st[I(i, j, maxm)] = sin ((j+1) * m_theta[i]);
        }

    /* Calculate the zernike polynomial matrix. */
    for (i=0; i < npoints; i++)
        for (j=0; j < m_spec->m_nterms; j++) {
            p = &m_zpoly[I(i, j, m_spec->m_nterms)];
            *p = r[I(i, rix(m_spec->m_n[j], m_spec->m_m[j]), rterms)];
            if (m_spec->m_s[j] == 1)
                *p *= ct[I(i, m_spec->m_m[j]-1, maxm)];
            else if (m_spec->m_s[j] == 2)
                *p *= st[I(i, m_spec->m_m[j]-1, maxm)];
    }
    delete[] ct;
    delete[] st;
    delete[] r;

}


void zern_generator::dump_zpoly()
{
    dump_matrix(m_theta, m_size, m_size, "zpoly");

}
double * zern_generator::zern_eval ()
{
    int i, j;
    int npoints = m_size * m_size;
    double *wf = new double[npoints];

    for (i=0; i < npoints; i++)
    {
        wf[i] = 0.;
        for (j=0; j < m_spec->m_nterms; j++)
            wf[i] += m_zcoef[j] * m_zpoly[I(i, j, m_spec->m_nterms)];
    }
    return (wf);
}
double *rand_zern ( zern_spec *s)
{
    /* Return a set of random Zernike coefficients according to spec. */
    int i;
    double* z = new double[s->m_nterms];

    for (i = 0; i < s->m_nterms; i++)
        z[i] = sqrt (1/((double)i+1));
    return (z);
}

void zern_generator::set_zcoefs(double *data)
{
    m_zcoef.assign(data,data + get_terms_cnt());		// temp.  chanage to param after debug
}



