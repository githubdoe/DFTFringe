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
    Ui::zernikeEditDlg *ui;
    SurfaceManager *m_sm;
    ZernTableModel * tableModel;
    bool shouldEnableAll;
    std::vector<bool> m_zernEnables;
    std::vector<double> zerns;
};

#endif // ZERNIKEEDITDLG_H
