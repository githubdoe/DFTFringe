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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cmath>
#include "zernikedlg.h"
#include <QDateTime>
#include <QtCore>
#include <qtconcurrentrun.h>
#include <qtconcurrentrun.h>
#include "zernikes.h"
#include "zernikeprocess.h"
#include "simigramdlg.h"
#include "usercolormapdlg.h"
#include <qlayout.h>
#include <opencv2/opencv.hpp>
#include "simulationsview.h"
#include "outlinehelpdocwidget.h"
#include "bathastigdlg.h"
#include "settingsigram.h"
#include "Circleoutline.h"
#include "cameracalibwizard.h"
#include "astigstatsdlg.h"
#include "astigscatterplot.h"
#include "rmsplot.h"
#include "regionedittools.h"
#include "utils.h"
#include "colorchannel.h"
#include "opencv2/opencv.hpp"
#include "spdlog/spdlog.h"


using namespace QtConcurrent;
std::vector<wavefront*> g_wavefronts;
int g_currentsurface = 0;
QScrollArea *gscrollArea;
MainWindow *MainWindow::me = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),m_showChannels(false), m_showIntensity(false),m_inBatch(false),m_OutlineDoneInBatch(false),
    m_batchMakeSurfaceReady(false), m_astigStatsDlg(0), m_cameraCalibWizard(nullptr)
{
    ui->setupUi(this);
    ui->useAnnulust->hide();
    spdlog::get("logger")->info("DFTFringe {} started", APP_VERSION);

    //const QString toolButtonStyle("QToolButton {"
    //                                "border-style: outset;"
    //                                "border-width: 3px;"
    //                                "border-radius:7px;"
    //                                "border-color: darkgray;"
    //                                "font: bold 12px;"
    //                                "min-width: 10em;"
    //                                "padding: 6px;}"
    //                            "QToolButton:hover {background-color: lightblue;"
    //                                    " }");
    //QWidget *rw = ui->toolBar->widgetForAction(ui->actionRead_WaveFront);
    //igramArea = new IgramArea(ui->tabWidget->widget(0));

    //rw = ui->toolBar->widgetForAction(ui->actionSubtract_wave_front);
    //rw->setStyleSheet(toolButtonStyle);
    me = this;
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->toolBar->setStyleSheet("QToolBar { spacing: 10px;}");

    ui->SelectOutSideOutline->setChecked(true);
    setCentralWidget(ui->tabWidget);

    m_colorChannels = new ColorChannelDisplay(nullptr);

    m_intensityPlot = new igramIntensity(nullptr);

    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(0);

    // setup igram window

    scrollArea = new QScrollArea();

    gscrollArea = scrollArea;
    m_igramArea = new IgramArea(scrollArea, this);
    connect(m_igramArea, SIGNAL(imageSize(QString)), this, SLOT(imageSize(QString)));
    connect(m_igramArea, SIGNAL(statusBarUpdate(QString,int)), this, SLOT(showMessage(QString, int)));
    connect(zernikeProcess::get_Instance(), SIGNAL(statusBarUpdate(QString,int)), this,SLOT(showMessage(QString,int)));
    connect(m_igramArea, SIGNAL(upateColorChannels(cv::Mat)), this, SLOT(updateChannels(cv::Mat)));
    connect(m_igramArea, SIGNAL(showTab(int)),  ui->tabWidget, SLOT(setCurrentIndex(int)));
    connect(this, SIGNAL(gammaChanged(bool,double)), m_igramArea, SLOT(gammaChanged(bool, double)));
    m_igramArea->setBackgroundRole(QPalette::Base);
    installEventFilter(m_igramArea);
    //scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(m_igramArea);
    ui->tabWidget->addTab(scrollArea, "igram");

    m_dftTools = new DFTTools(this);
    m_vortexDebugTool = new vortexDebug(this);

    // setup DFT window
    scrollAreaDft = new QScrollArea;
    scrollAreaDft->setBackgroundRole(QPalette::Base);
    m_dftArea = DFTArea::get_Instance(scrollAreaDft, m_igramArea, m_dftTools, m_vortexDebugTool);
    connect(m_dftArea, SIGNAL(statusBarUpdate(QString,int)), this, SLOT(showMessage(QString,int)));
    connect(ui->pushButton, SIGNAL(clicked()), m_dftArea, SLOT(outlineDoneSig()));
    scrollAreaDft->setWidget(m_dftArea);
    scrollAreaDft->resize(800,800);
    ui->tabWidget->addTab(scrollAreaDft, "Analyze");

    m_contourTools = new ContourTools(this);
    m_outlineHelp = new outlineHelpDocWidget(this);
    m_outlinePlots = new outlinePlots(this);
    m_surfTools = surfaceAnalysisTools::get_Instance(this);
    m_regionsEdit = new regionEditTools(this);
    m_regionsEdit->hide();
    m_igramArea->m_regionEdit = m_regionsEdit;
    connect(m_regionsEdit, SIGNAL(addregion()),m_igramArea, SLOT(addregion()));
    connect(m_regionsEdit, SIGNAL(deleteregion(int)), m_igramArea, SLOT(deleteregion(int)));
    connect(m_regionsEdit, SIGNAL(selectRegion(int)), m_igramArea, SLOT(selectRegion(int)));
    //DocWindows
    createDockWindows();

    userMapDlg = new userColorMapDlg();

    m_contourView = new contourView(this, m_contourTools);
    connect(m_contourView, SIGNAL(zoomMe(bool)),this, SLOT(zoomContour(bool)));
    m_ogl = new OGLView(0, m_contourTools);
    connect(m_ogl, SIGNAL(fullScreen()), this, SLOT(zoomOgl()));

    connect(userMapDlg, SIGNAL(colorMapChanged(int)), m_contourView->getPlot(), SLOT(ContourMapColorChanged(int)));
    //connect(userMapDlg, SIGNAL(colorMapChanged(int)),m_ogl->m_gl, SLOT(colorMapChanged(int)));
    review = new reviewWindow(this);
    review->s1->addWidget(m_ogl);

    m_profilePlot =  new ProfilePlot(review->s2,m_contourTools);
    connect(m_profilePlot, SIGNAL(zoomMe(bool)), this, SLOT(zoomProfile(bool)));
    connect(m_profilePlot, SIGNAL(profileAngleChanged(double)), m_contourView->getPlot(), SLOT(drawProfileLine(double)));
    connect(m_contourView->getPlot()    , SIGNAL(sigPointSelected(const QPointF&)), m_profilePlot, SLOT(contourPointSelected(const QPointF&)));
    m_mirrorDlg = mirrorDlg::get_Instance();
    review->s2->addWidget(review->s1);
    review->s2->addWidget(m_profilePlot);
    m_profilePlot->setStyleSheet( {"border: 3px outset darkgrey;"});
    review->s1->addWidget(m_contourView);
    QRect rec = QGuiApplication::primaryScreen()->geometry();
    review->s1->setSizes({INT_MAX, INT_MAX});
    review->s2->setSizes({ rec.height()/2,  rec.height()/2});
    review->s2->setStyleSheet(
        "QSplitter::handle:vertical{ border: 3px outset #004545; background: #ccffff }"
    );
    review->s1->setStyleSheet(
          "QSplitter::handle:horizontal{ border: 3px outset #004545; background: #ccffff }"
    );

    //Surface Manager
    m_surfaceManager = SurfaceManager::get_instance(this,m_surfTools, m_profilePlot, m_contourView,
                                          m_ogl->m_surface, metrics);
    connect(m_contourView, SIGNAL(showAllContours()), m_surfaceManager, SLOT(showAllContours()));
    connect(m_dftArea, SIGNAL(newWavefront(cv::Mat,CircleOutline,CircleOutline,QString, QVector<std::vector<cv::Point> >)),
            m_surfaceManager, SLOT(createSurfaceFromPhaseMap(cv::Mat,CircleOutline,CircleOutline,QString, QVector<std::vector<cv::Point> >)));
    connect(m_surfaceManager, SIGNAL(diameterChanged(double)),this,SLOT(diameterChanged(double)));
    connect(m_surfaceManager, SIGNAL(showTab(int)), ui->tabWidget, SLOT(setCurrentIndex(int)));
    connect(m_surfTools, SIGNAL(updateSelected()), m_surfaceManager, SLOT(backGroundUpdate()));
    ui->tabWidget->addTab(review, "Results");

    ui->tabWidget->addTab(SimulationsView::getInstance(ui->tabWidget), "Star Test, PSF, MTF");
    ui->tabWidget->addTab(foucaultView::get_Instance(m_surfaceManager), "Ronchi & Foucault");
    scrollArea->setWidgetResizable(true);
    scrollAreaDft->setWidgetResizable(true);
    createActions();

    //Recent Files list
    separatorAct = ui->menuFiles->addSeparator();
    for (int i = 0; i < MaxRecentFiles; ++i)
        ui->menuFiles->addAction(recentFileActs[i]);
    updateRecentFileActions();
    qRegisterMetaType<QVector<int> >();

    connect( m_igramArea, SIGNAL(enableShiftButtons(bool)), this,SLOT(enableShiftButtons(bool)));
    connect(m_dftArea, SIGNAL(dftReady(QImage)), m_igramArea,SLOT(dftReady(QImage)));
    connect(m_igramArea, SIGNAL(dftCenterFilter(double)), m_dftArea, SLOT(dftCenterFilter(double)));
    connect(m_igramArea, SIGNAL(doDFT()), m_dftArea, SLOT(doDFT()));
    enableShiftButtons(true);


    connect(m_dftTools,SIGNAL(doDFT()),m_dftArea,SLOT(doDFT()));
    settingsDlg = Settings2::getInstance();
    connect(settingsDlg->m_igram, SIGNAL(igramLinesChanged(outlineParms)), m_igramArea, SLOT(igramOutlineParmsChanged(outlineParms)));
    connect(settingsDlg->m_general, SIGNAL(updateContourPlot()),m_contourView, SLOT(updateRuler()));

    QSettings settings;
    spdlog::get("logger")->trace("qSettings stored at: {}", settings.fileName().toStdString());

    restoreState(settings.value("MainWindow/windowState").toByteArray());
    restoreGeometry(settings.value("geometry").toByteArray());
    connect(m_dftArea,SIGNAL(selectDFTTab()), this, SLOT(selectDftTab()));
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this, SLOT(mainTabChanged(int)));
    tabifyDockWidget(ui->outlineTools, m_dftTools);
    setTabPosition(Qt::RightDockWidgetArea, QTabWidget::West);
    setTabShape(QTabWidget::Triangular);
    ui->tabWidget->setTabShape(QTabWidget::Triangular);
    tabifyDockWidget(m_dftTools,m_contourTools);
    tabifyDockWidget(m_contourTools,m_surfTools);
    //tabifyDockWidget(m_contourTools,m_vortexTools);
    //tabifyDockWidget(m_vortexDebugTool,m_metrics);
    ui->outlineTools->show();
    ui->outlineTools->raise();
    ui->useExistingAsGuide->setChecked(settings.value("useGuideOutline", false).toBool());
    ui->autoTraceCB->setChecked(settings.value("autoOutline", false).toBool());
    ui->scanMargin->setValue( settings.value("outlineScanRange",40).toInt());
    // setup auto outline controls.
    zernEnables = std::vector<bool>(Z_TERMS, true);

    //disable first 8 enables except for astig
    // enable first 8 nulls except for astig
    for (int i = 0; i < 8; ++ i)
    {
        if (i == 4 || i == 5)
            continue;
        zernEnables[i] = false;
    }

    connect(m_surfaceManager, SIGNAL(rocChanged(double)),this, SLOT(rocChanged(double)));
    connect(m_mirrorDlg, SIGNAL(newPath(QString)),this, SLOT(newMirrorDlgPath(QString)));
    progBar = new QProgressBar(this);

    status1 = new QLabel();
    status2 = new QLabel();
    status3 = new QLabel();
    ui->statusBar->addWidget(status1);
    ui->statusBar->addWidget(status3);
    ui->statusBar->addPermanentWidget(status2,2);
    ui->statusBar->addPermanentWidget(progBar,1);
    QStringList args = QCoreApplication::arguments();

    // setup color channel selection controls
    ui->autoChannel->setChecked(settings.value("colorChannelUseAuto", true).toBool());
    ui->greenChannel->setChecked(settings.value("colorChannelUseGreen",false).toBool());
    ui->redChannel->setChecked(settings.value("colorChannelUseRed", false).toBool());
    ui->blueChannel->setChecked(settings.value("colorChannelUseBlue", false).toBool());
    ui->showColorIgram->setChecked(settings.value("colorChannelShowColor", false).toBool());

    openWaveFrontonInit(args);

}
int showmem(QString t);
void MainWindow::openWaveFrontonInit(QStringList args){
    QProgressDialog pd("    Loading wavefronts in progress.", "Cancel", 0, 100);
    pd.setRange(0, args.size());
    if (args.length()> 0)
        pd.show();
    m_surfaceManager->initWaveFrontLoad();
    int cnt = 0;

    foreach( QString arg, args){
        //int mem = showmem("loading");

//        if (mem< memThreshold){
//            int resp = QMessageBox::warning(0,"low on memory", "Do you want to continue?", QMessageBox::Yes|QMessageBox::No);
//            if (resp == QMessageBox::No)
//               break;
//        }
        qApp->processEvents();
        if (pd.wasCanceled())
            break;

        if (arg.endsWith(".wft", Qt::CaseInsensitive)){
            pd.setLabelText(arg);
            try {
            m_surfaceManager->loadWavefront(arg);
            }
            catch (int i){
                break;
            }
            pd.setValue(++cnt);

        }
    }

    ui->tabWidget->setCurrentIndex(2);


}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    QMainWindow::closeEvent(event);
    QWidget::closeEvent(event);
}

