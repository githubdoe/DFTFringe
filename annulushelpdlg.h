#ifndef ANNULUSHELPDLG_H
#define ANNULUSHELPDLG_H

#include <QDialog>

namespace Ui {
class annulusHelpDlg;
}

class annulusHelpDlg : public QDialog
{
    Q_OBJECT

public:
    explicit annulusHelpDlg(QWidget *parent = nullptr);
    ~annulusHelpDlg();

private:
    Ui::annulusHelpDlg *ui;
};

#endif // ANNULUSHELPDLG_H
