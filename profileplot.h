#ifndef PROFILEPLOT_H
#define PROFILEPLOT_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <qwt_plot.h>
#include "wavefront.h"
#include "contourtools.h"
#include <qwt_compass.h>
#include <qwt_dial.h>
#include "opencv/cv.h"
#include <QRadioButton>
#include <QCheckBox>
namespace Ui {
class ProfilePlot;
}

class ProfilePlot : public QWidget
{
    Q_OBJECT

public:
    QwtPlot *m_plot;
    wavefront* m_wf;
    ProfilePlot( QWidget *parent = NULL, ContourTools* tools = 0 );
    QVector<wavefront*> *wfs;
    void setSurface(wavefront * wf);
    virtual void resizeEvent( QResizeEvent * );
    QPolygonF createProfile(double units, wavefront *wf);
    ContourTools *m_tools;
    double m_waveRange;
    virtual bool eventFilter( QObject *, QEvent * );
    QCheckBox *showNmCB;
    QCheckBox *showSurfaceCB;
    QRadioButton *OneOnly;
    QRadioButton *Show16;
    QRadioButton *ShowAll;
    int type;
    double m_showSurface;
    double m_showNm;

public slots:
    void setWavefronts(QVector<wavefront*> *wf);
    void angleChanged(double a);
    void newDisplayErrorRange(double min, double max);
    void zeroOffsetChanged(QString);
    void showOne();
    void show16();
    void showAll();
    void showNm(bool);
    void showSurface(bool);
private:
    void populate();
    void updateGradient();
    bool dragging;
    QPoint startPos;
    QString offsetType;
    QwtCompass *compass;


private:
    Ui::ProfilePlot *ui;
};

#endif // PROFILEPLOT_H

