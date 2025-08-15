#ifndef PERCENTCORRECTIONSURFACE_H
#include <QColor>
#define PERCENTCORRECTIONSURFACE_H
class surfaceData {
    public:

    double igramlambda;
    QColor penColor;
  // set of zernike values at each zone (rho)
    std::vector<double> zernvalues;
    QString m_name;

    surfaceData( double igramlambda, QColor pen, const std::vector<double> &zernvalues, const QString &name): igramlambda(igramlambda),
        penColor(pen), zernvalues(zernvalues){ m_name = name;};
} ;
#endif // PERCENTCORRECTIONSURFACE_H
