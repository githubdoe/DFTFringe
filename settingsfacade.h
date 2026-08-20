#ifndef SETTINGSFACADE_H
#define SETTINGSFACADE_H

#include "settingsstores.h"

// Forward declaration for friend access
class mirrorDlg;

/**
 * @brief Thin entry point that delegates to domain-specific settings stores.
 *
 * This class intentionally stays small; domain behavior belongs in internal
 * stores such as MirrorSettingsStore or ContourSettingsStore.
 *
 * Access control: saveMirrorSettings() is restricted to mirrordlg (via friend)
 * to ensure mirror configuration is saved only from the dialog's OK button.
 */
class SettingsFacade
{
    friend class mirrorDlg;  // Only mirrordlg can call saveMirrorSettings()
public:

    static SettingsFacade &instance();

    MirrorSettingsStore &mirrorStore();
    const MirrorSettingsStore &mirrorStore() const;

    ApplicationSettingsStore &appStore();
    const ApplicationSettingsStore &appStore() const;

    // examples for future PRs
    //GeneralProcessingSettingsStore &generalProcessingStore();
    //const GeneralProcessingSettingsStore &generalProcessingStore() const;
//
    //ContourSettingsStore &contourStore();
    //const ContourSettingsStore &contourStore() const;


private:
    friend class mirrorDlg;  // Allow mirrordlg to call restricted save
    
    /** @brief Restricted save for mirror settings (mirrordlg only via friend).
     *  Ensures single source of truth: only the mirror dialog's OK button can persist changes. 
     *  Nobody will be able to save mirror settings directly elsewhere in the code. */
    void saveMirrorSettings(const MirrorSettings &settings);
    
    SettingsFacade() = default; // Enforce singleton

    // Only facade owns these
    MirrorSettingsStore m_mirrorStore;
    ApplicationSettingsStore m_appStore;
    // examples for future PRs
    //GeneralProcessingSettingsStore m_generalProcessingStore;
    //ContourSettingsStore m_contourStore;
};

#endif // SETTINGSFACADE_H
