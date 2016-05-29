#ifndef CCSWAPPEDDLG_H
#define CCSWAPPEDDLG_H

#include <QDialog>

namespace Ui {
class CCSwappedDlg;
}
enum  selection {NONE, POSITIVE, NEGATIVE};
class CCSwappedDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CCSwappedDlg(QWidget *parent = 0);
    ~CCSwappedDlg();
    selection getSelection();

private:
    Ui::CCSwappedDlg *ui;
};

#endif // CCSWAPPEDDLG_H
