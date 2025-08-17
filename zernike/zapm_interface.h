#include "armadillo"
#include <QString>
#include <Qvector>

arma::mat zapm(const arma::vec &rho, const arma::vec &theta, const double &eps,
               const int &maxorder = 12);
