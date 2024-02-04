#ifndef ZERNIKESMOOTHINGDLG_H
#define ZERNIKESMOOTHINGDLG_H

#include <QDialog>
#include "zernikeprocess.h"
#include "wavefront.h"
#include "simigramdlg.h"
namespace Ui {
class ZernikeSmoothingDlg;
}

class ZernikeSmoothingDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ZernikeSmoothingDlg(wavefront &wf, QWidget *parent = nullptr);
    ~ZernikeSmoothingDlg();

private slots:
    void on_maxOrder_valueChanged(int arg1);
    void on_createWaveFront_clicked();
    void intiZernTable();
private:
    Ui::ZernikeSmoothingDlg *ui;
    int m_maxOrder;
    int m_noOfTerms;
    zernikeProcess *m_zp;
    zTableModel * tableModel;
    SurfaceManager *m_sm;
    bool shouldEnableAll;
    std::vector<bool> m_zernEnables;
    std::vector<double> zerns;
    double m_xCen;
    double m_yCen;
    double m_rad;
    int m_originalSize;
    std::vector<double> theZerns;

    QTimer m_timer;
    wavefront *p_wf;
    wavefront m_wf;
};

#endif // ZERNIKESMOOTHINGDLG_H
