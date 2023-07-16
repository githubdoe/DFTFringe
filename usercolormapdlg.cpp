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
#include "usercolormapdlg.h"
#include "ui_usercolormapdlg.h"
#include "qwt_scale_widget.h"
#include <qwt_plot_layout.h>
#include <QList>
#include <QColor>
#include <QSettings>
#include <QColorDialog>
#include "dftcolormap.h"
#include <QFileDialog>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QTextStream>
#include "colormapviewerdlg.h"
#include "plotcolor.h"
/*
m_listeWidget->setViewMode(QListWidget::IconMode);

m_listeWidget->setIconSize(QSize(200,200));

m_listeWidget->setResizeMode(QListWidget::Adjust);

m_listeWidget->addItem(new QListWidgetItem(QIcon("../earth.jpg"),"Earth"));
m_listeWidget->addItem(new QListWidgetItem(QIcon("../tornado.jpg"),"Tornado"));
m_listeWidget->addItem(new QListWidgetItem(QIcon("../ics.jpg"),"Wallpaper"));
*/

userColorMapDlg::userColorMapDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::userColorMapDlg),m_reverse(false)
{
    ui->setupUi(this);
std::cout <<" main2";
    QwtScaleWidget *rightAxis = ui->plot->axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( "wavefront error" );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorBarWidth(30);
    ui->plot->enableAxis(QwtPlot::yRight );
    QSettings set;

    stops.clear();

    QString sx = QString().sprintf("userColorStopColor%02d",10);
    for (int i = 0; i < 11; ++i){
        double pos = set.value(QString().sprintf("userColorStopPos%02d", i), (double)i * .1).toDouble();

        QColor c = QColor(set.value(QString().sprintf("userColorStopColor%02d", i), plotColors[i%10]).toString());

        stops << colorStop(pos,c);

        QCheckBox * cb = findChild<QCheckBox *>(QString().sprintf("cb%02d",i));
        if (cb) {
            cb->setChecked(set.value(QString().sprintf("userColorStopEnable%02d",i), 1).toBool());
        }
        QPushButton * pb = findChild<QPushButton *>(QString().sprintf("pb%02d",i));
        if (pb){
            QString s ="background-color: #" + QString::number(stops[i].color.rgb(), 16).toUpper();
            pb->setStyleSheet(s);
            pb->update();
        }

    }


    d_spectrogram = new QwtPlotSpectrogram();
    d_spectrogram->setRenderThreadCount( 0 ); // use system specific thread count

    d_spectrogram->setColorMap( new ColorMap(stops) );
    d_spectrogram->setCachePolicy( QwtPlotRasterItem::PaintCache );

    d_spectrogram->setData( new SpectrogramData2() );
    d_spectrogram->attach( ui->plot );

    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );
    // A color bar on the right axis

    rightAxis->setTitle( "Intensity" );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorMap( zInterval, new ColorMap(stops) );
    rightAxis->setColorBarWidth(30);
    ui->plot->setAxisScale( QwtPlot::yRight, zInterval.minValue(), zInterval.maxValue() );
    ui->plot->enableAxis( QwtPlot::yRight );
    ui->plot->plotLayout()->setAlignCanvasToScales( true );
    setColorMap();

}

void userColorMapDlg::setColorMap(){
    const QwtInterval zInterval = d_spectrogram->data()->interval( Qt::ZAxis );
    QwtScaleWidget *rightAxis = ui->plot->axisWidget( QwtPlot::yRight );

    QList<colorStop> stopList;
    if (ui->cb00->isChecked()){
        //stops[0].pos = .01;
        //stopList << stops[0];
    }
    for (int i = 0; i < 12; ++i){
        QCheckBox * cb = findChild<QCheckBox *>(QString().sprintf("cb%02d",i));
        if (cb && cb->isChecked()) {
            colorStop s(((m_reverse)? stops[10 - i].pos : stops[i].pos), stops[i].color);
            stopList << s;
            //qDebug() << stops[i].pos << stops[i].color;
            }
    }

    dftColorMap::userStops = stopList;
    d_spectrogram->setColorMap( new ColorMap(stopList ));
    rightAxis->setColorMap( zInterval, new ColorMap(stopList ));
    ui->plot->replot();
    ui->plot->show();

}

QColor userColorMapDlg::getColor(QColor c){
    QColorDialog dlg(c);
    c = dlg.getColor();
    return c;
}

userColorMapDlg::~userColorMapDlg()
{
    delete ui;
}

void userColorMapDlg::on_buttonBox_accepted()
{

}


