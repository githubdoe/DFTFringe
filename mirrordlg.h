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
    
    /** @brief Access current mirror settings (read-only snapshot).
     *  Returns the draft which is the canonical storage for all mirror config.
     *  All internal member variables are kept in sync with this. */
    const MirrorSettings& currentSettings() const { return m_draft; }
    
private:
    // Configuration members (access via currentSettings() or setters)
    QString m_name;
    double diameter;
    double roc;
    double obs; // obstruction
    double cc;
    bool doNull;
    double lambda;
    double fringeSpacing;
    bool flipv;
    bool fliph;
    bool m_useAnnular;
    bool m_connectAnnulusToObs;
    double m_annularObsPercent; // a value from 0 to 1 (not 0 to 100)
    double m_clearAperature;
    double aperatureReduction;
    bool m_aperatureReductionEnabled;
    double m_verticalAxis;
    outlineShape m_outlineShape;
    
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
    /** @brief Reload draft settings before dialog becomes visible.
     *  Ensures Cancel always reverts to the last-saved state (issue #121). */
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
    
    /** @brief Working copy of mirror settings during dialog edit.
     *  All UI modifications update this draft. On OK, it persists via facade.
     *  On Cancel, it's discarded, leaving member variables unchanged. */
    MirrorSettings m_draft;
};

#endif // MIRRORDLG_H
