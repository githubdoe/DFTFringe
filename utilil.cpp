#include <QDebug>

#if defined(_WIN32)
#include <Windows.h>
#define WIDTH 12
#define DIV 1048576ull

long showmem(QString /*title*/){

    //qDebug () << title;
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx (&statex);
         return statex.ullAvailVirtual/DIV;
    qDebug() <<QString("%1% in use.").arg(statex.dwMemoryLoad, WIDTH);

     qDebug() <<QString("%1 free  MB of physical memory.").arg(
              statex.ullAvailPhys/DIV, WIDTH);


     qDebug() <<QString("%1 free  MB of paging file.").arg(
              statex.ullAvailPageFile/DIV, WIDTH);
     qDebug() <<QString("%1 total MB of virtual memory.").arg(
              statex.ullTotalVirtual/DIV, WIDTH);
     qDebug() <<QString("%1 free  MB of virtual memory.").arg(
              statex.ullAvailVirtual/DIV, WIDTH);

     return statex.ullAvailVirtual/DIV;
}
#else
int showmem(QString){return 1000;}
#endif
