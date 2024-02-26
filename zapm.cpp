/**********************

Copyright © 2022 Michael Peck <mlpeck54 -at- gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the “Software”), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

**************************/


// [[Rcpp::depends(RcppArmadillo)]]

# include <cmath>
# include "armadillo"
#include <QDebug>
#include <QString>



using namespace arma;

void stop(QString str){
    qDebug() << "Error" << str;
}

/*************************
 *
 * Golub-welsch algorithm to get the nodes and weights for Gauss Legendre quadrature
 *
 ************************/

//' Golub-Welsch method to find quadrature points and weights for Gauss-Legendre quadrature
//'
//' Calculates the nodes and weights for Legendre polynomials shifted
//'   to the interval (eps^2, 1).
//'
//' @param eps obstruction fraction 0 <= eps < 1
//' @param qwts an input R vector with length equal to the number of quatrature points.
//'
//' @return a vector of quadrature nodes the same length as `qwts` in the open interval
//'   (eps^2, 1). `qwts` will be overwritten with the quadrature weights.
//'
//' @details If N is the maximum polynomial order to be evaluated qwts should be at least
//'   of length N/2 + 1. Quadratures will then be nominally exact.
//'
//' @seealso Called by [zapm()] and [zapm_iso()].
// [[Rcpp::export]]
vec gol_welsch(const double& eps, vec& qwts) {

  uword nq = qwts.n_elem;
  mat J(nq, nq), evec(nq, nq);
  vec eval(nq);
  rowvec wts(nq);
  double ak = (1. + eps*eps)/2.;
  double sqbi;
  double mu0 = 1. - eps*eps;

  J.diag().fill(ak);
  for (uword i = 1; i < nq; i++) {
    sqbi = i * 0.5 * mu0/std::sqrt(4.*i*i - 1.);
    J(i-1, i) = sqbi;
    J(i, i-1) = sqbi;
  }

  eig_sym(eval, evec, J);
  wts = mu0 * square(evec.row(0));
  qwts = wts.as_col();

  // eigenvalues are the abscissa values for the quadrature
  return eval;
}

/********************
 *
 * Radial Zernike annular polynomials
 *
********************/


