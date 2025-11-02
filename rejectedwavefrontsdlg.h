#ifndef REJECTEDWAVEFRONTSDLG_H
#define REJECTEDWAVEFRONTSDLG_H

#include <QDialog>
#include <QStringList>
namespace Ui {
class rejectedWavefrontsDlg;
}

class rejectedWavefrontsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit rejectedWavefrontsDlg(const QStringList &list, QWidget *parent = 0);
    ~rejectedWavefrontsDlg();

private:
    Ui::rejectedWavefrontsDlg *ui;
};

#endif // REJECTEDWAVEFRONTSDLG_H