MainWindow::~MainWindow()
{
    spdlog::get("logger")->trace("MainWindow::~MainWindow");
    if(m_cameraCalibWizard != nullptr){
        m_cameraCalibWizard->close();
    }
    delete m_colorChannels;
    delete m_intensityPlot;
    delete m_ogl;
    userMapDlg->close();
    mirrorDlg::get_Instance()->close();
    settingsDlg->close();
    delete settingsDlg;
    delete ui;
}


void MainWindow::mainTabChanged(int ndx){

    switch(ndx){

    case 0:
        ui->outlineTools->show();
        ui->outlineTools->raise();
        break;
    case 1: //dft
        m_dftTools->show();
        m_dftTools->raise();
        break;
    case 2: //surface
        m_surfTools->show();
        m_surfTools->raise();
        break;
    case 3:
    {
        SimulationsView *sv = SimulationsView::getInstance(0);
        if (sv->needs_drawing){
            sv->on_MakePB_clicked();
            //QApplication::restoreOverrideCursor();
        }
        break;
    }
    case 4:
    {
        foucaultView *fv = foucaultView::get_Instance();
        if (fv->needsDrawing) {
            fv->on_makePb_clicked();

        }
    }
        break;
    }
}

void MainWindow::openRecentFile()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action)
        loadFile(action->data().toString());
}


bool MainWindow::loadFile(const QString &fileName)
{
    m_igramArea->openImage(fileName);
    ui->SelectOutSideOutline->setChecked(true);
    setCurrentFile(fileName);
    return true;
}
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    setWindowFilePath(curFile);
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();
    files.removeAll(fileName);
    files.prepend(fileName);
    while (files.size() > MaxRecentFiles)
        files.removeLast();

    settings.setValue("recentFileList", files);
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
        MainWindow *mainWin = qobject_cast<MainWindow *>(widget);
        if (mainWin)
            mainWin->updateRecentFileActions();
    }
}

