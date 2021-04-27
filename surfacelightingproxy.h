#ifndef SURFACELIGHTINGPROXY_H
#define SURFACELIGHTINGPROXY_H
#include "surface3dcontrolsdlg.h"
class SurfaceGraph;
class surfaceLightingProxy
{
    Surface3dControlsDlg *m_dlg;
public:
    surfaceLightingProxy(SurfaceGraph *sg);
    void show();
};

#endif // SURFACELIGHTINGPROXY_H