//' Radial Zernike Annular polynomials
//'
//' Create a matrix of Zernike Annular polynomial values in
//' extended Fringe sequence for a set of polar coordinates.
//'
//' @param rho a vector of radial coordinates.
//' @param eps the obstruction fraction 0 <= eps < 1.
//' @param n the maximum radial order required
//' @param m azimuthal order
//' @param xq nodes for Gauss-Legendre quadrature
//' @param qwts weights for Gauss-Legendre quadrature
//'
//' @return A length(rho) x (n-m)/2+1 column matrix of radial Zernike Annular polynomial values evaluated at the input
//'  radial coordinates. The radial indexes are in increasing order from m, m+2, ..., n.
//'
//' @details To the author's knowledge no recurrence relations for radial Zernike annular polynomials
//'  have been published, even though several are well known for the closely related Zernike circle polynomials.
//'  However the m=0 polynomials representing axially symmetric aberrations are just shifted Legendre polynomials
//'  with an easily derived recurrence relation. This routine makes use of that fact to generate
//'  recurrence relations for arbitrary polynomial indexes using chebyshev's algorithm with modified moments.
//'  The modified moments are calculated using Gauss-Legendre quadrature. If enough quadrature nodes
//'  were chosen the quadrature is nominally exact, as are the resulting annular Zernike values.
//'
//' @seealso This function is called by [zapm()] and [zapm_iso()].
//'
//' @md
// [[Rcpp::export]]
mat rzernike_ann(const vec& rho, const double& eps, const int& n, const int& m, const vec& xq, const vec& qwts) {

  if (n < m) {
    stop("n < m");
  }

  if ((n-m) % 2 != 0) {
    stop("n,m must be relatively even");
  }

  uword nr = rho.n_elem;
  int nq = xq.n_elem;
  int nz = (n-m)/2 + 1;      // input n is the maximum radial order required. nz is the total number to be generated
  int nmax = std::min(2*nz, m+1); //number of modified moments that are non-zero

  vec u(nr);
  u = rho % rho;

  // return radial Zernikes

  mat RZ(nr, nz);
  vec rm(nr);
  double eps2 = eps*eps;
  double e1 = 1. - eps2;
  double ak = (1 + eps2)/2.;

  rm = pow(rho, m);

  if (nz == 1) {
    RZ.col(0) = std::sqrt(e1/(1-std::pow(eps2, m+1))) * rm;
    return RZ;
  }

  // things we need to calculate for the recurrences

  vec c(nz), alpha(nz), beta(nz);
  vec pn(nq), pnp1(nq), pnm1(nq), w(nq);
  vec nu(2*nz), b(2*nz);
  mat sigma(3, 2*nz);

  if (m == 0) {    // know the recursion for this case
    alpha.fill(ak);
    c(0) = e1;
    for (int j = 1; j < nz; j++) {
      beta(j) = j * j * 0.25 * e1 * e1/(4.*j*j - 1.0);
      c(j) = beta(j) * c(j-1);
    }
  } else {
    for (int j = 1; j < 2*nz; j++) {
      b(j) = j * j * 0.25 * e1 * e1/(4.*j*j - 1.0);
    }


    // calculate modified moments

    w = pow(xq, m);
    nu(0) = sum(w % qwts);

    pnm1.fill(1.0);
    pn = (xq - ak);

    for (int j=1; j < nmax; j++) {
      nu(j) = sum(pn % w % qwts);
      pnp1 = (xq - ak) % pn - b(j) * pnm1;
      pnm1 = pn;
      pn = pnp1;
    }


    //chebyshev algorithm with modified moments

    alpha(0) = ak + nu(1)/nu(0);
    beta(0) = nu(0);
    c(0) = nu(0);

    for (int l=0; l<(2*nz); l++) {
      sigma(0, l) = 0.0;
      sigma(1, l) = nu(l);
    }

    for (int k=1; k<nz; k++) {
      for (int l=k; l<(2*nz-k); l++) {
        sigma(2, l) = sigma(1, l+1) - (alpha(k-1) - ak) * sigma(1, l) -
                        beta(k-1) * sigma(0, l) + b(l) * sigma(1, l-1);
      }

      alpha(k) = ak + sigma(2, k+1)/sigma(2, k) - sigma(1, k)/sigma(1, k-1);
      beta(k) = sigma(2, k)/sigma(1, k-1);
      c(k) = beta(k) * c(k-1);
      for (int l=k; l<(2*nz-k); l++) {
        sigma(0, l) = sigma(1, l);
        sigma(1, l) = sigma(2, l);
      }
    }
  }

  RZ.col(0).fill(1.0);
  RZ.col(1) = (u - alpha(0));
  for (int i = 1; i < (nz-1); i++) {
    RZ.col(i+1) = (u - alpha(i)) % RZ.col(i) - beta(i) * RZ.col(i-1);
  }
  for (int i=0; i<nz; i++) {
    RZ.col(i) = RZ.col(i) % rm * std::sqrt(e1/(2. * i + m + 1.)/c(i));
  }

  return RZ;
}



/*****************
 *
 * Create matrix of Zernike Annular polynomials
 * in extended Fringe index scheme.
 *
 *
******************/

