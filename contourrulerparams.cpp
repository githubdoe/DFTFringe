#include "contourrulerparams.h"
#include "ui_contourrulerparams.h"
#include <QSettings>
#include <QColorDialog>
static inline QString colorButtonStyleSheet(const QColor &bgColor)
{
    if (bgColor.isValid()) {
        QString rc = QLatin1String("border: 2px solid black; border-radius: 2px; background:");
        rc += bgColor.name();
        return rc;
    }
    return QLatin1String("border: 2px dotted black; border-radius: 2px;");
}
ContourRulerParams::ContourRulerParams(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContourRulerParams)
{
    ui->setupUi(this);
    QSettings set;
    ui->setColorPB->setStyleSheet(colorButtonStyleSheet(set.value("contourRulerColor","grey").toString()));
}

ContourRulerParams::~ContourRulerParams()
{
    delete ui;
}

void ContourRulerParams::on_radialAngleSB_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("contourRulerRadialDeg", arg1);
    emit updateParms();
}

void ContourRulerParams::on_setColorPB_clicked()
{

    QColor color = QColorDialog::getColor( ui->setColorPB->palette().color(QPalette::Background));

    QSettings set;
    set.setValue("contourRulerColor", color.name());
    ui->setColorPB->setStyleSheet(colorButtonStyleSheet(color));

    emit updateParms();
}