void MainWindow::updateRecentFileActions()
{
    QSettings settings;
    QStringList files = settings.value("recentFileList").toStringList();

    int numRecentFiles = qMin(files.size(), (int)MaxRecentFiles);

    for (int i = 0; i < numRecentFiles; ++i) {
        QString text = tr("&%1 %2").arg(i + 1).arg(strippedName(files[i]));
        recentFileActs[i]->setText(text);
        recentFileActs[i]->setData(files[i]);
        recentFileActs[i]->setVisible(true);
    }
    for (int j = numRecentFiles; j < MaxRecentFiles; ++j)
        recentFileActs[j]->setVisible(false);

    separatorAct->setVisible(numRecentFiles > 0);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::on_actionLoad_Interferogram_triggered()
{
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QFileDialog dialog(this, tr("Open File"), lastPath);
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
    ui->SelectObsOutline->setChecked(false);
    ui->useAnnulust->hide();
    if (dialog.exec()){
        if (dialog.selectedFiles().size() == 1){
            loadFile(dialog.selectedFiles().first());

        }
        else{
            m_igramsToProcess = dialog.selectedFiles();
            Batch_Process_Interferograms();
        }
    }

}

void MainWindow::createActions()
{
    for (int i = 0; i < MaxRecentFiles; ++i) {
        recentFileActs[i] = new QAction(this);
        recentFileActs[i]->setVisible(false);
        connect(recentFileActs[i], SIGNAL(triggered()),
                this, SLOT(openRecentFile()));

    }
}
//Crop button
void MainWindow::on_pushButton_5_clicked()
{
    m_igramArea->crop();
    // make sure current boundary is the outside one.
    ui->SelectOutSideOutline->setChecked(true);
    ui->SelectObsOutline->setChecked(false);

}


void MainWindow::on_pushButton_8_clicked()
{
    m_igramArea->deleteOutline();
}

void MainWindow::on_pushButton_7_clicked()
{
    m_igramArea->readOutlines();
}

void MainWindow::enableShiftButtons(bool enable){
    ui->shiftDown->setEnabled(enable);
    ui->ShiftUp->setEnabled(enable);
    ui->shiftLeft->setEnabled(enable);
    ui->shiftRight->setEnabled(enable);
}
void MainWindow::createDockWindows(){

    m_contourTools->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    m_dftTools->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    metrics = metricsDisplay::get_instance(this);
    metrics->setWindowTitle(QString("metrics      DFTFringe %1").arg(APP_VERSION));
    zernTablemodel = metrics->tableModel;
    addDockWidget(Qt::LeftDockWidgetArea, m_outlineHelp);
    addDockWidget(Qt::LeftDockWidgetArea, m_outlinePlots);
    addDockWidget(Qt::RightDockWidgetArea, m_regionsEdit);
    splitDockWidget(m_regionsEdit, ui->outlineTools, Qt::Vertical);
    addDockWidget(Qt::RightDockWidgetArea, m_dftTools);
    addDockWidget(Qt::RightDockWidgetArea, m_contourTools);
    addDockWidget(Qt::RightDockWidgetArea, m_surfTools);
    addDockWidget(Qt::LeftDockWidgetArea,  metrics);
    addDockWidget(Qt::LeftDockWidgetArea, m_vortexDebugTool);
    ui->menuView->addAction(m_dftTools->toggleViewAction());
    ui->menuView->addAction(ui->outlineTools->toggleViewAction());
    ui->menuView->addAction(m_contourTools->toggleViewAction());
    ui->menuView->addAction(m_surfTools->toggleViewAction());
    ui->menuView->addAction(metrics->toggleViewAction());
    ui->menuView->addAction(m_vortexDebugTool->toggleViewAction());
    ui->menuView->addAction(m_outlineHelp->toggleViewAction());
    ui->menuView->addAction(m_outlinePlots->toggleViewAction());
    m_outlineHelp->hide();
    metrics->hide();
    m_vortexDebugTool->hide();
    m_outlinePlots->hide();
}
void MainWindow::updateMetrics(wavefront& wf){
    metrics->setName(wf.name);
    metrics->mDiam->setText(QString("%1").arg(wf.diameter, 6, 'f', 3));
    metrics->mROC->setText(QString("%1").arg(wf.roc, 6, 'f', 3));
    const double  e = 2.7182818285;
    metrics->mRMS->setText(QString("<b><FONT FONT SIZE = 12>%1</b>").arg(wf.std, 6, 'f', 3));
    double st =(2. *M_PI * wf.std);
    st *= st;
    double Strehl = pow(e, -st);
    metrics->mStrehl->setText(QString("<b><FONT FONT SIZE = 12>%1</b>").arg(Strehl, 6, 'f', 3));
    QString ztitle("Zernike Values");
    if (m_mirrorDlg->m_useAnnular){
        ztitle = QString("Annular Zernike values %1% center hole").arg(100 * m_mirrorDlg->m_annularObsPercent, 6, 'f',1);
    }
    metrics->setZernTitle(ztitle);
    double z8 = zernTablemodel->values[8];
    if (m_mirrorDlg->doNull && wf.useSANull){
        Settings2 &settings = *Settings2::getInstance();

        BestSC = z8/m_mirrorDlg->z8;

    }
    else {
        BestSC = m_mirrorDlg->cc +z8/m_mirrorDlg->z8;
    }
    metrics->setOutputLambda(outputLambda);

    metrics->setWavePerFringe(m_mirrorDlg->fringeSpacing, m_mirrorDlg->lambda);
    if (m_mirrorDlg->doNull)
        metrics->mCC->setText(QString("<FONT FONT SIZE = 7>%1").arg(BestSC, 6 ,'f', 3));
    else {
        metrics->mCC->setText("NA");
    }
    if (m_mirrorDlg->isEllipse()){
        metrics->mCC->setText("NA");
        metrics->mROC->setText("NA");
    }
    m_outlineHelp->hide();
    metrics->show();
}

void MainWindow::on_actionLoad_outline_triggered()
{
    m_igramArea->readOutlines();
}

void MainWindow::on_ShiftUp_clicked()
{
    m_igramArea->shiftUp();
}

void MainWindow::on_shiftLeft_clicked()
{
    m_igramArea->shiftLeft();
}

void MainWindow::on_shiftDown_clicked()
{
    m_igramArea->shiftDown();
}

void MainWindow::on_shiftRight_clicked()
{
    m_igramArea->shiftRight();
}
int MainWindow::getCurrentTab(){
    return ui->tabWidget->currentIndex();
}
void MainWindow::setTab(int ndx){
    ui->tabWidget->setCurrentIndex(ndx);
}
void MainWindow::selectDftTab(){

    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::updateChannels(cv::Mat img){


    if (m_showChannels){
        m_colorChannels->setImage(img);
        m_colorChannels->show();
    }

    if (m_showIntensity){
        m_intensityPlot->setIgram(img);
        m_intensityPlot->show();
    }
}

QStringList MainWindow::SelectWaveFrontFiles(){
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();

    QFileDialog dialog(this);
    dialog.setDirectory(lastPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("wft (*.wft)"));

    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        if (fileNames.size() > 0){
            QFileInfo info(fileNames[0]);
            lastPath = info.absolutePath();
            settings.setValue("lastPath",lastPath);
            return dialog.selectedFiles();
        }
    }
    return QStringList();
}

void MainWindow::on_actionRead_WaveFront_triggered()
{
    QStringList fileNames = SelectWaveFrontFiles();
    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();
    openWaveFrontonInit(fileNames);
    this->setCursor(Qt::ArrowCursor);
    ui->tabWidget->setCurrentIndex(2);

}
/*
fields = l.split()
if fields[0] == 'outside':
    el = Bounds.Circle(float(fields[2]), float(fields[3]), float(fields[4]))

elif fields[0] == 'DIAM':
    diam = float(fields[1])
elif fields[0] == 'ROC':
    roc = float(fields[1])
elif fields[0] == 'Lambda':
    iLambda = float(fields[1])
    pass
elif fields[0] == 'obstruction':
    obs = Bounds.Circle(float(fields[2]), float(fields[3]), float(fields[4]))

if not el:
*/

void MainWindow::on_actionPreferences_triggered()
{
    settingsDlg->show();
}



void MainWindow::on_actionMirror_triggered()
{
    m_mirrorDlg->show();
}


wavefront* MainWindow::getCurrentWavefront()
{
    return m_surfaceManager->m_wf;
}
void MainWindow::on_actionSave_outline_triggered()
{
       m_igramArea->saveOutlines();
}
void MainWindow::on_saveOutline_clicked()
{
    m_igramArea->saveOutlines();
}

//void MainWindow::on_actionSimulated_interferogram_triggered()
//{
//    m_igramArea->generateSimIgram();
//}

void MainWindow::on_actionPrevious_Wave_front_triggered()
{
    m_surfaceManager->previous();
}

void MainWindow::on_actionNext_Wave_Front_triggered()
{
    m_surfaceManager->next();
}

void MainWindow::on_actionDelete_wave_front_triggered()
{
    m_surfaceManager->deleteCurrent();
}



void MainWindow::on_actionWrite_WaveFront_triggered()
{

}


void MainWindow::on_actionSave_Wavefront_triggered()
{
    m_surfaceManager->SaveWavefronts(false);
}
void MainWindow::showMessage(QString msg, int id){

    switch(id){
    case 1:
        status1->setText(msg);
        break;
    case 2:
        status2->setText(QString("      ") + msg);
        break;
    case 3:
        status3->setText(msg);
        break;
    }


}

void MainWindow::diameterChanged(double v){
    m_mirrorDlg->on_diameter_Changed(v);
}
void MainWindow::rocChanged(double v){
    m_mirrorDlg->on_roc_Changed(v);
}



void MainWindow::on_SelectOutSideOutline_clicked(bool checked)
{
    m_igramArea->SideOutLineActive( checked);
    m_regionsEdit->hide();
    ui->useAnnulust->hide();
}

void MainWindow::on_SelectObsOutline_clicked(bool checked)
{
    ui->useAnnulust->show();
    m_igramArea->CenterOutlineActive(checked);
    m_regionsEdit->hide();
}
//Outline Done button
void MainWindow::on_pushButton_clicked()
{
    if (!m_inBatch)
        m_igramArea->nextStep();
    else {
        m_OutlineDoneInBatch = true;
    }
    ui->SelectOutSideOutline->setChecked(true);
}
void MainWindow::skipBatchItem()
{
    m_OutlineDoneInBatch = true;
    m_skipItem = true;
    m_batchMakeSurfaceReady = true;
    if (batchIgramWizard::autoCb->isChecked()){
        batchConnections(false);
    }
}

void MainWindow::on_showChannels_clicked(bool checked)
{
    m_showChannels = checked;
    if (checked){
        m_colorChannels->setImage(m_igramArea->qImageToMat(m_igramArea->igramColor));
        m_colorChannels->show();
    }
    else
        m_colorChannels->close();
}
void MainWindow::imageSize(QString txt){
    ui->igramSize->setText(txt);
}

void MainWindow::on_showIntensity_clicked(bool checked)
{
    m_showIntensity = checked;
    if (checked){
        m_intensityPlot->setIgram(m_igramArea->qImageToMat(m_igramArea->igramColor));
        m_intensityPlot->show();
    }
    else
        m_intensityPlot->close();
}
void MainWindow::newMirrorDlgPath(QString path){
    QFileInfo info(path);
    QSettings settings;
    settings.setValue("lastPath",info.path());
}
//make a simulated wavefront based on zernike values
#define TSIZE 200
void MainWindow::on_actionWavefront_triggered()
{
    simIgramDlg &dlg = *simIgramDlg::get_instance();

    dlg.setWindowTitle("Wavefront terms");
    if (!dlg.exec())
        return;
    int border = 3;
    int wx = dlg.size + 2 * border;

    double rad = (double)(wx-1)/2.;
    double xcen = rad;
    double ycen = rad;


    rad -= border;
    cv::Mat result = zernikeProcess::get_Instance()->makeSurfaceFromZerns(border, false);
    m_surfaceManager->createSurfaceFromPhaseMap(result,
                                                CircleOutline(QPointF(xcen,ycen),rad),
                                                CircleOutline(QPointF(0,0),0),
                                                QString("Simulated_Wavefront"));
}

void MainWindow::on_actionIgram_triggered()
{
    m_igramArea->generateSimIgram();
}

void MainWindow::on_actionSave_interferogram_triggered()
{
    m_igramArea->save();
}

void MainWindow::on_actionSave_screen_triggered()
{
    QSettings set;
    QString path = set.value("mirrorConfigFile").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString dd = info.dir().absolutePath();
    const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList filter;
    if ( imageFormats.size() > 0 )
    {
        QString imageFilter( tr( "Images" ) );
        imageFilter += " (";
        for ( int i = 0; i < imageFormats.size(); i++ )
        {
            if ( i > 0 )
                imageFilter += " ";
            imageFilter += "*.";
            imageFilter += imageFormats[i];
        }
        imageFilter += ")";

        filter += imageFilter;
    }
    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save screen"), dd + "//DFTScreen.jpg",filter.join( ";;" ));


    if (fName.isEmpty())
        return;

    QImage image( this->size(), QImage::Format_ARGB32 );


    QImage SurfaceImage = m_ogl->m_surface->render(1000,1000);

    QPainter painter( &image );

    this->render( &painter);

    painter.setPen(QPen(Qt::red,5));
    QRect widgetRect = m_ogl->m_container->geometry();
    widgetRect.moveTopLeft(m_ogl->mapToGlobal(QPoint(0,20)));

    painter.drawImage(widgetRect, SurfaceImage);


    painter.end();


    image.save( fName, QFileInfo( fName ).suffix().toLatin1() );

}

void MainWindow::on_actionWave_Front_Inspector_triggered()
{
    m_surfaceManager->inspectWavefront();
}

void MainWindow::on_actionUser_Color_Map_triggered()
{
    userMapDlg->show();
}

void MainWindow::on_actionSave_nulled_smoothed_wavefront_triggered()
{
        m_surfaceManager->SaveWavefronts(true);
}

void MainWindow::on_minus_clicked()
{
    m_igramArea->decrease();
}

void MainWindow::on_pluss_clicked()
{
    m_igramArea->increase();
}

void MainWindow::on_actionTest_Stand_Astig_Removal_triggered()
{
    m_surfaceManager->computeTestStandAstig();
}


void MainWindow::on_reverseGammaCB_clicked(bool checked)
{
    emit gammaChanged(checked, ui->gammaValue->text().toDouble());
}


void MainWindow::on_actionSubtract_wave_front_triggered()
{
    m_surfaceManager->subtractWavefronts();
}

void MainWindow::on_actionSave_PDF_report_triggered()
{
    m_surfaceManager->report();
}


void MainWindow::on_actionHelp_triggered()
{
    QString link = qApp->applicationDirPath() + "/res/Help/help.html";
    QDesktopServices::openUrl(QUrl(link));

}

void MainWindow::on_actionAbout_triggered()
{

    QMessageBox::information(this, "___________________________________________________________________________About", 
                            QString("<html><body><h1>DFTFringe version %1</h1>").arg(APP_VERSION)+
                             "<p>This program was compiled using:<ul><li>"
                             "Qt version " + QT_VERSION_STR + " </li>"
                    #if defined(__GNUC__) && defined(__VERSION__)
                        #if defined(__clang__)
                             "<li> Compiled with clang " + __VERSION__ + "</li>"
                        #elif defined(__llvm__)
                             "<li> Compiled with llvm-gcc " + __VERSION__ + "</li>"
                        #elif defined(__MINGW64__)
                             "<li> Compiled with MinGW-w64 64bit " + QString::number(__MINGW64_VERSION_MAJOR) + "." + QString::number(__MINGW64_VERSION_MINOR) + " GCC " + __VERSION__ +"</li>"
                        #elif defined(__MINGW32__)
                             "<li> Compiled with MinGW32 " + QString::number(__MINGW32_VERSION_MAJOR) + "." + QString::number(__MINGW32_VERSION_MINOR) + " GCC " + __VERSION__ +"</li>"
                        #else
                             "<li> Compiled with GCC " + __VERSION__ + "</li>"
                        #endif
                    #elif defined(_MSC_FULL_VER)
                             "<li> Compiled with Microsoft Visual Studio " + QString::number(_MSC_FULL_VER) + "</li>"
                    #else
                             "<li> Compiled with unknown compiler (please report to add support)</li>"
                    #endif
                             "<li> OpenCV " + CV_VERSION + "</li>"
                             "<li> QWT " + QWT_VERSION_STR + ", armadillo " + QString::fromStdString(arma::arma_version::as_string()) + ", Lapack, BLAS</li></ul></p>"
                             "<h3>Credits</h3>"
                             "<ul><li>Mike Peck for<ul><li>Researching and explaining FFT algorithm.</li>"
                             "<li>Researching and explaining Unwrap algorithms.</li></li>"
                             "<li>Steve Koehler for much help with Vortex implementation.</li>"
                             "<li>Dave Rowe for starting the Interferometry group and publishig the initial program.</li>"
                             "<li>Kieran Larken for deriving and publishing the Vortex Transform.</li>"
                             "<li>Jim Burrows for developing the original diffract program which used FFT for Foucault Simulation."
                             "</ul>"
                             "<li><a href=\"https://groups.io/g/Interferometry\"><span style=\" text-decoration: underline; color:#0000ff;\">Additional help at Interferometry Forum</span></a></li>"

                             "</body></html>");
}

void MainWindow::on_actionVideos_triggered()
{
    QString link = "https://www.youtube.com/channel/UCPj57WFSSLpVqPir7Im-kiw";
    QDesktopServices::openUrl(QUrl(link));

}

void MainWindow::on_undo_pressed()
{
    m_igramArea->undo();
}

void MainWindow::on_Redo_pressed()
{
    m_igramArea->redo();
}

void MainWindow::on_pushButton_clicked(bool checked)
{
    m_igramArea->hideOutline(checked);
}

void MainWindow::on_checkBox_clicked(bool checked)
{
    m_igramArea->hideOutline(checked);
}
#include "nullvariationdlg.h"
void MainWindow::on_actionError_Margins_triggered()
{
    nullVariationDlg dlg;
    dlg.exec();
}
void MainWindow::batchMakeSurfaceReady(){
    m_batchMakeSurfaceReady = true;
}
void MainWindow::batchFinished(int ){
    batchConnections(false);
}

void MainWindow::batchConnections(bool flag){
    qDebug() << "BatchConnection " << flag;
    if (flag){
        m_inBatch = true;
        disconnect(m_dftTools, SIGNAL(makeSurface()), m_dftArea, SIGNAL(makeSurface()));
        connect(m_dftTools, SIGNAL(makeSurface()), this, SLOT(batchMakeSurfaceReady()));
        connect(batchIgramWizard::saveZerns, SIGNAL(pressed()), this, SLOT(saveBatchZerns()));
    }
    else {
        m_inBatch = false;
        connect(m_dftTools, SIGNAL(makeSurface()), m_dftArea, SLOT(makeSurface()));
        disconnect(m_dftTools, SIGNAL(makeSurface()), this, SLOT(batchMakeSurfaceReady()));
        disconnect(batchIgramWizard::saveZerns, SIGNAL(pressed()), this, SLOT(saveBatchZerns()));
    }
}
void MainWindow::saveBatchZerns(){
    QSettings set;
    QString path = set.value("lastPath",".").toString();
    QFile fn(path);

    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save Zernike values"), path + "//Zerns.csv",0,0,QFileDialog::DontConfirmOverwrite);


    if (fName.isEmpty())
        return;
    QFile f(fName);
    QTextStream out(&f);
    if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QStringList dirs = path.split("/");
        out << Qt::endl << Qt::endl;

        foreach(QVector<QString>   zerns, batchZerns){
            out << dirs[dirs.size()-1] + "/" + zerns[0];
            for (int i = 1; i < zerns.size(); ++i){

                out << "," << zerns[i];
            }
            out << Qt::endl;
        }
    }
}


