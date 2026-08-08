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
#include "spdlog/spdlog.h"
#include <QtGui>
#include <QShowEvent>
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include "annulushelpdlg.h"
#include "surfacemanager.h"

QString mirrorDlg::m_projectPath = "";

mirrorDlg *mirrorDlg::get_Instance(){
    static mirrorDlg m_Instance{};
    return &m_Instance;
}

mirrorDlg::mirrorDlg(QWidget *parent) :
    QDialog(parent),
    mm(true),m_obsChanged(false),ui(new Ui::mirrorDlg)
{
    ui->setupUi(this);
    
    // Initialize defaults only; loadDraftFromSettings() populates m_current and m_draft
    FNumber = 0.0;
    ui->FNumber->blockSignals(true);
    ui->roc->blockSignals(true);
    ui->lambda->blockSignals(true);
    ui->cc->blockSignals(true);
    ui->unitsCB->blockSignals(true);
    ui->fringeSpacingEdit->blockSignals(true);
    ui->minorAxisEdit->blockSignals(true);
    
    ui->unitsCB->setChecked(mm);
    connect(&spacingChangeTimer, &QTimer::timeout, this, &mirrorDlg::spacingChangeTimeout);
    
    ui->FNumber->blockSignals(false);
    ui->roc->blockSignals(false);
    ui->lambda->blockSignals(false);
    ui->cc->blockSignals(false);
    ui->unitsCB->blockSignals(false);
    ui->fringeSpacingEdit->blockSignals(false);
    ui->minorAxisEdit->blockSignals(false);
    
    m_aperatureReductionValueChanged = false;
    
    // Initialize settings from persistent storage (in case dialog used without showEvent)
    loadDraftFromSettings();
}

mirrorDlg::~mirrorDlg()
{
    spdlog::get("logger")->trace("mirrorDlg::~mirrorDlg");
    delete ui;
}

void mirrorDlg::loadDraftFromSettings()
{
    // Load mirror settings from persistent storage via facade into BOTH copies.
    // m_current: persistent copy (source of truth for external code)
    // m_draft: working copy for dialog edits (discarded on Cancel)
    m_current = SettingsFacade::instance().mirrorStore().load();
    m_draft = m_current;
    
    // Load application-level settings separately (projectPath, mirrorConfigFile, lastPath)
    ApplicationSettings appSettings = SettingsFacade::instance().appStore().load();
    m_projectPath = appSettings.projectPath;
}

void mirrorDlg::showEvent(QShowEvent *event)
{
    // Reload draft from persistent settings before dialog becomes visible.
    // This ensures Cancel always reverts to the last-saved state.
    loadDraftFromSettings();
    
    // Sync UI with reloaded draft (working copy) values
    ui->name->setText(m_draft.mirrorName);
    ui->diameter->setText(QString("%1").arg(m_draft.diameter, 6, 'f', 2));
    ui->roc->setText(QString("%1").arg(m_draft.roc, 6, 'f', 2));
    ui->obs->setText(QString("%1").arg(m_draft.obstruction, 6, 'f', 2));
    ui->lambda->setText(QString("%1").arg(m_draft.lambda, 6, 'f', 1));
    ui->cc->setText(QString("%1").arg(m_draft.cc, 6, 'f', 2));
    ui->flipH->setChecked(m_draft.flipH);
    ui->nullCB->setChecked(m_draft.doNull);
    ui->fringeSpacingEdit->setText(QString("%1").arg(m_draft.fringeSpacing, 6, 'f', 3));
    ui->ellipseShape->setChecked((outlineShape)m_draft.outlineShape == ELLIPSE);
    ui->minorAxisEdit->setText(QString::number(m_draft.ellipseMinorAxis));
    ui->ReducApp->setChecked(m_draft.apertureReductionEnabled);
    ui->reduceValue->setValue(m_draft.apertureReduction);
    ui->useAnnulus->setChecked(m_draft.useAnnulus);
    ui->annulusPercent->setValue(m_draft.annulusPercent * 100);
    
    QDialog::showEvent(event);
}

