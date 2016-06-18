/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtCore>
#include "IgramArea.h"
#include "dfttools.h"
#include "dftarea.h"
#include "profileplot.h"
#include "wavefront.h"
#include "contourplot.h"
#include "contourtools.h"
#include "glwidget.h"
#include "oglcontrols.h"
#include "surfaceanalysistools.h"
#include "surfacemanager.h"
#include "zernikedlg.h"
#include "mirrordlg.h"
#include "metricsdisplay.h"
#include "reviewwindow.h"
#include "settings2.h"
#include "wavefrontloader.h"
#include "vortextools.h"
#include "colorchanneldisplay.h"
#include "igramintensity.h"
#include "vortexdebug.h"
#include "oglview.h"
#include "contourview.h"
#include "batchigramwizard.h"
#include "outlinehelpdocwidget.h"
#include "foucaultview.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *event);
    void updateMetrics(wavefront& wf);
    int addTab(QWidget* w, const QString &name);
    mirrorDlg *m_mirrorDlg;
    QProgressBar *progBar;
    ProfilePlot *m_profilePlot;
    uint zernEnableUpdateTime;
    ZernTableModel *zernTablemodel;
    metricsDisplay *metrics;
    reviewWindow *review;
    Settings2 *settingsDlg;
    wavefront *getCurrentWavefront();
public slots:
    void enableShiftButtons(bool enable);
    void showMessage(QString);
    void diameterChanged(double);
    void rocChanged(double);
    void batchProcess(QStringList fileList);
    void batchMakeSurfaceReady();
    void batchConnections(bool flag);
    void batchFinished(int);
    void startJitter();
    void stopJitter();
    void restoreOgl();
    void restoreContour();
    void zoomContour(bool flag);
    void zoomOgl(bool flag);
signals:
    void load(QStringList, SurfaceManager *);
    void messageResult(int);
    void gammaChanged(bool, double);
private slots:
    void updateChannels(QImage);
    void openRecentFile();
    void on_actionLoad_Interferogram_triggered();
    void on_pushButton_5_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_7_clicked();
    void on_actionLoad_outline_triggered();
    void on_ShiftUp_clicked();
    void on_shiftLeft_clicked();
    void on_shiftDown_clicked();
    void on_shiftRight_clicked();
    void selectDftTab();
    void newMirrorDlgPath(QString path);
    void on_actionRead_WaveFront_triggered();

    void on_actionPreferences_triggered();
    void mainTabChanged(int);
    void on_actionMirror_triggered();

    void on_saveOutline_clicked();

    //void on_actionSimulated_interferogram_triggered();

    void on_actionPrevious_Wave_front_triggered();

    void on_actionNext_Wave_Front_triggered();

    void on_actionDelete_wave_front_triggered();

    void on_actionWrite_WaveFront_triggered();

    void on_actionSave_Wavefront_triggered();

    void on_actionLighting_properties_triggered();

    void on_SelectOutSideOutline_clicked(bool checked);

    void on_SelectObsOutline_clicked(bool checked);

    void on_pushButton_clicked();

    void on_showChannels_clicked(bool checked);

    void on_showIntensity_clicked(bool checked);

    void on_actionWavefront_triggered();

    void on_actionIgram_triggered();

    void on_actionSave_interferogram_triggered();

    void on_actionSave_screen_triggered();

    void on_actionWave_Front_Inspector_triggered();

    void on_actionSave_outline_triggered();

    void on_actionUser_Color_Map_triggered();

    void on_actionSave_nulled_smoothed_wavefront_triggered();

    void on_minus_clicked();

    void on_pluss_clicked();

    void on_actionTest_Stand_Astig_Removal_triggered();


    void on_reverseGammaCB_clicked(bool checked);

    void on_actionSubtract_wave_front_triggered();

    void on_actionSave_PDF_report_triggered();

    void on_actionHelp_triggered();

    void on_actionAbout_triggered();

    void on_actionVideos_triggered();

    void on_undo_pressed();

    void on_Redo_pressed();

    void on_pushButton_clicked(bool checked);

    void on_checkBox_clicked(bool checked);

    void on_actionError_Margins_triggered();

    void on_HelpOutlinePb_pressed();

    void on_actionVersion_History_triggered();


    void on_actionIterate_outline_triggered();

    void on_actionLatest_Version_triggered();

    void on_actionShow_Statistics_of_Loaded_wavefronts_triggered();

    void on_edgeZoomCb_clicked(bool checked);

    void on_actionBath_Astig_Calculator_triggered();

    void on_actionEdit_Zernike_values_triggered();


private:

    Ui::MainWindow *ui;

    bool loadFile(const QString &fileName);
    void selectTool(const QString & tabName);
    void setCurrentFile(const QString &fileName);
    void updateRecentFileActions();
    QString strippedName(const QString &fullFileName);
    QMenu* m_view_menu;
    void createActions();
    void createDockWindows();
public:
    IgramArea *m_igramArea;
private:
    DFTArea *m_dftArea;

    contourView *m_contourView;
    DFTTools *m_dftTools;
    VortexTools *m_vortexTools;
    ContourTools *m_contourTools;
    OGLControls *m_oglTools;
    surfaceAnalysisTools *m_surfTools;
    outlineHelpDocWidget *m_outlineHelp;
    SurfaceManager *m_surfaceManager;
    QScrollArea *scrollArea;
    QScrollArea *scrollAreaDft;
    QDockWidget * m_metrics;
    OGLView *m_ogl;
    double scaleFactor;
    QString curFile;
    userColorMapDlg *userMapDlg;
    QTextEdit *textEdit;
    QMenu *fileMenu;
    QMenu *recentFilesMenu;
    QMenu *helpMenu;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *separatorAct;
    bool m_showChannels;
    bool m_showIntensity;
    bool m_inBatch;
    bool m_OutlineDoneInBatch;
    bool m_batchMakeSurfaceReady;

    enum { MaxRecentFiles = 5 };
    QAction *recentFileActs[MaxRecentFiles];

    waveFrontLoader * m_waveFrontLoader;
    QThread * m_loaderThread;
    ColorChannelDisplay *m_colorChannels;
    igramIntensity *m_intensityPlot;
    vortexDebug    *m_vortexDebugTool;
    batchIgramWizard *batchWiz;
    QStringList m_igramsToProcess;
    QWidget *oglFv;
    QWidget *contourFv;
    void Batch_Process_Interferograms();
};

#endif // MAINWINDOW_H
