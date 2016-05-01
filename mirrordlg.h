#ifndef MIRRORDLG_H
#define MIRRORDLG_H

#include <QDialog>

namespace Ui {
class mirrorDlg;
}

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
    double obs;
    double cc;
    bool doNull;
    double z8;
    double lambda;
    double fringeSpacing;
    bool flipv;
    bool fliph;
    static QString m_projectPath;
    void on_roc_Changed(const double newVal);
    void on_diameter_Changed(const double diam);
    bool shouldFlipH();
    static QString getProjectPath();
    bool m_obsChanged;

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

    void on_fringeSpacingEdit_textChanged(const QString &arg1);

signals:
    void diameterChanged(double);
    void rocChanged(double);
    void lambdaChanged(double);
    void saNullChanged(double);
    void CCChanged(double);
    void obstructionChanged();
    void newPath(QString);
    void recomputeZerns();

private:
    Ui::mirrorDlg *ui;
    static mirrorDlg *m_Instance;
};

#endif // MIRRORDLG_H
