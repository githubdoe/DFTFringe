#include "colorchannel.h"
#include <qsettings.h>
colorChannel *colorChannel::m_Instance = 0;
colorChannel *colorChannel::get_instance(){
    if  (m_Instance == NULL){
        m_Instance = new colorChannel();
    }
    return m_Instance;
}

colorChannel::colorChannel(QObject *parent) : QObject(parent), useAuto(true), useRed(false),
    useGreen(false), useBlue(false), m_showOriginalColorImage(false)
{
    QSettings set;
    useAuto = set.value("colorChannelUseAuto", true).toBool();
    useRed = set.value("colorChannelUseRed",false).toBool();
    useGreen = set.value("colorChannelUseGreen", false).toBool();
    useBlue = set.value("colorChannelUseBlue", false).toBool();
    m_showOriginalColorImage = set.value("colorChannelShowColor", false).toBool();
}
void colorChannel::clearAll(){
    useAuto = false;
    useRed = false;
    useGreen = false;
    useBlue = false;
}

void colorChannel::setAuto(bool flag){
    clearAll();
    useAuto = flag;

    emit useChannelsChanged();
}
void colorChannel::setRed(bool flag){
    clearAll();
    useRed = flag;

    emit useChannelsChanged();

}
void colorChannel::setGreen(bool flag){
    clearAll();
    useGreen = flag;
    emit useChannelsChanged();
}
void colorChannel::setBlue(bool flag){
    clearAll();
    useBlue = flag;
    emit useChannelsChanged();
}
void colorChannel::showOriginalColorImage(bool flag){
    m_showOriginalColorImage = flag;
    emit useChannelsChanged();
}
