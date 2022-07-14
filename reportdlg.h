#ifndef REPORTDLG_H
#define REPORTDLG_H

#include <QDialog>

namespace Ui {
class ReportDlg;
}

class ReportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDlg(QWidget *parent = 0);
    ~ReportDlg();
    bool show_startest;
    bool show_foucault;
    bool show_histogram;
    bool show_profile;
    bool show_igram;
    QString fileName;
    QString title;

private slots:

    void on_buttonBox_accepted();

private:
    Ui::ReportDlg *ui;
};

#endif // REPORTDLG_H
