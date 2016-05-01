#include "oglcontrols.h"
#include "ui_oglcontrols.h"
#include <QSettings>
OGLControls::OGLControls(QWidget *parent) :
    QDockWidget(parent),
    ui(new Ui::OGLControls)
{
    ui->setupUi(this);
    enablTools(false);
    QSettings s;
    bool v = s.value("oglFill", true).toBool();
    ui->checkBox->setChecked(v);
}

OGLControls::~OGLControls()
{
    delete ui;
}


void OGLControls::on_doPerspective_clicked()
{

}

void OGLControls::on_checkBox_clicked(bool checked)
{
    QSettings s;
    s.setValue("oglFill", true);
    emit fillGridChanged(checked);
}

void OGLControls::enablTools(bool b){
    QList<QWidget *> widgets = findChildren<QWidget *>();
    foreach(QWidget * widget, widgets)
    {
        widget->setEnabled(b);
    }
}

void OGLControls::on_spinBox_valueChanged(int arg1)
{
    emit resolutionChanged(arg1);
}

void OGLControls::on_surfaceMagnification_valueChanged(int arg1)
{
    emit ogheightMagValue(arg1);
}
