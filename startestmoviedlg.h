#ifndef STARTESTMOVIEDLG_H
#define STARTESTMOVIEDLG_H

#include <QDialog>
#include <QImage>
class SimulationsView;
namespace Ui {
class startestMovieDlg;
}

class startestMovieDlg : public QDialog
{
    Q_OBJECT

public:
    explicit startestMovieDlg(SimulationsView *view);
    ~startestMovieDlg();
    void setImage(QImage image);
    bool m_recording = false;
    int m_frameNumber = 0;
    int m_bright = 1;
    SimulationsView *m_view;
    QImage m_image;

private slots:
    void on_start_clicked();

    void on_browse_clicked();

    void on_abort_clicked();

    void on_brightnesssb_valueChanged(int value);

    void on_wavestart_valueChanged(double arg1);

    void on_waveEnd_valueChanged(double arg1);

    void on_stepsize_valueChanged(double arg1);

private:
    Ui::startestMovieDlg *ui;

signals:
    void makeStarTest(double wave, startestMovieDlg * dlg);
};

#endif // STARTESTMOVIEDLG_H