void MainWindow::batchProcess(QStringList fileList){
    m_contourView->getPlot()->blockSignals(true);
    QSettings settings;
    bool shouldBeep = settings.value("RMSBeep>", true).toBool();
    this->setCursor(Qt::WaitCursor);
    batchIgramWizard::goPb->setEnabled(false);
    batchIgramWizard::addFiles->setEnabled(false);
    batchIgramWizard::skipFile->setEnabled(true);
    m_skipItem = false;
    QApplication::processEvents();
    QFileInfo info(m_igramsToProcess[0]);
    batchWiz->showPlots(batchIgramWizard::showProcessPlots->isChecked());
    QString lastPath = info.absolutePath();
    settings.setValue("lastPath",lastPath);
    int memThreshold = settings.value("lowMemoryThreshold", 300).toInt();
    int last = fileList.size()-1;

    int ndx = 0;
    int cnt = 0;
    int width, height;
    foreach(QString fn, fileList){

        QApplication::processEvents();
        batchWiz->select(ndx++);
        int mem = showmem("batch loop++++++++++++++++++++++++++++++");
        if (mem < memThreshold){
            QApplication::beep();
            int resp = QMessageBox::warning(0,"low on memory", "Do you want to continue?", QMessageBox::Yes|QMessageBox::No);
            if (resp == QMessageBox::No)
                break;
        }


        m_OutlineDoneInBatch = false;
        ui->SelectOutSideOutline->setChecked(true);
        m_igramArea->openImage(fn);
        if (batchIgramWizard::autoCb->isChecked() &&
                batchIgramWizard::autoOutlineCenter->isChecked() &&
                m_igramArea->m_center.m_radius == 0){
            m_igramArea->findCenterHole();
        }

        QApplication::processEvents();
        QObject().thread()->msleep(1000);
        ui->SelectOutSideOutline->setChecked(true);
        if (!batchIgramWizard::autoCb->isChecked()){
            while (m_inBatch && !m_OutlineDoneInBatch && !m_skipItem) {
                QApplication::processEvents();
            }
        }
        if (m_skipItem){
            cnt++;
            m_skipItem = false;
            continue;
        }
        if (!m_inBatch)
            break;

        m_igramArea->nextStep();
        QApplication::processEvents();
        QObject().thread()->msleep(1000);

        m_batchMakeSurfaceReady = false;
        if (!batchIgramWizard::autoCb->isChecked() && !m_skipItem){
            while (m_inBatch && !m_batchMakeSurfaceReady && !m_skipItem) {
                QApplication::processEvents();
            }
        }
        if (m_skipItem){
            m_skipItem = false;
            ++cnt;
            continue;
        }
        if (!m_inBatch)
            break;

        ui->tabWidget->setCurrentIndex(2);
        m_dftTools->wasPressed = true;
        m_dftArea->makeSurface();
        QApplication::processEvents();
        if (!m_dftArea->success){
            batchConnections(false);
            break;
        }




        qApp->processEvents();
        if (!m_inBatch)
            break;
        if (batchIgramWizard::filterCb->isChecked()){

        }
        wavefront *wf = m_surfaceManager->m_wavefronts.back();

        if (batchWiz->introPage->shouldFilterWavefront(wf->std)){

            m_surfaceManager->deleteCurrent();
            if (shouldBeep) {
                QApplication::beep();
            }
        }
        else{
            QPointF astig(wf->InputZerns[4], wf->InputZerns[5]);
            batchWiz->addAstig(wf->name, astig);
            batchWiz->addRms(wf->name, QPointF(ndx,wf->std));


            QObject().thread()->msleep(1000);

            if (batchIgramWizard::saveFile->isChecked()){
                QSettings settings;
                QString lastPath = settings.value("lastPath","").toString();
                QString fileName = m_surfaceManager->m_wavefronts.back()->name;
                QString file = lastPath + "/" + fileName + ".wft";
                m_surfaceManager->writeWavefront(file, m_surfaceManager->m_wavefronts.back(), false);
            }
            if (batchIgramWizard::makeReviewAvi->isChecked()){

                if (cnt == 0){
                    QSettings settings;
                    QString lastPath = settings.value("lastPath","").toString();
                    width = 800;
                    height = 600;
                    //vw = new VideoWriter;
                    //videoFileName = QFileDialog::getSaveFileName(0, "Save Review video as:", lastPath,"review.avi" );
                    //vw->open(videoFileName.toStdString().c_str(),-1,4,cv::Size(width,height),true);
                    // check if file exists and if yes: Is it really a file and no directory?
                    QString reviewPath = lastPath + "/review";
                    QDir dir(reviewPath);
                    if (!dir.exists())
                        dir.mkpath(reviewPath);
                    //::reviewFileName = videoFileName;
                }
                QImage img = QImage(width,height,QImage::Format_RGB32);
                QImage d3 = m_ogl->m_surface->render(1000,1000).scaled(width/2,height/2);
                QImage dft = m_dftArea->grab().toImage().scaled(width/2, height/2);
                QImage igram = m_igramArea->grab().toImage().scaled(width/2, height/2);
                QImage contour = m_contourView->getPlot()->grab().toImage().scaled(width/2, height/2);
                QPainter painter(&img);
                painter.drawImage(0,0,igram);
                painter.drawImage(width/2,0, dft);
                painter.drawImage(0,height/2, d3);
                painter.drawImage(width/2,height/2,contour);
                painter.setPen(Qt::yellow);
                painter.setBrush(Qt::yellow);
                QFileInfo info(fn);
                double cx = settings.value("lastOutsideCx",0).toDouble();
                double cy = settings.value("lastOutsideCy",0.).toDouble();
                wavefront *wf = m_surfaceManager->m_wavefronts[m_surfaceManager->m_currentNdx];
                QString txt = QString("%1 RMS: %2 outline center x,y: %3, %4").arg(
                                                info.baseName().toStdString().c_str()).arg(
                                                wf->std, 6, 'f', 3).arg(
                                                cx, 6, 'f', 1).arg(
                                                cy, 6, 'f', 1);
                painter.drawText(20,height/2 + 15, txt);
                cv::Mat frame = cv::Mat(img.height(), img.width(),CV_8UC4, img.bits(), img.bytesPerLine()).clone();
                cv::Mat resized;
                cv::resize(frame, resized, cv::Size(width,height));

                //cv::imwrite(file.toStdString().c_str(),resized);
            }
            if (batchIgramWizard::deletePreviousWave->isChecked()){
                QVector<QString> zerns;
                zerns << wf->name;
                foreach(double v , wf->InputZerns){
                    zerns << QString::number(v);
                }

                batchZerns << zerns;

                if (m_surfaceManager->m_wavefronts.size() > 2){
                    m_surfaceManager->m_currentNdx =0;
                    m_surfaceManager->deleteCurrent();
                }
            }
        }
        batchWiz->progressValue(0,last, cnt++);
        qApp->processEvents();

    }
    if (batchIgramWizard::deletePreviousWave->isChecked()){
        batchIgramWizard::saveZerns->setEnabled(true);
        batchIgramWizard::saveZerns->setStyleSheet("background-color: yellow");
        batchIgramWizard::saveZerns->setToolTipDuration(10000);
        QToolTip::showText( batchIgramWizard::saveZerns->mapToGlobal(QPoint(0,20)),batchIgramWizard::saveZerns->toolTip());

    }
    connect(batchWiz->introPage->astigPlot, SIGNAL(waveSeleted(QString)), m_surfaceManager, SLOT(wavefrontDClicked(QString)));
    connect(batchWiz->introPage->m_rmsPlot, SIGNAL(waveSeleted(QString)), m_surfaceManager, SLOT(wavefrontDClicked(QString)));
    progBar->reset();
    batchIgramWizard::goPb->setEnabled(true);
    batchIgramWizard::addFiles->setEnabled(true);
    batchIgramWizard::skipFile->setEnabled(false);

    this->setCursor(Qt::ArrowCursor);
    m_contourView->getPlot()->blockSignals(false);

}

