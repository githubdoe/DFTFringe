#include "settingsstores.h"

#include <QSettings>

#define SETTINGS_STORE_LOAD_FIELD_FROM_QSETTINGS(type, name, defaultValue, key, converter) \
    value.name = s.value(key, defaultValue).converter();

#define SETTINGS_STORE_SAVE_FIELD_TO_QSETTINGS(type, name, defaultValue, key, converter) \
    s.setValue(key, value.name);

MirrorSettings MirrorSettingsStore::load() const
{
    QSettings s;

    MirrorSettings value{};
    SETTINGS_STORE_FOR_EACH_MIRROR_FIELD(SETTINGS_STORE_LOAD_FIELD_FROM_QSETTINGS)

    return value;
}

void MirrorSettingsStore::save(const MirrorSettings &value) const
{
    QSettings s;
    SETTINGS_STORE_FOR_EACH_MIRROR_FIELD(SETTINGS_STORE_SAVE_FIELD_TO_QSETTINGS)
}

GeneralProcessingSettings GeneralProcessingSettingsStore::load() const
{
    QSettings s;

    GeneralProcessingSettings value{};
    SETTINGS_STORE_FOR_EACH_GENERAL_FIELD(SETTINGS_STORE_LOAD_FIELD_FROM_QSETTINGS)

    return value;
}

void GeneralProcessingSettingsStore::save(const GeneralProcessingSettings &value) const
{
    QSettings s;
    SETTINGS_STORE_FOR_EACH_GENERAL_FIELD(SETTINGS_STORE_SAVE_FIELD_TO_QSETTINGS)
}

ContourSettings ContourSettingsStore::load() const
{
    QSettings s;

    ContourSettings value{};
    SETTINGS_STORE_FOR_EACH_CONTOUR_FIELD(SETTINGS_STORE_LOAD_FIELD_FROM_QSETTINGS)

    return value;
}

void ContourSettingsStore::save(const ContourSettings &value) const
{
    QSettings s;
    SETTINGS_STORE_FOR_EACH_CONTOUR_FIELD(SETTINGS_STORE_SAVE_FIELD_TO_QSETTINGS)
}

ApplicationSettings ApplicationSettingsStore::load() const
{
    QSettings s;

    ApplicationSettings value{};
    SETTINGS_STORE_FOR_EACH_APPLICATION_FIELD(SETTINGS_STORE_LOAD_FIELD_FROM_QSETTINGS)

    return value;
}

void ApplicationSettingsStore::save(const ApplicationSettings &value) const
{
    QSettings s;
    SETTINGS_STORE_FOR_EACH_APPLICATION_FIELD(SETTINGS_STORE_SAVE_FIELD_TO_QSETTINGS)
}

#undef SETTINGS_STORE_LOAD_FIELD_FROM_QSETTINGS
#undef SETTINGS_STORE_SAVE_FIELD_TO_QSETTINGS
