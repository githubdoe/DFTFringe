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