void MainWindow::Batch_Process_Interferograms()
{
    batchWiz = new batchIgramWizard(m_igramsToProcess, this,Qt::Window);
    connect(batchWiz,SIGNAL(swapBathConnections(bool)),this, SLOT(batchConnections(bool)));
    batchConnections(true);
    //connect(batchIgramWizard::goPb, &QPushButton::pressed, this, &MainWindow::batchProcess);
    connect(batchWiz, SIGNAL(finished(int)), this, SLOT(batchFinished(int)));
    batchWiz->show();

}

void MainWindow::on_HelpOutlinePb_pressed()
{
    m_outlineHelp->show();
}

void MainWindow::on_actionVersion_History_triggered()
{
    QString link = qApp->applicationDirPath() + "/RevisionHistory.html";
    QDesktopServices::openUrl(QUrl::fromLocalFile(link));
}


#include "jitteroutlinedlg.h"
void MainWindow::on_actionIterate_outline_triggered()
{
    jitterOutlineDlg *dlg = jitterOutlineDlg::getInstance(this);
    connect(dlg,SIGNAL(finished(int)),this,SLOT(stopJitter()));
    dlg->show();
}
static bool stopJittering = false;
void MainWindow::stopJitter(){
    stopJittering = true;
}

void MainWindow::startJitter(){
    if (m_igramArea->m_outside.m_radius == 0){
        QMessageBox::warning(this, "Error", "You must first load an interferogram and outline the mirror. and press 'Done'");
        return;
    }
    jitterOutlineDlg *dlg = jitterOutlineDlg::getInstance(this);
    stopJittering = false;
    int start = dlg->getStart();
    int end = dlg->getEnd();
    int step = dlg->getStep();
    int x = 0;
    int y = 0;
    int rad = 0;

    m_igramArea->openImage(m_igramArea->m_filename);
    CircleOutline  saved = (m_igramArea->m_current_boundry == OutSideOutline) ? m_igramArea->m_outside : m_igramArea->m_center;
    dlg->getProgressBar()->setMinimum(start);
    dlg->getProgressBar()->setMaximum(end);
    for (int delta = start; delta <= end; delta += step){
        dlg->getProgressBar()->setValue(delta);
        switch (dlg->getType()){
        case 1:
            x = delta;
            break;
        case 2:
            y = delta;
            break;
        case 3:
            rad = delta;
            break;
        }

        if (stopJittering)
            break;
        m_igramArea->openImage(m_igramArea->m_filename);
        if (m_igramArea->m_current_boundry == OutSideOutline)
            m_igramArea->m_outside = saved;
        else
            m_igramArea->m_center = saved;

        m_igramArea->increase(rad);
        m_igramArea->shiftoutline(QPointF(x,y));
        qApp->processEvents();
        QObject().thread()->msleep(1000);


        m_igramArea->nextStep();

        m_surfaceManager->m_surface_finished = false;
        ui->tabWidget->setCurrentIndex(2);
        m_dftTools->wasPressed = true;
        m_dftArea->makeSurface();
        qApp->processEvents();
        wavefront *wf = m_surfaceManager->m_wavefronts[m_surfaceManager->m_currentNdx];
        wf->name = QString("x:_%1_Y:_%2_radius:_%3").arg(x).arg(y).arg(rad);
        dlg->status(wf->name);
        m_surfTools->nameChanged(m_surfaceManager->m_currentNdx, wf->name);
        qApp->processEvents();
        QObject().thread()->msleep(500);
    }
    dlg->getProgressBar()->reset();
    stopJittering = false;

    m_igramArea->openImage(m_igramArea->m_filename);
    if (m_igramArea->m_current_boundry == OutSideOutline)
        m_igramArea->m_outside = saved;
    else
        m_igramArea->m_center = saved;
    m_igramArea->nextStep();
    m_igramArea->openImage(m_igramArea->m_filename);
}

