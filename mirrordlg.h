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

namespace Ui {
class mirrorDlg;
}
enum outlineShape {CIRCLE,ELLIPSE,RECTANGLE};
class mirrorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit mirrorDlg(QWidget *parent = 0);
    ~mirrorDlg();
    static mirrorDlg *get_Instance();
    void loadFile(QString & fileName);
    void updateZ8();

    QString m_name;
    bool mm;
    double diameter;
    double roc;
    double FNumber;
    double obs; // obstruction
    double cc;
    bool doNull;
    double z8;
    double lambda;
    double fringeSpacing;
    bool flipv;
    bool fliph;
    double m_clearAperature;
    double aperatureReduction;
    static QString m_projectPath;
    void on_roc_Changed(const double newVal);
    void on_diameter_Changed(const double diam);
    bool shouldFlipH();
    static QString getProjectPath();
    bool m_obsChanged;
    void newLambda(QString v);
    double getMinorAxis();
    bool m_majorHorizontal;
    double m_verticalAxis;
    outlineShape m_outlineShape;
    bool isEllipse();
    void setMinorAxis(double val);
    void changeEdgeMaskvalues(double val);
    bool m_aperatureReductionEnabled;
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

private:
    Ui::mirrorDlg *ui;
    static mirrorDlg *m_Instance;
    bool m_aperatureReductionValueChanged;
    QTimer spacingChangeTimer;
    void setclearAp();


};

#endif // MIRRORDLG_H
