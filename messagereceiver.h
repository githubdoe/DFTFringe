#ifndef MESSAGERECEIVER_H
#define MESSAGERECEIVER_H

#include <QObject>
#include "mainwindow.h"
class MessageReceiver : public QObject
{
    Q_OBJECT
public:
    explicit MessageReceiver(QObject *parent = 0);
    MainWindow *m_mainWindow;
public slots:
    void receivedMessage( int instanceId, QByteArray message );
};

#endif // MESSAGERECEIVER_H
