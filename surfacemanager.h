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
#include "igramarea.h"
#include "profileplot.h"
#include "surfaceanalysistools.h"
#include <QObject>
#include "zernikedlg.h"
#include "mirrordlg.h"
#include "metricsdisplay.h"
#include <QTimer>
#include <QProgressDialog>
#include "wftstats.h"
#include "circleoutline.h"
#include "simulationsview.h"
#include "contourview.h"
#include "standastigwizard.h"
#include <QPointF>
#include "surfacegraph.h"
enum configRESPONSE { YES, NO, ASK};
struct textres {
    QTextEdit *Edit;
    QList<QString> res;
};
class SurfaceManager : public QObject
{
    Q_OBJECT
public:

    explicit SurfaceManager(QObject *parent=0, surfaceAnalysisTools *tools = 0, ProfilePlot *profilePlot =0,
                   contourView *contourView = 0, SurfaceGraph *glPlot = 0, metricsDisplay *mets = 0);
    ~SurfaceManager();
    static SurfaceManager *get_instance(QObject *parent = 0, surfaceAnalysisTools *tools = 0,
                                        ProfilePlot *profilePlot = 0, contourView *contourPlot = 0,
                                        SurfaceGraph *glPlot = 0, metricsDisplay *mets = 0);
    static SurfaceManager *m_instance;
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
    void writeWavefront(QString fname, wavefront *wf, bool saveNulled);
    void useDemoWaveFront();
    void showUnwrap();
    void initWaveFrontLoad();
    void averageWavefrontFiles(QStringList files);
    void downSizeWf(wavefront *wf);
    void process(int wavefront_index, SurfaceManager *sm);
    wavefront *readWaveFront(QString fileName, bool mirrorParamsChanged);
    wavefront *xxx(QString name, bool t);
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

    bool m_askAboutReverse;
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
    int workToDo;
    int workProgress;

    wftStats *m_wftStats;
    textres Phase2(QList<rotationDef *> list, QList<wavefront *> inputs, int avgNdx);

signals:
    void currentNdxChanged(int);
    void waveFrontClicked(int);
    void deleteWavefront(int);
    void rotateTheseSig(int, QList<int>);
    void progress(int);
    void diameterChanged(double);
    void rocChanged(double);
    void nameChanged(QString, QString);
    void showTab(int);
    void load(SurfaceManager *);
    void load(QStringList, SurfaceManager *);
    void enableControls(bool);
private slots:
    void waveFrontClickedSlot(int ndx);
    void wavefrontDClicked(const QString & name);
    void centerMaskValue(int val);
    void outsideMaskValue(int val);
    void surfaceSmoothGBEnabled(bool b);
    void surfaceSmoothGBValue(double value);
    void computeZerns();
    void surfaceGenFinished(int ndx);
    void backGroundUpdate();
    void deleteWaveFronts(QList<int> list);
    void average(QList<int> list);
    void transfrom(QList<int> list);
    void filter(QList<int> list);
    void saveAllContours();
    void enableTools();
    void averageComplete(wavefront *wft);
    void outputLambdaChanged(double val);
    void resize(wavefront * wf, int size);
    void changeWavelength(wavefront *wf, double wavelength);
    void flipHorizontal(wavefront *wf);
    void flipVertical(wavefront *wf);
    void resizeW(int size);
    void changeWavelength(double wavelength);
    void flipHorizontal();
    void flipVertical();
    void defocusSetup();

public slots:
    void rotateThese(double angle, QList<int> list);
    void createSurfaceFromPhaseMap(cv::Mat phase, CircleOutline outside,
                                   CircleOutline center, QString name,
                                   QVector<std::vector<cv::Point> > polyArea= QVector<std::vector<cv::Point> >());
    void invert(QList<int> list);
    void wftNameChanged(int, QString);
    void showAllContours();
    void computeStandAstig(define_input *wizPage, QList<rotationDef *>);
    void ObstructionChanged();

    void loadComplete();
    void memoryLow();
    void defocusChanged();
    void tiltAnalysis();
};



#endif // SURFACEMANAGER_H
