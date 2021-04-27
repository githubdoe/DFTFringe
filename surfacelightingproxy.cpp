#include "surfacelightingproxy.h"


surfaceLightingProxy::surfaceLightingProxy(SurfaceGraph *sg)
{
    m_dlg = new Surface3dControlsDlg(sg);

}
void surfaceLightingProxy::show(){
    m_dlg->show();
}
