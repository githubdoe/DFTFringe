#include "psiphasedisplay.h"
#include "ui_psiphasedisplay.h"
#include "math.h"
#include <opencv2/highgui/highgui.hpp>
#include <qdebug.h>
#include <qimage.h>
#include <qpainter.h>
#include <QScreen>
#include <QDebug>
#define M_PI 3.14159265358979323846
PSIphaseDisplay::PSIphaseDisplay(QVector<double> phases,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PSIphaseDisplay),m_useRadians(false)
{
    ui->setupUi(this);
    qDebug() << "plot phases" << phases;
    setGeometry(10,10,500,500);
    plot(phases, 0, 1.);

}

PSIphaseDisplay::~PSIphaseDisplay()
{
    delete ui;
}
void PSIphaseDisplay::useRadians(bool use){

}
void PSIphaseDisplay::setPhases(QVector<double> phases, int iteration, double sdp){
    plot(phases, iteration, sdp);
}
void PSIphaseDisplay::plot(QVector<double> phases, int iteration, double sdp){

    QList<QScreen *>scrs = QApplication::screens();
    int width = scrs[0]->size().height() * .8;
    int height = width;
    QImage plot1(width, height,QImage::Format_RGB888 );
    QPainter p(&plot1);
    int half = width/2;
    int r = 100;
    int rdel = (half - 150)/phases.length();
    int rlast = r;
    p.fillRect(0,0,width,height, QColor(211,245,241));
    p.setPen(QPen(QColor(200,200,200)));
    p.drawEllipse(half,half, 5,5);
    p.drawLine(half,0, half,height);
    p.drawLine(0,half,width,half);
    double k = 180./M_PI;
    for (int i = 0; i < phases.length()-1; ++i){
        double angle = phases[i];
        double angle2 = phases[i+1];

        int x1 = half - rlast * cos(angle + M_PI/2.);
        int x2 = half - r * cos(angle2 + M_PI/2.);
        int y1 = half - rlast * sin(angle + M_PI/2.);
        int y2 = half - r * sin(angle2+ M_PI/2.);
       // qDebug() << "angle1" << angle * k << angle2 * k;
        while (angle < 0)
            angle = (2 * M_PI) + angle;
        while (angle2 < 0)
            angle2 = (2 * M_PI) + angle2;
        double delta = angle2 - angle;
        if ( delta < 0)
            delta = 2 * M_PI + delta;
        p.setBrush(Qt::white);
        p.drawEllipse(x2-25,y2-25,50,50);
        p.setPen(QPen(QColor(100,100,100),2));
        p.drawLine(x1,y1,x2,y2);
        p.setPen(QPen(QColor(0,0,255)));
        QFont font=p.font() ;
        font.setPointSize ( 12 );
        font.setWeight(QFont::DemiBold);
        p.setFont(font);
        p.drawText( (x1+x2)/2, (y1+y2)/2,QString().sprintf("% 6.2lf", delta *  k).toStdString().c_str());
        p.setPen(QPen(QColor(100,100,100)));
        font.setPointSize(10);
        p.setFont(font);
        p.drawText( x2-10,y2+10,QString().sprintf("%d", i+1).toStdString().c_str());
        p.drawText(x2 + 40, y2 ,QString().sprintf("%6.2lf", angle2 * k).toStdString().c_str());
        rlast = r;
        r += rdel;
    }
    QFont font=p.font() ;
    font.setPointSize ( 15 );
    p.setFont(font);
    p.drawText(30,100,QString().sprintf("iteration %i sdp: %lf", iteration, sdp).toStdString().c_str());
    p.setPen(QPen(QBrush(QColor(0,0,255)),5));

    p.drawLine(50,height -120, 90, height -120);
    p.drawText(100, height - 100, "difference from one phase to next");
    ui->display->setPixmap(QPixmap::fromImage(plot1));//.scaled(labelSize, Qt::KeepAspectRatio, Qt::FastTransformation)));
    update();

}