void MainWindow::on_actionLatest_Version_triggered()
{
    QString link = "https://github.com/githubdoe/DFTFringe/releases";
    QDesktopServices::openUrl(QUrl(link));
}

void MainWindow::on_actionShow_Statistics_of_Loaded_wavefronts_triggered()
{
     m_surfaceManager->saveAllWaveFrontStats();
}
void MainWindow::restoreContour(){
    m_contourView->setMinimumHeight(0);
    m_contourView->zoomed = false;
    review->s1->insertWidget(1,m_contourView);
}

void MainWindow::restoreOgl(){
    m_contourView->setMinimumHeight(0);
    review->s1->insertWidget(0,m_ogl);
    m_ogl->enableFullScreen();
}

void MainWindow::restoreProfile(){
    m_profilePlot->zoomed = false;
    review->s2->insertWidget(1,m_profilePlot);
}

void MainWindow::zoomProfile(bool flag){
    if (!flag){
        profileFv->close();
        return;
    }
    profileFv = new QWidget(0);
    profileFv->setAttribute( Qt::WA_DeleteOnClose );
    connect(profileFv,SIGNAL(destroyed(QObject*)),this, SLOT(restoreProfile()));
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(m_profilePlot);
    m_profilePlot->setMinimumHeight(300);
    profileFv->setLayout(l);
    profileFv->showMaximized();
}

