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
    m_useAnnular = false;
    m_connectAnnulusToObs = false;
    ui->setupUi(this);
    
    // Initialize defaults only; loadDraftFromSettings() called in showEvent() populates UI
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
}

mirrorDlg::~mirrorDlg()
{
    spdlog::get("logger")->trace("mirrorDlg::~mirrorDlg");
    delete ui;
}

void mirrorDlg::loadDraftFromSettings()
{
    // Load mirror settings from persistent storage via facade into working draft.
    // This ensures every dialog open/show starts with the last-saved state.
    m_draft = SettingsFacade::instance().mirrorStore().load();
    
    // Sync public member variables with draft for backward compatibility.
    m_name = m_draft.mirrorName;
    diameter = m_draft.diameter;
    roc = m_draft.roc;
    obs = m_draft.obstruction;
    cc = m_draft.cc;
    lambda = m_draft.lambda;
    fringeSpacing = m_draft.fringeSpacing;
    fliph = m_draft.flipH;
    doNull = m_draft.doNull;
    m_useAnnular = m_draft.useAnnulus;
    m_annularObsPercent = m_draft.annulusPercent;
    m_connectAnnulusToObs = m_draft.annulusToObstruction;
    m_outlineShape = (outlineShape)m_draft.outlineShape;
    m_verticalAxis = m_draft.ellipseMinorAxis;
    aperatureReduction = m_draft.apertureReduction;
    m_aperatureReductionEnabled = m_draft.apertureReductionEnabled;
    m_projectPath = m_draft.projectPath;
}

void mirrorDlg::showEvent(QShowEvent *event)
{
    // Reload draft from persistent settings before dialog becomes visible.
    // This ensures Cancel always reverts to the last-saved state.
    loadDraftFromSettings();
    
    // Sync UI with reloaded draft values
    ui->name->setText(m_draft.mirrorName);
    ui->diameter->setText(QString("%1").arg(diameter, 6, 'f', 2));
    ui->roc->setText(QString("%1").arg(roc, 6, 'f', 2));
    ui->obs->setText(QString("%1").arg(obs, 6, 'f', 2));
    ui->lambda->setText(QString("%1").arg(lambda, 6, 'f', 1));
    ui->cc->setText(QString("%1").arg(cc, 6, 'f', 2));
    ui->flipH->setChecked(m_draft.flipH);
    ui->nullCB->setChecked(m_draft.doNull);
    ui->fringeSpacingEdit->setText(QString("%1").arg(fringeSpacing, 6, 'f', 3));
    ui->ellipseShape->setChecked(m_outlineShape == ELLIPSE);
    ui->minorAxisEdit->setText(QString::number(m_verticalAxis));
    ui->ReducApp->setChecked(m_aperatureReductionEnabled);
    ui->reduceValue->setValue(aperatureReduction);
    ui->useAnnulus->setChecked(m_useAnnular);
    ui->annulusPercent->setValue(m_annularObsPercent * 100);
    
    QDialog::showEvent(event);
}

bool mirrorDlg::shouldFlipH(){
    return ui->flipH->isChecked();
}
double mirrorDlg::getMinorAxis(){
    return ui->minorAxisEdit->text().toDouble();
}

bool mirrorDlg::isEllipse(){
    return m_outlineShape == ELLIPSE;
}
void mirrorDlg::saveJson(const QString &fileName){
    QJsonObject jDoc, jMirror,jIgram, jEllipse, jAnnulus;
    jDoc["name"] = m_name;
    jDoc["show units in mm"] = mm;
    jDoc["useNull"] = doNull;
    jIgram["wavelength"] = lambda;
    jIgram["fringe spacing"] = fringeSpacing;
    jMirror["diameter"] = diameter;
    jMirror["obs diameter"] = obs;
    jMirror["roc"] = roc;
    jMirror["desired conic"] = cc;
    jMirror["edgeMaskon"] = m_aperatureReductionEnabled;
    jMirror["edge mask value"] = aperatureReduction;
    jIgram["wavelength"] = lambda;
    jIgram["null value"] = z8;
    jIgram["flip horizontal"] = fliph;
    jIgram["flip vert"] = flipv;
    jIgram["fringe spacing"] = fringeSpacing;
    jEllipse["is ellipse"] = m_outlineShape;
    jEllipse["ellipse vert axis"] = m_verticalAxis;
    jAnnulus["use annular Zernike values"] = m_useAnnular;
    jAnnulus["obs percentage"] = m_annularObsPercent;
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
    QString path = m_draft.mirrorConfigFile;
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
    
    // Update draft with new file path, then persist via facade
    m_draft.mirrorConfigFile = fileName;
    m_draft.projectPath = info.absolutePath();
    m_projectPath = m_draft.projectPath;
}

