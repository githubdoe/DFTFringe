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
#include "surfaceanalysistools.h"
#include "ui_surfaceanalysistools.h"
#include <QAction>
#include <QDebug>
#include <QMenu>
#include <QSettings>
#include <QtAlgorithms>
#include <QLineEdit>
#include "mirrordlg.h"
#include "renamewavefrontdlg.h"
#include <QMessageBox>
surfaceAnalysisTools *surfaceAnalysisTools::m_Instance = NULL;

surfaceAnalysisTools * surfaceAnalysisTools::get_Instance(QWidget *parent){
    if  (m_Instance == NULL){
        m_Instance = new surfaceAnalysisTools(parent);
    }
    return m_Instance;
}

surfaceAnalysisTools::surfaceAnalysisTools(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::surfaceAnalysisTools),lastCurrentItem(-1)
{
    ui->setupUi(this);
    //ui->deleteWave->setIcon(style()->standardIcon(QStyle::SP_TrashIcon));
    m_edgeMaskLabel = ui->outsideEdgeMaskLabel;
    m_centerMaskLabel = ui->insideEdgeMaskLabel;
    m_blurrRadius = ui->blurMm;
    QSettings settings;
    bool ch = settings.value("GBlur", true).toBool();
    double val = settings.value("GBValue", 20).toInt();
    ui->surfaceSmoothGausianBlurr->setValue(val);
    ui->blurCB->setCheckState((ch) ? Qt::Checked : Qt::Unchecked);
    m_useDefocus = false;
    m_defocus = 0.;
    connect(&m_defocusTimer, SIGNAL(timeout()), this, SLOT(defocusTimerDone()));
    connect(ui->wavefrontList->itemDelegate(), SIGNAL(closeEditor(QWidget*, QAbstractItemDelegate::EndEditHint)), this,
            SLOT(ListWidgetEditEnd(QWidget*, QAbstractItemDelegate::EndEditHint)));
    ui->wavefrontList->setContextMenuPolicy(Qt::CustomContextMenu);
}

void surfaceAnalysisTools::enableControls(bool flag){

    setEnabled(flag);
}


void surfaceAnalysisTools::setBlurText(QString txt){
    ui->blurMm->setText(txt);
}

void surfaceAnalysisTools::addWaveFront(const QString &name){

    QStringList list = name.split('/');
    QString shorter = name;
    if (list.size() > 1)
        shorter = list[list.size()-2] + "/" + list[list.size()-1];//fontMetrics().elidedText(name,Qt::ElideLeft,w);

    ui->wavefrontList->addItem(shorter);
    currentNdxChanged(ui->wavefrontList->count()-1);
    ui->wavefrontList->item(lastCurrentItem)->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
}

void surfaceAnalysisTools::removeWaveFront(const QString &){

}

surfaceAnalysisTools::~surfaceAnalysisTools()
{
    delete ui;
}

void surfaceAnalysisTools::on_blurCB_clicked(bool checked)
{
    ui->surfaceSmoothGausianBlurr->setEnabled(checked);
    emit surfaceSmoothGBEnabled(checked);
}

void surfaceAnalysisTools::on_spinBox_valueChanged(int arg1)
{
    emit outsideMaskValue(arg1);
}

void surfaceAnalysisTools::on_spinBox_2_valueChanged(int arg1)
{
    emit centerMaskValue(arg1);
}

// this is responsible of givin user feedback about which wft is currently displayed
void surfaceAnalysisTools::currentNdxChanged(int ndx){
    if (ui->wavefrontList->count() == 0)
        return;

    QListWidgetItem *item = ui->wavefrontList->item(lastCurrentItem);
    if (item){
        item->setIcon(style()->standardIcon(QStyle::SP_CustomBase));
    }
    ui->wavefrontList->setCurrentRow(ndx,QItemSelectionModel::Current);

    item = ui->wavefrontList->currentItem();
    item->setIcon(style()->standardIcon(QStyle::SP_DialogYesButton));

    lastCurrentItem = ui->wavefrontList->currentRow();
}

void surfaceAnalysisTools::deleteWaveFront(int i){
    QListWidgetItem* item = ui->wavefrontList->takeItem(i);
    delete item;
}

// this is called when pressing enter on a wavefront selected with arrow key
void surfaceAnalysisTools::on_wavefrontList_activated(const QModelIndex &index)
{
    on_wavefrontList_clicked(index);
}
void surfaceAnalysisTools::on_wavefrontList_clicked(const QModelIndex &index)
{
    /*
    QModelIndexList indexes = ui->wavefrontList->selectionModel()->selectedIndexes();
    foreach(QModelIndex index, indexes)
    {
        emit waveFrontClicked(index.row());
    }
            */
    // only inform about the clicked item in the list for efficency.
    //That way surface manager does not spend time updating things not shown.
    emit waveFrontClicked(index.row());
}

