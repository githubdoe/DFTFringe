#ifndef AVERAGEWAVEFRONTFILESDLG_H
#define AVERAGEWAVEFRONTFILESDLG_H

#include <QDialog>
#include <QStringList>
#include <QAbstractButton>
class wavefront;
class SurfaceManager;
namespace Ui {
class averageWaveFrontFilesDlg;
}

class averageWaveFrontFilesDlg : public QDialog
{
    Q_OBJECT
    SurfaceManager *sm;

public:
    explicit averageWaveFrontFilesDlg(const QStringList &list, SurfaceManager *sm, QWidget *parent = 0);
    ~averageWaveFrontFilesDlg();
    wavefront *average;
signals:
    void averageComplete(wavefront *);
private slots:

    void on_MoveUp_clicked();

    void on_process_clicked();

    void on_addFiles_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

    void on_moveDown_clicked();

    void on_deleteFiles_clicked();

    void on_filterCB_clicked(bool checked);

private:
    Ui::averageWaveFrontFilesDlg *ui;
    bool abort;
    double filterRMS;
    bool useFilter;
};

#endif // AVERAGEWAVEFRONTFILESDLG_H
