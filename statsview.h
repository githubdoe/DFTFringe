#ifndef STATSVIEW_H
#define STATSVIEW_H

#include <QDialog>
#include <QSplitter>
#include <QVector>
#include "surfacemanager.h"
#include "wftstats.h"

namespace Ui {
class statsView;
}

class statsView : public QDialog
{
    Q_OBJECT

public:
    explicit statsView(SurfaceManager *parent = 0);
    QSplitter *m_hs;
    QSplitter *m_vs;
    SurfaceManager *m_sm;
    wftStats *m_stats;
    QVector<wavefront *> wavefrontsToUse;
    QWidget *w1,*w2,*w3;
    ~statsView();
    void replot();
    QString title(QString dir);
    void getWavefronts();
    void sresize();

private slots:

    void on_showWftNames_toggled(bool checked);

    void on_removeOutliers_toggled(bool checked);

    void on_checkBox_2_toggled(bool checked);

    void on_saveImg_clicked();

    void on_SaveCSV_clicked();

    void on_savePdf_clicked();

    void on_replot_clicked();

    void on_checkBox_4_toggled(bool checked);

    void on_zernFromSP_valueChanged(int arg1);

    void on_zernToSP_valueChanged(int arg1);

private:
    Ui::statsView *ui;
    bool m_removeOutliers;
    bool m_removeRMS;
};

#endif // STATSVIEW_H