void MainWindow::zoomContour(bool flag){
    if (!flag){
        contourFv->close();
        return;
    }
    contourFv = new QWidget(0);
    contourFv->setAttribute( Qt::WA_DeleteOnClose );
    connect(contourFv,SIGNAL(destroyed(QObject*)),this, SLOT(restoreContour()));
    QVBoxLayout *l = new QVBoxLayout();
    l->addWidget(m_contourView);
    m_contourView->setMinimumHeight(300);
    contourFv->setLayout(l);
    contourFv->showMaximized();
}

void MainWindow::zoomOgl()
{

    oglFv = new QWidget(0);
    oglFv->setAttribute( Qt::WA_DeleteOnClose );
    connect(oglFv,SIGNAL(destroyed(QObject*)),this, SLOT(restoreOgl()));
    QVBoxLayout *l = new QVBoxLayout();
    //m_ogl->setMinimumHeight(300);
    l->addWidget(m_ogl);
    oglFv->setLayout(l);
    oglFv->showMaximized();
}

void MainWindow::on_edgeZoomCb_clicked(bool checked)
{
    m_igramArea->setZoomMode( (checked) ? EDGEZOOM : NORMALZOOM);
}

void MainWindow::on_actionBath_Astig_Calculator_triggered()
{
    bathAstigDlg dlg;
    dlg.exec();
}
#include "zernikeeditdlg.h"
void MainWindow::on_actionEdit_Zernike_values_triggered()
{
    zernikeEditDlg *dlg = new zernikeEditDlg(m_surfaceManager, this);
    dlg->setWindowFlags(Qt::Tool);
    dlg->setAttribute(Qt::WA_DeleteOnClose);
    dlg->show();
    connect(dlg, SIGNAL(termCountChanged(int)), metrics, SLOT(resizeRows(int)));
}

void MainWindow::on_actionCamera_Calibration_triggered()
{
    if(m_cameraCalibWizard == nullptr){
        spdlog::get("logger")->trace("new cameraCalibWizard");
        m_cameraCalibWizard = new cameraCalibWizard;
        m_cameraCalibWizard->setAttribute( Qt::WA_DeleteOnClose, true );
        m_cameraCalibWizard->show();
    }
    else{
        // bring to front the already oppened window
        m_cameraCalibWizard->activateWindow();
        m_cameraCalibWizard->raise();
    }
}
#include "unwraperrorsview.h"
void MainWindow::on_actionShow_unwrap_errors_triggered()
{
    m_surfaceManager->showUnwrap();
}



void MainWindow::on_actionastig_Stats_triggered()
{
    if (m_astigStatsDlg)
        delete m_astigStatsDlg;
    m_astigStatsDlg = new astigStatsDlg(m_surfaceManager->m_wavefronts, this);
    m_astigStatsDlg->show();
}

void MainWindow::on_actionSave_Zernike_Values_in_CSV_triggered()
{
    QSettings set;
    QString path = set.value("lastPath",".").toString();
    QFile fn(path);

    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save Zernike values"), path + "//Zerns.csv",0,0,QFileDialog::DontConfirmOverwrite);


    if (fName.isEmpty())
        return;
    QFile f(fName);
    QTextStream out(&f);
    if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {

        for (int z = 0; z < Z_TERMS; ++z){
            out << "," <<zernsNames[z];
        }

        QStringList dirs = path.split("/");
        out << Qt::endl << Qt::endl;
        for (int i = 0; i < m_surfaceManager->m_wavefronts.size(); ++i){
            wavefront* wf = m_surfaceManager->m_wavefronts[i];
            QStringList paths = wf->name.split('/');
            if (paths.size() > 1)
                out << paths[paths.size()-2] + "/" + paths.last();
            else{
                out <<  dirs.back() + "/" + wf->name;
            }
            for (int z = 0; z < Z_TERMS; ++z){
                out << "," << wf->InputZerns[z];
            }
            out << Qt::endl;
        }
    }
}

void MainWindow::on_actionAverage_wave_front_files_triggered()
{
    QStringList files = SelectWaveFrontFiles();
    if (files.size() == 0)
        return;
    m_surfaceManager->averageWavefrontFiles( files);
}
#include <qwt_plot_curve.h>


void MainWindow::on_polygonRb_clicked(bool checked)
{
    m_igramArea->PolyAreaActive(checked);
    m_regionsEdit->show();
}

void MainWindow::on_autoOutline_clicked()
{
    m_igramArea->autoTraceOutline();
}

void MainWindow::on_autoChannel_clicked(bool checked)
{
    if (!checked)
        return;
    colorChannel::get_instance()->setAuto(checked);
    QSettings set;
    set.setValue("colorChannelUseAuto", checked);
    set.setValue("colorChannelUseRed", !checked);
    set.setValue("colorChannelUseGreen", !checked);
    set.setValue("colorChannelUseBlue", !checked);

}

void MainWindow::on_redChannel_clicked(bool checked)
{
    colorChannel::get_instance()->setRed(checked);
    QSettings set;
    set.setValue("colorChannelUseRed", checked);
    set.setValue("colorChannelUseGreen", !checked);
    set.setValue("colorChannelUseBlue", !checked);
    set.setValue("colorChannelUseAuto", !checked);
}

void MainWindow::on_greenChannel_clicked(bool checked)
{
    colorChannel::get_instance()->setGreen(checked);
    QSettings set;
    set.setValue("colorChannelUseGreen", checked);
    set.setValue("colorChannelUseBlue", !checked);
    set.setValue("colorChannelUseRed", !checked);
    set.setValue("colorChannelUseAuto", !checked);
}

void MainWindow::on_blueChannel_clicked(bool checked)
{
    colorChannel::get_instance()->setBlue(checked);
    QSettings set;
    set.setValue("colorChannelUseBlue", checked);
    set.setValue("colorChannelUseGreen", !checked);
    set.setValue("colorChannelUseRed", !checked);
    set.setValue("colorChannelUseAuto", !checked);
}

void MainWindow::on_showColorIgram_clicked(bool checked)
{
    colorChannel::get_instance()->showOriginalColorImage(checked);
    QSettings set;
    set.setValue("colorChannelShowColor", checked);
}

