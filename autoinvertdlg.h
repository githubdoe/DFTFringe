#ifndef AUTOINVERTDLG_H
#define AUTOINVERTDLG_H

#include <QDialog>

namespace Ui {
class autoInvertDlg;
}

class autoInvertDlg : public QDialog
{
    Q_OBJECT

public:
    explicit autoInvertDlg(QWidget *parent = nullptr);
    ~autoInvertDlg();
    void setMainLabel(const QString & str);
    void enableConic(bool b);


private slots:
    void on_btnUseConic_clicked();

    void on_btnManual_clicked();

    void on_btnInside_clicked();

    void on_btnOutside_clicked();

private:
    Ui::autoInvertDlg *ui;
};

#endif // AUTOINVERTDLG_H
