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
#ifndef MIRRORDLG_H
#define MIRRORDLG_H

#include <QDialog>
#include <QTimer>
#include "autoinvertdlg.h"
#include "settingsfacade.h"

namespace Ui {
class mirrorDlg;
}
enum outlineShape {CIRCLE,ELLIPSE,RECTANGLE};
class mirrorDlg : public QDialog
{
    Q_OBJECT

public:
    //TODO if we get rid of the singleton design, settings are read from settingsFacade instead here
    // check if it makes sense
    static mirrorDlg *get_Instance();
    ~mirrorDlg();
    mirrorDlg(const mirrorDlg&) = delete;
    mirrorDlg& operator=(const mirrorDlg&) = delete;

    // File and configuration operations
    void loadFile(QString & fileName);
    void updateZ8();
    void updateAutoInvertStatus();

    // Computed/derived values (read-only, not from settings)
    bool mm;
    double FNumber;
    double z8;
    static QString m_projectPath;
    
    // State flags
    bool m_obsChanged;
    bool m_majorHorizontal;

    // Methods for configuration access/modification
    void on_roc_Changed(const double newVal);
    void on_diameter_Changed(const double diam);
    bool shouldFlipH();
    static QString getProjectPath();
    void newLambda(const QString &v);
    double getMinorAxis();
    bool isEllipse();
    void setMinorAxis(double val);
    void setVerticalAxis(double val);
    void setOutlineShape(outlineShape shape);
    void setObsPercent(double obs);
    
    /** @brief Access current mirror settings.
     *  Returns the persistent copy - the last saved state.
     *  External code reads this as source of truth. */
    const MirrorSettings& currentSettings() const { return m_current; }
    
private:
    // Persistent mirror configuration copy (source of truth for external code)
    MirrorSettings m_current;
    
    // Working copy for dialog edits (discarded on Cancel, committed to m_current on OK)
    MirrorSettings m_draft;
    
private slots:
    void on_ReadBtn_clicked();

    void on_diameter_textChanged(const QString &arg1);

    void on_roc_textChanged(const QString &arg1);

    void on_FNumber_textChanged(const QString &arg1);

    void on_obs_textChanged(const QString &arg1);

    void on_lambda_textChanged(const QString &arg1);

    void on_nullCB_clicked(bool checked);

    void on_unitsCB_clicked(bool checked);

    void on_buttonBox_accepted();

    void on_saveBtn_clicked();


    void on_cc_textChanged(const QString &arg1);

    void on_fringeSpacingEdit_textChanged(const QString &text);

    void on_name_editingFinished();

    void on_minorAxisEdit_textChanged(const QString &arg1);
    void spacingChangeTimeout();

    void on_ellipseShape_clicked(bool checked);

    void on_buttonBox_helpRequested();

    void on_ReducApp_clicked(bool checked);

    void on_reduceValue_valueChanged(double arg1);

    void on_annulusPercent_valueChanged(double arg1);

    void on_useAnnulus_clicked(bool checked);

    void on_annulusHelp_clicked();

    void on_annularDiameter_valueChanged(double arg1);

    void on_btnChangeAutoInvert_clicked();

signals:
    // TODO some of these are not used. 
    // also notify shoud probably only happen when OK is clicked, not on every change.
    void diameterChanged(double);
    void rocChanged(double);
    void lambdaChanged(double);
    void saNullChanged(double);
    void CCChanged(double);
    void obstructionChanged();
    void newPath(QString);
    void recomputeZerns();
    void aperatureChanged();

protected:
    /** @brief Reload settings before dialog becomes visible. */
    void showEvent(QShowEvent *event) override;

private:
    explicit mirrorDlg(QWidget *parent = 0);
    void setclearAp();
    
    /** @brief Load draft from persistent settings before dialog is shown.
     *  Ensures Cancel always reverts to the last saved state. */
    void loadDraftFromSettings();

    Ui::mirrorDlg *ui;
    bool m_aperatureReductionValueChanged;
    QTimer spacingChangeTimer;
    void saveJson(const QString &fileName);
    void enableAnnular(bool enable);
    
};

#endif // MIRRORDLG_H
