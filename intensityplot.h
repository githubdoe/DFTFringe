#ifndef INTENSITYPLOT_H
#define INTENSITYPLOT_H
#include <QWidget>
#include <qwt_plot.h>
#include <opencv/cv.h>

class intensityPlot:  public QwtPlot
{
Q_OBJECT
public:
    intensityPlot(QWidget *parent = NULL);
    virtual ~intensityPlot() {}
    void setSurface(QImage img);
    virtual void resizeEvent( QResizeEvent * );
    void showRed(bool);
    void showBlue(bool);
    void showGreen(bool);
    double m_waveRange;
    virtual bool eventFilter( QObject *, QEvent * );
    QImage m_img;
    cv::Mat planes[4];
public slots:
    void angleChanged(double a);


private:
    void populate();
    void updateGradient();
    QPoint startPos;
    double m_angle;
    bool m_showRed;
    bool m_showGreen;
    bool m_showBlue;

};




#endif // INTENSITYPLOT_H
