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
#ifndef COLORCHANNELDISPLAY_H
#define COLORCHANNELDISPLAY_H

#include <QDialog>
#include <QImage>
#include <opencv2/core/core.hpp>
namespace Ui {
class ColorChannelDisplay;
}

class ColorChannelDisplay : public QDialog
{
    Q_OBJECT


public:
    explicit ColorChannelDisplay(QWidget *parent = 0);
    ~ColorChannelDisplay();
    void setImage(cv::Mat imgMat);

private:
    Ui::ColorChannelDisplay *ui;
};

#endif // COLORCHANNELDISPLAY_H
