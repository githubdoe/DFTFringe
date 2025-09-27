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
#include <QList>
#include <QImageWriter>
#include "videosetupdlg.h"
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QSurfaceDataProxy>
#include <QtDataVisualization/QHeightMapSurfaceDataProxy>
#include <QtDataVisualization/QSurface3DSeries>
#include "surfacegraph.h"
#include <QMessageBox>
#include <QGroupBox>
#include <QFileDialog>
#include "surfacemanager.h"

OGLView::OGLView(QWidget *parent, ContourTools *m_tool) :
    QWidget(parent), m_spinRate(5)
{

    QSettings s;
    Q3DSurface *graph = new Q3DSurface();
    m_surface = new SurfaceGraph(graph);
    m_container = QWidget::createWindowContainer(graph);

    if (!graph->hasContext()) {
        QMessageBox msgBox;
        msgBox.setText("Couldn't initialize the OpenGL context.");
        msgBox.exec();
        return;
    }
    m_container->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_container->setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_controls = new Surface3dControlsDlg(m_surface);

    QHBoxLayout *topH = new QHBoxLayout();
    QHBoxLayout *lh = new QHBoxLayout();
    QVBoxLayout *lv = new QVBoxLayout();

    lv->addLayout(lh);

    QVBoxLayout *rightcontrols = new QVBoxLayout();

    QLabel *colorLedgend = new QLabel(this);

    m_surface->setLegend(colorLedgend);

    QGroupBox *heightMapGroupBox = new QGroupBox(QStringLiteral("Waves 550nm"));
    //heightMapGroupBox->setStyleSheet({"background: grey;"});
    QVBoxLayout *colorMapVBox = new QVBoxLayout;
    colorMapVBox->addWidget(colorLedgend);
    heightMapGroupBox->setLayout(colorMapVBox);


    rightcontrols->addWidget(heightMapGroupBox);
    lightingPb = new QPushButton("3D Controls",this);
    QPushButton *saveImagePb = new QPushButton("save Image");
    QPushButton *showSelectedPb = new QPushButton("show selected");
    m_fullScreenPb = new QPushButton("FullScreen");
    lh->addWidget(lightingPb);
    lh->addWidget(saveImagePb);
    lh->addWidget(showSelectedPb);
    lh->addWidget(m_fullScreenPb);
    connect(lightingPb, &QAbstractButton::pressed, this,&OGLView::openLightDlg);
    connect(saveImagePb, &QAbstractButton::pressed, this, &OGLView::saveImage);
    connect(showSelectedPb, &QAbstractButton::pressed, this, &OGLView::showSelected);
    connect(m_tool,&ContourTools::ContourMapColorChanged, m_surface, &SurfaceGraph::setColorMap);
    connect(m_fullScreenPb, &QAbstractButton::pressed, this, &OGLView::fullScreenPressed);
    lh->addStretch();
    lv->addWidget(m_container);
    topH->addLayout(lv,10);
    topH->addLayout(rightcontrols,1);
    setLayout(topH);
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QWidget::customContextMenuRequested, this,
            &OGLView::showContextMenu);
}
OGLView::~OGLView(){
    m_controls->close();
    delete m_controls;
    delete m_surface;
}

void OGLView::showContextMenu(QPoint pos)
{
    // Handle global position
    QPoint globalPos = mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;

    myMenu.addAction("FullScreen",  this, &OGLView::fullScreenPressed);

    // Show context menu at handling position
    myMenu.exec(globalPos);
}
void OGLView::fullScreenPressed(){
    m_fullScreenPb->setEnabled(false);
    emit fullScreen();
}
void OGLView::enableFullScreen(){
    m_fullScreenPb->setEnabled(true);
}
void OGLView::openLightDlg(){
    m_controls->show();
}
QSize OGLView::sizeHint() const{
    return QSize(300,300);

}

void OGLView::saveImage(){
    QSettings set;
    QString path =     set.value("lastPath","").toString();

    const QList<QByteArray> imageFormats = QImageWriter::supportedImageFormats();
    QStringList filter;
    if ( imageFormats.size() > 0 )
    {
        QString imageFilter( tr( "Images" ) );
        imageFilter += " (";
        for ( int i = 0; i < imageFormats.size(); i++ )
        {
            if ( i > 0 )
                imageFilter += " ";
            imageFilter += "*.";
            imageFilter += imageFormats[i];
        }
        imageFilter += ")";

        filter += imageFilter;
    }
    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save image"), path + "//surface3D.jpg",filter.join( ";;" ));


    if (fName.isEmpty())
        return;

    QImage img = m_surface->render(1000,1000);
    img.save(fName);
}
#include <QApplication>
#include "wavefront.h"
void OGLView::showSelected()    // show all selected wavefronts as 3D plots
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    QRect rec = QGuiApplication::primaryScreen()->geometry();
    int width = rec.width()/3;
    int height = rec.height()/2;
    QVector<wavefront *>  m_wavefronts =SurfaceManager::get_instance()->m_wavefronts;
    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    QList<int> list = saTools->SelectedWaveFronts();
    int cols = 3;
    if (list.size() %3 == 0)  cols = 3;
    int rows =  ceil((double)list.size()/cols);
    int columns = cols;//min(m_wavefronts.size(),int(ceil((double)m_wavefronts.size()/rows)));
    const QSizeF size(columns * (width + 10), rows * (height + 10));
    const QRect imageRect = QRect(0,0,size.width(),size.height());

    QImage m_allContours = QImage( imageRect.size(), QImage::Format_ARGB32 );
    m_allContours.fill( QColor( Qt::gray ).rgb() );
    QPainter painter(&m_allContours);
    QFont serifFont("Times", 18, QFont::Bold);


    for (int i = 0; i < list.size(); ++i)
    {
        wavefront * wf = m_wavefronts[list[i]];
        m_surface->setSurface(wf);
        QImage glImage = m_surface->render(width,width);
        QPainter p2(&glImage);
        p2.setFont(serifFont);
        p2.setPen(QPen(QColor(Qt::white)));
        QStringList l = wf->name.split("/");
        p2.drawText(10,40,l[l.size()-1] + QString("%1 RMS").arg(wf->std, 6, 'f', 3));
        int y_offset =  height * (i/columns) + 40;
        int x_offset = width * (i%columns) + 20;
        painter.drawImage(x_offset,y_offset, glImage.scaled(width, height,Qt::KeepAspectRatio));
    }

    //image.save( "tmp.png" );
    QWidget *w = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout;

    QScrollArea *scrollArea = new QScrollArea;

    QLabel *l = new QLabel();
    l->setPixmap(QPixmap::fromImage(m_allContours));
    l->setScaledContents( true );

    l->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );

    scrollArea->setWidget(l);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setAutoFillBackground(true);
    QPushButton *savePb = new QPushButton("Save as Image",w);
    SurfaceManager *sm = SurfaceManager::get_instance();
    connect(savePb, &QPushButton::pressed, sm, &SurfaceManager::saveAllContours);
    layout->addWidget(savePb,0,Qt::AlignHCenter);
    layout->addWidget(scrollArea);
    w->setLayout(layout);
    w->setWindowTitle("3D height map of All WaveFronts.");

    height = 2 * rec.height()/3;
    width = rec.width();
    w->resize(width,height);
    SurfaceManager::get_instance()->m_allContours = m_allContours;
    w->show();
    QApplication::restoreOverrideCursor();
}
