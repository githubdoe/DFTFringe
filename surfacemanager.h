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
#ifndef SURFACEMANAGER_H
#define SURFACEMANAGER_H
#include <QTextEdit>
#include <QWidget>
#include <qobject.h>
#include "wavefront.h"
#include "contourplot.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "dftarea.h"
#include "IgramArea.h"
#include "profileplot.h"
#include "surfaceanalysistools.h"
#include <QObject>
#include "zernikedlg.h"
#include "mirrordlg.h"
#include "metricsdisplay.h"
#include <QTimer>
#include <QProgressDialog>
#include "wftstats.h"
#include "Circleoutline.h"
#include "simulationsview.h"
#include "contourview.h"
#include "standastigwizard.h"
#include <QPointF>
#include "surfacegraph.h"
enum configRESPONSE { YES, NO, ASK};
enum AutoInvertMode {invNOTSET, invMANUAL, invCONIC, invINSIDE, invOUTSIDE};
struct textres {
    QTextEdit *Edit;
    QList<QString> res;
};
class SurfaceManager : public QObject
{
    Q_OBJECT
public:

    static SurfaceManager *get_instance(QObject *parent = 0, surfaceAnalysisTools *tools = 0,
                                        ProfilePlot *profilePlot = 0, contourView *contourPlot = 0,
                                        SurfaceGraph *glPlot = 0, metricsDisplay *mets = 0);
    SurfaceManager(const SurfaceManager&) = delete;
    SurfaceManager& operator=(const SurfaceManager&) = delete;
    ~SurfaceManager();

    bool loadWavefront(const QString &fileName);
    void sendSurface(wavefront* wf);
    void computeMetrics(wavefront *wf);
    void makeMask(int waveNdx, bool useCenterCircle = true);
    void previous();
    void next();
    void deleteCurrent();
    void processSmoothing();
    void saveAllWaveFrontStats();
    void SaveWavefronts(bool saveNulled);
    void writeWavefront(const QString &fname, wavefront *wf, bool saveNulled);
    void useDemoWaveFront();
    void showUnwrap();
    void initWaveFrontLoad();
    void averageWavefrontFiles(const QStringList &files);
    void downSizeWf(wavefront *wf);
    void process(int wavefront_index, SurfaceManager *sm);
    wavefront *readWaveFront(const QString &fileName);
    inline wavefront *getCurrent(){
        if (m_wavefronts.size() == 0)
            return 0;
        return m_wavefronts[m_currentNdx];
    }
    cv::Mat computeWaveFrontFromZernikes(int wx,int wy, std::vector<double> &zerns, QVector<int> zernsToUse);
    void report();
    void computeTestStandAstig();
    QVector<wavefront*> m_wavefronts;
    surfaceAnalysisTools *m_surfaceTools;
    ProfilePlot *m_profilePlot;
    contourView* m_contourView;
    SimulationsView *m_simView;
    SurfaceGraph *m_SurfaceGraph;
    QImage m_allContours;
    metricsDisplay *m_metrics;
    int m_gbValue;
    bool m_GB_enabled;
    int m_currentNdx;
    wavefront* m_wf;
    wavefront* m_standAvg;
    wavefront* m_standRemoved;
    int insideOffset;
    int outsideOffset;
    int messageResult;
    void inspectWavefront();

    cv::Mat_<double> subtractPlane(cv::Mat_<double> phase, cv::Mat_<bool> mask);

    void average(QList<wavefront *> wfList);
    void subtractWavefronts();

    AutoInvertMode m_inverseMode;
    bool m_ignoreInverse;
    bool m_surface_finished;
    configRESPONSE diamResp;
    configRESPONSE rocResp;
    configRESPONSE lambdResp;
    int okToContinue;
    bool okToUpdateSurfacesOnGenerateComplete;
    void makeMask(wavefront* wf, bool useInsideCircle = true);
    void generateSurfacefromWavefront(int ndx);
    void generateSurfacefromWavefront(wavefront *wf);
    void transform();
    void subtract(wavefront *wf1, wavefront *wf2, bool use_null = true);
private:
    QProgressDialog *pd;
    QTimer *m_waveFrontTimer;
    QTimer *m_toolsEnableTimer;
    QPointer<standAstigWizard> m_standAstigWizard;
    int workToDo;
    int workProgress;

    wftStats *m_wftStats;

    explicit SurfaceManager(QObject *parent=0, surfaceAnalysisTools *tools = 0, ProfilePlot *profilePlot =0,
                   contourView *contourView = 0, SurfaceGraph *glPlot = 0, metricsDisplay *mets = 0);
    textres Phase2(QList<rotationDef *> list, QList<wavefront *> inputs, int avgNdx, int Width, QPrinter &printer);
    void changeWavelength(wavefront *wf, double wavelength);
    void flipHorizontal(wavefront *wf);
    void flipVertical(wavefront *wf);

signals:
    void currentNdxChanged(int);
    void waveFrontClicked(int);
    void deleteWavefront(int);
    void rotateTheseSig(int, QList<int>);
    void progress(int);
    void diameterChanged(double);
    void rocChanged(double);
    void nameChanged(const QString &, const QString &);
    void showTab(int);
    void enableControls(bool);
private slots:
    void waveFrontClickedSlot(int ndx);
    void centerMaskValue(int val);
    void outsideMaskValue(int val);
    void surfaceSmoothGBEnabled(bool b);
    void surfaceSmoothGBValue(double value);
    void computeZerns();
    void surfaceGenFinished();
    void deleteWaveFronts(QList<int> list);
    void averageWavefrontIndices(QList<int> list);
    void transfrom(const QList<int> &list);
    void filter();
    void enableTools();
    void averageComplete(wavefront *wft);
    void outputLambdaChanged(double val);
    void resize(wavefront * wf, int size);
    void resizeW(int size);
    void changeWavelength(double wavelength);
    void flipHorizontal();
    void flipVertical();
    void defocusSetup();

public slots:
    void rotateThese(double angle, QList<int> list);
    void createSurfaceFromPhaseMap(cv::Mat phase, CircleOutline outside,
                                   CircleOutline center, const QString &name,
                                   QVector<std::vector<cv::Point> > polyArea= QVector<std::vector<cv::Point> >());
    void invert(QList<int> list);
    void wftNameChanged(int, const QString&);
    void showAllContours();
    void computeStandAstig(define_input *wizPage, QList<rotationDef *>);
    void ObstructionChanged();

    void loadComplete();
    void memoryLow();
    void defocusChanged();
    void tiltAnalysis();
    void saveAllContours();
    void backGroundUpdate();
    void wavefrontDClicked(const QString & name);
};



#endif // SURFACEMANAGER_H
