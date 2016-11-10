#ifndef GENERATETARGETDLG_H
#define GENERATETARGETDLG_H

#include <QDialog>

namespace Ui {
class generateTargetDlg;
}

class generateTargetDlg : public QDialog
{
    Q_OBJECT
    enum units {MM, INCHES, PIXELS};
public:
    explicit generateTargetDlg(QWidget *parent = 0);
    ~generateTargetDlg();

private slots:
    void on_generate_clicked();

    void on_printed_clicked(bool checked);

    void on_fullScreen_clicked(bool checked);

    void on_rows_valueChanged(int arg1);

    void on_useChecker_toggled(bool checked);

    void on_lineGridRb_clicked();

    void on_usePixels_clicked(bool checked);
    void on_useInches_clicked(bool checked);

    void on_useMm_clicked(bool checked);

private:
    Ui::generateTargetDlg *ui;
    int m_dpix, m_dpiy;
    units m_units;
    bool m_usePixels;
};

#endif // GENERATETARGETDLG_H