void userColorMapDlg::on_pb00_clicked()
{
    QColor c = QColorDialog::getColor(stops[0].color);
    stops[0].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb00->setStyleSheet(s);
    ui->pb00->update();
    QSettings set;
    set.setValue("userColorStopColor00",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb01_clicked()
{
    QColor c = QColorDialog::getColor(stops[1].color);
    stops[1].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb01->setStyleSheet(s);
    ui->pb01->update();
    QSettings set;
    set.setValue("userColorStopColor01",c.name());
    setColorMap();
    emit colorMapChanged(5);
}
void userColorMapDlg::on_pb02_clicked()
{
    QColor c = QColorDialog::getColor(stops[2].color);
    stops[2].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb02->setStyleSheet(s);
    ui->pb02->update();
    QSettings set;
    set.setValue("userColorStopColor02",c.name());
    setColorMap();
    emit colorMapChanged(5);

}

void userColorMapDlg::on_pb03_clicked()
{
    QColor c = QColorDialog::getColor(stops[3].color);
    stops[3].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb03->setStyleSheet(s);
    ui->pb03->update();
    QSettings set;
    set.setValue("userColorStopColor03",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb04_clicked()
{
    QColor c = QColorDialog::getColor(stops[4].color);
    stops[4].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb04->setStyleSheet(s);
    ui->pb04->update();
    QSettings set;
    set.setValue("userColorStopColor04",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb05_clicked()
{
    QColor c = QColorDialog::getColor(stops[5].color);
    stops[5].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb05->setStyleSheet(s);
    ui->pb05->update();
    QSettings set;
    set.setValue("userColorStopColor05",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb06_clicked()
{
    QColor c = QColorDialog::getColor(stops[6].color);
    stops[6].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb06->setStyleSheet(s);
    ui->pb06->update();
    QSettings set;
    set.setValue("userColorStopColor06",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb07_clicked()
{
    QColor c = QColorDialog::getColor(stops[7].color);
    stops[7].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb07->setStyleSheet(s);
    ui->pb07->update();
    QSettings set;
    set.setValue("userColorStopColor07",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb08_clicked()
{
    QColor c = QColorDialog::getColor(stops[8].color);
    stops[8].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb08->setStyleSheet(s);
    ui->pb08->update();
    QSettings set;
    set.setValue("userColorStopColor08",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb09_clicked()
{
    QColor c = QColorDialog::getColor(stops[9].color);
    stops[9].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb09->setStyleSheet(s);
    ui->pb09->update();
    QSettings set;
    set.setValue("userColorStopColor09",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_pb10_clicked()
{
    QColor c = QColorDialog::getColor(stops[10].color);
    stops[10].color = c;
    QString s("background-color: #" + QString::number(c.rgb(), 16).toUpper());
    ui->pb10->setStyleSheet(s);
    ui->pb10->update();
    QSettings set;
    set.setValue("userColorStopColor09",c.name());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_apply_clicked()
{
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb01_clicked()
{    QSettings set;
     set.setValue("userColorStopEnable01", ui->cb01->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb00_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable00", ui->cb00->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb02_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable02", ui->cb02->isChecked());
    setColorMap();
     emit colorMapChanged(5);
}

void userColorMapDlg::on_cb03_clicked()
{    QSettings set;
     set.setValue("userColorStopEnable03", ui->cb03->isChecked());
     setColorMap();
     emit colorMapChanged(5);
}

void userColorMapDlg::on_cb04_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable04", ui->cb04->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb05_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable05", ui->cb05->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb06_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable06", ui->cb06->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb07_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable07", ui->cb07->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb08_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable08", ui->cb08->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb09_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable09", ui->cb09->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}

void userColorMapDlg::on_cb10_clicked()
{
    QSettings set;
    set.setValue("userColorStopEnable10", ui->cb10->isChecked());
    setColorMap();
    emit colorMapChanged(5);
}


void userColorMapDlg::on_savePb_clicked()
{

    QString fileName = QFileDialog::getSaveFileName(0,
        tr("Save User Color Map"),".",tr("color map file (*.cmp)"));

    if (fileName.isEmpty())
        return;

    std::ofstream file((fileName.toStdString().c_str()));
    if (!file.is_open()) {
        QMessageBox::warning(this, tr("Write color map"),
                             tr("Cannot write file %1: ")
                             .arg(fileName));
        return;
    }

    for(int i = 0; i < stops.size(); ++i){

        file << i << "," << stops[i].pos << "," << stops[i].color.name().toStdString().c_str() << ",";
        QString s = QString().sprintf("cb%02d",i);
        QCheckBox *cb = findChild<QCheckBox *>(s);
        if (cb && cb->isChecked())
            file << "checked";
        else
            file << "notChecked";
        file << std::endl;
    }
    file.close();
}

void userColorMapDlg::on_loadPb_clicked()
{
    colorMapViewerDlg vdlg;
    if (!vdlg.exec()){
        return;
    }

    QString fileName = vdlg.m_selection;

    QFile inputFile(fileName);
    if (inputFile.open(QIODevice::ReadOnly))
    {
       QTextStream in(&inputFile);
       while (!in.atEnd())
       {
          QString line = in.readLine();
          QStringList parms = line.split(",");
          int ndx = parms[0].toInt();
          colorStop s(parms[1].toDouble(), QColor(parms[2]));
          stops[parms[0].toInt()] = s;
          QPushButton * pb = findChild<QPushButton *>(QString().sprintf("pb%02d",ndx));
          if (pb && ndx < 12){
              QString s ="background-color: #" + QString::number(stops[ndx].color.rgb(), 16).toUpper();
              pb->setStyleSheet(s);
              pb->update();
              QSettings set;
              set.setValue(QString().sprintf("userColorStopColor%02d",ndx), QColor(parms[2]).name());
          }
          QCheckBox * cb = findChild<QCheckBox *>(QString().sprintf("cb%02d",ndx));
          if (cb) {
              cb->setChecked(parms[3] == "checked");

          }
       }
       inputFile.close();
       setColorMap();
    }
}

void userColorMapDlg::on_reverseCB_clicked(bool checked)
{
    m_reverse = checked;
    setColorMap();
    emit colorMapChanged(5);
}
