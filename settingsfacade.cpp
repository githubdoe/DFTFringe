#include "settingsfacade.h"

SettingsFacade &SettingsFacade::instance()
{
    static SettingsFacade facade;
    return facade;
}

MirrorSettingsStore &SettingsFacade::mirrorStore()
{
    return m_mirrorStore;
}

const MirrorSettingsStore &SettingsFacade::mirrorStore() const
{
    return m_mirrorStore;
}

GeneralProcessingSettingsStore &SettingsFacade::generalProcessingStore()
{
    return m_generalProcessingStore;
}

const GeneralProcessingSettingsStore &SettingsFacade::generalProcessingStore() const
{
    return m_generalProcessingStore;
}

ContourSettingsStore &SettingsFacade::contourStore()
{
    return m_contourStore;
}

const ContourSettingsStore &SettingsFacade::contourStore() const
{
    return m_contourStore;
}
