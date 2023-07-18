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
    qDebug() <<QString().sprintf("%*ld%% in use.", WIDTH, statex.dwMemoryLoad);

     qDebug() <<QString().sprintf ("%*lld free  MB of physical memory.",
              WIDTH, statex.ullAvailPhys/DIV);


     qDebug() <<QString().sprintf ("%*lld free  MB of paging file.",
              WIDTH, statex.ullAvailPageFile/DIV);
     qDebug() <<QString().sprintf ("%*lld total MB of virtual memory.",
              WIDTH, statex.ullTotalVirtual/DIV);
     qDebug() <<QString().sprintf("%*lld free  MB of virtual memory.",
              WIDTH, statex.ullAvailVirtual/DIV);

     return statex.ullAvailVirtual/DIV;
}
#else
int showmem(QString){return 1000;}
#endif
