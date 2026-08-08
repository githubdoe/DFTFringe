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
public:
    
    MirrorSettings load() const;
    void save(const MirrorSettings &value) const;
};



struct GeneralProcessingSettings {
    SETTINGS_STORE_FOR_EACH_GENERAL_FIELD(SETTINGS_STORE_DECLARE_STRUCT_FIELD)
};

class GeneralProcessingSettingsStore {
public:

    GeneralProcessingSettings load() const;
    void save(const GeneralProcessingSettings &value) const;
};



struct ContourSettings {
    SETTINGS_STORE_FOR_EACH_CONTOUR_FIELD(SETTINGS_STORE_DECLARE_STRUCT_FIELD)
};

class ContourSettingsStore {
public:

    ContourSettings load() const;
    void save(const ContourSettings &value) const;
};

#undef SETTINGS_STORE_DECLARE_STRUCT_FIELD

#endif // SETTINGSSTORES_H