void mirrorDlg::loadFile(QString & fileName){

    // clear ellipse in case this is an old config that does not have it.
    ui->ellipseShape->setChecked(false);
    m_outlineShape = CIRCLE;
    QFileInfo info(fileName);
    
    // Only persist non-mirror-settings to QSettings (lastPath)
    QSettings settings;
    settings.setValue("lastPath", info.absolutePath());
    
    emit newPath(info.absolutePath());
    
    // Update draft with new file path and project path via facade
    m_draft.projectPath = info.absolutePath();
    m_draft.mirrorConfigFile = fileName;


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
        diameter = QJsonValue(mirror["diameter"]).toDouble();
        ui->diameter->blockSignals(true);
        ui->diameter->setText(QString("%1").arg(diameter, 6, 'f', 2));
        ui->diameter->blockSignals(false);

        ui->nullCB->setChecked( QJsonValue(loadDoc["useNull"]).toBool());

        obs = QJsonValue(mirror["obs diameter"]).toDouble();
        roc = QJsonValue(mirror["roc"]).toDouble();
        cc = QJsonValue(mirror["desired conic"]).toDouble();
        m_aperatureReductionEnabled = QJsonValue(mirror["edgeMaskon"]).toBool();
        aperatureReduction=QJsonValue( mirror["edge mask value"]).toDouble();

        QJsonObject Igram = loadDoc["igram"].toObject();
        lambda = QJsonValue(Igram["wavelength"]).toDouble();
        z8 = QJsonValue(Igram["null value"]).toDouble();
        fliph = QJsonValue(Igram["flip horizontal"]).toBool();
        flipv = QJsonValue(Igram["flip vert"]).toBool();
        fringeSpacing = QJsonValue(Igram["fringe spacing"]).toDouble();
        QJsonObject Ellipse = loadDoc["ellipse"].toObject();
        m_outlineShape = (outlineShape)QJsonValue(Ellipse["is ellipse"]).toInt();
        m_verticalAxis = QJsonValue(Ellipse["ellipse vert axis"]).toDouble();
        QJsonObject Annulus = loadDoc["Annulus"].toObject();
        m_useAnnular = QJsonValue(Annulus["use annular Zernike values"]).toBool();
        m_annularObsPercent = QJsonValue(Annulus["obs percentage"]).toDouble();
        ui->useAnnulus->setChecked(m_useAnnular);
        ui->annulusPercent->setValue(m_annularObsPercent * 100);
        on_annulusPercent_valueChanged(m_annularObsPercent * 100);
        enableAnnular(m_useAnnular);

        ui->fringeSpacingEdit->blockSignals(true);
        ui->fringeSpacingEdit->setText(QString("%1").arg(fringeSpacing, 3, 'f', 1));
        ui->fringeSpacingEdit->blockSignals(false);

        ui->obs->setText(QString().number(obs));


        ui->roc->blockSignals(true);
        ui->roc->setText(QString("%1").arg(roc, 6, 'f', 2));
        ui->roc->blockSignals(false);

        ui->cc->setText(QString().number(cc));

        ui->z8->setText(QString().number(z8));

        ui->ellipseShape->setChecked(m_outlineShape == ELLIPSE);

        ui->minorAxisEdit->setText(QString::number(m_verticalAxis));

        FNumber = roc/(2. * diameter);
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
            ui->fringeSpacingEdit->setText(QString("%1").arg(*dp, 3, 'f', 1));
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
                // diameter *= 25.4;
                //roc *= 25.4;
            }
            ui->diameter->blockSignals(true);
            ui->diameter->setText(QString("%1").arg(diameter, 6, 'f', 2));
            ui->diameter->blockSignals(false);
            ui->roc->blockSignals(true);
            ui->roc->setText(QString("%1").arg(roc, 6, 'f', 2));
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
                // read outlineShape
                file.read(buf,4);
                m_outlineShape = *(outlineShape*)buf;
                ui->ellipseShape->setChecked(m_outlineShape == ELLIPSE);

            }
            // vertical axis
            if (file.tellg() > 0){
                file.read(buf,8);
                m_verticalAxis = *(double*)buf;
                ui->minorAxisEdit->setText(QString::number(m_verticalAxis));
            }

            FNumber = roc/(2. * diameter);
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
    return m_projectPath;
}

