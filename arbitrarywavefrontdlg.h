#ifndef ARBITRARYWAVEFRONTDLG_H
#define ARBITRARYWAVEFRONTDLG_H

#include <QDialog>
#include "arbitrarywavwidget.h"
#include "qcombobox.h"
#include "qlabel.h"
#include "qplaintextedit.h"

namespace Ui {
class ArbitraryWavefrontDlg;
}

class ArbitraryWavefrontDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ArbitraryWavefrontDlg(QWidget *parent = nullptr);
    ~ArbitraryWavefrontDlg();
    ArbitraryWavWidget * pww;

    static ArbitraryWavefrontDlg* get_instance();
    void setNull(double _mirror_null);
    void setDiameter(double _mirror_diam); // unit: mm      see md.diameter
    int getSize(); // qty of pixels for wavefront

    QSize sizeHint() const override;

    bool bOkPressed=false;
    
    void prepare(int size);
    double getValue(double rho); // return a wave height value for this diameter (rho = radius from 0.0 to 1.0)



protected:
    void showEvent(QShowEvent * event) override;
    QComboBox * m_cmbMode;
    QComboBox * m_cmbUnit;
    QLineEdit * m_txtSize;
    static ArbitraryWavefrontDlg *m_instance;

private slots:
    void onCancelPressed();
    void onOkPressed();
    void onModeChange();
    void onUnitChange();
    void onHelpPressed();

private:
    Ui::ArbitraryWavefrontDlg *ui;


};

#endif // ARBITRARYWAVEFRONTDLG_H
