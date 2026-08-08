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

ApplicationSettingsStore &SettingsFacade::appStore()
{
    return m_appStore;
}

const ApplicationSettingsStore &SettingsFacade::appStore() const
{
    return m_appStore;
}

void SettingsFacade::saveMirrorSettings(const MirrorSettings &settings)
{
    m_mirrorStore.save(settings);
}
