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
#include "settings2.h"
#include "ui_settings2.h"

settingsIGram *Settings2::m_igram = 0;
settingsDFT *Settings2::m_dft = 0;
settingsDebug *Settings2::m_debug = 0;
settingsProfile *Settings2::m_profile = 0;
SettingsGeneral2 *Settings2::m_general = 0;
Settings2::Settings2(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings2)
{
    ui->setupUi(this);
    ui->listWidget->addItem(QString("Interferogram"));
    ui->listWidget->addItem(QString("DFT"));
    ui->listWidget->addItem(QString("Profile colors"));
    ui->listWidget->addItem(QString("General"));
    ui->listWidget->addItem(QString("Debug"));
    m_igram = new settingsIGram();
    ui->stackedWidget->addWidget(m_igram);
    m_dft = new settingsDFT();
    m_debug = new settingsDebug();
    m_profile = new settingsProfile();
    m_general = new SettingsGeneral2();
    ui->stackedWidget->setCurrentIndex(0);
    ui->stackedWidget->addWidget(m_dft);
    ui->stackedWidget->addWidget(m_profile);
    ui->stackedWidget->addWidget(m_general);
    ui->stackedWidget->addWidget(m_debug);

}
Settings2 *Settings2::m_Instance = NULL;
Settings2 *Settings2::getInstance(){
    if (m_Instance == NULL) {
        m_Instance = new Settings2(0);

    }
     return m_Instance;
}

Settings2::~Settings2()
{
    delete ui;
}


bool Settings2::showDFT(){
    return m_dft->showThumb();
}
bool Settings2::showMask(){
    return m_debug->showMask();
}


void Settings2::on_listWidget_clicked(const QModelIndex &index)
{
    ui->stackedWidget->setCurrentIndex(index.row());
}

