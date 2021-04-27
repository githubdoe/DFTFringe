#include "showallcontoursdlg.h"
#include "ui_showallcontoursdlg.h"
#include <QSettings>
#include <QDesktopWidget>
showAllContoursDlg::showAllContoursDlg( QWidget *parent) :
    QDialog(parent),
    ui(new Ui::showAllContoursDlg)
{
    QSettings set;
    ui->setupUi(this);
    QRect rec = QApplication::desktop()->screenGeometry();
    ui->Width->setValue(rec.width());

    ui->columns->setValue(set.value("showContours cols", 4).toInt());
}

showAllContoursDlg::~showAllContoursDlg()
{
    delete ui;
}

int showAllContoursDlg::getColumns(){

    return ui->columns->value();
}
int showAllContoursDlg::getWidth(){
    return ui->Width->value();
}

void showAllContoursDlg::on_Width_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("showContours width", arg1 );
}

void showAllContoursDlg::on_columns_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("showContours cols", arg1 );
}
