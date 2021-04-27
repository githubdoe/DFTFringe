#include "psi_dlg.h"
#include "ui_psi_dlg.h"
#include <QImageReader>
#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QtCore>
#include "psitiltoptions.h"
PSI_dlg::PSI_dlg(QWidget *parent) :
    QDialog(parent),m_knownPhase(true),
    ui(new Ui::PSI_dlg),m_tiltOptionsDlg(0), m_doTiltPSI(false)
{
    ui->setupUi(this);
    QSettings set;

    ui->PSIPhaseValue->setValue(set.value("PSIPhaseValue", 60.).toDouble());
    ui->PhaseKnown->blockSignals(true);
    m_knownPhase = set.value("psiPhaseKnown", true).toBool();
    ui->PhaseKnown->setChecked( m_knownPhase);

    ui->PhaseApproximate->setChecked(!m_knownPhase);
    ui->PhaseKnown->blockSignals(false);

    ui->maxiter->setValue(set.value("PSIMaxiter",001).toDouble());
    ui->tolerance->setValue(set.value("PSIPtol",.001).toDouble());
    if (m_knownPhase){
        ui->computePhase->hide();
        ui->masiterlabel->hide();
        ui->maxiter->hide();
        ui->tolerance->hide();
        ui->tolLabel->hide();
        ui->status->hide();
    }

}

PSI_dlg::~PSI_dlg()
{
    delete ui;
}

void PSI_dlg::on_browse_clicked()
{
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QFileDialog dialog(this, tr("Open File"),lastPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    mimeTypeFilters.insert(0, "application/octet-stream");
    mimeTypeFilters.insert(1,"Image files (*.png *.xpm *.jpg)");
    dialog.setMimeTypeFilters(mimeTypeFilters);

    QSettings set;
    QString mime = set.value("igramExt","jpeg").toString();
    mime.replace("jpg", "jpeg",Qt::CaseInsensitive);
    dialog.selectMimeTypeFilter("image/"+mime);
    dialog.setDefaultSuffix(mime);
    if (dialog.exec()){

            QFileInfo a(dialog.selectedFiles().first());
            QString ext = a.completeSuffix();
            set.setValue("igramExt", ext);
            //m_surfaceManager->process_psi(dialog.selectedFiles());
            QStringList files = dialog.selectedFiles();
            ui->igramList->addItems(files);
            QStringList phases;
            double deltaAngle = ui->PSIPhaseValue->value();
            int index = 0;
            double startAngle = 0;

            if (ui->PhaseList->count() > 0){
                startAngle = deltaAngle + ui->PhaseList->item(ui->PhaseList->count()-1)->text().toDouble();
            }
            qDebug() << "start and delta" << startAngle << deltaAngle;
            foreach(QString file, files){

                double ang2 = startAngle + index++ * deltaAngle;
                while (ang2 > 360.) ang2 -= 360.;
                ui->PhaseList->addItem(QString().sprintf("%6.2lf",ang2));
                QListWidgetItem* item = ui->PhaseList->item(ui->PhaseList->count()-1);
                item->setFlags(item->flags() | Qt::ItemIsEditable);

            }
    }
    if (ui->PhaseList->count() > 0){
        ui->computePhase->setEnabled(true);
    }

}
QStringList PSI_dlg::files(){
    QStringList list;
    for(int cnt = 0; cnt < ui->igramList->count(); ++cnt){
        list.append(ui->igramList->item(cnt)->text());
    }
    return list;
}
QVector<double>  PSI_dlg::phases(){
    QVector<double> list;
    for(int cnt = 0; cnt < ui->PhaseList->count(); ++cnt){
        double angle = ui->PhaseList->item(cnt)->text().toDouble() * M_PI/180.;
        while (angle > 2 * M_PI) angle -= 2 * M_PI;
        list << angle;
    }
    return list;
}
#include <qtextedit.h>
#include "helpdlg.h"
void PSI_dlg::on_help_clicked()
{
    HelpDlg dlg;
    QString html;
    QString path = qApp->applicationDirPath() + "/res/Help/PSIHelp.html";

    QFile file(path);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    html.append(stream.readAll());
    html.append("</body></html>");
    dlg.setHelpText(html);
    dlg.exec();
}

void PSI_dlg::on_clear_clicked()
{
    ui->igramList->clear();
    ui->PhaseList->clear();
    ui->computePhase->setEnabled(false);

}

void PSI_dlg::on_PhaseKnown_clicked(bool checked)
{
    m_knownPhase = true;
    QSettings set;
    set.setValue("psiPhaseKnown", m_knownPhase);
    ui->computePhase->setEnabled(!checked);
    if (m_knownPhase){
        ui->computePhase->hide();
        ui->masiterlabel->hide();
        ui->maxiter->hide();
        ui->tolerance->hide();
        ui->tolLabel->hide();
        ui->status->hide();
    }
}

void PSI_dlg::on_PhaseApproximate_clicked(bool checked)
{
    m_knownPhase = false;
    QSettings set;
    set.setValue("psiPhaseKnown", m_knownPhase);
    ui->computePhase->setEnabled(checked);
    if (!m_knownPhase){
        ui->computePhase->show();
        ui->masiterlabel->show();
        ui->maxiter->show();
        ui->tolerance->show();
        ui->tolLabel->show();
        ui->status->show();
    }
}
double PSI_dlg::tolerance(){
    return ui->tolerance->value();
}

void PSI_dlg::on_PSIPhaseValue_valueChanged(const QString &arg1)
{
    QSettings set;

    ui->PhaseList->clear();
    double angle = arg1.toDouble();
    set.setValue("PSIPhaseValue", angle);
    for (int i = 0; i < ui->igramList->count(); ++i){
        double ang2 = angle * i;
        while (ang2 > 360.) ang2 -= 360.;
        ui->PhaseList->addItem(QString().sprintf("%6.2lf",ang2));
        QListWidgetItem* item = ui->PhaseList->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);

    }
}
// set phases input in radians
void PSI_dlg::setPhases(QVector<double> phases){
    for (int i = 0; i < phases.length(); ++i){
        double ang2 = phases[i] * 180./M_PI;

        QListWidgetItem* item = ui->PhaseList->item(i);
        item->setText(QString().sprintf("%6.4lf", ang2));
    }
}

void PSI_dlg::on_computePhase_clicked()
{
    emit computePhase();
}
int PSI_dlg::maxIter(){
    return ui->maxiter->value();
}

void PSI_dlg::on_maxiter_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("PSIMaxiter", arg1);

}

void PSI_dlg::on_tolerance_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("PSIPtol", arg1);
}
void PSI_dlg::setStatusText(const QString &txt){
    ui->status->setText(txt);
}

void PSI_dlg::on_VaryingRB_clicked(bool checked)
{
    if (checked){
        if (m_tiltOptionsDlg == 0){
            m_tiltOptionsDlg = new PSITiltOptions() ;
        }
        if (m_tiltOptionsDlg->exec()){
            m_doTiltPSI = true;
        }
    }
    else{
        m_doTiltPSI = false;
    }
}

void PSI_dlg::on_showVariationPb_clicked()
{
    // Add plots to dialog

    //Then disply it.
     m_tiltOptionsDlg->exec();
}
