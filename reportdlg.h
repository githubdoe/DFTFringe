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
     bool show_profile;
    bool show_igram;
    QString fileName;
    QString title;
    QPrinter *m_printer;
    double contourWidth;
    double surfaceWidth;
    double profileWidth;
    double ronchiWidth;
    double igramWidth;
    double histoWidth;
    double startestWidth;



private slots:

    void on_buttonBox_accepted();

    void on_contourWidth_valueChanged(double arg1);

    void on_igramWidth_valueChanged(double arg1);

    void on_SurfaceWidth_valueChanged(double arg1);

    void on_profileWidth_valueChanged(double arg1);

    void on_ronchiWidth_valueChanged(double arg1);

    void on_pixelHistoWidth_valueChanged(double arg1);

    void on_startestWidth_valueChanged(double arg1);


public:
    Ui::ReportDlg *ui;
};

#endif // REPORTDLG_H
