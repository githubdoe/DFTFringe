#ifndef ZERNIKEEDITDLG_H
#define ZERNIKEEDITDLG_H

#include <QDialog>
#include "zernikedlg.h"
#include "surfacemanager.h"
namespace Ui {
class zernikeEditDlg;
}

class zernikeEditDlg : public QDialog
{
    Q_OBJECT

public:
    explicit zernikeEditDlg(SurfaceManager *sfm ,QWidget *parent = 0);
    ~zernikeEditDlg();

private slots:
    void on_disable_clicked();

    void on_createSurface_clicked();

    void on_clearAll_clicked();

    void on_read_clicked();

    void on_save_clicked();

    void on_useCurrent_clicked();

private:
    SurfaceManager *m_sm;
    Ui::zernikeEditDlg *ui;
    ZernTableModel * tableModel;

};

#endif // ZERNIKEEDITDLG_H