bool mirrorDlg::shouldFlipH(){
    return ui->flipH->isChecked();
}
double mirrorDlg::getMinorAxis(){
    return ui->minorAxisEdit->text().toDouble();
}

bool mirrorDlg::isEllipse(){
    return (outlineShape)m_draft.outlineShape == ELLIPSE;
}
void mirrorDlg::saveJson(const QString &fileName){
    QJsonObject jDoc, jMirror,jIgram, jEllipse, jAnnulus;
    jDoc["name"] = m_draft.mirrorName;
    jDoc["show units in mm"] = mm;
    jDoc["useNull"] = m_draft.doNull;
    jIgram["wavelength"] = m_draft.lambda;
    jIgram["fringe spacing"] = m_draft.fringeSpacing;
    jMirror["diameter"] = m_draft.diameter;
    jMirror["obs diameter"] = m_draft.obstruction;
    jMirror["roc"] = m_draft.roc;
    jMirror["desired conic"] = m_draft.cc;
    jMirror["edgeMaskon"] = m_draft.apertureReductionEnabled;
    jMirror["edge mask value"] = m_draft.apertureReduction;
    jIgram["wavelength"] = m_draft.lambda;
    jIgram["null value"] = z8;
    jIgram["flip horizontal"] = m_draft.flipH;
    jIgram["fringe spacing"] = m_draft.fringeSpacing;
    jEllipse["is ellipse"] = m_draft.outlineShape;
    jEllipse["ellipse vert axis"] = m_draft.ellipseMinorAxis;
    jAnnulus["use annular Zernike values"] = m_draft.useAnnulus;
    jAnnulus["obs percentage"] = m_draft.annulusPercent;
    jDoc["mirror"] = jMirror;
    jDoc["igram"] = jIgram;
    jDoc["ellipse"] = jEllipse;
    jDoc["Annulus"] = jAnnulus;
    QJsonDocument jsondoc = QJsonDocument(jDoc);

    QFile saveFile(fileName);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't save config file.");
        return;
    }

    saveFile.write(jsondoc.toJson());
    saveFile.close();
}
void mirrorDlg:: on_saveBtn_clicked()
{
    QString path = m_projectPath;  // Use current project path for file dialog default
    QString extensionTypes("config file (*.json)");
    QString fileName = QFileDialog::getSaveFileName(this,
                        tr("Save config file"), path,
                        extensionTypes);
    if (fileName.isEmpty()){
        // cancel has been pressed
        return;
    }
    if (!fileName.endsWith(".json" )){
        // filename.extension => filename.extension.json
        // filename           => filename.json
        // filename.json      => filename.json (unchanged)
        fileName.append(".json");
    }
    saveJson(fileName);
    QFileInfo info(fileName);
    
    // Update application settings with new file path
    ApplicationSettings appSettings = SettingsFacade::instance().appStore().load();
    appSettings.mirrorConfigFile = fileName;
    appSettings.projectPath = info.absolutePath();
    SettingsFacade::instance().appStore().save(appSettings);
    m_projectPath = appSettings.projectPath;
}

