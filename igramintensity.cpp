/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "igramintensity.h"
#include "ui_igramintensity.h"
#include <QImageWriter>
#include <QFileDialog>
#include <qwt_plot_renderer.h>
#include <QSettings>
#include <QLabel>
igramIntensity::igramIntensity(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::igramIntensity)
{
    ui->setupUi(this);
}

igramIntensity::~igramIntensity()
{
    delete ui;
}
void igramIntensity::setIgram(cv::Mat img){

    ui->plot->setSurface(img);

}

void igramIntensity::on_showRed_clicked(bool checked)
{
    ui->plot->showRed(checked);
}

void igramIntensity::on_showBlue_clicked(bool checked)
{
    ui->plot->showBlue(checked);
}

void igramIntensity::on_showGreen_clicked(bool checked)
{
    ui->plot->showGreen(checked);
}

void igramIntensity::on_pushButton_clicked()
{
    QSettings set;
    QString path = set.value("mirrorConfigFile").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString dd = info.dir().absolutePath();
    const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList filter;
    if ( imageFormats.size() > 0 )
    {
        QString imageFilter( tr( "Images" ) );
        imageFilter += " (";
        for ( int i = 0; i < imageFormats.size(); i++ )
        {
            if ( i > 0 )
                imageFilter += " ";
            imageFilter += "*.";
            imageFilter += imageFormats[i];
        }
        imageFilter += ")";

        filter += imageFilter;
    }
    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save stats plot"), dd + "//intensity.jpg",filter.join( ";;" ));


    if (fName.isEmpty())
        return;

    const QSizeF size = this->size();
    const QRect imageRect = QRect(0,0,size.width(),size.height());

    QImage image( imageRect.size(), QImage::Format_ARGB32 );


    QPainter painter( &image );
    QwtPlotRenderer renderer;
    this->render( &painter);
    painter.end();

    image.save( fName, QFileInfo( fName ).suffix().toLatin1() );
}

void igramIntensity::on_checkBox_clicked(bool checked)
{
    Qt::WindowFlags flags = this->windowFlags();
    if (checked)
    {
        this->setWindowFlags(flags | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
        this->show();
    }
    else
    {
        this->setWindowFlags(flags ^ (Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint));
        this->show();
    }
}
