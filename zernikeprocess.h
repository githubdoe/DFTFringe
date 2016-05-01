#ifndef ZERNIKEPROCESS_H
#define ZERNIKEPROCESS_H

#include <QObject>
#include "wavefront.h"
#include "zernikedlg.h"
#include "mirrordlg.h"
#include "mainwindow.h"
extern std::vector<double> zNulls;
extern std::vector<bool> zernEnables;
extern int Zw[];
extern double BestSC;
double Zernike(int n, double x, double y);
void gauss_jordan(int n, double* Am, double* Bm);
void ZernikeSmooth(Mat wf, Mat mask);
class zernikeProcess : public QObject
{
    Q_OBJECT
private:
    static zernikeProcess *m_Instance;
public:
    explicit zernikeProcess(QObject *parent = 0);
    static zernikeProcess *get_Instance();
    double unwrap_to_zernikes(wavefront &wf);
    cv::Mat null_unwrapped(wavefront&wf,  std::vector<double> zerns, std::vector<bool> enables,int start_term =0, int last_term = Z_TERMS);
    //double Wavefront(double x1, double y1, int Order);
    void unwrap_to_zernikes(zern_generator *zg, cv::Mat wf, cv::Mat mask);
    cv::Mat Z;
    cv::Mat inputZ;
    bool m_dirty_zerns;
    mirrorDlg *md;
    MainWindow *mw;

signals:

public slots:

};

#endif // ZERNIKEPROCESS_H