void MainWindow::on_useLastOutline_clicked()
{
    if (!m_igramArea->igramGray.isNull())
        m_igramArea->useLastOutline();
}

void MainWindow::on_actionCreate_Movie_of_wavefronts_triggered()
{
//    QStringList fileNames = SelectWaveFrontFiles();
//    this->setCursor(Qt::WaitCursor);
//    QProgressDialog pd("    Loading wavefronts in PRogress.", "Cancel", 0, 100);
//    pd.setRange(0, fileNames.size());
//    if (fileNames.length()> 0)
//        pd.show();
//    int cnt = 0;
//    QSettings set;
//    QString lastPath = set.value("lastPath",".").toString();
//    int memThreshold = set.value("lowMemoryThreshold",300).toInt();
//    QImage img = m_ogl->m_gl->grabFrameBuffer();

//    int width = img.width();
//    int height = img.height();

//    try {
//        QString fileName = QFileDialog::getSaveFileName(0, "Save AVI video as:", lastPath,"*.avi" );
//        if (fileName.length() > 0){
//            if (!(fileName.toUpper().endsWith(".AVI")))
//                fileName.append(".avi");
//            cv::VideoWriter video(fileName.toStdString().c_str(),-1,4,cv::Size(width,height),true);
//            if (!video.isOpened()){
//                QString msg = QString("could not open %1 %2x%3 for writing.").arg(fileName).arg(
//                                                width).arg(height);
//                QMessageBox::warning(0,"warning", msg);
//                return;
//            }
//            foreach (QString name, fileNames){
//                int mem = showmem("loading");
//                statusBar()->showMessage(QString("memory %1 MB").arg(mem));
//                if (mem< memThreshold + 50){
//                    while (m_surfaceManager->m_wavefronts.size() > 1){
//                        m_surfaceManager->deleteCurrent();
//                    }
//                }
//                QApplication::processEvents();

//                if (pd.wasCanceled())
//                    break;


//                pd.setLabelText(name);
//                QApplication::processEvents();
//                wavefront *wf = m_surfaceManager->readWaveFront(name);

//                m_surfaceManager->makeMask(wf);
//                m_surfaceManager->generateSurfacefromWavefront(wf);
//                m_surfaceManager->computeMetrics(wf);

//                pd.setValue(++cnt);

//                m_ogl->m_gl->setSurface(wf);
//                delete wf;
//                QApplication::processEvents();

//                QImage img = m_ogl->m_gl->grabFrameBuffer();
//                QPainter painter(&img);
//                painter.setPen(Qt::yellow);
//                painter.setBrush(Qt::yellow);
//                QFileInfo info(name);
//                painter.drawText(20,50, info.baseName());
//                cv::Mat frame = cv::Mat(img.height(), img.width(),CV_8UC4, img.bits(), img.bytesPerLine()).clone();
//                cv::Mat resized;
//                cv::resize(frame, resized, cv::Size(width,height));
//                video.write(resized);
//            }
//        }
//    }
//    catch(std::exception& e) {
//        qDebug() <<  "Exception writing video " << e.what();
//    }

//    this->setCursor(Qt::ArrowCursor);

}
arma::mat zapm(const arma::vec& rho, const arma::vec& theta,
               const double& eps, const int& maxorder=12) ;
#include "armadillo"
void dumpArma(arma::mat mm, QString title = "", QVector<QString> colHeading = QVector<QString>(0),
              QVector<QString> RowLable = QVector<QString>(0));
void MainWindow::on_actionDebugStuff_triggered()
{
    zernikeProcess *zp = zernikeProcess::get_Instance();
    wavefront *wf = m_surfaceManager->m_wavefronts[m_surfaceManager->m_currentNdx];


    // compute annular defocus from circular value
    double obs = 0.5;
    double e = sqrt(1 + obs * obs);

    double a4 = 1./ (1 - obs * obs) * (+ wf->InputZerns[3] - (std::sqrt(3)*obs*obs * wf->InputZerns[0] ));
    qDebug() << "defocus circular" << wf->InputZerns[3] << "annular" << a4;
    std::vector<double> rho = {1., 0., 1.};
    std::vector<double> theta;

    theta.push_back(M_PI);
    theta.push_back(0.);
    theta.push_back(0);

    arma::rowvec r(rho);
    arma::rowvec t(theta);

   arma::Mat<double> rhoTheta = arma::join_cols(r,t);
    int max =4;

    qDebug() << "e" << e;
   arma::mat zerns = zapm(r.as_col(), t.as_col(), obs, max);

   dumpArma(zerns, "Y = 0 obs = .5  ", {"   X   ","piston","xTilt","yTilt", "defocus", "xastig", "yastig"},{"x = -1", "x = 0","x = 1"});
   qDebug() << "dump done";


  std::vector<double> zs =  zp->ZernFitWavefront(*wf);
   for (int i = 0; i < 10; ++i){
       qDebug() << "z " << i << zs[i] << wf->InputZerns[i];

   }
   return;

}


#include "outlinestatsdlg.h"
void MainWindow::on_actionShow_outline_statistics_triggered()
{
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();

    QFileDialog dialog(this);
    dialog.setDirectory(lastPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("oln (*.oln)"));

    if (dialog.exec()) {
        QStringList fileNames = dialog.selectedFiles();
        outlineStatsDlg outDlg(fileNames, this);
        outDlg.exec();
        QFileInfo info(fileNames[0]);
        lastPath = info.absolutePath();
        settings.setValue("lastPath",lastPath);
    }
}

void MainWindow::on_actionPlay_batch_results_review_movie_triggered()
{
    QDesktopServices::openUrl(QUrl(batchIgramWizard::reviewFileName));
}

void MainWindow::on_useExistingAsGuide_clicked(bool checked)
{
    QSettings set;
    set.setValue("useGuideOutline", checked);
}

void MainWindow::on_autoTraceCB_clicked(bool checked)
{
    QSettings set;
    set.setValue("autoOutline", checked);
}

void MainWindow::on_scanMargin_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("outlineScanRange", arg1);
}

void MainWindow::on_autoOutlineHelp_clicked()
{
    QString link = qApp->applicationDirPath() + "/res/Help/outlineing.html";
    QDesktopServices::openUrl(QUrl(link));
}
#include "transformwavefrontdlg.h"
void MainWindow::on_actionwave_front_transforms_triggered()
{
   m_surfaceManager->transform();

}

void MainWindow::on_actiontilt_versus_astig_analysis_triggered()
{

    m_surfaceManager->tiltAnalysis();
}

void MainWindow::on_actionSave_curent_profile_triggered()
{
    if (m_profilePlot->m_wf == 0)
        return;
    QSettings settings;
    QString lastPath = settings.value("projectPath",".").toString();
    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save Profile"), lastPath + "//profile.txt");
    if (fName.isEmpty())
        return;
    QFile f(fName);
    QTextStream out(&f);
    if (f.open(QIODevice::WriteOnly )){
            QPolygonF points = m_profilePlot->createProfile(m_profilePlot->m_showNm * m_profilePlot->m_showSurface,m_profilePlot->m_wf);
            foreach (QPointF pt, points) {
                out << pt.x() << " " << pt.y() << Qt::endl;
            }
    }
}

void MainWindow::on_actionProcess_PSI_interferograms_triggered()
{
   m_dftArea->doPSIstep1();

}

#include "zernikesmoothingdlg.h"
void MainWindow::on_actionSmooth_current_wave_front_triggered()
{

    SurfaceManager &sm = *SurfaceManager::get_instance();
    if (sm.m_wavefronts.size() == 0){
        QMessageBox::warning(this, "No Wavefronts", "You must first load a wave front");
        return;
    }
    ZernikeSmoothingDlg *dlg = new ZernikeSmoothingDlg(*sm.m_wavefronts[sm.m_currentNdx]);
    dlg->resize(1000,1000);
    dlg->show();
    return;
}

void MainWindow::on_useAnnulust_clicked()
{
    m_igramArea->useAnnulusforCenterOutine();
}

