#ifndef SETTINGSGENERAL2_H
#define SETTINGSGENERAL2_H

#include <QDialog>

namespace Ui {
class SettingsGeneral2;
}

class SettingsGeneral2 : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsGeneral2(QWidget *parent = 0);
    ~SettingsGeneral2();
    bool useSVD();
    bool showConditionNumbers();
    bool useStarTestMake(){
        return m_useStarTestMakeOnly;
    }
    bool valsChanged;
    int m_waveFrontSize;
    bool m_downsize;
    bool shouldDownsize(){ return m_downsize;}
    int wavefrontSize(){ return m_waveFrontSize;}
    double getObs();
signals:
    void outputLambdaChanged(double val);
    void updateContourPlot();
public slots:
    void updateContour();
private slots:
    void on_checkBox_clicked(bool checked);
    void on_starTestMakeCb_clicked(bool checked);
    void on_showConditionNumbersCb_clicked(bool checked);
    void on_wavefrontSizeSb_valueChanged(int arg1);
    void on_downSizeCB_clicked(bool checked);
    void on_memThreshValue_valueChanged(int val);
    void on_AstigDistGraphWidth_valueChanged(int val);
    void on_applyOffsets_clicked(bool checked);
    void on_outputLambda_valueChanged(double val);
    void on_apply_clicked();
    void on_rulerParms_clicked();
    void on_useAnnular_clicked(bool checked);

    void on_percenObstruction_valueChanged(double arg1);

private:
    Ui::SettingsGeneral2 *ui;
    bool m_useSVD;
    bool m_showConditionNumbers;
    bool m_useStarTestMakeOnly;
};

#endif // SETTINGSGENERAL2_H
