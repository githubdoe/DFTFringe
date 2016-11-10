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
#ifndef WAVEFRONTLOADER_H
#define WAVEFRONTLOADER_H

#include <QObject>
#include <QtWidgets>
#include <QtCore>
#include "surfacemanager.h"
#include <QMutex>

class waveFrontLoader : public QObject
{
    Q_OBJECT
public:
    explicit waveFrontLoader(QObject *parent = 0);
    void addWavefront(QString filename);
    bool done;
signals:
    void status(int);

    void progressRange(int,int);
    void currentWavefront(QString);
    void showMessage(QString);
    void diameterChangedSig(double);

public slots:
    void loadx(SurfaceManager *sm);
    void loadx(QStringList list, SurfaceManager *sm);
    void cancel();



private:
    QProgressDialog *pd;
    bool shouldCancel;
    QMutex sync;
    QWaitCondition pauseCond;
    bool pause;
    int messageResp;
    QMutex mutex;
    QStringList m_list;
};

#endif // WAVEFRONTLOADER_H
