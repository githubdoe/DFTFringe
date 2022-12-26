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

    void setNull(double _mirror_null);
    void setDiameter(double _mirror_diam); // unit: mm      see md.diameter

    QSize sizeHint() const override;

    bool bOkPressed=false;



protected:
    void showEvent(QShowEvent * event) override;
    QComboBox * cmbMode;
    QComboBox * cmbUnit;

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