// this is "right click" on windows
void surfaceAnalysisTools::on_wavefrontList_customContextMenuRequested(const QPoint &pos)
{
    QListWidgetItem *item = ui->wavefrontList->itemAt(pos);
    if (item == 0)
        return;

    QMenu contextMenu(this);

    // Create "Delete" action
    QAction *deleteAction = new QAction("Delete", this);
    connect(deleteAction, &QAction::triggered, [this, item]() {
        on_deleteWave_clicked();
    });
    contextMenu.addAction(deleteAction);

    // Create "Invert" action
    QAction *invertAction = new QAction("Invert", this);
    connect(invertAction, &QAction::triggered, [this, item]() {
        on_InvertPB_pressed();
    });
    contextMenu.addAction(invertAction);

    // Create "Rename" action
    QAction *renameAction = new QAction("Rename", this);
    connect(renameAction, &QAction::triggered, [this, item]() {
        ui->wavefrontList->editItem(item);
    });
    contextMenu.addAction(renameAction);

    // Create "Rotate" action
    QAction *rotateAction = new QAction("Rotate", this);
    connect(rotateAction, &QAction::triggered, [this, item]() {
        on_transformsPB_clicked();
    });
    contextMenu.addAction(rotateAction);

    // Show the context menu at the cursor position
    contextMenu.exec(ui->wavefrontList->mapToGlobal(pos));
}

QList<int> surfaceAnalysisTools::SelectedWaveFronts(){
    QModelIndexList indexes = ui->wavefrontList->selectionModel()->selectedIndexes();

    QList<int> indexList;
    foreach(QModelIndex index, indexes)
    {
        indexList << index.row();
    }
    return indexList;
}

void surfaceAnalysisTools::on_deleteWave_clicked()
{
    QList<int> indexList = SelectedWaveFronts();
    std::sort(indexList.begin(), indexList.end(), std::greater<int>());
    emit deleteTheseWaveFronts(indexList);
}




void surfaceAnalysisTools::on_transformsPB_clicked()
{
    emit doxform(SelectedWaveFronts());
}

void surfaceAnalysisTools::on_averagePB_clicked()
{
    emit average(SelectedWaveFronts());
}
void surfaceAnalysisTools::select(int item){
    ui->wavefrontList->selectionModel()->reset();
    if (ui->wavefrontList->count() > 0)
        ui->wavefrontList->item(item)->setSelected(true);
}

void surfaceAnalysisTools::on_SelectAllPB_clicked()
{
    ui->wavefrontList->selectAll();
}

void surfaceAnalysisTools::on_SelectNonePB_clicked()
{
    ui->wavefrontList->selectionModel()->reset();
}
void surfaceAnalysisTools::nameChanged(int ndx, QString newname){
    ui->wavefrontList->item(ndx)->setText(newname);
}

void surfaceAnalysisTools::nameChanged(QString old, QString newname){

    // first look for full length name match
    QList<QListWidgetItem *> ql = ui->wavefrontList->findItems(old,Qt::MatchExactly);
    if (ql.size() == 1){
        ql[0]->setText(newname);
        return;
    }

    QStringList list = old.split('/');
    QString shorter = old;

    if (list.size() > 1)
        shorter = list[list.size()-2] + "/" + list[list.size()-1];

    ql = ui->wavefrontList->findItems(shorter,Qt::MatchEndsWith);
    if (ql.size()) {

        list = newname.split('/');
        QString shorter = newname;

        if (list.size() > 1)
            shorter = list[list.size()-2] + "/" + list[list.size()-1];
        ql[0]->setText(shorter);
    }
}



void surfaceAnalysisTools::defocusTimerDone(){
    m_defocusTimer.stop();
    emit defocusChanged();
}

#include "defocusdlg.h"
void surfaceAnalysisTools::defocusControlChanged(double val){
    m_defocus = val;
    emit defocusChanged();

}


void surfaceAnalysisTools::on_InvertPB_pressed()
{
    emit invert(SelectedWaveFronts());
}

// this is troggered on renaming/editing end
void surfaceAnalysisTools::ListWidgetEditEnd(QWidget* editor, QAbstractItemDelegate::EndEditHint ){
    const QString NewValue = reinterpret_cast<QLineEdit*>(editor)->text();
    QModelIndexList indexList = ui->wavefrontList->selectionModel()->selectedIndexes();
    // because we don't know exactly which wft has been renamed in case of multiple selection, we must update name for all
    for (const QModelIndex &index : indexList) {
        const int row = index.row();
        // update name only for thoseactually matching new name (that have actually be renamed)
        if(NewValue == ui->wavefrontList->item(row)->text()){
            emit wftNameChanged(index.row(), ui->wavefrontList->item(row)->text());
            //force complete redisplay to reload if rename was on currently displayed wft
            if(index.row() == lastCurrentItem) {
                on_wavefrontList_clicked(index);
            }
            // we cannot break; the loop because there might be duplicate names (rare but can happen)
        }
    }
}

void surfaceAnalysisTools::on_pushButton_clicked()
{
    emit updateSelected();
}


void surfaceAnalysisTools::on_filterPB_clicked()
{
    emit filterWavefronts();
}



void surfaceAnalysisTools::on_surfaceSmoothGausianBlurr_valueChanged(double arg1)
{
   emit     surfaceSmoothGBValue(arg1);
}






void surfaceAnalysisTools::on_defocus_clicked()
{
    if (ui->wavefrontList->count() == 0){
        return;
    }
    m_useDefocus = true;
    emit defocusSetup();

    defocusDlg *dlg = new defocusDlg();
    connect(dlg, SIGNAL(defocus(double)), this, SLOT(defocusControlChanged(double)));
    connect(dlg, SIGNAL(finished(int)), this, SLOT(closeDefocus(int)) );
    dlg->show();

return;
}

void surfaceAnalysisTools::closeDefocus(int /*result*/){
    m_useDefocus = false;
    m_defocus = 0.;
    emit defocusChanged();
}
