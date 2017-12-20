/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "oglview.h"
#include <QtCore>
#include <QLayout>
#include <QColorDialog>
#include <QShortcut>
#include <QMenu>
#include <QFileDialog>
#include <QSettings>
#include <QSpinBox>
#include "videosetupdlg.h"
#include <QMessageBox>
OGLView::OGLView(QWidget *parent, ContourTools *m_tool,
                 surfaceAnalysisTools *surfTools) :
    QWidget(parent),zoomed(false), m_spinRate(5)
{
    QSettings s;
    m_gl = new GLWidget(this, m_tool, surfTools);
    QHBoxLayout *lh = new QHBoxLayout();
    QVBoxLayout *lv = new QVBoxLayout();
    lv->addLayout(lh);

    fillCB = new QCheckBox("Fill",this);
    lh->addWidget(fillCB);

    bool v = s.value("oglFill", true).toBool();
    fillCB->setChecked(v);
    connect(fillCB, SIGNAL(clicked(bool)), m_gl, SLOT(fillGridChanged(bool)));

    lightingPb = new QPushButton("Lighting",this);
    lh->addWidget(lightingPb);
    connect(lightingPb, SIGNAL(clicked()), m_gl, SLOT(openLightingDlg()));

    QPushButton *showAllPb = new QPushButton("Show All");
    QCheckBox *animateCb = new QCheckBox("Spin");
    QSpinBox *spinBox = new QSpinBox();
    connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(spinRate(int)));
    QPushButton *saveImage = new QPushButton("SaveImage");
    QPushButton *saveVideo = new QPushButton("saveVideo");
    connect(saveImage, SIGNAL(pressed()), this, SLOT(saveImage()));
    connect(animateCb, SIGNAL(clicked(bool)), this, SLOT(animate(bool)));
    connect(saveVideo, SIGNAL(pressed()), this, SLOT(saveVideo()));
    backgroundPb = new QPushButton("Background");
    QColor backg = QColor(s.value("oglBackground", "black").toString());
    backgroundPb->setStyleSheet("  background-color: " + backg.name() +";  border-width: 2px;"
                                "border-color: black;");
    connect(backgroundPb, SIGNAL(clicked(bool)), this, SLOT(setBackground()));
    connect(showAllPb, SIGNAL(clicked()), this, SLOT(showAll()));
    QLabel *lb1 = new QLabel("Vertical Scale:",this);
    lb1->setMaximumHeight(10);
    lh->addWidget(lb1);

    vscale = new QSpinBox(this);
    vscale->setSingleStep(10);
    vscale->setMaximum(1000);
    vscale->setValue(200);
    connect(vscale, SIGNAL(valueChanged(int)), m_gl, SLOT(ogheightMagValue(int)));
    lh->addWidget(vscale);

    QLabel *lb2 = new QLabel("BackWall Scale:");
    lh->addWidget(lb2);
    backWallScaleSB = new QDoubleSpinBox(this);
    backWallScaleSB->setDecimals(3);
    backWallScaleSB->setValue(.125);
    backWallScaleSB->setMinimum(.001);
    backWallScaleSB->setSingleStep(.005);

    lh->addWidget(backWallScaleSB);
    connect(backWallScaleSB, SIGNAL(valueChanged(double)), m_gl, SLOT(backWallScale(double)));
    QLabel *lb3 = new QLabel("Waves",this);
    lb3->setMaximumHeight(10);
    lh->addWidget(lb3);
    lh->addWidget(backgroundPb);
    lh->addWidget(showAllPb);
    lh->addWidget(animateCb);
    lh->addWidget(spinBox);
    lh->addWidget(saveImage);
    lh->addWidget(saveVideo);

    lh->addStretch();
    lv->addWidget(m_gl);
    setLayout(lv);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showContextMenu(QPoint)));
    setMinimumWidth(50);
}
QSize OGLView::sizeHint() const{
    return QSize(300,300);
}
void OGLView::animate(bool f){
    if (!f) m_gl->aniTimer.stop();
    else m_gl->aniTimer.start(100);
}
void OGLView::spinRate(int v){
    m_gl->m_spinRate = v+5;
}

void OGLView::zoom(){
    zoomed = !zoomed;
    emit zoomMe(zoomed);
}
#include "opencv2/opencv.hpp"
using namespace cv;
void OGLView::saveVideo(){
    QSettings set;
    QString lastPath = set.value("lastPath",".").toString();
    QImage img = m_gl->grabFrameBuffer();
    videoSetupDlg dlg(img);
    if (dlg.exec()){
        int width = dlg.width();
        int height = dlg.height();
        qDebug() << "vidoe " << width << height;
        try {
            QString fileName = QFileDialog::getSaveFileName(0, "Save AVI video as:", lastPath,"*.avi" );
            if (fileName.length() > 0){
                if (!(fileName.toUpper().endsWith(".AVI")))
                    fileName.append(".avi");
                VideoWriter video(fileName.toStdString().c_str(),-1,dlg.fps(),cv::Size(width,height),true);
                if (!video.isOpened()){
                    QString msg = QString().sprintf("could not open %s %dx%d for writing.", fileName.toStdString().c_str(),
                                                     width, height);
                    QMessageBox::warning(0,"warning", msg);
                    return;
                }
                for (float angle = 0; angle < 360; angle += dlg.angleStep()){
                    m_gl->setYRotation(angle * 16);
                    QImage img = m_gl->grabFrameBuffer();
                    cv::Mat frame = cv::Mat(img.height(), img.width(),CV_8UC4, img.bits(), img.bytesPerLine()).clone();
                    cv::Mat resized;
                    cv::resize(frame, resized, cv::Size(width,height));
                    video.write(resized);
                }
            }
        }
        catch(std::exception& e) {
            qDebug() <<  "Exception writing video " << e.what();
        }

    }
}

void OGLView::saveImage(){
    QSettings set;
    QString lastPath = set.value("lastPath",".").toString();
    QString fileName = QFileDialog::getSaveFileName(0, "Image file Name", lastPath,"*.jpg" );
    if (fileName.length() > 0){
        //m_gl->swapBuffers();
        QImage glImage = m_gl->grabFrameBuffer();
        glImage.save(fileName);

    }
}

void OGLView::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    QString txt = (zoomed)? "Restore to MainWindow" : "FullScreen";
    myMenu.addAction(txt,  this, SLOT(zoom()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}
void OGLView::setBackground(){
    QColorDialog dlg(m_gl->m_background,this);
    QColor c = dlg.getColor();
    m_gl->setBackground(c);
    backgroundPb->setStyleSheet("  background-color: " + c.name() +";  border-width: 2px;"
                                "border-color: black;");
}

void OGLView::showAll(){
    emit showAll3d( m_gl);

}
