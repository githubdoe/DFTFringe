#ifndef HOTKEYSDLG_H
#define HOTKEYSDLG_H

#include <QDialog>

namespace Ui {
class HotKeysDlg;
}

class HotKeysDlg : public QDialog
{
    Q_OBJECT

public:
    explicit HotKeysDlg(QWidget *parent = nullptr);
    ~HotKeysDlg();

private:
    Ui::HotKeysDlg *ui;
};

#endif // HOTKEYSDLG_H
