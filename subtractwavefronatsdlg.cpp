#include "subtractwavefronatsdlg.h"
#include "ui_subtractwavefronatsdlg.h"

subtractWavefronatsDlg::subtractWavefronatsDlg(QList<QString> list, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::subtractWavefronatsDlg)
{
    ui->setupUi(this);
    ui->listWidget->addItems(list);
}

subtractWavefronatsDlg::~subtractWavefronatsDlg()
{
    delete ui;
}
int subtractWavefronatsDlg::getSelected(){
    QList<QListWidgetItem *> l = ui->listWidget->selectedItems();
    if (l.size() > 0)
        return ui->listWidget->row(l[0]);
    return -1;

}
