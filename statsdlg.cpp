#include "statsdlg.h"
#include "ui_statsdlg.h"
#include <QVBoxLayout>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>
#include <QColorDialog>
#include <QSettings>
StatsDlg::StatsDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatsDlg)
{
    ui->setupUi(this);
    QSettings settings;


    m_background = QColor(settings.value("statsPlotBackGround","black").toString());
    QString qss = QString().sprintf("background-color: %s",m_background.name().toStdString().c_str());
    ui->bgColor->setStyleSheet(qss);
}

StatsDlg::~StatsDlg()
{
    delete ui;
}

void StatsDlg::on_buttonBox_accepted()
{
    doAstig = ui->AstigCB->isChecked();
    doSA = ui->SACB->isChecked();
    doComa = ui->ComaCB->isChecked();
    doInputs = ui->doInputsCB->isChecked();
    saveCsv = ui->csv->isChecked();
    savePDF = ui->pdf->isChecked();
    saveJpg = ui->jpg->isChecked();
    zernFrom = ui->zernStart->value();
    zernTo = ui->zernEnd->value();
    doZernGroup = ui->zernGroup->isChecked();
    showFileNames = ui->showFileNames->isChecked();
    removeOutliers = ui->removeOutliers->isChecked();
    removeAboveLimit = ui->RemoveAboveCB->isChecked();
    RMSLimitValue = ui->RMSLimit->value();
}

void StatsDlg::on_bakcgroundpb_clicked()
{
    QColorDialog cdlg;
    if (cdlg.exec() == QDialog::Accepted){
        m_background = cdlg.selectedColor();
        QString qss = QString().sprintf("background-color: %s",m_background.name().toStdString().c_str());
        ui->bgColor->setStyleSheet(qss);
        QSettings settings;
        settings.setValue("statsPlotBackGround", m_background.name());
    }

}
