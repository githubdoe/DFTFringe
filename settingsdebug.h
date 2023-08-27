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
#ifndef SETTINGSDEBUG_H
#define SETTINGSDEBUG_H

#include <QDialog>

namespace Ui {
class settingsDebug;
}

class settingsDebug : public QDialog
{
    Q_OBJECT

public:
    explicit settingsDebug(QWidget *parent = 0);
    ~settingsDebug();
    bool showMask();
private slots:
    void on_checkBox_clicked(bool checked);
    void on_debugOutline_clicked(bool checked);
    void on_logLevelComboBox_currentTextChanged(const QString &text);

private:
    Ui::settingsDebug *ui;
};

#endif // SETTINGSDEBUG_H
