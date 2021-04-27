// portions of this code are
//	Copyright (C) 2016 Michael Peck <mpeck1 -at- ix.netcom.com>
//
//	License: MIT <https://opensource.org/licenses/MIT>
//
#include <cmath>
#include <iostream>
#include "mikespsiinterface.h"
#include <stdlib.h>
//#include <armadillo>
#include <QMessageBox>
#include "qmath.h"
#include <QDebug>
#include <QMap>
using namespace std;
// create two row vectors containing rho and theta values of the inclosed circular pupil
// return them as two columns of a mat.
arma::mat rhotheta(const double rows, const double cols, double obspercent){
    double hy = (rows-1)/2.;
    double hx = (cols-1)/2.;
    vector<double> rhov;
    vector<double> thetav;

    for (int i = 0; i < rows; ++i){
        double y = (i - hy)/hy;
        for (int j = 0; j< cols; ++j){
            double x = (j - hx)/ hx;
            double r = sqrt( x * x + y * y);
            if (r >1. || r < obspercent)
                continue;
            rhov.push_back(r);
            thetav.push_back(atan2(y,x));
        }
    }
    arma::rowvec r(rhov);
    arma::rowvec t(thetav);
    return arma::join_cols(r,t);
}

// Fill a matrix witha Zernike polynomial values
arma::mat zpmC(arma::vec rho, arma::vec theta, int maxorder) {

  int i, m, n, nn, n0, np2, nm2, order, nmax, mmax= maxorder/2;
  int nr = rho.size();
  int ncol = (mmax+1)*(mmax+1);
  double a0, a1, a2;
  double cosmtheta[mmax], sinmtheta[mmax];
  arma::mat zm(nr, ncol);

  //do some rudimentary error checking


  //if ((maxorder % 2) != 0) stop("maxorder must be even");

  //good enough


  for (n=0; n<nr; n++) {

      //cache values of cos and sin
      for (m=0; m<mmax; m++) {
          cosmtheta[m] = cos((m+1)*theta[n]);
          sinmtheta[m] = sin((m+1)*theta[n]);
      }

      zm(n, 0) = 1.0;                     //piston term
      zm(n, 1) = rho[n];                  //tilt
      zm(n, 3) = 2.*rho[n]*rho[n] - 1.;   //defocus

      // now fill in columns with m=n for n>0

      for (m=2; m <= mmax; m++) {
          zm(n, m*m) = pow(rho[n], (double) m);
      }

      //now the rest of the radially symmetric terms

      for (i=2; i<= mmax; i++) {
          nn = 2*i-2;
          np2 = (i*i+2*i);
          nm2 = (i*i-2*i);
          n0 =  (i*i-1);
          a0 = 1./(nn+2.);
          a1 = 4.*(nn+1);
          a2 = 2.*(nn+1);
          zm(n, np2) = a0*((a1*rho[n]*rho[n]-a2)*zm(n, n0)-nn*zm(n, nm2));
      }

      // non-symmetric terms

      for (m=1; m<mmax; m++) {
          order = 2*m+2;
          n0 = m*m;
          np2 = n0 + order +1;
          a0 = (double) (m+2.)/(4.*(m+1.)); //coefficients simplify for R(n=m+2,m)
          a1 = (double) 4.*(m+1.);          //in terms of R(n=m, m)
          a2 = (double) 4.*m + 4./(m+2.);
          zm(n, np2) = a0*(a1*rho[n]*rho[n] - a2)*zm(n, n0); //R(n=m+2,m) in terms of R(n=m,m)

          nmax = maxorder-m;
          if (nmax < (m+4)) break;
          for (nn = m+2; nn < nmax; nn += 2) {
              order += 2;
              nm2 = n0;
              n0 = np2;
              np2 = n0 + order + 1;
              a0 = (double) (nn+2.)/((nn+2.)*(nn+2.)-m*m);
              a1 = 4.*(nn+1);
              a2 = (double) (nn+m)*(nn+m)/nn+(nn-m+2.)*(nn-m+2.)/(nn+2.);
              zm(n, np2) = a0*((a1*rho[n]*rho[n] - a2)*zm(n, n0)
              - (double) (nn*nn-m*m)/nn * zm(n, nm2));
          }
      }

      // now multiply each column by normalizing factor and cos, sin

      n0 = 1;
      for (order=2; order <= maxorder; order+=2) {
          for(m=order/2; m>0; m--) {
              nn=order-m;
              a0 = sqrt(2.*(nn+1));
              zm(n, n0+1) = a0*sinmtheta[m-1]*zm(n, n0);
              zm(n, n0) *= a0*cosmtheta[m-1];
              n0 += 2;
          }
          nn = order;
          zm(n, n0) *= sqrt(nn+1.);
          n0++;
      }
  }
  return zm;
}




