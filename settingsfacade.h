#ifndef SETTINGSFACADE_H
#define SETTINGSFACADE_H

#include "settingsstores.h"

/**
 * @brief Thin entry point that delegates to domain-specific settings stores.
 *
 * This class intentionally stays small; domain behavior belongs in internal
 * stores such as MirrorSettingsStore or ContourSettingsStore.
 */
class SettingsFacade
{
public:

    static SettingsFacade &instance();

    MirrorSettingsStore &mirrorStore();
    const MirrorSettingsStore &mirrorStore() const;

    GeneralProcessingSettingsStore &generalProcessingStore();
    const GeneralProcessingSettingsStore &generalProcessingStore() const;

    ContourSettingsStore &contourStore();
    const ContourSettingsStore &contourStore() const;

    /** @brief Accessor for application-wide path settings (project path, file paths, etc.). */
    ApplicationSettingsStore &appStore();
    /** @brief Const accessor for application-wide path settings. */
    const ApplicationSettingsStore &appStore() const;

private:
    SettingsFacade() = default; // Enforce singleton

    // Only facade owns these
    MirrorSettingsStore m_mirrorStore;
    GeneralProcessingSettingsStore m_generalProcessingStore;
    ContourSettingsStore m_contourStore;
    ApplicationSettingsStore m_appStore;
};

#endif // SETTINGSFACADE_H
