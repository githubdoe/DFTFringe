/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "mirrordlg.h"
#include "ui_mirrordlg.h"
#include <QtGui>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QDebug>
QString mirrorDlg::m_projectPath = "";
mirrorDlg *mirrorDlg::m_Instance = 0;
mirrorDlg *mirrorDlg::get_Instance(){
    if (m_Instance == 0){
        m_Instance = new mirrorDlg();
    }
    return m_Instance;
}

mirrorDlg::mirrorDlg(QWidget *parent) :
    QDialog(parent),
    mm(true),m_obsChanged(false),ui(new Ui::mirrorDlg), m_isEllipse(false)
{
    ui->setupUi(this);
    QSettings settings;
    m_name = settings.value("config mirror name", "default").toString();
    ui->name->setText(m_name);
    diameter = settings.value("config diameter", 200.).toDouble();
    ui->diameter->setText(QString().sprintf("%6.2lf",diameter));
    roc = settings.value("config roc", 2000.).toDouble();
    ui->roc->setText(QString().sprintf("%6.2lf",roc));
    lambda = settings.value("config lambda", 640).toDouble();
    ui->lambda->blockSignals(true);
    ui->lambda->setText(QString().sprintf("%6.1lf",lambda));
    ui->lambda->blockSignals(false);
    obs = settings.value("config obstruction", 0.).toDouble();
    ui->obs->setText(QString().sprintf("%6.2lf",obs));
    cc = settings.value("config cc", -1.).toDouble();
    ui->cc->setText(QString().sprintf("%6.2lf",cc));

    ui->unitsCB->setChecked(mm);
    doNull = settings.value("config doNull", true).toBool();
    ui->nullCB->setChecked(doNull);
    FNumber = roc/(2. * diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString().sprintf("%6.2lf",FNumber));
    ui->FNumber->blockSignals(false);
    ui->flipH->setChecked((settings.value( "flipH", false).toBool()));
    m_projectPath = settings.value("projectPath", "").toString();
    fringeSpacing = settings.value("config fringe spacing", 1.).toDouble();
    ui->fringeSpacingEdit->blockSignals(true);
    ui->fringeSpacingEdit->setText(QString().sprintf("%3.2lf",fringeSpacing));
    ui->fringeSpacingEdit->blockSignals(false);
    m_isEllipse = settings.value("isEllipse", false).toBool();
    ui->enableEllipseCb->setChecked(m_isEllipse);
    ui->minorAxisEdit->setEnabled(m_isEllipse);
    ui->minorAxisEdit->setText(QString::number(settings.value("ellipseMinorAxis", 50.).toDouble()));
    connect(&spacingChangeTimer, SIGNAL(timeout()), this, SLOT(spacingChangeTimeout()));
    m_majorHorizontal = settings.value("ellispeMajorHorizontal", true).toBool();
    ui->majorHorizontalCb->setChecked(m_majorHorizontal);
}

mirrorDlg::~mirrorDlg()
{

}
bool mirrorDlg::shouldFlipH(){
    return ui->flipH->isChecked();
}
double mirrorDlg::getMinorAxis(){
    return ui->minorAxisEdit->text().toDouble();
}

