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
#ifndef USERCOLORMAPDLG_H
#define USERCOLORMAPDLG_H

#include <QDialog>
#include <QList>
#include <QColor>
#include "qwt_color_map.h"
#include "qwt_raster_data.h"
#include "qwt_interval.h"
#include "qwt_plot_spectrogram.h"
#include <math.h>
namespace Ui {
class userColorMapDlg;
}
class SpectrogramData2: public QwtRasterData
{
public:
    SpectrogramData2()
    {
        setInterval( Qt::XAxis, QwtInterval( -5, 5 ) );
        setInterval( Qt::YAxis, QwtInterval( -5, 5 ) );
        setInterval( Qt::ZAxis, QwtInterval( -25, 25) );
    }

    virtual double value( double x, double y ) const
    {
        return x  * y;

    }
};
class colorStop{
public:
    double pos;
    QColor color;
    colorStop(double pos, QColor c) :
        pos(pos), color(c){
    }
};

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap(QList<colorStop> &stops, QColor less = QColor(Qt::black), QColor more = QColor(Qt::yellow)):
        QwtLinearColorMap( less,more )
    {
        foreach(colorStop s, stops){
            addColorStop( s.pos,s.color );
        }
    }
};


class userColorMapDlg : public QDialog
{
    Q_OBJECT

public:
    explicit userColorMapDlg(QWidget *parent = 0);
    ~userColorMapDlg();
    QList<colorStop> stops;
    ColorMap *map;
    QwtPlotSpectrogram *d_spectrogram;
    QColor getColor(QColor c);
    void setColorMap();
    bool m_reverse;
signals:
    void colorMapChanged(int);
private slots:
    void on_buttonBox_accepted();

    void on_pb00_clicked();
    void on_pb01_clicked();
    void on_pb02_clicked();

    void on_pb03_clicked();

    void on_pb04_clicked();

    void on_pb05_clicked();

    void on_pb06_clicked();

    void on_pb07_clicked();

    void on_pb08_clicked();

    void on_pb09_clicked();

    void on_pb10_clicked();

    void on_apply_clicked();

    void on_cb01_clicked();

    void on_cb00_clicked();

    void on_cb02_clicked();

    void on_cb03_clicked();

    void on_cb04_clicked();

    void on_cb05_clicked();

    void on_cb06_clicked();

    void on_cb07_clicked();

    void on_cb08_clicked();

    void on_cb09_clicked();

    void on_cb10_clicked();

    void on_savePb_clicked();

    void on_loadPb_clicked();

    void on_reverseCB_clicked(bool checked);

private:
    Ui::userColorMapDlg *ui;
};

#endif // USERCOLORMAPDLG_H
