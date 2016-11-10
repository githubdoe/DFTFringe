#ifndef NULLMARGINHELPDLG_H
#define NULLMARGINHELPDLG_H

#include <QDialog>

namespace Ui {
class nullMarginHelpDlg;
}

class nullMarginHelpDlg : public QDialog
{
    Q_OBJECT

public:
    explicit nullMarginHelpDlg(QWidget *parent = 0);
    ~nullMarginHelpDlg();

private:
    Ui::nullMarginHelpDlg *ui;
};

#endif // NULLMARGINHELPDLG_H
