#include "wavefrontloader.h"

waveFrontLoader::waveFrontLoader(QObject *parent) :
    QObject(parent), shouldCancel(false)
{

    pd = new QProgressDialog("    Loading wavefronts in PRogress.", "Cancel", 0, 100);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    connect(this, SIGNAL(status(int)), pd, SLOT(setValue(int)));
    connect(this, SIGNAL(progressRange(int,int)), pd, SLOT(setRange(int,int)));
    connect(this, SIGNAL(currentWavefront(QString)), pd, SLOT(setLabelText(QString)));
}
void waveFrontLoader::cancel(){
    shouldCancel = true;
    qDebug() << "trying to cancel";
}

void waveFrontLoader::loadx(QStringList list, SurfaceManager *sm){
    shouldCancel = false;
    emit progressRange(0,list.size()+1);
    emit status(0);
    bool mirrorConfigChanged = false;
    for (int i = 0; i < list.size(); ++i){
        if (pd->wasCanceled())
                break;
        emit currentWavefront(list[i]);
        emit status(i+1);

        mirrorConfigChanged |= sm->loadWavefront(list[i]);
    }
    emit status(list.size()+1);

}