typedef struct {arma::vec x; bool convergence; int iter;} lmreturn;

/**************
 *
 * A stupidly simple implementation of a Levenberg-Marquardt
 * algorithm for nonlinear least squares.
 *
 * Adapted directly from the algorithm description on p. 27
 * of the booklet "Methods for Non-Linear Least Squares
 * Problems" by Madsen, Nielsen & Tingleff (2004)
 * download from http://www2.imm.dtu.dk/pubdb/views/edoc_download.php/3215/pdf/imm3215.pdf
 *
*/

lmreturn mylevmar(arma::vec (*res_fun)(const arma::vec&, const QMap<char*, arma::mat>&, double abar, double bbar),
             arma::mat (*jac_fun)(const arma::vec&, const QMap<char *, arma::mat>&, double bbar),
             unsigned int m, int n, arma::vec x0, QMap<char *, arma::mat> adata, double abar, double bbar,
             int itmax = 100, arma::vec control= {1.e-3, 2., sqrt(arma::datum::eps), sqrt(arma::datum::eps)}) {

  int k=0;
  double tau = control(0);
  double nu = control(1);
  double eps1 = control(2);
  double eps2 = control(3);

  arma::mat A(n, n);
  arma::vec g(n);
  arma::vec feval(m);
  arma::vec fnew(m);
  arma::mat Jac(m, n);
  arma::mat In = arma::eye<arma::mat>(n, n);
  arma::vec x = x0;
  arma::vec hx(n), xnew(n);
  bool found;
  double mu;
  double F, Fnew, dL, rho;
  lmreturn retvals;

  Jac = jac_fun(x, adata,bbar);
  feval = res_fun(x, adata, abar, bbar);
  A = Jac.t() * Jac;
  g = Jac.t() * feval;
  mu = tau * max(A.diag());
  found = (norm(g, "Inf") <= eps1);

  while (!found && k<itmax) {
    k++;
    hx = -arma::solve(A + mu*In, g);
    if (arma::norm(hx, 2) <= eps2 * (arma::norm(x, 2) + eps2)) {
      found = true;
      break;
    }
    xnew = x + hx;
    F = 0.5 * dot(feval, feval);
    fnew = res_fun(xnew, adata, abar,bbar);
    Fnew = 0.5 * arma::dot(fnew, fnew);
    dL = 0.5 * arma::dot(hx, (mu*hx - g));
    rho = (F-Fnew)/dL;
    if (rho > 0.) {
      x = xnew;
      Jac = jac_fun(x, adata,bbar);
      A = Jac.t() * Jac;
      g = Jac.t() * fnew;
      feval = fnew;
      mu *= fmax(1./3., 1 - pow(2.*rho -1., 3));
      nu = control(1);
      found = (arma::norm(g, "Inf") <= eps1);
    } else {
      mu *= nu;
      nu *= 2.;
    }
  }

  retvals.x = x;
  retvals.convergence = found;
  retvals.iter = k;

  return retvals;

}

// pixel-wise least squares with known phases, tilts, etc.

arma::mat pxls(const arma::mat& im, const arma::rowvec& phases, const arma::mat& zcs, const arma::mat& coords) {
  int nr = im.n_rows;
  unsigned int nf = im.n_cols;

  arma::rowvec ph(nf);
  arma::mat A(3, nf);
  arma::rowvec b(3);
  arma::mat B(nr, 3);

  for (unsigned int n=0; n<nr; n++) {
    ph = phases + coords.row(n) * zcs;
    A = arma::join_cols(arma::join_cols(arma::ones<arma::rowvec>(nf), arma::cos(ph)), arma::sin(ph));
    b = im.row(n) * pinv(A);
    B.row(n) = b;
  }
  return B;
}

// residuals and analytic Jacobian for mylevmar. Exported just in case I want to use minpack.lm

// [[Rcpp::export]]

arma::vec res_frame(const arma::vec& pars, const QMap<char*, arma::mat> & adata, double abar, double bbar) {
  arma::vec img = adata["img"];
  arma::mat coords = adata["coords"];

  arma::vec phi = adata["phi"];
  int np = pars.n_elem;
  arma::vec ph(img.n_elem);
  ph = pars[0] + coords * pars.tail(np-1);
  return img - abar - bbar * cos(phi + ph);
}

// [[Rcpp::export]]

