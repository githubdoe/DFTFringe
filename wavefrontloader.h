#ifndef WAVEFRONTLOADER_H
#define WAVEFRONTLOADER_H

#include <QObject>
#include <QtWidgets>
#include <QtCore>
#include "surfacemanager.h"


class waveFrontLoader : public QObject
{
    Q_OBJECT
public:
    explicit waveFrontLoader(QObject *parent = 0);

signals:
    void status(int);

    void progressRange(int,int);
    void currentWavefront(QString);
    void showMessage(QString);
    void diameterChangedSig(double);

public slots:
    void loadx(QStringList list, SurfaceManager *sm);
    void cancel();



private:
    QProgressDialog *pd;
    bool shouldCancel;
    QMutex sync;
    QWaitCondition pauseCond;
    bool pause;
    int messageResp;
};

#endif // WAVEFRONTLOADER_H
