#ifndef LENSDIALOG_H
#define LENSDIALOG_H

#include <QDialog>
#include <QStringList>
namespace Ui {
class lensDialog;
}

class lensDialog : public QDialog
{
    Q_OBJECT

public:
    explicit lensDialog(QStringList parms, QWidget *parent = 0);
    ~lensDialog();
    QStringList m_parms;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::lensDialog *ui;
};

#endif // LENSDIALOG_H