void mirrorDlg::on_saveBtn_clicked()
{
    QSettings settings;
    QString path = settings.value("mirrorConfigFile").toString();

    QString fileName = QFileDialog::getSaveFileName(0,
                        tr("Save stats file"), path,
                        tr("ini (*.ini)"));
    if (fileName.isEmpty())
        return;
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".ini"); }
    std::ofstream file((fileName.toStdString().c_str()),std::ios_base::out|std::ios_base::binary);
    if (!file.is_open()) {
        QMessageBox::warning(0, tr("Save mirror config."),
                             tr("Cannot write file %1: ")
                             .arg(fileName));
        return;
    }

    const unsigned char flag[] = {0xFF,0xFE,0xFF};
    const unsigned char zeros[] = {0,0,0,0};
    file.write((char*)flag,3);

    int cnt = m_name.length();
    qDebug() << "writing mirror name "<< m_name << "into "<<fileName;
    file.write((char*)(&cnt),1);
    const ushort *m = m_name.utf16();
    file.write((char*)m,2 * cnt);
    file.write((char *)&doNull, 1); // OpenFringe size of bool was 4 bytes but modern size is 1;
    file.write((char *)zeros, 3);  // fill out to size of 4 bytes;
    qDebug() << "size of null "<<sizeof(doNull);
    file.write((char*)&fringeSpacing,8);
    file.write((char*)&diameter,8);
    file.write((char*)&lambda,8);
    file.write((char*)&mm,4);
    file.write((char*)&obs,8);
    file.write((char*)&roc,8);
    file.write((char*)&cc,8);
    file.write((char*)&z8,8);
    file.write((char*)&zeros,4); // double pass
    file.write((char*)&zeros,4); // two colors traced
    file.write((char*)&fliph,1); // flip lr
    file.write((char*)&zeros,3); // remainder of flip
    file.write((char*)&flipv,1);
    file.write((char*)&zeros,3); // remainder vr
    file.write((char*)&m_isEllipse,1);  // use ellipse
    file.write((char*)&zeros,3);
    file.write((char*)&m_minorAxis,8);  // minor axis
    file.write((char*)&m_majorHorizontal,1);
    file.close();
    QFileInfo info(fileName);
    settings.setValue("mirrorConfigFile",fileName);
    settings.setValue("projectPath", info.absolutePath());
    m_projectPath = info.absolutePath();
}

void mirrorDlg::loadFile(QString & fileName){
    std::ifstream file((fileName.toStdString().c_str()));
    if (!file.is_open()) {
        QMessageBox::warning(this, tr("Read Mirror Config"),
                             tr("Cannot read file %1:.")
                             .arg(fileName));
        return;
    }
    QSettings settings;
    QFileInfo info(fileName);

    emit newPath(info.absolutePath());
    m_projectPath = info.absolutePath();
    settings.setValue("mirrorConfigFile",fileName);
    settings.setValue("projectPath", info.absolutePath());
    settings.setValue("lastPath",info.absolutePath());
    m_projectPath = info.absolutePath();
    char buf[125];
    /*		ar<<m_name;
        ar<<m_artificial_null;
        ar<<m_fringe_spacing;
        ar<<m_diam;
        ar<<m_Ilambda;
        ar<<m_mm;
        ar<<m_obstruction;
        ar<<m_roc;
        ar<<m_sc;
        ar<<m_z8;
        ar<<m_double_pass;
        ar<<m_two_color_traced;  // int
        ar<<m_flip_lr;
        ar<<m_flip_vert;

        */
    unsigned char c = file.peek();
    if (c == 0xff){// windows archive format

        file.read(buf,3);
        file.read(buf,1);
        char cnt = *((char *)buf);
        file.read(buf,2 * cnt);
        QString name = "";
        for (int i = 0; i < cnt; ++i){
            name.append(buf+2 *i);
        }

        ui->name->setText(name);
        m_name = name;

        // donull
        file.read(buf,4);
        bool *bp = (bool *)buf;
        ui->nullCB->setChecked(*bp);
        doNull = *bp;

        //fringe Spacing
        file.read(buf,8);
        double *dp = (double*)buf;
        fringeSpacing = *dp;
        ui->fringeSpacingEdit->blockSignals(true);
        ui->fringeSpacingEdit->setText(QString().sprintf("%3.1lf",*dp));
        ui->fringeSpacingEdit->blockSignals(false);

        //read diameter
        file.read(buf,8);
        diameter = *dp;

        //Lambda
        file.read(buf,8);
        lambda = *dp;
        ui->lambda->setText(QString().number(*dp));

        //Units mm
        file.read(buf,4);
        mm = *(bool*)buf;
        ui->unitsCB->setChecked(true);

        //obsruction
        file.read(buf,4 * 9);
        obs = *(dp++);
        ui->obs->setText(QString().number(obs));

        //ROC
        roc = *(dp++);

        //Diameter
        if (!mm){
            diameter *= 25.4;
            roc *= 25.4;
        }
        ui->diameter->blockSignals(true);
        ui->diameter->setText(QString().sprintf("%6.2lf",diameter));
        ui->diameter->blockSignals(false);
        ui->roc->blockSignals(true);
        ui->roc->setText(QString().sprintf("%6.2lf",roc));
        ui->roc->blockSignals(false);

        //conic
        cc = *(dp++);
        ui->cc->setText(QString().number(cc));

        //z8
        z8 = *(dp++);
        ui->z8->setText(QString().number(z8));
        file.read(buf,4);

        /*
        ar<<m_double_pass;
        ar<<m_two_color_traced;
        ar<<m_flip_lr;
        ar<<m_flip_vert;
        */


        //flips
        if (!file.eof()){
            file.read(buf,4);   // 1234 read right here
            fliph = *(bool*)buf;
            file.read(buf,4);
            flipv = *(bool*)buf;
        }

        // ellipse
        if (file.tellg() > 0){
            // read ellipse mode flag
            file.read(buf,4);
            m_isEllipse = *(bool*)buf;
            ui->enableEllipseCb->setChecked(m_isEllipse);
        }
        // minor axis
        if (file.tellg() > 0){
            file.read(buf,8);
            m_minorAxis = *(double*)buf;
            ui->minorAxisEdit->setText(QString::number(m_minorAxis));
        }
        // major is horizontal
        if (file.tellg() > 0){
            file.read(buf,1);
            m_majorHorizontal = *(bool*)buf;
            ui->majorHorizontalCb->setChecked(m_majorHorizontal);
        }


        FNumber = roc/(2. * diameter);
        ui->FNumber->blockSignals(true);
        ui->FNumber->setText(QString().sprintf("%6.2lf",FNumber));
        ui->FNumber->blockSignals(false);

        file.close();
        blockSignals(false);
        return;
    }
}
void mirrorDlg::on_ReadBtn_clicked()
{
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QString fileName = QFileDialog::getOpenFileName(this,
                        tr("Read mirror configuratoin file"), lastPath,
                        tr("ini (*.ini)"));
    if (fileName.isEmpty())
        return;
    loadFile(fileName);
}
QString mirrorDlg::getProjectPath(){
    return m_projectPath;
}

