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
#include "utils.h"
waveFrontLoader::waveFrontLoader(QObject *parent) :
    QObject(parent),  done(true),shouldCancel(false)
{
    pd = new QProgressDialog("    Loading wavefronts in PRogress.", "Cancel", 0, 100);
    connect(pd, SIGNAL(canceled()), this, SLOT(cancel()));
    connect(this, SIGNAL(status(int)), pd, SLOT(setValue(int)));
    connect(this, SIGNAL(progressRange(int,int)), pd, SLOT(setRange(int,int)));
    connect(this, SIGNAL(currentWavefront(QString)), pd, SLOT(setLabelText(QString)));
}

void waveFrontLoader::addWavefront(QString filename){
    mutex.lock();
    m_list << filename;
    mutex.unlock();
}

void waveFrontLoader::cancel(){
    shouldCancel = true;
    qDebug() << "trying to cancel";
}

void waveFrontLoader::loadx(QStringList list, SurfaceManager *sm){
    m_list = list;
    loadx(sm);
}

void waveFrontLoader::loadx( SurfaceManager *sm){

    shouldCancel = false;
    int prog = 0;
    int size = m_list.size();

    emit status(0);
    done = false;

    emit progressRange(0, size);
    sm->okToUpdateSurfacesOnGenerateComplete = false;
    try {
        while (m_list.size() > 0){
            int left = showmem();
            qDebug() << left;
            if (left < 300){
                qDebug() << "low memory";
                sm->okToContinue = -1;
                emit memoryLow();
                while(sm->okToContinue == -1){}
                qDebug() << "continue code" << sm->okToContinue;
                if (sm->okToContinue == QMessageBox::No)
                {
                    qDebug() << "Abort load";
                    break;
                }
                qDebug() << "Continue load";

            }
            mutex.lock();
            QString file = m_list.front();
            m_list.removeAt(0);
            mutex.unlock();

            if (pd->wasCanceled())
                break;

            emit currentWavefront(file);

            emit status(++prog);
            sm->loadWavefront(file);
        }
    }
    catch (const std::bad_alloc &ex)
    {
        // clean up here, e.g. save the session
        // and close all config files.

        return;
    }

    emit loadComplete();
    sm->okToUpdateSurfacesOnGenerateComplete = true;
    done = true;
    emit progressRange(0, 1);
    emit status(0);
    emit status(1);

}

