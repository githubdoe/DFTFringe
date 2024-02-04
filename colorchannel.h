#ifndef COLORCHANNEL_H
#define COLORCHANNEL_H

#include <QObject>

class colorChannel : public QObject
{
    Q_OBJECT

    static colorChannel *m_Instance;
public:
    explicit colorChannel(QObject *parent = 0);
    static colorChannel *get_instance();
    bool useAuto;
    bool useRed;
    bool useGreen;
    bool useBlue;
    bool m_showOriginalColorImage;
    void setAuto(bool flag);
    void setRed(bool flag);
    void setGreen(bool flag);
    void setBlue(bool flag);
    void clearAll();
    void showOriginalColorImage(bool flag);

signals:
    void useChannelsChanged();
public slots:

};

#endif // COLORCHANNEL_H