arma::mat jac_frame(const arma::vec& pars, const QMap<char*, arma::mat> & adata, double bbar) {
  arma::vec img = adata["img"];
  arma::mat coords = adata["coords"];

  arma::vec phi = adata["phi"];
  int np = pars.n_elem;
  unsigned int m = img.n_elem;
  arma::vec ph(m);
  arma::vec sp(m);
  arma::mat jac(m, np);
  ph = phi + pars[0] + coords * pars.tail(np-1);
  sp = sin(ph);
  jac = arma::join_rows(arma::ones(m), coords);
  jac.each_col() %= sp;
  return bbar*jac;
}

/************
 *
 * wrap phase into [-pi, pi)
 * This is slower than R version
 * but export it anyway with a different name.
 *
*/

//[[Rcpp::export]]

arma::mat pwrap(const arma::mat& phase) {
  unsigned int nr = phase.n_rows;
  unsigned int nc = phase.n_cols;
  unsigned int ne = phase.n_elem;
  arma::mat wphase(nr, nc);

  for (unsigned int i=0; i<ne; i++) {
    wphase(i) = fmod(phase(i) + M_PI, 2.*M_PI) - M_PI;
  }

  return wphase;
}



// The main routine. This is what gets called from R

//[[Rcpp::export]]

psitiltReturn tiltpsiC(const arma::mat& images, const arma::rowvec& phases_init, const arma::mat& coords,
              const double& ptol, const int& maxiter, const bool& trace) {
  int N = images.n_cols;
  unsigned int M = images.n_rows;
  int np = coords.n_cols + 1;

  //if (np < 3) stop("It doesn't make sense to use this algorithm without at least variable tilts.\nUse AIA or PC instead");
  //if (phases_init.n_elem != N) stop("# phases must match # of images");
  //if (coords.n_rows != M) stop("coordinates must have same # rows as image matrix");

  arma::rowvec phases = phases_init - phases_init(0);

  arma::mat S(3, N);
  arma::mat Phi(M, 3);
  arma::vec phi(M);
  arma::vec mod(M);
  arma::vec res(M);
  arma::mat zcs = arma::zeros<arma::mat>(np-1, N);
  arma::vec t0(np-1);
  arma::vec sse = arma::zeros<arma::vec>(maxiter);
  double abar, bbar;
  QMap<char*,arma::mat> adata;
  adata["coords"] = coords;
  arma::vec pars(np);
  arma::mat pt_last = join_cols(phases, zcs);
  arma::mat pt = pt_last;
  double dpt;
  int i;
  lmreturn lmrets;

  S = arma::join_cols(arma::join_cols(arma::ones<arma::rowvec>(N), arma::cos(phases)), arma::sin(phases));
  Phi = images * pinv(S);

  for (i=0; i<maxiter; i++) {
    abar = arma::mean(Phi.col(0));
    bbar = arma::mean(arma::sqrt(arma::square(Phi.col(1)) + arma::square(Phi.col(2))));
    phi = arma::atan2(-Phi.col(2), Phi.col(1));

    adata["phi"] = phi;

    for (int n=0; n<N; n++) {
      pars(0) = phases(n);
      pars.tail(np-1) = zcs.col(n);
      adata["img"] = images.col(n);
      lmrets = mylevmar((*res_frame),
                      (*jac_frame),
                      M, np, pars, adata, abar, bbar);
      if (!lmrets.convergence) QMessageBox::warning(0, "","lm convergence reported failed");
      pars = lmrets.x;
      res = res_frame(pars, adata, abar, bbar);
      sse(i) = sse(i) + pow((double) arma::norm(res, 2), 2);
      phases(n) = pars(0);
      zcs.col(n) = pars.tail(np-1);
    }
    phases = phases - phases(0);
    t0 = zcs.col(0);
    zcs.each_col() -= t0;
    pt = join_cols(phases, zcs);
    dpt = norm(pt - pt_last, 2);
    if (trace) {
      qDebug() << "Iteration " << i << " sse = " << sse(i) << " dpt = " << dpt << endl;
    }
    if (dpt < ptol) break;
    Phi = pxls(images, phases, zcs, coords);
    pt_last = pt;
  }
  Phi = pxls(images, phases, zcs, coords);
  phi = atan2(-Phi.col(2), Phi.col(1));
  mod = sqrt(square(Phi.col(1)) + square(Phi.col(2)));
  mod = mod/max(mod);
  psitiltReturn list  = { phi, mod, pwrap(phases), zcs/(2.*M_PI),i, sse};
  return list;
}