void mirrorDlg::on_diameter_textChanged(const QString &arg1) {

    double diam = arg1.toDouble() *  ((mm) ? 1.: 25.4);
    if (m_outlineShape == ELLIPSE){
        double e = m_verticalAxis/diameter;
        m_verticalAxis = e * diam;
        ui->minorAxisEdit->setText(QString().number(m_verticalAxis));
    }
    diameter = diam;
    FNumber = roc/(2. * diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString("%1").arg(FNumber, 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    updateZ8();
    if (m_useAnnular){
        on_annulusPercent_valueChanged(m_annularObsPercent * 100);
    }

}

//Used when the just loading wavfront is different
void mirrorDlg::on_diameter_Changed(const double diam)
{
    if (m_outlineShape == ELLIPSE){
        double e = m_verticalAxis/diameter;
        m_verticalAxis = e * diam;
        ui->minorAxisEdit->setText(QString().number(m_verticalAxis));
    }
    diameter = diam ;
    FNumber = roc/(2. * diameter);
    ui->FNumber->blockSignals(true);
    const QSignalBlocker blocker(ui->diameter);
    ui->FNumber->setText(QString("%1").arg(FNumber *( (mm) ? 1.: 25.4), 6, 'f', 2));
    ui->diameter->setText(QString("%1").arg(diameter * ((mm) ? 1.: 25.4), 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    ui->diameter->blockSignals(false);

    setclearAp();
    updateZ8();

}

void mirrorDlg::on_roc_textChanged(const QString &arg1)
{
    roc = arg1.toDouble() * ((mm) ? 1: 25.4);
    FNumber = roc /(2. * diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString("%1").arg(FNumber, 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    updateZ8();
}

/* used when the just loading wavefront is different */
void mirrorDlg::on_roc_Changed(const double newVal)
{
    roc = newVal;

    FNumber = roc /(2. * diameter);
    ui->FNumber->blockSignals(true);
    ui->FNumber->setText(QString("%1").arg(FNumber * ((mm) ? 1.: 25.4), 6, 'f', 2));
    ui->FNumber->blockSignals(false);
    ui->roc->blockSignals(true);
    ui->roc->setText(QString("%1").arg(roc * ((mm) ? 1.: 25.4), 6, 'f', 2));
    ui->roc->blockSignals(false);
    updateZ8();
}
void mirrorDlg::updateZ8(){
//Z = d^6 / (16 * R^5)

    double aperature = (ui->ReducApp->isChecked()) ?  diameter- aperatureReduction*2. : diameter;

    z8 = (pow(aperature,4) * 1000000.) /
            (384. * pow(roc, 3) * lambda);


    if (m_useAnnular){
        double f = (1 - (m_annularObsPercent * m_annularObsPercent));
        f *= f;
        z8 *= f;
    }
    ui->z8->blockSignals(true);
    ui->z8->setText(QString().number(z8 * cc));
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
void mirrorDlg::newLambda(const QString &v){
    ui->lambda->setText(v);
}

void mirrorDlg::on_lambda_textChanged(const QString &arg1)
{
    lambda = arg1.toDouble();
    updateZ8();
}

void mirrorDlg::on_nullCB_clicked(bool checked)
{
    doNull = checked;
    ui->FNumber->blockSignals(true);
    ui->roc->blockSignals(true);
    if (!doNull){

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
     ui->diameter->setText(QString("%1").arg(diameter/div, 6, 'f', 2));
     ui->roc->setText(QString().number(roc/div));
     ui->obs->setText(QString().number(obs/div));
     ui->diameter->blockSignals(false);
     ui->roc->blockSignals(false);
     ui->minorAxisEdit->blockSignals(true);
     ui->minorAxisEdit->setText(QString("%1").arg(m_verticalAxis/div, 6, 'f', 2));
     ui->reduceValue->blockSignals(true);
     ui->annularDiameter->blockSignals(true);
     ui->annularDiameter->setValue(diameter * m_annularObsPercent * ((mm)? 1.: 1./25.4));
     ui->annularDiameter->blockSignals(false);
     
     // Get aperatureReduction from draft (already loaded from persistent storage)
     aperatureReduction = m_draft.apertureReduction;

     ui->reduceValue->setValue(aperatureReduction * ((mm) ? 1. : 1./25.4));
     ui->reduceValue->blockSignals(false);
     ui->ClearAp->setText(QString("%1 ").arg(m_clearAperature * ((mm) ? 1: 1./25.4), 6, 'f', 2));
}

void mirrorDlg::on_buttonBox_accepted()
{
    setclearAp();
    updateZ8();

    SurfaceManager * sm = SurfaceManager::get_instance();
    if (sm->m_inverseMode == invCONIC && cc==0) {
        sm->m_inverseMode = invNOTSET; // don't allow inverse mode to be conic if conic constant is zero
        updateAutoInvertStatus();
    }

    // Update draft with current UI values
    m_draft.mirrorName = ui->name->text();
    m_draft.diameter = diameter;
    m_draft.roc = roc;
    m_draft.obstruction = obs;
    m_draft.cc = cc;
    m_draft.lambda = lambda;
    m_draft.fringeSpacing = ui->fringeSpacingEdit->text().toDouble();
    m_draft.flipH = ui->flipH->isChecked();
    m_draft.doNull = doNull;
    m_draft.useAnnulus = m_useAnnular;
    m_draft.annulusPercent = m_annularObsPercent;
    m_draft.annulusToObstruction = m_connectAnnulusToObs;
    m_draft.outlineShape = (int)m_outlineShape;
    m_draft.ellipseMinorAxis = m_verticalAxis;
    m_draft.apertureReductionEnabled = m_aperatureReductionEnabled;
    m_draft.apertureReduction = aperatureReduction;
    m_draft.projectPath = m_projectPath;
    
    // Persist draft to QSettings via facade (single atomic save)
    SettingsFacade::instance().mirrorStore().save(m_draft);

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

}

void mirrorDlg::on_fringeSpacingEdit_textChanged(const QString & /*text*/)
{
    spacingChangeTimer.start(1000);
}

void mirrorDlg::on_name_editingFinished()
{
    m_name = ui->name->text();

}


void mirrorDlg::on_minorAxisEdit_textChanged(const QString &arg1)
{

    m_verticalAxis = arg1.toDouble();
}

void mirrorDlg::setMinorAxis(double val){
    m_verticalAxis = val;
    ui->minorAxisEdit->setText(QString::number(val));
    //on_minorAxisEdit_textChanged( QString::number(val));
}

void mirrorDlg::on_ellipseShape_clicked(bool checked)
{
    if (checked) m_outlineShape = ELLIPSE;
    else m_outlineShape = CIRCLE;

    if (m_verticalAxis == 0){
        m_verticalAxis = diameter;
        ui->minorAxisEdit->setText(QString().number(m_verticalAxis));
    }
}


void mirrorDlg::on_buttonBox_helpRequested()
{
    QString link = qApp->applicationDirPath() + "/res/Help/mirrorConfig.html";
    QDesktopServices::openUrl(QUrl(link));
}

void mirrorDlg::setclearAp(){

    m_clearAperature = (diameter - aperatureReduction * 2) ;
    if (m_aperatureReductionEnabled == false)
        m_clearAperature = diameter;
    ui->ClearAp->setText(QString("%1 ").arg(m_clearAperature * ((mm) ? 1: 1./25.4), 6, 'f', 2));
}

void mirrorDlg::on_ReducApp_clicked(bool checked)
{
    m_aperatureReductionEnabled = checked;
    ui->reduceValue->setEnabled(checked);
    ui->ClearAp->setVisible(checked);
    ui->clearApLabel->setVisible(checked);
    updateZ8();

    ui->reduceValue->setValue(aperatureReduction);
    m_aperatureReductionValueChanged = true;
    setclearAp();
    emit aperatureChanged();
}


void mirrorDlg::on_reduceValue_valueChanged(double arg1)
{
    aperatureReduction = ((mm) ? 1: 25.4) * arg1;
    updateZ8();

    setclearAp();
    m_aperatureReductionValueChanged = true;
    emit aperatureChanged();
}

void mirrorDlg::on_annulusPercent_valueChanged(double arg1)
{
    ui->annularDiameter->blockSignals(true);
    m_annularObsPercent = .01 * arg1;
    ui->annularDiameter->setValue( m_annularObsPercent * diameter * ( (mm) ? 1.: 1./25.4));
    ui->annularDiameter->blockSignals(false);

    if (m_connectAnnulusToObs){
        ui->obs->setText(QString::number(m_annularObsPercent * diameter * ((mm)? 1.: 1./25.4)));
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
    m_useAnnular = checked;
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
    m_annularObsPercent = arg1/diameter;
    ui->annulusPercent->setValue(m_annularObsPercent * 100);
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
    dlg.enableConic(cc != 0);
    dlg.exec();
    updateAutoInvertStatus();
}