//' Zernike Annular polynomials
//'
//' Create a matrix of Zernike Annular polynomial values in
//' extended Fringe sequence for a set of polar coordinates.
//'
//' @param rho a vector of radial coordinates with eps <= rho <= 1.
//' @param theta a vector of angular coordinates, in radians.
//' @param eps the obstruction fraction 0 <= eps < 1.
//' @param maxorder the maximum radial polynomial order (defaults to 12).
//' @param nq the number of quadrature points for numerical integration
//'
//' @return a matrix of Zernike Annular polynomial values evaluated at the input
//'  polar coordinates and all radial orders from
//'  0 through `maxorder` in Fringe sequence, with orthonormal scaling.
//'
//' @details The *radial* polynomials are calculated using recurrence relations
//'  generated numerically using chebyshev's algorithm with modified moments.
//'  See the documentation for [rzernike_ann()]. A formal presentation is
//'  included in the package documentation.
//' @examples
//'   sample_az <- function(maxorder=12, eps=0.33, col=rev(zernike::rygcb(400)), addContours=TRUE, cscale=TRUE) {
//'
//'     ## get coordinates for unobstructed and obstructed apertures
//'     cpa <- cp.default
//'     cpa$obstruct <- eps
//'     prt <- pupil.rhotheta(nrow.default,ncol.default,cp.default)
//'     prta <- pupil.rhotheta(nrow.default,ncol.default,cp=cpa)
//'     rho0 <- prt$rho[!is.na(prt$rho)]
//'     theta0 <- prt$theta[!is.na(prt$theta)]
//'     rhoa <- prta$rho[!is.na(prta$rho)]
//'     thetaa <- prta$theta[!is.na(prta$theta)]
//'
//'     ## fill up matrixes of Zernikes and Annular Zernikes
//'
//'     zm <- zpmC(rho0, theta0, maxorder=maxorder)
//'     zam <- zapm(rhoa, thetaa, eps=eps, maxorder=maxorder, nq=maxorder/2+5)
//'
//'     ## pick a column at random and look up its index pair
//'
//'     zlist <- makezlist(0, maxorder)
//'     i <- sample(2:ncol(zm), 1)
//'     n <- zlist$n[i]
//'     m <- zlist$m[i]
//'
//'     ## fill up the wavefront representations and plot them
//'
//'     wf0 <- prt$rho
//'     wf0[!is.na(wf0)] <- zm[,i]
//'     class(wf0) <- "pupil"
//'
//'     wfa <- prta$rho
//'     wfa[!is.na(wfa)] <- zam[,i]
//'     class(wfa) <- "pupil"
//'
//'     plot(wf0, cp=cp.default, col=col, addContours=addContours, cscale=cscale)
//'     mtext(paste("Zernike, n =", n, " m =", m))
//'
//'     x11()
//'     plot(wfa, cp=cpa, col=col, addContours=addContours, cscale=cscale)
//'     mtext(paste("Annular Zernike, n =", n, " m =", m))
//'
//'     ## return Zernike matrices and wavefronts invisibly
//'     ## just in case user wants to do something with them
//'
//'     invisible(list(zm=zm, wf0=wf0, zam=zam, wfa=wfa))
//'   }
//'
//'   sample_az()
//'
//' @md
// [[Rcpp::export]]
mat zapm(const vec& rho, const vec& theta, const double& eps, const int& maxorder=12) {

  int j, k, nmax, nz, mmax = maxorder/2;
  uword nr = rho.size();
  int ncol = (mmax+1)*(mmax+1);
  mat cosmtheta(nr, mmax), sinmtheta(nr, mmax);
  mat zm(nr, ncol);

    //do some rudimentary error checking

  if (rho.size() != theta.size()) {
    stop("Numeric vectors must be same length");
  }
  if ((maxorder % 2) != 0) {
    stop("maxorder must be even");
  }

  //good enough

  // get points and weights for quadrature
  int nq = maxorder/2 + 5;
  vec xq(nq), qwts(nq);
  xq = gol_welsch(eps, qwts);

  //cache values of cos and sin

  cosmtheta.col(0) = cos(theta);
  sinmtheta.col(0) = sin(theta);
  for (int m=1; m<mmax; m++) {
    cosmtheta.col(m) = cosmtheta.col(m-1) % cosmtheta.col(0) - sinmtheta.col(m-1) % sinmtheta.col(0);
    sinmtheta.col(m) = sinmtheta.col(m-1) % cosmtheta.col(0) + cosmtheta.col(m-1) % sinmtheta.col(0);
  }

  //n=0 zernikes are just the scaled radial zernikes

  nz = maxorder/2 + 1;
  mat RZ(nr, nz);

  RZ = rzernike_ann(rho, eps, maxorder, 0, xq, qwts);
  for (int n=0; n<=maxorder; n += 2) {
    k = (n*n)/4 + n;
    zm.col(k) = RZ.col(n/2);
  }

  for (int m=1; m<=mmax; m++) {
    nmax = maxorder - m;
    nz = (nmax - m)/2 + 1;
    mat RZ(nr, nz);
    RZ = rzernike_ann(rho, eps, nmax, m, xq, qwts);
    j = 0;
    for (int n=m; n<= nmax; n += 2) {
      k = ((n+m)*(n+m))/4 + n - m;
      zm.col(k) = RZ.col(j) % cosmtheta.col(m-1);
      k++;
      zm.col(k) =  RZ.col(j) % sinmtheta.col(m-1);
      j++;
    }
  }

  return zm;
}

