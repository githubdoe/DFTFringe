#ifndef REPORTDLG_H
#define REPORTDLG_H

#include <QDialog>
#include <QPrinter>
namespace Ui {
class ReportDlg;
}

class ReportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDlg(QPrinter *printer, QWidget *parent = 0);
    ~ReportDlg();
    bool show_startest;
    bool show_foucault;
    bool show_histogram;
    bool show_profile;
    bool show_igram;
    QString fileName;
    QString title;
    QPrinter *m_printer;
    void setDPI(int);
    int getDPI();


private slots:

    void on_buttonBox_accepted();

    void on_DPI_valueChanged(int arg1);

private:
    Ui::ReportDlg *ui;
};

#endif // REPORTDLG_H
