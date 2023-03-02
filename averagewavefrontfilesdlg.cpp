#include "averagewavefrontfilesdlg.h"
#include "ui_averagewavefrontfilesdlg.h"
#include "surfacemanager.h"
#include "mainwindow.h"
#include "wavefrontaveragefilterdlg.h"
#include <opencv2/opencv.hpp>
#include "rejectedwavefrontsdlg.h"
#include <QFileInfo>
#include "utils.h"
#include <opencv2/imgproc.hpp>

void showData(const std::string &txt, Mat mat, bool useLog);
averageWaveFrontFilesDlg::averageWaveFrontFilesDlg(QStringList list, SurfaceManager *m, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::averageWaveFrontFilesDlg)
{
    sm = m;
    average = 0;
    ui->setupUi(this);
    ui->fileList->addItems(list);
    ui->progressBar->setRange(0,10);
    ui->progressBar->setValue(0);
    useFilter = false;
}
averageWaveFrontFilesDlg::~averageWaveFrontFilesDlg()
{
    delete ui;
}


void averageWaveFrontFilesDlg::on_MoveUp_clicked()
{
    int currentIndex = ui->fileList->currentRow();
    if (currentIndex == 0)
        return;
    QListWidgetItem *currentItem = ui->fileList->takeItem(currentIndex);
    ui->fileList->insertItem(currentIndex-1, currentItem);
    ui->fileList->setCurrentRow(currentIndex-1);
}
extern double outputLambda;
void averageWaveFrontFilesDlg::on_process_clicked()
{
    bool first = true;
    abort = false;
    QStringList rejects;
    bool configChanged = false;
    int count = ui->fileList->count();
    if (count == 0)
        return;
    ui->progressBar->setMaximum(count -1);
    ui->progressBar->setFormat("%p%");
    QApplication::setOverrideCursor(Qt::WaitCursor);
    cv::Mat mask;
    static mirrorDlg *md = mirrorDlg::get_Instance();
    int total = 0;
    for(int i = 0; i < count; ++i)
    {
        if (abort)
            break;
        QListWidgetItem* item = ui->fileList->item(i);
        QString name = item->text();
        ui->fileList->setCurrentRow(i);
        qApp->processEvents();
        wavefront *wf = sm->readWaveFront(name,configChanged);
        if (useFilter){
            sm->makeMask(wf);
            sm->generateSurfacefromWavefront(wf);
            cv::Scalar mean,std;
            cv::meanStdDev(wf->workData,mean,std,wf->workMask);
            double stdVal = std.val[0]* md->lambda/outputLambda;
            if (stdVal > filterRMS){
                QFileInfo info(name);
                QString item = QString().sprintf("%s RMS:%lf", info.baseName().toStdString().c_str(), stdVal);
                rejects << item;
                ui->fileList->item(i)->setForeground(Qt::red);
                qApp->processEvents();
                delete wf;
                continue;
            }
        }

        if (first){
            first = false;
            average = wf;
            sm->makeMask(average, false);
            mask = average->mask.clone();
            total = 1;

        }
        else {
            wavefront *second = wf;

            ++total;
            sm->makeMask(second, false);
            int size1 = average->data.rows * average->data.cols;
            int size2 = second->data.rows * second->data.cols;
            cv::Mat resize = second->data.clone();
            cv::Mat resizeMask = second->mask;
            if (size1 != size2) {
                cv::resize(resize,resize, cv::Size(average->data.cols, average->data.rows));
                cv::resize(mask,resizeMask, cv::Size(average->data.cols, average->data.rows));
            }
            average->data = average->data + resize;
            cv::Mat maskResult;
            cv::bitwise_and(mask, resizeMask, mask);
            delete second;
        }

        ui->progressBar->setValue(i);

    }
    if (total > 1){
        average->mask = mask.clone();
        average->data.copyTo(average->data,average->mask);
        if (abort){
            if (average)
                delete average;
            average = 0;
        }
        else {
            average->data/= total;
            // set average to surface manager.
        }
        ui->progressBar->setValue(0);
        ui->progressBar->setFormat("Done");

        emit averageComplete( average );
    }
    if (rejects.size() > 0){
        rejectedWavefrontsDlg dlg(rejects);
        dlg.exec();
    }
    QApplication::restoreOverrideCursor();
}

void averageWaveFrontFilesDlg::on_addFiles_clicked()
{
    MainWindow * mw = ((MainWindow*)(sm->parent()));
    QStringList files = mw->SelectWaveFrontFiles();
    if (files.size() == 0)
        return;

    ui->fileList->addItems(files);

}

void averageWaveFrontFilesDlg::on_buttonBox_clicked(QAbstractButton *button)
{

}

void averageWaveFrontFilesDlg::on_moveDown_clicked()
{
    int currentIndex = ui->fileList->currentRow();
    if (currentIndex == ui->fileList->count() -1)
        return;
    QListWidgetItem *currentItem = ui->fileList->takeItem(currentIndex);
    ui->fileList->insertItem(currentIndex+1, currentItem);
    ui->fileList->setCurrentRow(currentIndex+1);
}

void averageWaveFrontFilesDlg::on_deleteFiles_clicked()
{
    int currentIndex = ui->fileList->currentRow();
    QListWidgetItem *currentItem = ui->fileList->takeItem(currentIndex);
    delete currentItem;
    ui->fileList->setCurrentRow(currentIndex-1);
}


void averageWaveFrontFilesDlg::on_filterCB_clicked(bool checked)
{
    useFilter = checked;
    if (checked) {
        wavefrontAverageFilterDlg dlg;
        if (dlg.exec()){
            filterRMS = dlg.rms();

        }
    }
}