void mirrorDlg::on_diameter_textChanged(const QString &arg1) {
    diameter = arg1.toDouble() *  ((mm) ? 1.: 25.4);

    FNumber = roc/(2. * diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString().sprintf("%6.2lf",FNumber));
    ui->FNumber->blockSignals(false);
    updateZ8();

}

//Used when the just loading wavfront is different
void mirrorDlg::on_diameter_Changed(const double diam)
{

    diameter = diam ;
    FNumber = roc/(2. * diameter);
    ui->FNumber->blockSignals(true);
    const QSignalBlocker blocker(ui->diameter);
    ui->FNumber->setText(QString().sprintf("%6.2lf",FNumber *( (mm) ? 1.: 25.4)));
    ui->diameter->setText(QString().sprintf("%6.2lf",diameter * ((mm) ? 1.: 25.4)));
    ui->FNumber->blockSignals(false);
    ui->diameter->blockSignals(false);
    updateZ8();

}

void mirrorDlg::on_roc_textChanged(const QString &arg1)
{

    roc = arg1.toDouble() * ((mm) ? 1: 25.4);
    FNumber = roc /(2. * diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString().sprintf("%6.2lf",FNumber));
    ui->FNumber->blockSignals(false);
    updateZ8();

}
/* used when the just loading wavefront is different */
void mirrorDlg::on_roc_Changed(const double newVal)
{
    roc = newVal;

    FNumber = roc /(2. * diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString().sprintf("%6.2lf",FNumber * ((mm) ? 1.: 25.4)));
    ui->FNumber->blockSignals(false);
    ui->roc->blockSignals(true);
    ui->roc->setText(QString().sprintf("%6.2lf",roc * ((mm) ? 1.: 25.4)));
    ui->roc->blockSignals(false);
    updateZ8();
}
void mirrorDlg::updateZ8(){

    z8 = (diameter * diameter * diameter * diameter * 1000000.) /
            (384. * roc * roc * roc * lambda);
    ui->z8->blockSignals(true);
    ui->z8->setText(QString().number(z8 * -cc));
    ui->z8->blockSignals(false);

}

