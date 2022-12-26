#ifndef ARBITRARYWAVEFRONTHELP_H
#define ARBITRARYWAVEFRONTHELP_H

#include <QDialog>

namespace Ui {
class ArbitraryWavefrontHelp;
}

class ArbitraryWavefrontHelp : public QDialog
{
    Q_OBJECT

public:
    explicit ArbitraryWavefrontHelp(QWidget *parent = nullptr);
    ~ArbitraryWavefrontHelp();

private slots:
    void on_pushButton_clicked();

private:
    Ui::ArbitraryWavefrontHelp *ui;
};

#endif // ARBITRARYWAVEFRONTHELP_H
