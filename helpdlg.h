#ifndef HELPDLG_H
#define HELPDLG_H

#include <QDialog>

namespace Ui {
class HelpDlg;
}

class HelpDlg : public QDialog
{
    Q_OBJECT

public:
    explicit HelpDlg(QWidget *parent = 0);
    ~HelpDlg();

private:
    Ui::HelpDlg *ui;
};

#endif // HELPDLG_H
