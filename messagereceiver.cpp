#include <QDebug>
#include <QString>
#include "messagereceiver.h"

MessageReceiver::MessageReceiver(QObject *parent) : QObject(parent)
{
}

void MessageReceiver::receivedMessage(int instanceId, QByteArray message)
{
    QStringList args = QString::fromUtf8(message.data()).split("\'");
    m_mainWindow->openWaveFrontonInit(args);
    qDebug() << "file" << args[1];

}
