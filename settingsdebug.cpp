/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "settingsdebug.h"
#include "ui_settingsdebug.h"
#include "spdlog/spdlog.h"
#include <qsettings.h>

settingsDebug::settingsDebug(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::settingsDebug)
{
    ui->setupUi(this);
    QSettings set;
    ui->checkBox->setChecked(set.value("DebugShowMask",false).toBool());
    ui->debugOutline->setChecked(set.value("DebugShowOutlining",false).toBool());
    ui->logLevelComboBox->setCurrentText(getLogLevel());
}

settingsDebug::~settingsDebug()
{
    delete ui;
}
bool settingsDebug::showMask(){
    return ui->checkBox->isChecked();
}

void settingsDebug::on_checkBox_clicked(bool arg)
{
    QSettings set;
    set.setValue("DebugShowMask",arg );
}

void settingsDebug::on_debugOutline_clicked(bool checked)
{
    QSettings set;
    set.setValue("DebugShowOutlining", checked);
}

void settingsDebug::on_logLevelComboBox_currentTextChanged(const QString &text)
{
    QSettings set;
    set.setValue("LogLevel", text);
    spdlog::get("logger")->set_level(spdlog::level::trace);
    spdlog::get("logger")->trace(std::string("Log level changed to: ") + text.toStdString());
    setLogLevel(text);
}

QString settingsDebug::getLogLevel()
{
    QSettings set;
    return set.value("LogLevel", "Info").toString();
}

void settingsDebug::setLogLevel(const QString& text)
{
    if( text == "Trace"){
        spdlog::get("logger")->set_level(spdlog::level::trace);
    }
    else if( text == "Debug"){
        spdlog::get("logger")->set_level(spdlog::level::debug);
    }
    else if( text == "Info"){
        spdlog::get("logger")->set_level(spdlog::level::info);
    }
    else if( text == "Warning"){
        spdlog::get("logger")->set_level(spdlog::level::warn);
    }
    else if( text == "Error"){
        spdlog::get("logger")->set_level(spdlog::level::err);
    }
    else if( text == "Critical"){
        spdlog::get("logger")->set_level(spdlog::level::critical);
    }
    else if( text == "Off"){
        spdlog::get("logger")->set_level(spdlog::level::off);
    }
    else{
        spdlog::get("logger")->warn(std::string("Unexpected log level: ") + text.toStdString());
    }
}
