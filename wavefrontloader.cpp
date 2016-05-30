/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
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
        qDebug() << " loading " << i;
        mirrorConfigChanged |= sm->loadWavefront(list[i]);
    }
    emit status(list.size()+1);

}

