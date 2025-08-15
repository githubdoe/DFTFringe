#ifndef BATCHIGRAMWIZARD_H
#define BATCHIGRAMWIZARD_H

#include <QWizard>
#include <QPushButton>
#include <QRadioButton>
#include <QStringList>
#include <QCheckBox>
#include <QPointF>
#include <QProgressBar>
class wavefrontFilterDlg;
class QwtPlot;
class astigScatterPlot;
class rmsPlot;
class SurfaceManager;
namespace Ui {
class batchIgramWizard;
}
class QListWidget;
class batchIntro;
class QLabel;
class batchIgramWizard : public QWizard
{
    Q_OBJECT
    enum { Page_Intro, Page_Process};

public:
    static QCheckBox *autoCb;
    static QCheckBox *filterCb;
    static QPushButton *goPb;
    static QPushButton *skipFile;
    static QPushButton *addFiles;
    static QCheckBox *saveFile;
    static QCheckBox *deletePreviousWave;
    static QCheckBox *showProcessPlots;
    static QPushButton *saveZerns;
    static QCheckBox *makeReviewAvi;
    static QLabel *memStatus;
    static QString reviewFileName;

    static QCheckBox *autoOutlineOutside;
    static QCheckBox *autoOutlineCenter;
    explicit batchIgramWizard(const QStringList &files, QWidget *parent = 0 , Qt::WindowFlags flags = Qt::Widget);
    ~batchIgramWizard();
    void listReady(QStringList list);

    void addAstig(const QString &name, QPointF value);
    void addRms(const QString &name, QPointF p);
    void progressValue(int min, int max, int value);
    void select(int n);
    void showPlots(bool flags);
    batchIntro *introPage;
signals:
    void abort();


private slots:
    void on_batchIgramWizard_finished(int result);

private:
    Ui::batchIgramWizard *ui;

};

class batchIntro : public QWizardPage
{

    Q_OBJECT

    public:
        batchIntro(QStringList files, QWidget *manger, QWidget *parent = 0);
        QListWidget *filesList;
        QProgressBar *pgrBar;
        astigScatterPlot *astigPlot;
        rmsPlot *m_rmsPlot;
        bool filterFile;
        bool filterWavefront;
        double filterRms;
        bool shouldFilterFile(double rms);
        bool shouldFilterWavefront(double rms);

public slots:
    void processBatch();
    void addFiles();
    void showContextMenu(QPoint pos);
    void eraseItem();
    void showPlots(bool flags);
    void on_filter(bool);
    void on_saveFiles(bool);
    void on_deletePreviousWave(bool);

signals:
    void processBatchList(QStringList);
private:
    wavefrontFilterDlg *filterDlg;
    void setupPlots();


};
#endif // BATCHIGRAMWIZARD_H
