#ifndef SURFACEANALYSISTOOLS_H
#define SURFACEANALYSISTOOLS_H

#include <QDockWidget>
#include <QListWidget>
#include <QLabel>
#include <QList>
#include <QTimer>
namespace Ui {
class surfaceAnalysisTools;
}

class surfaceAnalysisTools : public QDockWidget
{
    Q_OBJECT
private:
    static surfaceAnalysisTools *m_Instance;
    QTimer m_defocusTimer;
public:
    explicit surfaceAnalysisTools(QWidget *parent = 0);
    ~surfaceAnalysisTools();
    static surfaceAnalysisTools *get_Instance(QWidget *parent = 0);
    void addWaveFront(const QString &name);
    void removeWaveFront(const QString &);
    QLabel* m_edgeMaskLabel;
    QLabel* m_centerMaskLabel;
    QLabel *m_blurrRadius;
    QList<int> SelectedWaveFronts();
    bool m_useDefocus;
    double m_defocus;
    void setBlurText(QString txt);
signals:
    void doxform(QList<int>);
    void surfaceSmoothGBValue(int);
    void surfaceSmoothGBEnabled(bool);
    void wavefrontDClicked(const QString &);
    void waveFrontClicked(int ndx);
    void outsideMaskValue(int);
    void centerMaskValue(int);
    void deleteTheseWaveFronts(QList<int>);
    void average(QList<int>);
    void surfaceBaseChanged(bool);
    void defocusChanged();
    void invert(QList<int>);
    void wftNameChanged(int, QString);

public slots:
    void currentNdxChanged(int);
    void deleteWaveFront(int);
    void defocusTimerDone();
    void nameChanged(QString, QString);
    void enableControls(bool);
private slots:
    void on_surfaceSmoothGausianBlurr_valueChanged(int arg1);

    void on_wavefrontList_itemDoubleClicked(QListWidgetItem *item);

    void on_spinBox_valueChanged(int arg1);

    void on_spinBox_2_valueChanged(int arg1);

    void on_wavefrontList_clicked(const QModelIndex &index);

    void on_blurCB_clicked(bool checked);

    void on_deleteWave_clicked();

    void on_transformsPB_clicked();

    void on_averagePB_clicked();

    void on_SelectAllPB_clicked();

    void on_SelectNonePB_clicked();

    void on_wavefrontBased_clicked(bool);

    void on_zernikeBased_clicked(bool);

    void on_defocusDial_valueChanged(int value);

    void on_checkBox_clicked(bool checked);

    void on_defocusWaves_textChanged(const QString &arg1);

    void on_InvertPB_pressed();

    void on_wavefrontList_customContextMenuRequested(const QPoint &pos);

    void ListWidgetEditEnd(QWidget *editor, QAbstractItemDelegate::EndEditHint);
private:
    Ui::surfaceAnalysisTools *ui;
    int lastCurrentItem;
};

#endif // SURFACEANALYSISTOOLS_H
