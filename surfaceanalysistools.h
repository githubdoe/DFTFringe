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
#ifndef SURFACEANALYSISTOOLS_H
#define SURFACEANALYSISTOOLS_H

#include <QDockWidget>
#include <QListWidget>
#include <QLabel>
#include <QList>
#include <QTimer>
#include <opencv2/opencv.hpp>
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
    void nameChanged(int, QString);
    void select(int item);

signals:
    void doxform(QList<int>);
    void surfaceSmoothGBValue(double);
    void surfaceSmoothGBEnabled(bool);
    void wavefrontDClicked(const QString &);
    void waveFrontClicked(int ndx);
    void outsideMaskValue(int);
    void centerMaskValue(int);
    void deleteTheseWaveFronts(QList<int>);
    void average(QList<int>);
    void defocusChanged();
    void invert(QList<int>);
    void wftNameChanged(int, QString);
    void updateSelected();
    void filterWavefronts(QList<int>);
    void defocusSetup();



public slots:
    void currentNdxChanged(int);
    void deleteWaveFront(int);
    void defocusTimerDone();
    void nameChanged(QString, QString);
    void defocusControlChanged(double);
    void enableControls(bool);
private slots:

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

     void on_InvertPB_pressed();

    void on_wavefrontList_customContextMenuRequested(const QPoint &pos);

    void ListWidgetEditEnd(QWidget *editor, QAbstractItemDelegate::EndEditHint);

    void on_pushButton_clicked();

    void on_filterPB_clicked();

    void on_surfaceSmoothGausianBlurr_valueChanged(double arg1);

    void closeDefocus(int);

    void on_defocus_clicked();

private:
    Ui::surfaceAnalysisTools *ui;
    int lastCurrentItem;
};

#endif // SURFACEANALYSISTOOLS_H
