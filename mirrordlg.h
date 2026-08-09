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
    // TODO if we get rid of the singleton design, settings could be read from settingsFacade instead here
    // check if it makes sense
    static mirrorDlg *get_Instance();
    ~mirrorDlg();
    mirrorDlg(const mirrorDlg&) = delete;
    mirrorDlg& operator=(const mirrorDlg&) = delete;

    void updateAutoInvertStatus(); //This one makes sense. Not saved

    // ---- file loading ----
    //TODO This one not OK. Edits m_draft.ellipseMinorAxis and wont get saved. 
    // need to investigate why external code needs to change the minor axis. If it is a user preference, it should be saved in settings. If it is a computed value, it should be computed from other values and not set directly.
    void setMinorAxis(double val); 
    // TODO not OK. Edits m_draft.outlineShape and wont get saved
    // from file load
    void setOutlineShape(outlineShape shape);
    

    // Apply loaded wavefront settings to both runtime and persisted mirror settings.
    // Intended for a single call after wavefront load mismatch decisions are finalized.
    void adoptWavefrontSettings(double diameter, double roc, double lambda);

    
    // Computed/derived value accessors (read-only)
    double getFNumber() const { return FNumber; }
    double getZ8() const { return z8; }
    static QString getProjectPath();
    double getMinorAxis();
    bool isEllipse();
    bool shouldFlipH();

    
    /** @brief Access current mirror settings.
     *  Returns the persistent copy - the last saved state.
     *  External code reads this as source of truth. */
    const MirrorSettings& currentSettings() const { return m_current; }
    
private:

    
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
    // Emitted only after OK/accept when committed settings are saved.
    void obstructionChanged();
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

    // State flags
    bool m_aperatureReductionValueChanged;
    bool m_obsChanged;

    QTimer spacingChangeTimer;

    // Persistent mirror configuration copy (source of truth)
    MirrorSettings m_current;
    
    // Working copy for dialog edits (discarded on Cancel, committed to m_current on OK)
    MirrorSettings m_draft;
    
    // Computed/derived values (read-only, not from settings)
    //TODO actually mm is not saved in settings. should probably be saved as it's a user preference 
    bool mm;                           // Unit display flag: true = mm, false = other units
    double FNumber;                    // Computed f-number (focal length / diameter)
    double z8;                         // Z8 Zernike coefficient or null reference value
    static QString m_projectPath;      // Current project directory path
    
    void saveJson(const QString &fileName);
    void enableAnnular(bool enable);
    void updateZ8();
    void loadFile(QString & fileName);
    
};

#endif // MIRRORDLG_H
