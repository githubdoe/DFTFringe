#include "dftthumb.h"
#include "ui_dftthumb.h"
#include <QDebug>
dftThumb::dftThumb(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dftThumb)
{
    ui->setupUi(this);
    ui->label->setScaledContents( true );
    //ui->label->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
}

void dftThumb::setImage(QImage img){
    m_img = img.scaled(300, 300, Qt::KeepAspectRatio);;

    ui->label->setPixmap(QPixmap::fromImage(m_img));//.scaled(labelSize, Qt::KeepAspectRatio)));

}

dftThumb::~dftThumb()
{
    delete ui;
}
