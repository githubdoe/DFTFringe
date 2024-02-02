#ifndef MIKESPSIINTERFACE_H
#define MIKESPSIINTERFACE_H
#include "armadillo"
arma::mat zpmC(arma::vec rho, arma::vec theta, int maxorder);

typedef struct { arma::mat phi; arma::mat mod; arma::mat phase; arma::mat zcx; int iter; arma::vec sse;} psitiltReturn;
arma::mat pxls(const arma::mat& im, const arma::rowvec& phases, const arma::mat& zcs, const arma::mat& coords);
arma::mat pwrap(const arma::mat& phase);
arma::mat rhotheta(const double rows, const double cols, double obspercent=0);
#endif // MIKESPSIINTERFACE_H
