#include "regionedittools.h"
#include "ui_regionedittools.h"

regionEditTools::regionEditTools(QWidget *parent) :
    QDockWidget(parent),
    m_doLine(false), m_doFreeform(true), 
    ui(new Ui::regionEditTools)
{
    ui->setupUi(this);
}

regionEditTools::~regionEditTools()
{
    delete ui;
}


void regionEditTools::on_delete_2_clicked()
{
    QList<QListWidgetItem*> items = ui->listWidget->selectedItems();
    foreach(QListWidgetItem * item, items){
        emit deleteregion(ui->listWidget->row(item));
    }
}
void regionEditTools::deleteRegion(int r){
    delete ui->listWidget->takeItem(r);
}

void regionEditTools::on_freeform_clicked(bool /*checked*/)
{
    m_doFreeform = true;
    m_doLine = false;
}

void regionEditTools::on_line_clicked(bool /*checked*/)
{
    m_doLine = true;
    m_doFreeform = false;
}

void regionEditTools::addRegion(const QString &text){
    ui->listWidget->addItem(text);
}

void regionEditTools::on_listWidget_clicked(const QModelIndex &index)
{
    currentNdx = index.row();
    emit selectRegion(currentNdx);
}
void regionEditTools::clear(){
    ui->listWidget->clear();
}
