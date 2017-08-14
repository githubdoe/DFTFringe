#ifndef ASTIGSTATSDLG_H
#define ASTIGSTATSDLG_H

#include <QDialog>
#include "wavefront.h"
#include <QVector>
#include <QMap>
#include <QPointF>
#include <qwt_plot_item.h>
class QwtPlot;
class QwtPlotPicker;
class Zoomer;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;
namespace Ui {

class astigStatsDlg;
}

class astigStatsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit astigStatsDlg( QVector<wavefront *> wavefronts,QWidget *parent = 0);
    ~astigStatsDlg();
    void plot();
private slots:
    void on_zernikePB_pressed();
    void on_bestFitCB_clicked(bool checked);

    void on_distribution_clicked();

    void on_sdCB_clicked();

    void on_help_clicked();
    void on_savePdf();

    void on_clearPb_clicked();

    void on_onlyAverages_clicked();
    void showItem(QVariant item, bool on, int ndx);

    void on_savePB_clicked();
    void showSamplesChanged(bool);
    void runningNChanged(int);
    void usePolar(bool flag);

private:
    int mndx;
    QwtPlot *dplot;
    QVector<QStringList> m_zerns;
    Zoomer *zoomer;
    Ui::astigStatsDlg *ui;
    QVector<wavefront*> m_wavefronts;
    QwtPlot *avgPlot(cv::Mat x, cv::Mat y);
    QTextEdit *editor;
    QwtPlotPicker *d_picker;
    bool PDFMode;
    QWidget *distributionWindow;
    QVector<QPointF> points;
    int runningAvgN;
    bool showSamples;
    QVBoxLayout *layout;
    QHBoxLayout *toolLayout;
    bool m_usePolar;
};

#endif // ASTIGSTATSDLG_H
