#include "surface3dcontrolsdlg.h"
#include "ui_surface3dcontrolsdlg.h"
#include <QSettings>
double maxHeightSelections[] = {.01, .02, .05 , .1, .125, .2, .25, .3,.4 ,.5, 1.,2.,5.,10., 20., 50., 100.};
Surface3dControlsDlg::Surface3dControlsDlg(SurfaceGraph *surfP) :
     m_surf(surfP),
    ui(new Ui::Surface3dControlsDlg)
{

    ui->setupUi(this);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    QSettings set;
    ui->lightXSlider->setValue(set.value("xSurfLightParam", 67).toInt());
    ui->lightZSlider->setValue(set.value("zSurfLightParam", 71).toInt());
    ui->lightYSlider->setValue(set.value("ySurfLightParam", 99).toInt());
    ui->ambientSlider->setValue(set.value("ambientLightParam", 71).toInt());
    ui->yScaleSlider->setValue(set.value("yMaxNdx", 8).toInt());
    ui->yScaleValue->setText(QString().number(maxHeightSelections[ui->yScaleSlider->value()]));
    ui->yshiftAuto->setChecked( set.value("yShiftAuto", 1).toBool());
    QObject::connect(ui->lightXSlider, &QSlider::valueChanged, surfP, &SurfaceGraph::lightX);
    QObject::connect(ui->lightZSlider, &QSlider::valueChanged, surfP, &SurfaceGraph::lightZ);
    QObject::connect(ui->lightYSlider, &QSlider::valueChanged, surfP, &SurfaceGraph::lightY);
    QObject::connect(ui->ambientSlider, &QSlider::valueChanged, surfP, &SurfaceGraph::ambient);
    QObject::connect(ui->yScaleSlider, &QSlider::valueChanged, surfP, &SurfaceGraph::scaleY);
    QObject::connect(ui->yOffsetSlider, &QSlider::valueChanged, surfP, &SurfaceGraph::yOffsetChanged);
    QObject::connect(surfP, &SurfaceGraph::yOffsetValue, this, &Surface3dControlsDlg::setYoffsetValue);
    QObject::connect(ui->yshiftAuto, &QCheckBox::clicked, surfP, &SurfaceGraph::yshiftAutoChanged);
    QObject::connect(ui->range, &QSlider::valueChanged, surfP, &SurfaceGraph::range);
}

Surface3dControlsDlg::~Surface3dControlsDlg()
{
    delete ui;
}

void Surface3dControlsDlg::setYoffsetValue(double value){
    ui->yOffsetValueDisplay->setText(QString().number(value));
}

void Surface3dControlsDlg::on_ambientSlider_valueChanged(int value)
{
    ui->lightAmbientVal->setText(QString().number(value));
}

void Surface3dControlsDlg::on_lightXSlider_valueChanged(int value)
{
    float pos = (value - 50) * .2;
    ui->lightXVal->setText(QString().number(pos));
}

void Surface3dControlsDlg::on_lightZSlider_valueChanged(int value)
{
    float pos = (value - 50) * .2;
    ui->lightZVal->setText(QString().number(pos));
}

void Surface3dControlsDlg::on_lightYSlider_valueChanged(int value)
{
    float pos = (value - 50) * .2;
    ui->lightYVal->setText(QString().number(pos));
}

void Surface3dControlsDlg::on_yScaleSlider_valueChanged(int value)
{
    QSettings set;
    qDebug() << "height" << value;
    set.setValue("yMaxNdx",value);
    ui->yScaleValue->setText(QString().number(maxHeightSelections[value]));
}


void Surface3dControlsDlg::on_yOffsetSlider_valueChanged(int value)
{

}

void Surface3dControlsDlg::on_yshiftAuto_clicked(bool checked)
{

    QSettings set;
    set.setValue("yShiftAuto", checked);
    if (checked){
        ui->yOffsetSlider->hide();
        ui->yOffsetValueDisplay->hide();
    }
    else {
        ui->yOffsetSlider->show();
        ui->yOffsetValueDisplay->show();
    }
}

void Surface3dControlsDlg::on_range_valueChanged(int value)
{

}