void mirrorDlg::on_FNumber_textChanged(const QString &arg1)
{
    FNumber = arg1.toDouble();
    roc = FNumber *(2 * diameter);
    ui->roc->blockSignals(true);
    ui->roc->setText(QString().number(roc * ((mm) ? 1.: 1./25.4)));
    ui->roc->blockSignals(false);
    updateZ8();
}

void mirrorDlg::on_obs_textChanged(const QString &arg1)
{
    if (arg1.toDouble() != obs)
        m_obsChanged = true;
    obs = ((mm) ? 1: 25.4) * arg1.toDouble();

}

void mirrorDlg::on_lambda_textChanged(const QString &arg1)
{
    lambda = arg1.toDouble();
    updateZ8();
}

void mirrorDlg::on_nullCB_clicked(bool checked)
{
    doNull = checked;
}

void mirrorDlg::on_unitsCB_clicked(bool checked)
{
    mm = checked;
    double div = ((mm) ? 1: 25.4);
    ui->roc->blockSignals(true);
    ui->diameter->blockSignals(true);
     ui->diameter->setText(QString().sprintf("%6.2lf",diameter/div));
     ui->roc->setText(QString().number(roc/div));
     ui->obs->setText(QString().number(obs/div));
     ui->diameter->blockSignals(false);
     ui->roc->blockSignals(false);
     ui->minorAxisEdit->blockSignals(true);
     ui->minorAxisEdit->setText(QString().sprintf(("%6.2lf"),m_minorAxis/div));
}

void mirrorDlg::on_buttonBox_accepted()
{
    QSettings settings;

    settings.setValue("config mirror name", ui->name->text());
    settings.setValue("config roc", roc);
    settings.setValue("config lambda",lambda);
    settings.setValue("config diameter",diameter);
    settings.setValue("config obstruction", obs);
    settings.setValue("config cc", cc);
    settings.setValue("flipH", ui->flipH->isChecked());
    fringeSpacing = ui->fringeSpacingEdit->text().toDouble();
    settings.setValue("config fringe spacing", fringeSpacing);
    //settings.setValue("config unitsMM", mm);
    settings.setValue("config doNull",doNull);
    if (m_obsChanged)
        emit obstructionChanged();
    emit recomputeZerns();
}



void mirrorDlg::on_cc_textChanged(const QString &arg1)
{
   cc = arg1.toDouble();
   updateZ8();
}

void mirrorDlg::spacingChangeTimeout(){
    spacingChangeTimer.stop();
    double v = ui->fringeSpacingEdit->text().toDouble();
    if ( v != fringeSpacing){
        QMessageBox::information(0,"Fringe Spacing Changed",  "This change will only be used when Interferograms are analyzed. "
                    "It will not be applied to any existing wavefronts already loaded.");
    }
    fringeSpacing = v;
    QSettings set;
    set.setValue("config fringe spacing", fringeSpacing);
}

void mirrorDlg::on_fringeSpacingEdit_textChanged(const QString &arg1)
{
    spacingChangeTimer.start(1000);
}

void mirrorDlg::on_name_editingFinished()
{
    m_name = ui->name->text();
    QSettings set;
    set.setValue("config mirror name", ui->name->text());
}

void mirrorDlg::on_enableEllipseCb_clicked(bool checked)
{
    m_isEllipse = checked;
    QSettings set;
    set.setValue("isEllipse", checked);
    ui->minorAxisEdit->setEnabled(checked);
}

void mirrorDlg::on_minorAxisEdit_textChanged(const QString &arg1)
{
    QSettings set;
    set.setValue("ellipseMinorAxis", arg1.toDouble());
    m_minorAxis = arg1.toDouble();
}

void mirrorDlg::on_majorHorizontalCb_clicked(bool checked)
{
    m_majorHorizontal = checked;
    QSettings set;
    set.setValue("ellispeMajorHorizontal", checked);
}
