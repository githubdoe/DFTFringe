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
    /** @brief Returns process-wide facade instance. */
    static SettingsFacade &instance();

    /** @brief Accessor for mirror settings persistence. */
    MirrorSettingsStore &mirrorStore();
    /** @brief Const accessor for mirror settings persistence. */
    const MirrorSettingsStore &mirrorStore() const;

    /** @brief Accessor for general processing settings persistence. */
    GeneralProcessingSettingsStore &generalProcessingStore();
    /** @brief Const accessor for general processing settings persistence. */
    const GeneralProcessingSettingsStore &generalProcessingStore() const;

    /** @brief Accessor for contour settings persistence. */
    ContourSettingsStore &contourStore();
    /** @brief Const accessor for contour settings persistence. */
    const ContourSettingsStore &contourStore() const;

private:
    SettingsFacade() = default;

    MirrorSettingsStore m_mirrorStore;
    GeneralProcessingSettingsStore m_generalProcessingStore;
    ContourSettingsStore m_contourStore;
};

#endif // SETTINGSFACADE_H
