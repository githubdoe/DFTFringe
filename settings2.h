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
#ifndef SETTINGS2_H
#define SETTINGS2_H

#include <QDialog>
#include "settingsigram.h"
#include "settingsdft.h"
#include "settingsdebug.h"
#include "settingsprofile.h"
#include "settingsgeneral2.h"
namespace Ui {
class Settings2;
}

class Settings2 : public QDialog
{
    Q_OBJECT

public:
    explicit Settings2(QWidget *parent = 0);
    static Settings2 *getInstance();
    ~Settings2();
    static settingsIGram *m_igram;
    static settingsDFT *m_dft;
    static settingsDebug *m_debug;
    static settingsProfile *m_profile;
    static SettingsGeneral2 *m_general;
    static bool showDFT();
    static int dftSize();
    static bool showMask();
    static bool shouldHflipIgram();

private slots:
    void on_listWidget_clicked(const QModelIndex &index);

private:
    Ui::Settings2 *ui;
    static Settings2 * m_Instance;
};

#endif // SETTINGS2_H
