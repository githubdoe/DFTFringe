#ifndef SETTINGSSTORES_H
#define SETTINGSSTORES_H

#include <QString>

class QSettings;

#include "settingsstores_fields.inc"

#define SETTINGS_STORE_DECLARE_STRUCT_FIELD(type, name, defaultValue, key, converter) type name = defaultValue;


struct MirrorSettings {
    SETTINGS_STORE_FOR_EACH_MIRROR_FIELD(SETTINGS_STORE_DECLARE_STRUCT_FIELD)
};

class MirrorSettingsStore {
private:
    friend class SettingsFacade;  // Only facade can construct
    MirrorSettingsStore() = default;
    
public:
    MirrorSettings load() const;
    void save(const MirrorSettings &value) const;
};



struct GeneralProcessingSettings {
    SETTINGS_STORE_FOR_EACH_GENERAL_FIELD(SETTINGS_STORE_DECLARE_STRUCT_FIELD)
};

class GeneralProcessingSettingsStore {
private:
    friend class SettingsFacade;
    GeneralProcessingSettingsStore() = default;
    
public:
    GeneralProcessingSettings load() const;
    void save(const GeneralProcessingSettings &value) const;
};



struct ContourSettings {
    SETTINGS_STORE_FOR_EACH_CONTOUR_FIELD(SETTINGS_STORE_DECLARE_STRUCT_FIELD)
};

class ContourSettingsStore {
private:
    friend class SettingsFacade;
    ContourSettingsStore() = default;
    
public:
    ContourSettings load() const;
    void save(const ContourSettings &value) const;
};



struct ApplicationSettings {
    SETTINGS_STORE_FOR_EACH_APPLICATION_FIELD(SETTINGS_STORE_DECLARE_STRUCT_FIELD)
};

class ApplicationSettingsStore {
private:
    friend class SettingsFacade;
    ApplicationSettingsStore() = default;
    
public:
    ApplicationSettings load() const;
    void save(const ApplicationSettings &value) const;
};

#undef SETTINGS_STORE_DECLARE_STRUCT_FIELD

#endif // SETTINGSSTORES_H
