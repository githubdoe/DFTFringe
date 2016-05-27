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
#include <opencv/cv.h>
#include "simulationsview.h"
#include "outlinehelpdocwidget.h"


using namespace QtConcurrent;
vector<wavefront*> g_wavefronts;
int g_currentsurface = 0;
QScrollArea *gscrollArea;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),m_showChannels(false), m_showIntensity(false),m_inBatch(false),m_OutlineDoneInBatch(false),
    m_batchMakeSurfaceReady(false)
{
    ui->setupUi(this);
    const QString toolButtonStyle("QToolButton {"
                                "border-style: outset;"
                                "border-width: 3px;"
                                "border-radius:7px;"
                                "border-color: darkgray;"
                                "font: bold 12px;"
                                "min-width: 10em;"
                                "padding: 6px;}"
                            "QToolButton:hover {background-color: lightblue;"
                                    " }");
    QWidget *rw = ui->toolBar->widgetForAction(ui->actionRead_WaveFront);
    //igramArea = new IgramArea(ui->tabWidget->widget(0));
    rw->setStyleSheet(toolButtonStyle);
    rw = ui->toolBar->widgetForAction(ui->actionSubtract_wave_front);
    rw->setStyleSheet(toolButtonStyle);


    ui->toolBar->setStyleSheet("QToolBar { spacing: 10px;}");
              ui->toolBar->setStyleSheet("QToolButton {border-style: outset;border-width: 3px;"
                                         "border-color: darkgray;border-radius:10px;}"
                                         "QToolButton:hover {background-color: lightblue;}");
    ui->SelectOutSideOutline->setChecked(true);
    setCentralWidget(ui->tabWidget);
    m_loaderThread = new QThread();
    m_waveFrontLoader = new waveFrontLoader();
    m_waveFrontLoader->moveToThread(m_loaderThread);
    m_colorChannels = new ColorChannelDisplay();
    connect(this, SIGNAL(load(QStringList,SurfaceManager*)), m_waveFrontLoader, SLOT(loadx(QStringList,SurfaceManager*)));
    m_intensityPlot = new igramIntensity(0);


    m_loaderThread->start();
    ui->tabWidget->removeTab(0);
    ui->tabWidget->removeTab(0);

    // setup igram window
    scrollArea = new QScrollArea;
    gscrollArea = scrollArea;
    m_igramArea = new IgramArea(scrollArea, this);
    connect(m_igramArea, SIGNAL(statusBarUpdate(QString)), statusBar(), SLOT(showMessage(QString)));
    connect(m_igramArea, SIGNAL(upateColorChannels(QImage)), this, SLOT(updateChannels(QImage)));
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
    m_dftArea = new DFTArea(scrollAreaDft, m_igramArea, m_dftTools, m_vortexDebugTool);
    scrollAreaDft->setWidget(m_dftArea);
    scrollAreaDft->resize(800,800);
    ui->tabWidget->addTab(scrollAreaDft, "Analyze");

    m_contourTools = new ContourTools(this);
    m_outlineHelp = new outlineHelpDocWidget(this);
    m_surfTools = surfaceAnalysisTools::get_Instance(this);

    //DocWindows
    createDockWindows();

    userMapDlg = new userColorMapDlg();

    m_contourView = new contourView(this, m_contourTools);
    connect(m_contourView, SIGNAL(zoomMe(bool)),this, SLOT(zoomContour(bool)));
    m_ogl = new OGLView(0, m_contourTools, m_surfTools);
    connect(m_ogl,SIGNAL(zoomMe(bool)), this, SLOT(zoomOgl(bool)));

    connect(userMapDlg, SIGNAL(colorMapChanged(int)), m_contourView->getPlot(), SLOT(ContourMapColorChanged(int)));
    connect(userMapDlg, SIGNAL(colorMapChanged(int)),m_ogl->m_gl, SLOT(colorMapChanged(int)));
    review = new reviewWindow(this);
    review->s1->addWidget(m_ogl);

    m_profilePlot =  new ProfilePlot(review->s2,m_contourTools);
    m_mirrorDlg = mirrorDlg::get_Instance();
    review->s2->addWidget(m_profilePlot);
    review->s1->addWidget(m_contourView);
    //Surface Manager
    m_surfaceManager = new SurfaceManager(this,m_surfTools, m_profilePlot, m_contourView->getPlot(),
                                          m_ogl->m_gl, metrics);
    connect(m_surfaceManager, SIGNAL(load(QStringList,SurfaceManager*)), m_waveFrontLoader, SLOT(loadx(QStringList,SurfaceManager*)));
    connect(m_surfaceManager, SIGNAL(showMessage(QString)), this, SLOT(showMessage(QString)));
    connect(m_contourView, SIGNAL(showAllContours()), m_surfaceManager, SLOT(showAllContours()));
    connect(m_dftArea, SIGNAL(newWavefront(cv::Mat,CircleOutline,CircleOutline,QString)),
            m_surfaceManager, SLOT(createSurfaceFromPhaseMap(cv::Mat,CircleOutline,CircleOutline,QString)));
    connect(m_surfaceManager, SIGNAL(diameterChanged(double)),this,SLOT(diameterChanged(double)));
    connect(m_surfaceManager, SIGNAL(showTab(int)), ui->tabWidget, SLOT(setCurrentIndex(int)));
    connect(m_ogl, SIGNAL(showAll3d(GLWidget *)), m_surfaceManager, SLOT(showAll3D(GLWidget *)));

    ui->tabWidget->addTab(review, "Results");

    ui->tabWidget->addTab(SimulationsView::getInstance(ui->tabWidget), "Star Test, PSF, MTF");
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
    connect(m_igramArea, SIGNAL(upateColorChannels(QImage)), m_dftArea, SLOT(newIgram(QImage)));
    enableShiftButtons(true);


    connect(m_dftTools,SIGNAL(doDFT()),m_dftArea,SLOT(doDFT()));
    settingsDlg = Settings2::getInstance();
    connect(settingsDlg->m_igram,SIGNAL(igramLinesChanged(int,int,QColor,QColor,double,int)),
            m_igramArea,SLOT(igramOutlineParmsChanged(int, int, QColor, QColor, double, int)));

    QSettings settings;
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

    zernEnables = std::vector<bool>(Z_TERMS, true);

    //disable first 8 enables except for astig
    // enable first 8 nulls except for astig
    for (int i = 0; i < 8; ++ i)
    {
        if (i == 4 || i == 5)
            continue;
        zernEnables[i] = false;
    }
    zernEnableUpdateTime = QDateTime::currentDateTime().toTime_t();

    connect(m_surfaceManager, SIGNAL(rocChanged(double)),this, SLOT(rocChanged(double)));
    connect(m_mirrorDlg, SIGNAL(newPath(QString)),this, SLOT(newMirrorDlgPath(QString)));
    progBar = new QProgressBar(this);
    ui->statusBar->addPermanentWidget(progBar);


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
        SimulationsView *sv = SimulationsView::getInstance(0);
        if (sv->needs_drawing){

            sv->on_MakePB_clicked();
            QApplication::restoreOverrideCursor();

        }

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
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QFileDialog dialog(this, tr("Open File"),lastPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

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
    zernTablemodel = metrics->tableModel;
    addDockWidget(Qt::LeftDockWidgetArea, m_outlineHelp);
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
    m_outlineHelp->hide();
    metrics->hide();
    m_vortexDebugTool->hide();
}


void MainWindow::updateMetrics(wavefront& wf){
    metrics->setName(wf.name);
    metrics->mDiam->setText(QString().sprintf("%6.3lf",wf.diameter));
    metrics->mROC->setText(QString().sprintf("%6.3lf",wf.roc));
    const double  e = 2.7182818285;
    metrics->mRMS->setText(QString().sprintf("<b><FONT FONT SIZE = 12>%6.3lf</b>",wf.std));
    double st =(2. *M_PI * wf.std);
    st *= st;
    double Strehl = pow(e, -st);
    metrics->mStrehl->setText(QString().sprintf("<b><FONT FONT SIZE = 12>%6.3lf</b>",Strehl));

    double roc = m_mirrorDlg->roc;
    double diam = wf.diameter;
    double r3 = roc * roc * roc;
    double d4 = diam * diam * diam * diam;
    double z8 = zernTablemodel->values[8];
    if (m_mirrorDlg->doNull)
        z8 = z8 - m_mirrorDlg->cc * m_mirrorDlg->z8;

    double z1 = z8 * 384. * r3 * m_mirrorDlg->lambda * 1.E-6/(d4);
    BestSC = m_mirrorDlg->cc + z1;
    metrics->setWavePerFringe(m_mirrorDlg->fringeSpacing, m_mirrorDlg->lambda);
    metrics->mCC->setText(QString().sprintf("<FONT FONT SIZE = 7>%6.3lf",BestSC));
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

void MainWindow::selectDftTab(){
    ui->tabWidget->setCurrentIndex(1);
}

void MainWindow::updateChannels(QImage img){

    m_colorChannels->setImage(img);
    if (m_showChannels)
        m_colorChannels->show();
    m_intensityPlot->setIgram(img);
    if (m_showIntensity)
        m_intensityPlot->show();
}

void MainWindow::on_actionRead_WaveFront_triggered()
{
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();

    QFileDialog dialog(this);
    dialog.setDirectory(lastPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setNameFilter(tr("wft (*.wft)"));
    QStringList fileNames;

    if (dialog.exec()) {
        this->setCursor(Qt::WaitCursor);

        QApplication::processEvents();
        fileNames = dialog.selectedFiles();
        QFileInfo info(fileNames[0]);
        lastPath = info.absolutePath();
        QSettings settings;
        settings.setValue("lastPath",lastPath);
        emit load(fileNames, m_surfaceManager);
        this->setCursor(Qt::ArrowCursor);
        ui->tabWidget->setCurrentIndex(2);
    }
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
void MainWindow::showMessage(QString msg){

    int ok = QMessageBox(QMessageBox::Information,msg, "",QMessageBox::Yes|QMessageBox::No).exec();
    m_surfaceManager->messageResult = ok;
    m_surfaceManager->pauseCond.wakeAll();


}
void MainWindow::diameterChanged(double v){
    m_mirrorDlg->on_diameter_Changed(v);
}
void MainWindow::rocChanged(double v){
    m_mirrorDlg->on_roc_Changed(v);
}

void MainWindow::on_actionLighting_properties_triggered()
{
    m_ogl->m_gl->openLightingDlg();
}

void MainWindow::on_SelectOutSideOutline_clicked(bool checked)
{
    m_igramArea->SideOutLineActive( checked);
}

void MainWindow::on_SelectObsOutline_clicked(bool checked)
{
        m_igramArea->SideOutLineActive( !checked);
}

void MainWindow::on_pushButton_clicked()
{
    if (!m_inBatch)
        m_igramArea->nextStep();
    else {
        m_OutlineDoneInBatch = true;
    }
}

void MainWindow::on_showChannels_clicked(bool checked)
{
    m_showChannels = checked;
    if (checked)
        m_colorChannels->show();
    else
        m_colorChannels->close();
}

void MainWindow::on_showIntensity_clicked(bool checked)
{
    m_showIntensity = checked;
    if (checked)
        m_intensityPlot->show();
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

    int wx = dlg.size;
    int wy = wx;
    double rad = (double)(wx-1)/2.;
    double xcen = rad,ycen = rad;
    int border = 5;
    rad -= border;


    cv::Mat result = makeSurfaceFromZerns(border);

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
    QPoint wr = m_ogl->m_gl->mapTo(this, m_ogl->pos());

    QImage gl = m_ogl->m_gl->grabFrameBuffer();

    QPainter painter( &image );

    this->render( &painter);
    painter.drawImage(wr, gl);
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
    QMessageBox::information(this, "Help", "Sorry no help other than videos yet.");
}

void MainWindow::on_actionAbout_triggered()
{

    QMessageBox::information(this, "About", QString().sprintf("<html><body><h1>DFTFringe version %s</h1>",APP_VERSION)+
                             "<p>This program was compiled using:<ul><li>"
                             "Qt version 5.3 </li>"
                             "<li> Compiled with mingw482_32</li>"
                             "<li> OpenCV 2.4.10</li>"
                             "<li> QWTPlot</li></ul></p>"
                             "<h3>Credits</h3>"
                             "<ul><li>Mike Peck for<ul><li>Researching and explaining FFT algorithm.</li>"
                             "<li>Researching and explaining Unwrap algorithms.</li></li>"
                             "<li>Steve Koehler for much help with Vortex implementation.</li>"
                             "<li>Dave Rowe for starting the Interferometry group and publishig the initial program.</li>"
                             "<li>Kieran Larken for deriving and publishing the Vortex Transform.</li>"
                             "</ul>"
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
    if (flag){
        m_inBatch = true;
        disconnect(m_dftTools, SIGNAL(makeSurface()), m_dftArea, SIGNAL(makeSurface()));
        connect(m_dftTools, SIGNAL(makeSurface()), this, SLOT(batchMakeSurfaceReady()));
    }
    else {
        m_inBatch = false;
        disconnect(m_dftTools, SIGNAL(makeSurface()), this, SLOT(batchMakeSurfaceReady()));
        connect(m_dftTools, SIGNAL(makeSurface()), m_dftArea, SLOT(makeSurface()));
    }
}

void MainWindow::batchProcess(QStringList fileList){

        this->setCursor(Qt::WaitCursor);
        batchIgramWizard::goPb->setEnabled(false);
        QApplication::processEvents();
        QFileInfo info(m_igramsToProcess[0]);

        QString lastPath = info.absolutePath();
        QSettings settings;
        settings.setValue("lastPath",lastPath);
        foreach(QString fn, fileList){
            m_OutlineDoneInBatch = false;
            m_igramArea->openImage(fn);
            if (batchIgramWizard::manualRb->isChecked()){
                while (m_inBatch && !m_OutlineDoneInBatch) {
                    QApplication::processEvents();
                }
            }
            if (!m_inBatch)
                break;
            m_igramArea->nextStep();
            m_batchMakeSurfaceReady = false;
            if (batchIgramWizard::manualRb->isChecked()){
                while (m_inBatch && !m_batchMakeSurfaceReady) {
                    QApplication::processEvents();
                }
            }
            if (!m_inBatch)
                break;
            m_surfaceManager->m_surface_finished = false;
            ui->tabWidget->setCurrentIndex(2);
            m_dftArea->makeSurface();
            while(m_inBatch && !m_surfaceManager->m_surface_finished){qApp->processEvents();}
            if (!m_inBatch)
                break;
            Sleep(1000);


        }
        batchIgramWizard::goPb->setEnabled(true);
        batchWiz->close();
        delete batchWiz;

        this->setCursor(Qt::ArrowCursor);


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
    CircleOutline  saved = m_igramArea->m_outside;
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
        m_igramArea->m_outside = saved;
        m_igramArea->increase(rad);
        m_igramArea->shiftoutline(QPointF(x,y));
        qApp->processEvents();
        Sleep(1000);

        m_igramArea->nextStep();

        m_surfaceManager->m_surface_finished = false;
        ui->tabWidget->setCurrentIndex(2);
        m_dftArea->makeSurface();
        while(m_inBatch && !m_surfaceManager->m_surface_finished){qApp->processEvents();}
        wavefront *wf = m_surfaceManager->m_wavefronts[m_surfaceManager->m_currentNdx];
        wf->name = QString().sprintf("x:_%d_Y:_%d_radius:_%d",x,y,rad);
        dlg->status(wf->name);
        m_surfTools->nameChanged(m_surfaceManager->m_currentNdx, wf->name);
        qApp->processEvents();
        Sleep(500);
    }
    dlg->getProgressBar()->reset();
    stopJittering = false;

    m_igramArea->openImage(m_igramArea->m_filename);
    m_igramArea->m_outside = saved;
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
    //m_ogl->setMinimumHeight(50);
    m_ogl->zoomed = false;
    review->s1->insertWidget(0,m_ogl);
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

void MainWindow::zoomOgl(bool flag)
{
    if (!flag){
        oglFv->close();
        return;
    }
    oglFv = new QWidget(0);
    oglFv->setAttribute( Qt::WA_DeleteOnClose );
    connect(oglFv,SIGNAL(destroyed(QObject*)),this, SLOT(restoreOgl()));
    QVBoxLayout *l = new QVBoxLayout();
    //m_ogl->setMinimumHeight(300);
    l->addWidget(m_ogl);
    oglFv->setLayout(l);
    oglFv->showMaximized();


}
