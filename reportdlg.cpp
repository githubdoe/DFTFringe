#include "reportdlg.h"
#include "ui_reportdlg.h"
#include <qfiledialog.h>
#include <QSettings>
#include "mirrordlg.h"
#include <QSettings>
ReportDlg::ReportDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReportDlg)
{
    ui->setupUi(this);
    ui->title->setText( mirrorDlg::get_Instance()->m_name);
}

ReportDlg::~ReportDlg()
{
    delete ui;
}



void ReportDlg::on_buttonBox_accepted()
{
    show_foucault = ui->foucault->isChecked();
    show_histogram = ui->histogram->isChecked();
    show_startest = ui->starTest->isChecked();
    show_profile = ui->ProfilePlot->isChecked();
    show_igram = ui->showIgram->isChecked();


    QSettings set;
    fileName = QFileDialog::getSaveFileName((QWidget* )0, "Save PDF report", QString(mirrorDlg::get_Instance()->getProjectPath() +
                                                                                        "/Report.pdf"), "*.pdf");
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    title = ui->title->text();
}