void mirrorDlg::loadFile(QString & fileName){

    // clear ellipse in case this is an old config that does not have it.
    ui->ellipseShape->setChecked(false);
    m_draft.outlineShape = (int)CIRCLE;
    QFileInfo info(fileName);
    
    // Persist UI convenience path to QSettings
    QSettings settings;
    settings.setValue("lastPath", info.absolutePath());
    
    emit newPath(info.absolutePath());
    
    // Update application settings with new file path and mirror config file via facade
    ApplicationSettings appSettings = SettingsFacade::instance().appStore().load();
    appSettings.projectPath = info.absolutePath();
    appSettings.mirrorConfigFile = fileName;
    SettingsFacade::instance().appStore().save(appSettings);
    m_projectPath = appSettings.projectPath;


    if (fileName.endsWith(".json")){

        QFile loadFile(fileName);

        if (!loadFile.open(QIODevice::ReadOnly)) {
            qWarning("Couldn't open .json  file.");
            return;
        }

        QByteArray saveData = loadFile.readAll();
        QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));

        ui->name->setText(QJsonValue(loadDoc["name"]).toString());
        ui->unitsCB->setChecked(true);
        mm=true;  // setChecked() does not call on_unitsCB_clicked()

        // set diameter early - before setting roc and annulus percentage
        QJsonObject mirror = loadDoc["mirror"].toObject();
        m_draft.diameter = QJsonValue(mirror["diameter"]).toDouble();
        ui->diameter->blockSignals(true);
        ui->diameter->setText(QString("%1").arg(m_draft.diameter, 6, 'f', 2));
        ui->diameter->blockSignals(false);

        ui->nullCB->setChecked( QJsonValue(loadDoc["useNull"]).toBool());

        m_draft.obstruction = QJsonValue(mirror["obs diameter"]).toDouble();
        m_draft.roc = QJsonValue(mirror["roc"]).toDouble();
        m_draft.cc = QJsonValue(mirror["desired conic"]).toDouble();
        m_draft.apertureReductionEnabled = QJsonValue(mirror["edgeMaskon"]).toBool();
        m_draft.apertureReduction = QJsonValue( mirror["edge mask value"]).toDouble();

        QJsonObject Igram = loadDoc["igram"].toObject();
        m_draft.lambda = QJsonValue(Igram["wavelength"]).toDouble();
        z8 = QJsonValue(Igram["null value"]).toDouble();
        m_draft.flipH = QJsonValue(Igram["flip horizontal"]).toBool();
        m_draft.fringeSpacing = QJsonValue(Igram["fringe spacing"]).toDouble();
        QJsonObject Ellipse = loadDoc["ellipse"].toObject();
        m_draft.outlineShape = QJsonValue(Ellipse["is ellipse"]).toInt();
        m_draft.ellipseMinorAxis = QJsonValue(Ellipse["ellipse vert axis"]).toDouble();
        QJsonObject Annulus = loadDoc["Annulus"].toObject();
        m_draft.useAnnulus = QJsonValue(Annulus["use annular Zernike values"]).toBool();
        m_draft.annulusPercent = QJsonValue(Annulus["obs percentage"]).toDouble();
        ui->useAnnulus->setChecked(m_draft.useAnnulus);
        ui->annulusPercent->setValue(m_draft.annulusPercent * 100);
        on_annulusPercent_valueChanged(m_draft.annulusPercent * 100);
        enableAnnular(m_draft.useAnnulus);

        ui->fringeSpacingEdit->blockSignals(true);
        ui->fringeSpacingEdit->setText(QString("%1").arg(m_draft.fringeSpacing, 3, 'f', 1));
        ui->fringeSpacingEdit->blockSignals(false);

        ui->obs->setText(QString().number(m_draft.obstruction));


        ui->roc->blockSignals(true);
        ui->roc->setText(QString("%1").arg(m_draft.roc, 6, 'f', 2));
        ui->roc->blockSignals(false);

        ui->cc->setText(QString().number(m_draft.cc));

        ui->z8->setText(QString().number(z8));

        ui->ellipseShape->setChecked((outlineShape)m_draft.outlineShape == ELLIPSE);

        ui->minorAxisEdit->setText(QString::number(m_draft.ellipseMinorAxis));

        FNumber = m_draft.roc/(2. * m_draft.diameter);
        ui->FNumber->blockSignals(true);
        ui->FNumber->setText(QString("%1").arg(FNumber, 6, 'f', 2));
        ui->FNumber->blockSignals(false);

        blockSignals(false);
    }
    else {
        enableAnnular(false);
        ui->useAnnulus->setChecked(false);
        std::ifstream file((fileName.toStdString().c_str()));
        if (!file.is_open()) {
            QMessageBox::warning(this, tr("Read Mirror Config"),
                                 tr("Cannot read file %1:.")
                                 .arg(fileName));
            return;
        }

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
            m_draft.mirrorName = name;

            // donull
            file.read(buf,4);
            bool *bp = (bool *)buf;
            ui->nullCB->setChecked(*bp);
            m_draft.doNull = *bp;

            //fringe Spacing
            file.read(buf,8);
            double *dp = (double*)buf;
            m_draft.fringeSpacing = *dp;
            ui->fringeSpacingEdit->blockSignals(true);
            ui->fringeSpacingEdit->setText(QString("%1").arg(*dp, 3, 'f', 1));
            ui->fringeSpacingEdit->blockSignals(false);

            //read diameter
            file.read(buf,8);
            m_draft.diameter = *dp;

            //Lambda
            file.read(buf,8);
            m_draft.lambda = *dp;
            ui->lambda->setText(QString().number(*dp));

            //Units mm
            file.read(buf,4);
            mm = *(bool*)buf;
            ui->unitsCB->setChecked(true);

            //obsruction
            file.read(buf,4 * 9);
            m_draft.obstruction = *(dp++);
            ui->obs->setText(QString().number(m_draft.obstruction));

            //ROC
            m_draft.roc = *(dp++);

            //Diameter
            if (!mm){
                // diameter *= 25.4;
                //roc *= 25.4;
            }
            ui->diameter->blockSignals(true);
            ui->diameter->setText(QString("%1").arg(m_draft.diameter, 6, 'f', 2));
            ui->diameter->blockSignals(false);
            ui->roc->blockSignals(true);
            ui->roc->setText(QString("%1").arg(m_draft.roc, 6, 'f', 2));
            ui->roc->blockSignals(false);

            //conic
            m_draft.cc = *(dp++);
            ui->cc->setText(QString().number(m_draft.cc));

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
                m_draft.flipH = *(bool*)buf;
                file.read(buf,4);
                // Skip vertical flip - not stored in MirrorSettings struct and not used
            }

            // ellipse
            if (file.tellg() > 0){
                // read outlineShape
                file.read(buf,4);
                m_draft.outlineShape = (int)*(outlineShape*)buf;
                ui->ellipseShape->setChecked((outlineShape)m_draft.outlineShape == ELLIPSE);

            }
            // vertical axis
            if (file.tellg() > 0){
                file.read(buf,8);
                m_draft.ellipseMinorAxis = *(double*)buf;
                ui->minorAxisEdit->setText(QString::number(m_draft.ellipseMinorAxis));
            }

            FNumber = m_draft.roc/(2. * m_draft.diameter);
            ui->FNumber->blockSignals(true);
            ui->FNumber->setText(QString("%1").arg(FNumber, 6, 'f', 2));
            ui->FNumber->blockSignals(false);

            file.close();
            blockSignals(false);
            return;
        }
    }
}
void mirrorDlg::on_ReadBtn_clicked()
{
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QString fileName = QFileDialog::getOpenFileName(this,
                        tr("Read mirror configuration file"), lastPath,
                        tr("ini (*.ini *.json)"));
    if (fileName.isEmpty())
        return;
    loadFile(fileName);
}
QString mirrorDlg::getProjectPath(){
    return m_projectPath;  // Already synced from application settings in loadDraftFromSettings()
}

