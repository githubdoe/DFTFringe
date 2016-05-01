#ifndef STATSDLG_H
#define STATSDLG_H

#include <QDialog>
#include "qwt_plot.h"
namespace Ui {
class StatsDlg;
}

class StatsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit StatsDlg(QWidget *parent = 0);
    ~StatsDlg();
    QwtPlot *m_wavefrontPlot;
    QwtPlot *m_zernPlot;
    bool doAstig;
    bool doSA;
    bool doComa;
    bool saveCsv;
    bool savePDF;
    bool saveJpg;
    bool doInputs;
    bool doZernGroup;
    int zernFrom;
    int zernTo;
    bool showFileNames;
    bool removeOutliers;
    QColor m_background;
    bool removeAboveLimit;
    double RMSLimitValue;

private slots:
    void on_buttonBox_accepted();

    void on_bakcgroundpb_clicked();

private:
    Ui::StatsDlg *ui;
};

#endif // STATSDLG_H
