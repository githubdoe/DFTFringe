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
    QSettings set;
    contourWidth = set.value("ReprotContourWidth", 1.).toDouble();
    ui->contourWidth->setValue(contourWidth);
    surfaceWidth = set.value("ReportSurfaceWidth", 1.).toDouble();
    ui->SurfaceWidth->setValue(surfaceWidth);
    igramWidth = set.value("ReprotIgramWidth", 1.).toDouble();
    ui->igramWidth->setValue(igramWidth);
    profileWidth = set.value("ReprotProfileWidth", 1.).toDouble();
    ui->profileWidth->setValue(profileWidth);
    startestWidth = set.value("ReprotStartestWidth", 1.).toDouble();
    ui->startestWidth->setValue(startestWidth);
    histoWidth = set.value("ReprotHistoWidth", 1.).toDouble();
    ui->pixelHistoWidth->setValue(histoWidth);
    ui->ronchiWidth->setValue(set.value("ReprotRonchiWidth", 1.).toDouble());
    ui->show3D->setChecked(set.value("reportShow3D", true).toBool());
    ui->showContour->setChecked(set.value("reportShowContour", true).toBool());
    ui->showProfile->setChecked(set.value("reportShowProfile", true).toBool());
    ui->showPDF->setChecked(set.value("reportShowPDF", true).toBool());
    ui->showHistogram->setChecked(set.value("reportShowHisto", false).toBool());
    ui->showRonchi->setChecked(set.value("reportShowRonchi", true).toBool());
    ui->showStarTest->setChecked(set.value("reportShowStartest", true).toBool());
}

ReportDlg::~ReportDlg()
{
    delete ui;
}



void ReportDlg::on_buttonBox_accepted()
{
    QSettings set;

    set.setValue("reportShowProfile",ui->showProfile->isChecked());
    set.setValue("reportShowIgram",ui->showIgram->isChecked());
    set.setValue("reportShow3D", ui->show3D->isChecked());
    set.setValue("reportShowContour", ui->showContour->isChecked());
    set.setValue("reportShowPDF", ui->showPDF->isChecked());
    set.setValue("reportShowHisto", ui->showHistogram->isChecked());
    set.setValue("reportShowRonchi", ui->showRonchi->isChecked());
    set.setValue("reportShowStartest", ui->showStarTest->isChecked());
    QString path = set.value("ReportFilePath",
                 QString(mirrorDlg::get_Instance()->getProjectPath())).toString();
    fileName = QFileDialog::getSaveFileName((QWidget* )0, "Save PDF report",
             path + "/Report.pdf", "*.pdf");
    if (fileName.isEmpty()){
        reject();
    }

    path = QFileInfo(fileName).absolutePath();
    set.setValue("ReportFilePath", path);
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    title = ui->title->text();
}






void ReportDlg::on_contourWidth_valueChanged(double arg1)
{
    contourWidth = arg1;
    QSettings set;
    set.setValue("ReprotContourWidth", arg1);
}


void ReportDlg::on_igramWidth_valueChanged(double arg1)
{
    igramWidth = arg1;
    QSettings set;
    set.setValue("ReprotIgramWidth", arg1);
}


void ReportDlg::on_SurfaceWidth_valueChanged(double arg1)
{
    surfaceWidth = arg1;
    QSettings set;
    set.setValue("ReportSurfaceWidth", arg1);
}


void ReportDlg::on_profileWidth_valueChanged(double arg1)
{
    profileWidth = arg1;
    QSettings set;
    set.setValue("ReprotProfileWidth", arg1);
}


void ReportDlg::on_ronchiWidth_valueChanged(double arg1)
{
    ronchiWidth = arg1;
    QSettings set;
    set.setValue("ReprotRonchiWidth", arg1);
}


void ReportDlg::on_pixelHistoWidth_valueChanged(double arg1)
{
    histoWidth = arg1;
    QSettings set;
    set.setValue("ReprotHistoWidth", arg1);
}


void ReportDlg::on_startestWidth_valueChanged(double arg1)
{
    startestWidth = arg1;
    QSettings set;
    set.setValue("ReprotStarTestWidth", arg1);
}



