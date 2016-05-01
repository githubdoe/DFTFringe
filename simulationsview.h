#ifndef SIMULATIONSVIEW_H
#define SIMULATIONSVIEW_H

#include <QWidget>
#include "wavefront.h"
#include <opencv/cv.h>
namespace Ui {
class SimulationsView;
}

class SimulationsView : public QWidget
{
    Q_OBJECT

public:
    explicit SimulationsView(QWidget *parent = 0);
    ~SimulationsView();
    static SimulationsView *getInstance(QWidget *parent);
    void setSurface(wavefront *wf);
    cv::Mat  computeStarTest(cv::Mat surface, int pupil_size, double pad);
    void computeMPF();
    void compute();
    bool needs_drawing;
    cv::Mat nulledSurface(double defocus);
private:
    bool alias;

    void mtf(cv::Mat star, QString txt, QColor color);
public slots:
        void on_MakePB_clicked();
private slots:

    void on_defocusSB_valueChanged(double);

    void on_gammaSB_valueChanged(double);

    void on_centerMagnifySB_valueChanged(double);

    void on_FFTSizeSB_valueChanged(int val);


private:
    Ui::SimulationsView *ui;
    static SimulationsView* m_Instance;
    bool m_Computed;
    wavefront *m_wf;
};
// class to save value on construction and then restore old value on destruction
template<class T> class save_restore
{
    T* m_pVal;
    T m_Val;
public:
    save_restore(T* pval, T new_val) : m_pVal(pval), m_Val(*pval)
    {
        *m_pVal = new_val;
    };
    save_restore(T* pval) :m_pVal(pval),m_Val(*pval){};
    void setVal(T val) {m_Val = val;};

    ~save_restore() { *m_pVal = m_Val;};

};
#endif // SIMULATIONSVIEW_H
