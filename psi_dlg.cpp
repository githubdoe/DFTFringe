#include "psi_dlg.h"
#include "ui_psi_dlg.h"
#include <QImageReader>
#include <QSettings>
#include <QWidget>
#include <QFileDialog>
#include <QDebug>
#include <QtCore>
#include "psitiltoptions.h"
#include "psiphasedisplay.h"
#include <QScreen>
#include <QPainter>
PSI_dlg::PSI_dlg(QWidget *parent) :
    QDialog(parent),
    m_tiltOptionsDlg(0),m_knownPhase(true), m_doTiltPSI(false),
    m_stop(false), ui(new Ui::PSI_dlg),  m_useRadians(false), m_last_itr(0), m_last_sdp(0.)
{
    ui->setupUi(this);
    QSettings set;

    ui->PSIPhaseValue->setValue(set.value("PSIPhaseValue", 60.).toDouble());
    ui->PhaseKnown->blockSignals(true);
    m_knownPhase = set.value("psiPhaseKnown", true).toBool();
    ui->PhaseKnown->setChecked( m_knownPhase);
    ui->graph->hide();
    ui->PhaseApproximate->setChecked(!m_knownPhase);
    ui->PhaseKnown->blockSignals(false);

    ui->maxiter->setValue(set.value("PSIMaxiter",0020).toInt());
    ui->tolerance->setValue(set.value("PSIPtol",.004).toDouble());
    ui->progressBar->setMaximum(set.value("PSIMaxiter", 0020).toInt());
    ui->progressBar->setValue(0);
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
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QFileDialog dialog(this, tr("Open File"),lastPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    // the QT default extension are obtained by doing 
    // `for(const QByteArray &mimeTypeName : QImageReader::supportedMimeTypes())`
    // `   mimeTypeFilters.append(mimeTypeName);`
    // `dialog.setMimeTypeFilters(mimeTypeFilters);`
    // `qDebug() << dialog.nameFilters();`
    // manually added upper case and first char upper case
    const QStringList filters({"Image files (*.bmp *.dib *.BMP *.DIB *.Bmp *.Dib *.gif *.GIF *.Gif *.jpg *.jpeg *.jpe *.JPG *.JPEG *.JPE *.Jpg *.Jpeg *.Jpe*.png *.PNG *.Png*.svg *.SVG *.Svg*.svgz *.SVGZ *.Svgz*.ico *.ICO *.Ico*.pbm *.PBM *.Pbm*.pgm *.PGM *.Pgm*.ppm *.PPM *.Ppm*.xbm *.XBM *.Xbm*.xpm *.XPM *.Xpm)",
                           "Any files (*)"
                          });
    dialog.setNameFilters(filters);
    if (dialog.exec()){
            QStringList files = dialog.selectedFiles();
            QFileInfo a(files.constFirst());
            settings.setValue("lastPath",a.absoluteFilePath());
            //m_surfaceManager->process_psi(dialog.selectedFiles());
            ui->igramList->addItems(files);
            double deltaAngle = ui->PSIPhaseValue->value();
            int index = 0;
            double startAngle = 0;

            if (ui->PhaseList->count() > 0){
                startAngle = deltaAngle + ui->PhaseList->item(ui->PhaseList->count()-1)->text().toDouble();
            }
            qDebug() << "start and delta" << startAngle << deltaAngle;
            const int fileCount = files.size();
            for(int i=0 ; i<fileCount ; i++){
                double ang2 = startAngle + index++ * deltaAngle;
                while (ang2 > 360.) ang2 -= 360.;
                ui->PhaseList->addItem(QString("%1").arg(ang2, 6, 'f', 2));
                QListWidgetItem* item = ui->PhaseList->item(ui->PhaseList->count()-1);
                item->setFlags(item->flags() | Qt::ItemIsEditable);
                m_phases << ang2 * M_PI/180.;
            }
    }

    if (ui->PhaseList->count() > 0){
        ui->computePhase->setEnabled(true);
        plot(m_phases, 0,1);
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
    return m_phases;
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
    m_phases.clear();
    ui->computePhase->setEnabled(false);
    plot(m_phases, 1,0.);

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
    m_phases.clear();
    double angle = arg1.toDouble();
    set.setValue("PSIPhaseValue", angle);
    for (int i = 0; i < ui->igramList->count(); ++i){
        double ang2 = angle * i;
        while (ang2 > 360.) ang2 -= 360.;
        m_phases << ang2;
        ui->PhaseList->addItem(QString("%1").arg(ang2, 6, 'f', 2));
        QListWidgetItem* item = ui->PhaseList->item(i);
        item->setFlags(item->flags() | Qt::ItemIsEditable);
    }
}
// set phases input in radians
void PSI_dlg::setPhases(QVector<double> phases){
    m_phases = phases;
    for (int i = 0; i < phases.length(); ++i){
        double ang2 = phases[i];
        if (ui->showRadians)
            ang2 *= 180./M_PI;

        QListWidgetItem* item = ui->PhaseList->item(i);
        item->setText(QString("%1").arg(ang2, 6, 'f', 4));
    }

}

void PSI_dlg::on_computePhase_clicked()
{
    m_stop = false;
    ui->stop->setEnabled(true);
    emit computePhase();
}
int PSI_dlg::maxIter(){
    return ui->maxiter->value();
}

void PSI_dlg::on_maxiter_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("PSIMaxiter", arg1);
    ui->progressBar->setMaximum(arg1);
    ui->progressBar->setValue(0);

}

void PSI_dlg::on_tolerance_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("PSIPtol", arg1);
}
void PSI_dlg::setStatusText(const QString &txt, int prog){
    ui->status->setText(txt);
    ui->progressBar->setValue(prog);
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

void PSI_dlg::on_showRadians_clicked(bool checked)
{
    m_useRadians = checked;
    for(int cnt = 0; cnt < ui->PhaseList->count(); ++cnt){
        QString txt = QString("%1").arg(m_phases[cnt] * ((checked) ? 1: 180./M_PI), 6, 'f', 4);
        ui->PhaseList->item(cnt)->setText(txt);
    }
    plot(m_phases, m_last_itr,m_last_sdp);
    update();
}

void PSI_dlg::plot(QVector<double> phases, int iteration, double sdp){

    m_last_itr = iteration;
    m_last_sdp = sdp;
    ui->graph->show();
    QList<QScreen *>scrs = QApplication::screens();
    int width = scrs[0]->size().width() * .4;
    int height = width;
    QImage plot1(width, height,QImage::Format_RGB888 );
    QPainter p(&plot1);
    int half = width/2;
    int r = 100;
     int rlast = r;
    p.fillRect(0,0,width,height, QColor(211,245,241));
    p.setPen(QPen(QColor(200,200,200)));
    p.drawEllipse(half,half, 5,5);
    p.drawLine(half,0, half,height);
    p.drawLine(0,half,width,half);
    double k =  ((m_useRadians) ? 1: 180./M_PI);
    if (phases.length() > 0){
        int rdel = (half - 150)/phases.length();

        for (int i = 0; i < phases.length()-1; ++i){
            double angle = phases[i];
            double angle2 = phases[i+1];

            int x1 = half - rlast * cos(angle + M_PI/2.);
            int x2 = half - r * cos(angle2 + M_PI/2.);
            int y1 = half - rlast * sin(angle + M_PI/2.);
            int y2 = half - r * sin(angle2+ M_PI/2.);
            // qDebug() << "angle1" << angle * k << angle2 * k;
            while (angle < 0)
                angle = (2 * M_PI) + angle;
            while (angle2 < 0)
                angle2 = (2 * M_PI) + angle2;
            double delta = angle2 - angle;
            if ( delta < 0)
                delta = 2 * M_PI + delta;
            p.setBrush(Qt::white);
            p.drawEllipse(x2-25,y2-25,50,50);
            p.setPen(QPen(QColor(100,100,100),2));
            p.drawLine(x1,y1,x2,y2);
            p.setPen(QPen(QColor(0,0,255)));
            QFont font=p.font() ;
            font.setPointSize ( 12 );
            font.setWeight(QFont::DemiBold);
            p.setFont(font);
            p.drawText( (x1+x2)/2, (y1+y2)/2,QString("%1").arg(delta *  k, 6, 'f', 2).toStdString().c_str());
            p.setPen(QPen(QColor(100,100,100)));
            font.setPointSize(10);
            p.setFont(font);
            p.drawText( x2-10,y2+10,QString("%1").arg( i+1).toStdString().c_str());
            p.drawText(x2 + 40, y2 ,QString("%1").arg( angle2 * k, 6, 'f', 2).toStdString().c_str());
            rlast = r;
            r += rdel;
        }
        QFont font=p.font() ;
        font.setPointSize ( 15 );
        p.setFont(font);
        p.drawText(30,100,QString("iteration %1 sdp: %2").arg(iteration).arg(sdp, 0, 'f').toStdString().c_str());
        p.setPen(QPen(QBrush(QColor(0,0,255)),5));

        p.drawLine(50,height -120, 90, height -120);
        p.drawText(100, height - 100, "difference from one phase to next");
            }
        ui->graph->setPixmap(QPixmap::fromImage(plot1));//.scaled(labelSize, Qt::KeepAspectRatio, Qt::FastTransformation)));

    update();

}

void PSI_dlg::on_stop_clicked()
{
    m_stop = true;
    ui->stop->setEnabled(false);
    QApplication::processEvents();
}

bool PSI_dlg::shouldResize(){
    return ui->doResize->isChecked();
}

int PSI_dlg::getResizeValue(){
    return ui->resizeSb->value();
}


void PSI_dlg::on_resizeSb_valueChanged(int arg1)
{
    int b = (arg1 / 100) * 100;
    ui->resizeSb->setValue(b);
}


void PSI_dlg::on_doResize_clicked(bool checked)
{
    ui->resizeSb->setEnabled(checked);
}