void mirrorDlg::on_diameter_textChanged(const QString &arg1) {

    double diam = arg1.toDouble() *  ((mm) ? 1.: 25.4);
    if ((outlineShape)m_draft.outlineShape == ELLIPSE){
        double e = m_draft.ellipseMinorAxis/m_draft.diameter;
        m_draft.ellipseMinorAxis = e * diam;
        ui->minorAxisEdit->setText(QString().number(m_draft.ellipseMinorAxis));
    }
    m_draft.diameter = diam;
    FNumber = m_draft.roc/(2. * m_draft.diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString("%1").arg(FNumber, 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    updateZ8();
    if (m_draft.useAnnulus){
        on_annulusPercent_valueChanged(m_draft.annulusPercent * 100);
    }

}

//Used when the just loading wavfront is different
void mirrorDlg::on_diameter_Changed(const double diam)
{
    if ((outlineShape)m_draft.outlineShape == ELLIPSE){
        double e = m_draft.ellipseMinorAxis/m_draft.diameter;
        m_draft.ellipseMinorAxis = e * diam;
        ui->minorAxisEdit->setText(QString().number(m_draft.ellipseMinorAxis));
    }
    m_draft.diameter = diam ;
    FNumber = m_draft.roc/(2. * m_draft.diameter);
    ui->FNumber->blockSignals(true);
    const QSignalBlocker blocker(ui->diameter);
    ui->FNumber->setText(QString("%1").arg(FNumber *( (mm) ? 1.: 25.4), 6, 'f', 2));
    ui->diameter->setText(QString("%1").arg(m_draft.diameter * ((mm) ? 1.: 25.4), 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    ui->diameter->blockSignals(false);

    setclearAp();
    updateZ8();

}

void mirrorDlg::on_roc_textChanged(const QString &arg1)
{
    m_draft.roc = arg1.toDouble() * ((mm) ? 1: 25.4);
    FNumber = m_draft.roc /(2. * m_draft.diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString("%1").arg(FNumber, 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    updateZ8();
}

/* used when the just loading wavefront is different */
void mirrorDlg::on_roc_Changed(const double newVal)
{
    m_draft.roc = newVal;

    FNumber = m_draft.roc /(2. * m_draft.diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString("%1").arg(FNumber * ((mm) ? 1.: 25.4), 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    ui->roc->blockSignals(true);
    ui->roc->setText(QString("%1").arg(m_draft.roc * ((mm) ? 1.: 25.4), 6, 'f', 2));
    ui->roc->blockSignals(false);
    updateZ8();
}
void mirrorDlg::updateZ8(){
//Z = d^6 / (16 * R^5)

    double aperature = (ui->ReducApp->isChecked()) ?  m_draft.diameter - m_draft.apertureReduction*2. : m_draft.diameter;

    z8 = (pow(aperature,4) * 1000000.) /
            (384. * pow(m_draft.roc, 3) * m_draft.lambda);


    if (m_draft.useAnnulus){
        double f = (1 - (m_draft.annulusPercent * m_draft.annulusPercent));
        f *= f;
        z8 *= f;
    }
    ui->z8->blockSignals(true);
    ui->z8->setText(QString().number(z8 * m_draft.cc));
    ui->z8->blockSignals(false);

}

void mirrorDlg::on_FNumber_textChanged(const QString &arg1)
{

    FNumber = arg1.toDouble();
    m_draft.roc = FNumber *(2 * m_draft.diameter);
    ui->roc->blockSignals(true);
    ui->roc->setText(QString().number(m_draft.roc * ((mm) ? 1.: 1./25.4)));
    ui->roc->blockSignals(false);
    updateZ8();
}

void mirrorDlg::on_obs_textChanged(const QString &arg1)
{
    if (arg1.toDouble() != m_draft.obstruction)
        m_obsChanged = true;
    m_draft.obstruction = ((mm) ? 1: 25.4) * arg1.toDouble();

}
void mirrorDlg::newLambda(const QString &v){
    ui->lambda->setText(v);
}

void mirrorDlg::on_lambda_textChanged(const QString &arg1)
{
    m_draft.lambda = arg1.toDouble();
    updateZ8();
}

void mirrorDlg::on_nullCB_clicked(bool checked)
{
    m_draft.doNull = checked;
    ui->FNumber->blockSignals(true);
    ui->roc->blockSignals(true);
    if (!m_draft.doNull){

        ui->FNumber->hide();
        ui->fnumberLab->hide();
        ui->roc->hide();
        ui->rocLab->hide();
    }
    else {
        ui->FNumber->show();
        ui->fnumberLab->show();
        ui->roc->show();
        ui->rocLab->show();
    }
    ui->FNumber->blockSignals(false);
    ui->roc->blockSignals(false);
}

void mirrorDlg::on_unitsCB_clicked(bool checked)
{
    mm = checked;
    double div = ((mm) ? 1: 25.4);

    ui->roc->blockSignals(true);
    ui->diameter->blockSignals(true);
     ui->diameter->setText(QString("%1").arg(m_draft.diameter/div, 6, 'f', 2));
     ui->roc->setText(QString().number(m_draft.roc/div));
     ui->obs->setText(QString().number(m_draft.obstruction/div));
     ui->diameter->blockSignals(false);
     ui->roc->blockSignals(false);
     ui->minorAxisEdit->blockSignals(true);
     ui->minorAxisEdit->setText(QString("%1").arg(m_draft.ellipseMinorAxis/div, 6, 'f', 2));
     ui->reduceValue->blockSignals(true);
     ui->annularDiameter->blockSignals(true);
     ui->annularDiameter->setValue(m_draft.diameter * m_draft.annulusPercent * ((mm)? 1.: 1./25.4));
     ui->annularDiameter->blockSignals(false);
     
     // Get apertureReduction from draft (already loaded from persistent storage)
     ui->reduceValue->setValue(m_draft.apertureReduction * ((mm) ? 1. : 1./25.4));
     ui->reduceValue->blockSignals(false);
     ui->ClearAp->setText(QString("%1 ").arg(m_draft.apertureReduction * ((mm) ? 1: 1./25.4), 6, 'f', 2));
}

void mirrorDlg::on_buttonBox_accepted()
{
    setclearAp();
    updateZ8();

    SurfaceManager * sm = SurfaceManager::get_instance();
    if (sm->m_inverseMode == invCONIC && m_draft.cc == 0) {
        sm->m_inverseMode = invNOTSET; // don't allow inverse mode to be conic if conic constant is zero
        updateAutoInvertStatus();
    }

    // Commit draft edits to persistent copy
    m_current = m_draft;
    
    // Persist persistent copy to QSettings via facade (single atomic save)
    // Note: Only mirrordlg can call this (via friend declaration) - enforces single source of truth
    SettingsFacade::instance().saveMirrorSettings(m_current);

    if (m_obsChanged)
        emit obstructionChanged();
    emit recomputeZerns();
    if (m_aperatureReductionValueChanged){
        QMessageBox::warning(0, tr("Aperature Reduction value was  changed."),
                             tr("Aperature Reduction was changed.\n"
                                "The wave front will not be correct until it is recomputed from the interferogram."));
    }
    m_aperatureReductionValueChanged = false;
}


void mirrorDlg::on_cc_textChanged(const QString &arg1)
{
   m_draft.cc = arg1.toDouble();
   updateZ8();
}

void mirrorDlg::spacingChangeTimeout(){
    spacingChangeTimer.stop();
    double v = ui->fringeSpacingEdit->text().toDouble();
    if ( v != m_draft.fringeSpacing){
        QMessageBox::information(0,"Fringe Spacing Changed",  "This change will only be used when Interferograms are analyzed. "
                    "It will not be applied to any existing wavefronts already loaded.");
    }
    m_draft.fringeSpacing = v;

}

void mirrorDlg::on_fringeSpacingEdit_textChanged(const QString & /*text*/)
{
    spacingChangeTimer.start(1000);
}

void mirrorDlg::on_name_editingFinished()
{
    m_draft.mirrorName = ui->name->text();

}


void mirrorDlg::on_minorAxisEdit_textChanged(const QString &arg1)
{

    m_draft.ellipseMinorAxis = arg1.toDouble();
}

void mirrorDlg::setMinorAxis(double val){
    m_draft.ellipseMinorAxis = val;
    ui->minorAxisEdit->setText(QString::number(val));
    //on_minorAxisEdit_textChanged( QString::number(val));
}

void mirrorDlg::setVerticalAxis(double val){
    m_draft.ellipseMinorAxis = val;
}

void mirrorDlg::setOutlineShape(outlineShape shape){
    m_draft.outlineShape = (int)shape;
    ui->ellipseShape->setChecked(shape == ELLIPSE);
}

void mirrorDlg::on_ellipseShape_clicked(bool checked)
{
    if (checked) m_draft.outlineShape = (int)ELLIPSE;
    else m_draft.outlineShape = (int)CIRCLE;

    if (m_draft.ellipseMinorAxis == 0){
        m_draft.ellipseMinorAxis = m_draft.diameter;
        ui->minorAxisEdit->setText(QString().number(m_draft.ellipseMinorAxis));
    }
}


void mirrorDlg::on_buttonBox_helpRequested()
{
    QString link = qApp->applicationDirPath() + "/res/Help/mirrorConfig.html";
    QDesktopServices::openUrl(QUrl(link));
}

void mirrorDlg::setclearAp(){

    double clearAperature = (m_draft.diameter - m_draft.apertureReduction * 2) ;
    if (m_draft.apertureReductionEnabled == false)
        clearAperature = m_draft.diameter;
    ui->ClearAp->setText(QString("%1 ").arg(clearAperature * ((mm) ? 1: 1./25.4), 6, 'f', 2));
}

void mirrorDlg::on_ReducApp_clicked(bool checked)
{
    m_draft.apertureReductionEnabled = checked;
    ui->reduceValue->setEnabled(checked);
    ui->ClearAp->setVisible(checked);
    ui->clearApLabel->setVisible(checked);
    updateZ8();

    ui->reduceValue->setValue(m_draft.apertureReduction);
    m_aperatureReductionValueChanged = true;
    setclearAp();
    emit aperatureChanged();
}


void mirrorDlg::on_reduceValue_valueChanged(double arg1)
{
    m_draft.apertureReduction = ((mm) ? 1: 25.4) * arg1;
    updateZ8();

    setclearAp();
    m_aperatureReductionValueChanged = true;
    emit aperatureChanged();
}

void mirrorDlg::on_annulusPercent_valueChanged(double arg1)
{
    ui->annularDiameter->blockSignals(true);
    m_draft.annulusPercent = .01 * arg1;
    ui->annularDiameter->setValue( m_draft.annulusPercent * m_draft.diameter * ( (mm) ? 1.: 1./25.4));
    ui->annularDiameter->blockSignals(false);

    if (m_draft.annulusToObstruction){
        ui->obs->setText(QString::number(m_draft.annulusPercent * m_draft.diameter * ((mm)? 1.: 1./25.4)));
    }
    updateZ8();
}

void mirrorDlg::enableAnnular(bool enable){
    ui->annularDiameter->setEnabled(enable);
    ui->annulusPercent->setEnabled(enable);
    if (enable) {
        ui->annularDiameter->show();
        ui->annulusPercent->show();
        ui->annularDiamLb->show();
        ui->annularpercentLb->show();
        ui->annulusHelp->show();
    }
    else {
        ui->annularDiameter->hide();
        ui->annulusPercent->hide();
        ui->annularDiamLb->hide();
        ui->annularpercentLb->hide();
        ui->annulusHelp->hide();
    }
}

void mirrorDlg::on_useAnnulus_clicked(bool checked)
{
    m_draft.useAnnulus = checked;
    enableAnnular(checked);
    updateZ8();

}


void mirrorDlg::on_annulusHelp_clicked()
{
    annulusHelpDlg dlg;
    dlg.resize(1000,800);
    dlg.exec();

}


void mirrorDlg::on_annularDiameter_valueChanged(double arg1)
{
    m_draft.annulusPercent = arg1/m_draft.diameter;
    ui->annulusPercent->setValue(m_draft.annulusPercent * 100);
    updateZ8();
}

void mirrorDlg::setObsPercent(double obs){
    ui->annulusPercent->setValue(obs);
}

void mirrorDlg::updateAutoInvertStatus()
{
    switch(SurfaceManager::get_instance()->m_inverseMode)
    {
        case invNOTSET:
            ui->lblAutoInvert->setText("Autoinvert: Not Set");
            break;
        case invMANUAL:
            ui->lblAutoInvert->setText("Autoinvert: Manual");
            break;
        case invCONIC:
            ui->lblAutoInvert->setText("Autoinvert: Conic");
            break;
        case invINSIDE:
            ui->lblAutoInvert->setText("Autoinvert: Inside Focus");
            break;
        case invOUTSIDE:
            ui->lblAutoInvert->setText("Autoinvert: Outside Focus");
            break;
    }

}

void mirrorDlg::on_btnChangeAutoInvert_clicked()
{
    autoInvertDlg dlg;
    dlg.setMainLabel("How should DFTFringe choose to auto invert?");
    dlg.enableConic(m_draft.cc != 0);
    dlg.exec();
    updateAutoInvertStatus();
}


