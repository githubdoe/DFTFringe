#ifndef RENAMEWAVEFRONTDLG_H
#define RENAMEWAVEFRONTDLG_H

#include <QDialog>

namespace Ui {
class renameWavefrontDlg;
}

class renameWavefrontDlg : public QDialog
{
    Q_OBJECT

public:
    explicit renameWavefrontDlg(QString name, QWidget *parent = 0);
    ~renameWavefrontDlg();
    QString name();

private:
    Ui::renameWavefrontDlg *ui;
};

#endif // RENAMEWAVEFRONTDLG_H
