#include "psi_dlg.h"
#include "ui_psi_dlg.h"
#include <QImageReader>
#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QtCore>

PSI_dlg::PSI_dlg(QWidget *parent) :
    QDialog(parent),m_knownPhase(true),
    ui(new Ui::PSI_dlg)
{
    ui->setupUi(this);
    QSettings set;

    ui->PSIPhaseValue->setValue(set.value("PSIPhaseValue", 60.).toDouble());
    ui->PhaseKnown->blockSignals(true);
    m_knownPhase = set.value("psiPhaseKnown", true).toBool();
    ui->PhaseKnown->setChecked( m_knownPhase);
    ui->PhaseApproximate->setChecked(!m_knownPhase);
    ui->PhaseKnown->blockSignals(false);

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
            foreach(QString file, files){
                double ang2 = index * deltaAngle;
                while (ang2 > 360.) ang2 -= 360.;
                ui->PhaseList->addItem(QString().sprintf("%6.2lf",ang2));
                QListWidgetItem* item = ui->PhaseList->item(index++);
                item->setFlags(item->flags() | Qt::ItemIsEditable);

            }
    }
}
QStringList PSI_dlg::files(){
    QStringList list;
    for(int cnt = 0; cnt < ui->igramList->count(); ++cnt){
        list.append(ui->igramList->item(cnt)->text());
    }
    return list;
}
cv::Mat PSI_dlg::phases(){
    cv::Mat list((int)(ui->PhaseList->count()),1, CV_64F);
    for(int cnt = 0; cnt < ui->PhaseList->count(); ++cnt){
        double angle = ui->PhaseList->item(cnt)->text().toDouble() * M_PI/180.;
        while (angle > 2 * M_PI) angle -= 2 * M_PI;
        list.at<double>(cnt) = angle;
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
}

void PSI_dlg::on_PhaseKnown_clicked(bool checked)
{
    m_knownPhase = true;
    QSettings set;
    set.setValue("psiPhaseKnown", m_knownPhase);
}

void PSI_dlg::on_PhaseApproximate_clicked(bool checked)
{
    m_knownPhase = false;
    QSettings set;
    set.setValue("psiPhaseKnown", m_knownPhase);
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
