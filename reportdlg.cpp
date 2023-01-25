#include "reportdlg.h"
#include "ui_reportdlg.h"
#include <qfiledialog.h>
#include <QSettings>
#include "mirrordlg.h"
#include <QSettings>
ReportDlg::ReportDlg(QPrinter *p, QWidget *parent) :
     QDialog(parent),m_printer(p),
    ui(new Ui::ReportDlg)
{
    ui->setupUi(this);
    ui->title->setText( mirrorDlg::get_Instance()->m_name);
    ui->DPI->setValue(m_printer->resolution());

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
    QString path = set.value("ReportFilePath",
                 QString(mirrorDlg::get_Instance()->getProjectPath())).toString();
    fileName = QFileDialog::getSaveFileName((QWidget* )0, "Save PDF report",
             path + "/Report.pdf", "*.pdf");
    path = QFileInfo(fileName).absolutePath();
    set.setValue("ReportFilePath", path);
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    title = ui->title->text();
}
void ReportDlg::setDPI(int val){
    ui->DPI->setValue(val);
}
int ReportDlg::getDPI(){
    return ui->DPI->value();
}

void ReportDlg::on_DPI_valueChanged(int arg1)
{
    m_printer->setResolution(arg1);
    ui->width->setText(QString("{%1}").arg(m_printer->width()));
    ui->height->setText(QString("{%1}").arg(m_printer->height()));
}

