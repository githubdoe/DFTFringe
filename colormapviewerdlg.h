#ifndef COLORMAPVIEWERDLG_H
#define COLORMAPVIEWERDLG_H

#include <QDialog>

#include <QListWidgetItem>
namespace Ui {
class colorMapViewerDlg;
}

class colorMapViewerDlg : public QDialog
{
    Q_OBJECT

public:
    explicit colorMapViewerDlg( QWidget *parent = 0);
    ~colorMapViewerDlg();
    QString m_selection;

void genList(QString path);
private slots:
    void on_browsePb_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_buttonBox_accepted();


    void on_aboutpb_clicked();

private:
    Ui::colorMapViewerDlg *ui;
};

#endif // COLORMAPVIEWERDLG_H
