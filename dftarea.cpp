/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "dftarea.h"
#include "ui_dftarea.h"
#include "dfttools.h"
#include "vortex.h"
#include <queue>
#include "punwrap.h"
#include "zernikeprocess.h"
#include "settings2.h"
#include "myutils.h"
#include <opencv2/core/core_c.h>
#include <opencv2/imgproc/types_c.h>
#include "opencv2/imgproc/imgproc.hpp"
#include "utils.h"
#include "showaliasdlg.h"
#include <QLabel>
#include "mikespsiinterface.h"
#include <QShortcut>
using namespace cv;


cv::Mat  makeMask(CircleOutline outside, CircleOutline center, cv::Mat data,
                  QVector<std::vector<cv::Point> > poly){
    int width = data.cols;
    int height = data.rows;
    double radm = ceil(outside.m_radius) + 1;
    double rady = radm;
    mirrorDlg &md = *mirrorDlg::get_Instance();
    if (md.isEllipse())
        rady = radm * md.m_verticalAxis/md.diameter;
    double rado = center.m_radius;
    double cx = outside.m_center.x();
    double cy = outside.m_center.y();
    cv::Mat mask = cv::Mat::zeros(height,width,CV_8UC1);

    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            double dx = (double)(x - cx)/(radm);
            double dy = (double)(y - cy)/(rady);
            if (sqrt(dx * dx + dy * dy) <= 1.)
                mask.at<uchar>(y,x) = 255;
        }
    }
    cx = center.m_center.x();
    cy = center.m_center.y();
    if (rado > 0) {
        for (int y = 0; y < height; ++y){
            for (int x = 0; x < width; ++x){
                double dx = (double)(x - (cx))/(rado);
                double dy = (double)(y - (cy))/(rado);
                if (sqrt(dx * dx + dy * dy) < 1.)
                    mask.at<uchar>(y,x) = 0;
            }
        }
    }
    if (poly.size()>0){
        for (int n = 0; n < poly.size(); ++n){
            cv::Point points[1][poly[n].size()];
            for (int i = 0; i < poly[n].size(); ++i){

                points[0][i] = cv::Point(poly[n][i].x, poly[n][i].y);

            }
            for (int j = 0; j < poly[n].size()-1; ++j){
                cv::line(mask, points[0][j], points[0][j+1], cv::Scalar(0));

            }
            const Point* ppt[1] = { points[0]};
            int npt[] = { poly[n].size() };

            fillPoly( mask, ppt, npt, 1, Scalar(0), 8 );
        }
    }

    if (Settings2::showMask())
        showData("DFT mask",mask);
    return mask;
}

DFTArea *DFTArea::m_Instance = 0;
DFTArea * DFTArea::get_Instance(QWidget *parent, IgramArea *ip, DFTTools *tools, vortexDebug *vdbug){
    if  (m_Instance == NULL){
        m_Instance = new DFTArea(parent, ip,tools,vdbug);
    }
    return m_Instance;
}

DFTArea::DFTArea(QWidget *mparent, IgramArea *ip, DFTTools * tools, vortexDebug *vdbug) :
    QWidget(mparent),m_size(640), tools(tools),
    dftSizeStr("640 X 640"), m_center_filter(10.),ui(new Ui::DFTArea),igramArea(ip),m_smooth(9.),
    m_vortexDebugTool(vdbug),zoom(1.)

{
    m_outlineComplete = false;
    m_PSIstate = 0;
    QRect rec = QGuiApplication::screens()[0]->geometry();

    m_Psidlg = new PSI_dlg;
    rec.setLeft(rec.width()/6);
    rec.setTop(rec.height()/4);
    rec.setWidth(rec.width()/4);
    rec.setHeight(rec.height()/4);
    m_Psidlg->setGeometry(rec);


    connect(m_Psidlg, SIGNAL(computePhase()),this, SLOT(computePhases()));
    ui->setupUi(this);
    m_gamma = 2.5;

    connect(tools,SIGNAL(dftChannel(const QString&)), this, SLOT(setChannel(const QString&)));
    connect(tools,SIGNAL(dftSizeVal(int)), this, SLOT(dftSizeVal(int)));
    connect(tools,SIGNAL(dftCenterFilter(double)), this, SLOT(dftCenterFilter(double)));
    connect(tools,SIGNAL(makeSurface()), this,SLOT(makeSurface()));
    connect(tools,SIGNAL(doDFT()), this,SLOT(doDFT()));
    connect(tools,SIGNAL(showResized()),this, SLOT(showResizedDlg()));

    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Plus), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(zoomPlus()));
    shortcut = new QShortcut(QKeySequence(Qt::Key_Minus), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(zoomMinus()));
    shortcut = new QShortcut(QKeySequence(Qt::Key_F), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(zoomFit()));

    tools->connectTo(this);
    capture = false;
    QSettings set;
    m_center_filter = set.value("DFT Center Filter", 10).toDouble();
    qDebug() << "init center" << m_center_filter;
    emit updateFilterSize(m_center_filter);
    installEventFilter(this);

    /*
    QwtPlot *test = new QwtPlot();

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(test);
    QPolygonF points;
    mirrorDlg &md = *mirrorDlg::get_Instance();
    double roc = md.roc;
    double diam = md.diameter;
    double r3 = roc * roc * roc;
    double d4 = diam * diam * diam * diam;


    for (double i = 0; i < .03; i += .0001){
    double z1 = -i * 384. * r3 * md.lambda * 1.E-6/(d4);

        points << QPointF( -i,  z1 );

    }
    test->setAxisTitle( test->xBottom, "Z8" );
    test->setAxisTitle( test->yLeft, "Best conic" );
    QwtPlotCurve *c3 = new QwtPlotCurve("Best fit Conic");
    c3->setSamples(points);
    c3->attach(test);
    test->resize(800,800);
    test->show();
    */

}
#include <QWheelEvent>
void DFTArea::zoomPlus(){
    zoom *= 1.1;
    update();
}
void DFTArea::zoomMinus(){
    zoom /= 1.1;
    update();
}
void DFTArea::zoomFit(){
    zoom = double(this->size().height())/magIImage.size().height();
    update();
}
bool DFTArea::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::Wheel) {

      QWheelEvent *w = (QWheelEvent *)event;
      if (w->angleDelta().y() > 0){
          zoom *= 1.1;
      }
      else {
          zoom /= 1.1;
      }
      if (zoom < 1.)
          zoom = 1.;
       //magIImage = magIImage.scaled(magIImage.width() * scale, magIImage.height() * scale);
      update();
      // Your implementation.
      // You can't use QWheelEvent, as Graphicscene works with its own events...
     //handleWheelOnGraphicsScene(static_cast<QGraphicsSceneWheelEvent*> (event));

     // Don't propagate
     return true;
  }

  // Other events should propagate
  return false;
}
DFTArea::~DFTArea()
{
    delete ui;
}
void DFTArea::setChannel(const QString& val){
    channel = val;
}

void DFTArea::dftCenterFilter(double v){

    m_center_filter = v;
    QSettings set;
    set.setValue("DFT Center Filter", v);
    emit updateFilterSize(v);
    update();
}

void DFTArea::dftSizeVal(int val){
    m_size = val;
}

void DFTArea::dftSizeChanged(const QString& val){
    dftSizeStr = val;
    if (val == "same as Igram"){
        int newVal = igramArea->igramGray.width();
        emit setDftSizeVal(newVal);
    }
    if (val == "640 x 640") {
        int newVal = 640;
        emit setDftSizeVal(newVal);
    }
}

cv::Mat DFTArea::grayComplexMatfromImage(QImage &img){
    // create an roi that is a square around the outline.
    double centerX = igramArea->m_outside.m_center.x();
    double centerY = igramArea->m_outside.m_center.y();
    mirrorDlg &md = *mirrorDlg::get_Instance();

    double pixelsPermm =(igramArea->m_outside.m_radius/(md.diameter/2.));
    double reduction = md.aperatureReduction * pixelsPermm;

    double rad = igramArea->m_outside.m_radius - reduction;


    double rady = rad;

    if (md.isEllipse()){
        rady = rady * md.m_verticalAxis/ md.diameter;
    }

    double left = centerX - rad;
    double top = centerY - rad;
    std::vector<Mat > bgr_planes;
    top = max(top,0.);
    left = max(left,0.);
    int width = 2. * (rad);
    int height = width;
    width = min(width, img.width());
    height = min(height, img.height());

    // new center because of crop
    double xCenterShift = centerX - left;
    double yCenterShift = centerY - top;

    cv::Mat iMat(img.height(), img.width(), CV_8UC3, img.bits(), img.bytesPerLine());
    cv::Mat tmp = iMat.clone();
    cv::Mat roi = iMat(cv::Rect((int)left,(int)top,(int)width,(int)height)).clone();

    double centerDx = centerX - igramArea->m_center.m_center.x();
    double centerDy = centerY - igramArea->m_center.m_center.y();

    roi.convertTo(roi,CV_32FC3);
    QSettings set;
    int dftSize = set.value("DFTSize", 640).toInt();
    double scaleFactor = (double)dftSize/roi.cols;

    m_outside = CircleOutline(QPointF(xCenterShift,yCenterShift), rad);
    m_center = CircleOutline(QPointF(xCenterShift - centerDx, yCenterShift - centerDy),
                             igramArea->m_center.m_radius);

    //scaleFactor = 1;
    if (scaleFactor < 1.){

        cv::resize(roi,roi, cv::Size(0,0), scaleFactor, scaleFactor,INTER_AREA);
        double roicx = (roi.cols-1)/2.;
        double roicy = (roi.rows-1)/2.;
        m_outside = CircleOutline(QPointF(roicx,roicy),roicx);
        m_center = CircleOutline(QPointF((roicx - centerDx * scaleFactor), (roicy - centerDy * scaleFactor)),
                                 m_center.m_radius * scaleFactor);
    }
    else {
        scaleFactor = 1.;
    }
    m_poly.clear();
    for (int n = 0; n < igramArea->m_polygons.size(); ++n){
        m_poly.append(std::vector< cv::Point>());
        for (unsigned int i = 0; i < igramArea->m_polygons[n].size(); ++i){
            int x = round((igramArea->m_polygons[n][i].x - left) * scaleFactor);
            int y = round((igramArea->m_polygons[n][i].y - top) * scaleFactor);
            m_poly.back().push_back(cv::Point(x,y));
        }
    }
    // split image into three color planes
    split( roi, bgr_planes );

    cv::Scalar mean;
    mean =  cv::mean(bgr_planes[0]);

    n = 0;


    cv::Mat padded = bgr_planes[0].clone();
    padded = padded - mean[0];


    // disabled adding optomizing DFT boarder.
    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};

    m_mask = makeMask(m_outside,m_center,*planes, m_poly);
    if (Settings2::showMask())
        showData("Mask", m_mask);
    cv::Mat tmpMask;

    planes[0].copyTo(tmpMask,m_mask);    // Convert image to binary
    planes[0] = tmpMask.clone();
    mean =  cv::mean(planes[0],m_mask);

    planes[0] -= mean;

    Mat  complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
    if (scaleFactor == 1.){
        tools->imageSize(QString().sprintf("DFT Size will be %d", width));
    }
    else
        tools->imageSize(QString().sprintf("Image is resized from %d to %d pixels",width, complexI.cols));
    return complexI;
}

//swap quadrants
void shiftDFT(cv::Mat &magI){

    // crop the spectrum, if it has an odd number of rows or columns
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

    // rearrange the quadrants of Fourier image  so that the origin is at the image center
    int cx = magI.cols/2;
    int cy = magI.rows/2;

    Mat  q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat  q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat  q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat  q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

    Mat  tmp;                           // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

void showData(const std::string& txt, cv::Mat mat, bool useLog){

    cv::Mat tmp = mat.clone();
    if (useLog){
        tmp = mat+1;
        log(tmp, tmp);
    }
    cv::namedWindow(txt, WINDOW_NORMAL);
    normalize(tmp, tmp,0,255,CV_MINMAX);
    tmp.convertTo(tmp,CV_8U);
    cvtColor(tmp,tmp, CV_GRAY2RGB);
    cv::imshow(txt, tmp);
    cv::waitKey(1);
}


QImage  showMag(cv::Mat complexI, bool show, const char* title, bool doLog, double gamma){
    // compute the magnitude and switch to logarithmic scale
    // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
    Mat planes[2];
    split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
    Mat  magI = planes[0];
    double mmin;
    double mmax;
    minMaxIdx(magI, &mmin,&mmax);
    magI-= mmin;

    if (doLog)
        log((magI+0.1), magI);

    if (gamma != 0.){
        cv::pow(magI,gamma,magI);
    }
    normalize(magI, magI,0,255,CV_MINMAX, CV_8U);
    minMaxIdx(magI, &mmin,&mmax);

    cv::Mat tmp = magI.clone();
    cv::waitKey(1);
    cvtColor(magI,magI, CV_GRAY2RGB);
    if (show){
        imshow(title, magI);
        waitKey(1);
    }
    return QImage((uchar*)magI.data, magI.cols, magI.rows, magI.step, QImage::Format_RGB888).copy();
}

RNG rng(12345);



void DFTArea::doDFT(){

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QImage img = igramArea->igramGray;

    cv::Mat complexI = grayComplexMatfromImage(img);

    cv::Mat planes[2];

    split(complexI,planes);

    cv::Scalar mean,std;
    cv::meanStdDev(planes[0],mean,std, m_mask);
    //qDebug() << "Mean " << mean[0];
    planes[0] -= mean[0];
    //cv::imshow("input", planes[0].clone());

    //cv::waitKey(1);
    cv::merge(planes,2,complexI);
    dft(complexI, complexI);
    cv::Mat realImage;
    dft(complexI,realImage,DFT_INVERSE);
    split(realImage,planes);

    shiftDFT(complexI);
    m_dft.release();
    m_dft = complexI/complexI.size().area();
    magIImage = showMag(complexI,false,"", true, m_gamma);


    double h = magIImage.height();
    QRect rec = QApplication::desktop()->screenGeometry();



    magIImage = magIImage.scaled(magIImage.width() , magIImage.height() );
    setMinimumSize(magIImage.size());

    emit selectDFTTab();
    update();


    zoom = double(this->size().height())/magIImage.size().height();

    QSettings set;

    if (set.value("showDFTHelp", true).toBool()){
        if (QMessageBox::Yes == QMessageBox::question(0,"Did you know you can use the Scroll wheel to zoom this image?.","Selct yes to not display this message again."))
        {
            set.setValue("showDFTHelp",false);
        }

    }
    //if (Settings2::showDFT())
        //emit dftReady(magIImage);     //Creates a thumbnail dft area
    QApplication::restoreOverrideCursor();
}
void DFTArea::gamma(int i){
    double v = 1. + 5. * (double)i/99.;
    m_gamma = v;
    doDFT();
}

void DFTArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap rp = QPixmap::fromImage(magIImage);

    QPainter dftpainter(&rp);

    dftpainter.drawImage(QPoint(0,0),magIImage);
    dftpainter.setBrush(QColor(0,0,100,50));

    int centerx = (magIImage.width()+1)/2;
    int centery = (magIImage.height()+1)/2;
    dftpainter.drawEllipse(QPointF(centerx, centery),
                        m_center_filter, m_center_filter);
    if (m_center_filter > 0.) {
        double val = (double)m_center_filter;
        val *= val;
        int last = 0;

        for (int i = 0; i < magIImage.width()-1; ++i){
            double rho = i -magIImage.width()/2;
            rho *= rho;

            double a =1-exp (-rho/(.25 * val));
            int y = magIImage.height()/2 + 50  -a * 50;
            dftpainter.drawLine(i,last,i+1,y);
            last = y;
        }
    }

    int xoffset = -centerx * (zoom -1) +(width() - magIImage.width() )/2;
    int yoffset = -centery * (zoom -1) + (height() - magIImage.height())/2;
    painter.drawPixmap(xoffset,yoffset,rp.scaled( zoom * rp.width(), zoom * rp.height()));

}



#define WRAP(x) (((x) > 0.5) ? ((x)-1.0) : (((x) <= -0.5) ? ((x)+1.0) : (x)))
#define WRAPPI(x) (((x) > M_PI) ? ((x)-2*M_PI) : (((x) <= -M_PI) ? ((x)+2*M_PI) : (x)))

#define BORDER      0x1
#define UNWRAPPED   0x2
double *g_qmap;
class comp_qual {
public:
  int operator() (const int &p1, const int &p2) {
    return (g_qmap[p1] < g_qmap[p2]);
  }
};
#define unwrap_and_insert(ndx, val) \
  { \
    unwrapped[ndx] = val;  \
    flags[ndx] |= UNWRAPPED; \
    path[ndx] = order++; \
    todo.push (ndx); \
  }

// Quality-guided path following phase unwrapper.
void qg_path_follower_vortex (Size size, double *phase, double *qmap,
               double *unwrapped, double *path)
{
  std::priority_queue<int, std::vector<int>, comp_qual> todo;
  int total = size.area();
  int order = 0;
  g_qmap = qmap;

  // Initialize the flags array to mark the border.
  char *flags = new char[total];
  for (int k=0; k < total; ++k)
    flags[k] = phase[k] == 0.0;

  // Repeat while still elements to unwrap (handles disjoint regions).
  while (1) {

    // Find the point of highest quality.
    double m = -HUGE;
    int mndx;
    for (int k=0; k < total; ++k)
      if (qmap[k] > m && ! flags[k])
    m = qmap[mndx = k];
    if (m == -HUGE) break;

    // Unwrap the first point.
    unwrap_and_insert (mndx, phase[mndx]);

    // Unwrap the rest of the points in order of quality.
    while (!todo.empty()) {
      int ndx = todo.top();
      todo.pop();
      int x = ndx%size.width;
      int y = ndx/size.width;
      double val = unwrapped[ndx];
      if (x > 0 && ! flags[ndx-1])
    unwrap_and_insert (ndx-1, val+WRAP(phase[ndx-1]-phase[ndx]));
      if (x < size.width-1 && ! flags[ndx+1])
    unwrap_and_insert (ndx+1, val+WRAP(phase[ndx+1]-phase[ndx]));
      if (y > 0 && ! flags[ndx-size.width])
    unwrap_and_insert (ndx-size.width, val+WRAP(phase[ndx-size.width]-phase[ndx]));
      if (y < size.height-1 && ! flags[ndx+size.width])
    unwrap_and_insert (ndx+size.width, val+WRAP(phase[ndx+size.width]-phase[ndx]));
    }
  }
  delete[] flags;
}



cv::Mat DFTArea::vortex(QImage &img, double low)
  {

    try
    {
    int startMem = showmem("start Vortex");
    cv::Mat image = grayComplexMatfromImage(img);

    // convert from 32 to 64 bit double values
    cv::Mat imageMat;
    image.convertTo(imageMat, numType);
    image.release();

    int xsize = imageMat.cols;
    int ysize = imageMat.rows;

    int size = xsize*ysize;
    double smooth = .01 * m_vortexDebugTool->m_smooth * xsize/2.;
    double *dir = new double[size];
    double *path = new double[size];
    double *qmap = new double[size];
    double *orient = new double[size];


    double *imRe = new double[size];


    //double *fdom[2]; fdom[0] = new double[size]; fdom[1] = new double[size];
    double *d1[2]; d1[0] = new double[size]; d1[1] = new double[size];
    double *d2[2]; d2[0] = new double [size]; d2[1] = new double [size];

    double *spiralRe = new double [size];
    double *spiralIm = new double[size];





    if (m_vortexDebugTool->m_showInput){
        cv::Mat tmp = cv::Mat(ysize,xsize,numType, imRe);
        cv::Mat xx;
        tmp.convertTo(xx,CV_32F);
        cv::imshow("input", xx);
        cv::waitKey(1);
    }


    cv::Mat fdomMat;
    cv::Mat fdomPlanes[2];


    dft(imageMat,fdomMat);
    split(fdomMat, fdomPlanes);


    // Create rho and theta arrays for later use.
    int *ix = new int[xsize];
    int *iy = new int[ysize];
    for (int i=0; i<=xsize/2; ++i) ix[i] = -i;
    for (int i=1; i<=xsize/2; ++i) ix[xsize-i] = i;
    for (int i=0; i<=ysize/2; ++i) iy[i] = -i;
    for (int i=1; i<=ysize/2; ++i) iy[ysize-i] = i;


    double *rho = new double[size];
    double *theta = new double[size];

    for (int j=0; j<ysize; ++j) {
      int base = j*xsize;
      for (int i=0; i<xsize; ++i) {
        rho[base+i] = sqrt (ix[i]*ix[i] + iy[j]*iy[j]);
        theta[base+i] = atan2 (iy[j], ix[i]);
      }
    }
    delete[] ix;
    delete[] iy;
    // High-pass filter the Fourier domain to remove the background.
    double *planes[2] = {fdomPlanes[0].ptr<double>(0), fdomPlanes[1].ptr<double>(0)};
    if (low > 0)
    {
        for (int i=0; i<size; ++i) {
          double a = 1.0 - exp (-(rho[i]*rho[i])/(low*low));

            planes[0][i] *= a;
            planes[1][i] *= a;
        }
    }
    merge(fdomPlanes,2, fdomMat);
    if (m_vortexDebugTool->m_showFdom){
        showMag(fdomMat.clone(),true,"fdom");
    }

    // Take the inverse Fourier transform to get the cleaned igram.

    //p = fftw_plan_dft_2d (ysize, xsize, fdom, im, FFTW_BACKWARD, FFTW_ESTIMATE);
    //fftw_execute (p);
    dft(fdomMat, imageMat, DFT_INVERSE);
    cv::Mat imPlanes[2];
    split(imageMat,imPlanes);



    imPlanes[0]/= size;
    cv::Mat tmp;
    imPlanes[0].copyTo(tmp, m_mask);
    imPlanes[0] = tmp.clone();
    tmp.release();
    // Normalize the image by removing the exterior and centering values.
    cv::Scalar mean,std;
    cv::meanStdDev(imPlanes[0],mean,std, m_mask);
    double sum = 0;
    double *q = imPlanes[0].ptr<double>(0);

    merge(imPlanes,2, imageMat);

    dft(imageMat,fdomMat);
    imageMat.release();

    double dc = fdomMat.at<double>(0,0,0);
    dc/=size;
    int count = 0;
    bool *bp = m_mask.ptr<bool>(0);
    for (int i = 0; i < size; ++i){
        if (bp[i]){
            sum += q[i];
            if (q[i] != 0.0)
            ++count;
        }
    }
    double m2 = sum/count;
    imPlanes[0] -= m2;

    imPlanes[1] *= 0.;

    imRe = (double *)(imPlanes[0].data);
    if (0) { //(0 == strcmp (what, "im2")) {
        showData("im border added", imPlanes[0].clone());
    }

    // Calculate the intermediate values d1 and d2.

    for (int i=0; i<size; ++i) {
      spiralRe[i] = cos(theta[i]);
      spiralIm[i] = sin(theta[i]);
    }
    delete[] theta;

    split(fdomMat,fdomPlanes);
    double *fdomRe = (double*)(fdomPlanes[0].data);
    double *fdomIm = (double *)(fdomPlanes[1].data);

    for (int i=0; i<size; ++i) {
      double re = fdomRe[i]*spiralRe[i] - fdomIm[i]*spiralIm[i];
      double im = fdomRe[i]*spiralIm[i] + fdomIm[i]*spiralRe[i];
      fdomRe[i] = re;
      fdomIm[i] = im;
    }
    merge(fdomPlanes,2,fdomMat);
    cv::Mat d1Mat;
    dft(fdomMat,d1Mat, DFT_INVERSE);
    //p = fftw_plan_dft_2d (ysize, xsize, fdomMat, d1Mat, FFTW_BACKWARD, FFTW_ESTIMATE);
    //fftw_execute (p);
    d1Mat/= size;

    if (0){ //(0 == strcmp (what, "d1")) {
        showMag(d1Mat.clone(), true, "D1");
    }


    for (int i=0; i<size; ++i) {
      double re = fdomRe[i]*spiralRe[i] - fdomIm[i]*spiralIm[i];
      double im = fdomRe[i]*spiralIm[i] + fdomIm[i]*spiralRe[i];
      fdomRe[i] = re;
      fdomIm[i] = im;
    }
    cv::Mat d2Mat;
    merge(fdomPlanes,2, fdomMat);
    dft(fdomMat,d2Mat,DFT_INVERSE);
    //p = fftw_plan_dft_2d (ysize, xsize, fdom, d2, FFTW_BACKWARD, FFTW_ESTIMATE);
    //fftw_execute (p);
    d2Mat/= size;

    if (0){//(0 == strcmp (what, "d2")) {
        showMag(d2Mat.clone(),true, "D2");
    }


    cv::Mat rMat;
    cv::Mat rPlanes[2] = {cv::Mat::zeros(Size(xsize,ysize),numType), cv::Mat::zeros(Size(xsize,ysize),numType)};
    cv::Mat d1Planes[2];
    cv::Mat d2Planes[2];

    split(d1Mat,d1Planes);
    d1Mat.release();
    split(d2Mat,d2Planes);
    d2Mat.release();

    double *d1Re = (double *)(d1Planes[0].data);
    double *d1Im = (double *)(d1Planes[1].data);
    double *d2Re = (double *)(d2Planes[0].data);
    double *d2Im = (double *)(d2Planes[1].data);
    double *rRe = (double *)(rPlanes[0].data);
    double *rIm = (double *)(rPlanes[1].data);

    // Calculate the orientation and the quality map for unwrapping.
    for (int i=0; i<size; ++i) {
      rRe[i] = d1Re[i]*d1Re[i] - d1Im[i]*d1Im[i] - imRe[i]*d2Re[i];
      rIm[i] = d1Re[i]*d1Im[i] + d1Im[i]*d1Re[i] - imRe[i]*d2Im[i];
    }

    //smooth = 0;
    if (smooth > 0) {

        merge(rPlanes,2, rMat);

        cv::Mat temp;
        cv::Mat tempPlanes[2];
      // Low-pass filter r to smooth it.
      dft(rMat,temp);

      //p = fftw_plan_dft_2d (ysize, xsize, r, temp, FFTW_FORWARD, FFTW_ESTIMATE);
      //fftw_execute (p);
      split(temp,tempPlanes);

      double *tempRe = (double *)(tempPlanes[0].data);
      double *tempIm = (double *)(tempPlanes[1].data);

      for (int i=0; i<size; ++i) {
        double a = exp (-(rho[i]*rho[i])/(smooth * smooth));
        tempRe[i] *= a;
        tempIm[i] *= a;
      }
      delete[] rho;

      merge(tempPlanes,2,temp);

      tempPlanes[0].release();
      tempPlanes[1].release();

      dft(temp,rMat,DFT_INVERSE);
      temp.release();

      //p = fftw_plan_dft_2d (ysize, xsize, temp, r, FFTW_BACKWARD, FFTW_ESTIMATE);
      //fftw_execute (p);
      rMat/= size;
      split(rMat,rPlanes);
      rMat.release();
    }

    for (int i=0; i<size; ++i)
      orient[i] = atan2 (rIm[i], rRe[i]);

    if (m_vortexDebugTool->m_showOrientation){
        cv::Mat orm(ysize,xsize,numType,orient);
        showData("orient", orm.clone());
    }


    for (int i=0; i<size; ++i) {
      qmap[i] = sqrt (rRe[i]*rRe[i] + rIm[i]*rIm[i]);
      orient[i] /= (2.*M_PI);  // put in range -.5..5 for unwrap
    }

    // Unwrap the orientation to get the direction.
    qg_path_follower_vortex (Size(xsize,ysize), orient, qmap, dir, path);
    for (int i=0; i<size; ++i)
     dir[i] = WRAPPI(dir[i]*M_PI);

    // Calculate the quadrature.
    imPlanes[1] = cv::Mat::zeros(Size(xsize,ysize), numType);
    double *imIm = (double *)(imPlanes[1].data);
    for (int i=0; i<size; ++i)
      imIm[i] = d1Re[i]*cos(-dir[i]) - d1Im[i]*sin(-dir[i]);

      // Display the isolated side lobe.
    if (m_vortexDebugTool->m_showFdom3){
        cv::Mat sideLobe;
        merge(imPlanes,2,sideLobe);
        shiftDFT(sideLobe);
        dft(sideLobe,fdomMat);
        shiftDFT(fdomMat);\
        showMag(fdomMat, true, "fdom3");
        sideLobe.release();
    }

    cv::Mat phase(Size(xsize,ysize), numType);
    double *p = (double *)(phase.data);
    for (int i=0; i<size; ++i) {
       p[i] = atan2 (imIm[i], imRe[i]);

    }

    imPlanes[0].release();
    imPlanes[1].release();
    // mask to only the mirror portion.
    phase.copyTo(tmp, m_mask);
    phase = tmp.clone();
    if (m_vortexDebugTool->m_showWrapped){
        cv::Mat tt = phase.clone();
        cv::normalize(tt,tt,0.f,1.f,CV_MINMAX);
        cv::imshow(" wrapped ", tt);
        cv::waitKey(1);
    }

    delete[] qmap;
    delete[] orient;
    delete[] d1[0];
    delete[] d1[1];
    delete[] d2[0];
    delete[] d2[1];

    delete[] spiralRe;
    delete[] spiralIm;
    delete[] dir;
    delete[] path;
    tmp.release();

    return phase;
    }
    catch (std::bad_alloc &e){
        showmem();
        qDebug() << QString().sprintf("Error %s", e.what());
       //cv::Mat phase = cv::Mat::zeros(Size(100,100), numType);

       return cv::Mat();
    }
    catch ( std::exception &e){

        qDebug() << QString().sprintf(" some Error %s", e.what());
       cv::Mat phase = cv::Mat::zeros(Size(0,0), numType);
       return phase;
    }
    catch (...){
        qDebug() << QString().sprintf(" Unknown error ");
       cv::Mat phase = cv::Mat::zeros(Size(100,100), numType);
       return phase;
    }

}
cv::Mat_<double> subtractPlane(cv::Mat_<double> phase, cv::Mat_<bool> mask){
    cv::Mat_<double> coeff(3,1);
    cv::Mat_<double> X(phase.rows * phase.cols,3);
    cv::Mat_<double> Z(phase.rows * phase.cols,1);
    int ndx = 0;
    for (int y = 0; y < phase.rows; ++y){
        for (int x = 0; x < phase.cols; ++x){
            if (mask(y,x)){
                Z(ndx) =  phase(y,x);
                X(ndx,0) = x;
                X(ndx,1) = y;
                X(ndx++,2) = 1.;
            }
        }
    }
    cv::solve(X,Z,coeff,CV_SVD);
    // plane generation, Z = Ax + By + C
    // distance calculation d = Ax + By - z + C / sqrt(A^2 + B^2 + C^2)


    cv::Mat_<double> newPhase(phase.size());
    for (int y = 0; y < phase.rows; ++y){
        for (int x = 0; x  < phase.cols; ++x){
            int b = (int)mask(y,x);
            if (b == 0 ){
                continue;
            }

            double val = x * coeff(0) + y * coeff(1) + coeff(2) - phase(y,x);
            double z = val/sqrt(coeff(0) * coeff(0) + coeff(1) * coeff(1) + 1);
            newPhase(y,x) = z;
        }
    }
    return newPhase;
}

// make a surface from the image using DFT and vortex transfroms.
void DFTArea::makeSurface(){
    if (!tools->wasPressed)
        return;
    MainWindow::me->m_outlinePlots->hide();
    if (igramArea->igramGray.isNull()){
        QMessageBox::warning(0,"warning","First load an interferogram and circle the mirror.");
        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    tools->wasPressed = false;
    if (Settings2::getInstance()->m_igram->m_autoSaveOutline){
        igramArea->writeOutlines(igramArea->makeOutlineName());  // save outlines including center filter
    }
    cv::Mat phase;
    try {

        phase = vortex(igramArea->igramGray,  m_center_filter);

    }
        catch (std::exception const& e){
    }

    if (phase.cols == 0){
        QMessageBox::warning(0,"warning", "Possible out of memory problem.  If possible try deleting some wave front files.");
        QApplication::restoreOverrideCursor();
        success = false;
        return;
    }

    cv::Mat result = cv::Mat::zeros(phase.size(), numType);

    phase.copyTo(result, m_mask);
    phase = result.clone();

    normalize(phase, phase,0,1.,CV_MINMAX, numType,m_mask);


    cv::Mat mask = m_mask.clone();
    mask = (255 - m_mask)/255;
    unwrap((double *)(phase.data), (double *)(result.data), (char *)(mask.data),
           phase.size().width, phase.size().height);
    phase.release();
    if (!Settings2::m_dft->flipv){  // Y is normally inverted because 0 is at bottom not top of image.
        flip(result,result,0); // flip around x axis.
         m_outside.m_center.ry() = (result.rows-1) - m_outside.m_center.y();
         m_center.m_center.ry() =  (result.rows-1) - m_center.m_center.y();
    }
    if (Settings2::m_dft->fliph){
        flip(result,result,1); // flip around x axis.
         m_outside.m_center.rx() = (result.cols-1) - m_outside.m_center.x();
         m_center.m_center.rx() =  (result.cols-1) - m_center.m_center.x();
    }

    mirrorDlg *md = mirrorDlg::get_Instance();

    if (md->fringeSpacing != 1.){
        result *= md->fringeSpacing;
    }

    if (md->isEllipse()) {
        CircleOutline t = m_outside;
        t.enlarge(-2);

        m_mask = makeMask(t,m_center, result, m_poly);
        result = subtractPlane(result, m_mask);
    }

    if (m_vortexDebugTool->m_showUnwrapped){

        showData("result surface", result.clone());
    }

    emit newWavefront(result, m_outside, m_center, QFileInfo(igramArea->m_filename).baseName(),
                        m_poly);
    QApplication::restoreOverrideCursor();
    success = true;
}
void DFTArea::newIgram(QImage){
    doDFT();
}
void DFTArea::mouseReleaseEvent(QMouseEvent *)
{
    setCursor(Qt::ArrowCursor);
    capture = false;
}

void DFTArea::mouseMoveEvent(QMouseEvent *event){
    if (!capture)
        return;
    QPointF pos = event->pos();
    int xcenter = width() /2;
    int ycenter = height()/2;
    int xdel = pos.x() - xcenter;
    int ydel = pos.y() - ycenter;
    int rad = sqrt(pow(xdel,2)+pow(ydel,2));
qDebug() << "xcenter" << xcenter << ycenter << xdel << ydel;
    emit updateFilterSize(rad/zoom);
}


void DFTArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {

        QPointF pos = event->pos();
        int xcenter = width() /2;
        int ycenter = height()/2;
        int xdel = pos.x() - xcenter;
        int ydel = pos.y() - ycenter;
        int rad = sqrt(pow(xdel,2)+pow(ydel,2));

        emit updateFilterSize(rad/zoom);
        capture = true;
    }
}
void DFTArea::showResizedDlg(){
    if (igramArea->igramGray.width() == 0)
            return;
    QSettings set;
    int size = set.value("DFTSize", 640).toInt();
//    if (size > igramArea->igramGray.width()){
//        QString msg;
//        msg = "The DFT size must be smaller than " + QString::number(size);
//        QMessageBox::warning(0, "________________DFT size too Large_____________", msg);
//        return;
//    }
    showAliasDlg dlg(size);
    dlg.setImage(igramArea->igramGray);
    if (dlg.exec()){
        tools->setDFTSize(dlg.resizedValue);
        doDFT();

    }
}

void dumpMat(cv::Mat m, QString title = ""){
    qDebug() << "\r" << title << m.rows <<"X" << m.cols;
    for (int r = 0; r < m.rows; ++r){
        QString msg;
        for (int c = 0; c < m.cols; ++c){
          msg = msg + QString().sprintf("% 6.2e",m.at<double>(r,c)) + " ";
          if ( c > 8){
              msg = msg + "...";
              break;
          }
        }
        qDebug() << msg;
        if (r > 8) {
            qDebug() << "...";
            break;
        }
    }
}

void DFTArea::outlineDoneSig(){
    m_outlineComplete = true;

}
#include "psiphasedisplay.h"
void DFTArea::doPSIstep1(){
    //m_Psidlg = new Psidlg;
    if (!doPSIstep2())
        return;
    if (m_psiFiles.size() == 0)
        return;
    //cv::Mat hold = m_psiPhases.clone();
    if (!m_Psidlg->m_knownPhase){
        m_psiPhases = getPhases();
        m_Psidlg->setPhases(m_psiPhases);
        if (m_Psidlg->m_stop) {
           emit statusBarUpdate(QString(" PSI Aborted"),1);

            return;
        }
    }
    cv::Mat images = PSILoadFullImages();
    if (images.rows == 0)
        return;

    doPSIstep4(images, m_psiPhases);


}
void DFTArea::doPSITilt(){
    // create the rho and theta arrays.

}

bool DFTArea::doPSIstep2(){
    if (m_Psidlg->exec()){
        m_psiFiles = m_Psidlg->files();
        m_psiPhases = m_Psidlg->phases();
        return true;
    }
    return false;
}
void DFTArea::doPSIstep3(){
// waiting


}
#include "psiresizeimagesdlg.h"
cv::Mat DFTArea::PSILoadFullImages(){
    int cnt = 0;
    cv::Mat data;

    QApplication::setOverrideCursor(Qt::WaitCursor);

    foreach(QString name, m_psiFiles){

        emit statusBarUpdate(QString("Loading " + name),1);
        QApplication::processEvents();
        QImage loadedImage;
        if (!loadedImage.load(name)){
            QString msg =  QString().sprintf("Failed to load %s",name.toStdString().c_str());
            QMessageBox::warning(0, "load image failed", msg);
            continue;
        }
        if (m_Psidlg->shouldResize()){
            int resizeWidth = m_Psidlg->getResizeValue();
            loadedImage = loadedImage.scaled( resizeWidth, resizeWidth, Qt::KeepAspectRatio);
        }
        // convert image to 3 channel image
        if (loadedImage.format() != QImage::Format_RGB888)
            loadedImage = loadedImage.convertToFormat(QImage::Format_RGB888);
        cv::Mat bestChan = igramArea->igramToGray(igramArea->qImageToMat(loadedImage));

        cv::Mat planes[3];
        split(bestChan,planes);
        cv::Mat datam = planes[0];

        datam.convertTo(datam, CV_64FC1);

        cv::Mat rowImage = datam.reshape(0,datam.total()).t();


        if (cnt++ == 0){
            data = rowImage;
            m_psiRows = datam.rows;
            m_psiCols = datam.cols;
        }
        else {
            if ((datam.cols != m_psiCols) || (datam.rows != m_psiRows)){
                QString msg = QString().sprintf("igram %s (%d,%d)is not the same size as %s (%d,%d)",
                  m_psiFiles[cnt].toStdString().c_str(),datam.rows,datam.cols, m_psiFiles[0].toStdString().c_str(),
                        m_psiRows,m_psiCols);
                QMessageBox::warning(0, "Failed", msg);
                 QApplication::restoreOverrideCursor();
                 return data;
            }

            cv::vconcat(data,rowImage, data);

        }
    }
    QApplication::restoreOverrideCursor();
    return data;
}


cv::Mat sinMat(cv::Mat mat){
    cv::Mat result(mat.rows,mat.cols, mat.type());
    double* resptr = result.ptr<double>(0);
    double* matptr = mat.ptr<double>(0);
    int last = mat.total();
    for (int i = 0; i < last; ++i ){
        resptr[i] = sin(matptr[i]);
    }
    return result;
}
cv::Mat cosMat(cv::Mat mat){
    cv::Mat result(mat.rows,mat.cols, mat.type());
    double* resptr = result.ptr<double>(0);
    double* matptr = mat.ptr<double>(0);
    int last = mat.total();
    for (int i = 0; i < last; ++i ){
        resptr[i] = cos(matptr[i]);
        //qDebug() << "cos" << matptr[i] << resptr[i];
    }
    return result;
}

cv::Mat atan2Mat(cv::Mat y, cv::Mat x){

    cv::Mat result(x.rows,x.cols, x.type());
    double* resptr = result.ptr<double>(0);
    double* xptr = x.ptr<double>(0);
    double* yptr = y.ptr<double>(0);
    int last = x.total();
    int cnt = 0;
    for (int i = 0; i < last; ++i ){
        resptr[i] = atan2(yptr[i],xptr[i]);
        //if (++cnt < 20)
        //qDebug() << "atan2"<< resptr[i] << yptr[i] << xptr[i];
    }
    return result;

}
#include <armadillo>


arma::mat zpmCxx(double rho, double theta, int maxorder) {

  int m, n, n0, mmax = maxorder/2;
  int i, imax = 1;

  int order, nm, nm1mm1, nm1mp1, nm2m;
  int ncol = (mmax+1)*(mmax+1);
  double a0;
  double cosmtheta[mmax], sinmtheta[mmax];
  arma::mat  zm(imax, ncol);

  //do some rudimentary error checking

  //if (rho.length() != theta.length()) stop("Numeric vectors must be same length");
  //if ((maxorder % 2) != 0) stop("maxorder must be even");

  //good enough


  for (i=0; i<imax; i++) {

    //cache values of cos and sin
    cosmtheta[0] = std::cos(theta);
    sinmtheta[0] = std::sin(theta);
    for (m=1; m<mmax; m++) {
      cosmtheta[m] = cosmtheta[m-1]*cosmtheta[0] - sinmtheta[m-1]*sinmtheta[0];
      sinmtheta[m] = sinmtheta[m-1]*cosmtheta[0] + cosmtheta[m-1]*sinmtheta[0];
    }

    zm(i, 0) = 1.0;                     //piston term
    zm(i, 3) = 2. * rho * rho - 1.; //defocus

    // now fill in columns with m=n for n>0

    for (m=1; m <= mmax; m++) {
      zm(i, m*m) = rho * zm(i, (m-1)*(m-1));
    }

    // non-symmetric terms

    for (order=4; order<=maxorder; order+=2) {
      for (m=order/2-1; m>0; m--) {
        n=order-m;
        nm = order*order/4 + n - m;
        nm1mm1 = (order-2)*(order-2)/4 + n - m;
        nm1mp1 = nm - 2;
        nm2m = nm1mm1 - 2;
        zm(i, nm) = rho*(zm(i, nm1mm1) + zm(i, nm1mp1)) - zm(i, nm2m);
      }

      // m=0 (symmetric) term
      nm = order*order/4 + order;
      nm1mp1 = nm-2;
      nm2m = (order-2)*(order-2)/4+order-2;
      zm(i, nm) = 2.*rho*zm(i, nm1mp1) - zm(i, nm2m);
    }

    // now multiply each column by normalizing factor and cos, sin

    n0 = 1;
    for (order=2; order <= maxorder; order+=2) {
      for(m=order/2; m>0; m--) {
        n=order-m;
        //a0 = sqrt(2.*(n+1));
        zm(i, n0+1) = sinmtheta[m-1]*zm(i, n0);
        zm(i, n0) *= cosmtheta[m-1];
        n0 += 2;
      }

      n0++;
    }
  }
  return zm;
}

#include "outlinedialog.h"
void DFTArea::doPSIstep4(cv::Mat images, QVector<double> phases){

    emit statusBarUpdate("computing surface from  interferograms.",1);
    m_outlineComplete = false;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    //images = PSILoadFullImages();

    int imagecount = m_psiFiles.size();

    //mat X = join_rows(join_rows(ones(nf), cos(m_psiPhases)), sin(m_psiPhases));
    cv::Mat ones =  cv::Mat::ones(imagecount, 1, CV_64FC1);
    cv::Mat phasemat = cv::Mat(imagecount, 1, CV_64FC1, phases.data());
    cv::Mat phasesine = sinMat(phasemat);
    cv::Mat phasecosine = cosMat(phasemat);

    cv::hconcat(ones,phasesine,ones);
    cv::hconcat(ones,phasecosine,ones);
    //qDebug() << "ones" << ones.rows << ones.cols;
    cv::Mat diag = cv::Mat::eye(imagecount,imagecount,CV_64FC1);
    cv::Mat B;
    cv::Mat X = diag * ones;
    //qDebug() << "x";
    cv::Mat A = diag * images;
    //qDebug() << "a";
    //qDebug() << "solve";
    try{
        cv::solve(X, A,B,DECOMP_QR);
    }
    catch (cv::Exception ex) {
        QMessageBox::warning(0,"error", ex.what());
        QApplication::restoreOverrideCursor();
        return;
    }


    qDebug() << "solve Done" << B.cols << B.rows;

    cv::Mat phi(1,imagecount,CV_64FC1);
    cv::Mat background = B.row(0).reshape(0,m_psiRows);
    //showData("background", background.clone());

    cv::Mat surface = atan2Mat(-B.row(2), B.row(1));
    cv::Mat mag;
    cv::magnitude(B.row(2), B.row(1), mag);
    cv::Mat mag2d = mag.reshape(0,m_psiRows);

    surface = surface.reshape(0,m_psiRows);

    // generate outline
    QFileInfo info(m_psiFiles[0]);
    QString outlineName = info.absolutePath() + "/" + info.completeBaseName() + ".oln";


    QFileInfo finfo( outlineName);
    if (finfo.exists()){
        igramArea->deleteRegions();
        igramArea->loadOutlineFile(outlineName);
    }
    else {
    qDebug() << "use last outline";
        igramArea->useLastOutline();
        // setup auto outline controls.
    }
    outlineDialog dlg(0,0,0);

    dlg.setImage(mag2d);
    cv::Mat mask;
    QApplication::restoreOverrideCursor();
    if (dlg.exec()){
        QSettings set;
        set.setValue("lastOutsideRad", dlg.m_rad);
        set.setValue("lastOutsideCx",dlg.m_x);
        set.setValue("lastOutsideCy",dlg.m_y);
        igramArea->m_outside = CircleOutline(QPointF(dlg.m_x, dlg.m_y), dlg.m_rad);
        if (Settings2::getInstance()->m_igram->m_autoSaveOutline){
            QSettings settings;
            settings.setValue("lastPath", info.absolutePath());
            qDebug() << "outline name" << outlineName;
            igramArea->writeOutlines(outlineName);
        }
qDebug() << "dlg" << dlg.m_x << dlg.m_rad;
        int left = dlg.m_x - dlg.m_rad;
        int right = dlg.m_x + dlg.m_rad;
        int top = dlg.m_y - dlg.m_rad;
        int bottom = dlg.m_y + dlg.m_rad;
        int width = dlg.m_rad * 2;
        int height = width;
        if (left < 0)
            left = 0;
        if (right > mag2d.cols-1)
            right = mag2d.cols-1;
        if (top < 0)
            top = 0;
qDebug() << "rec" << left << top << width << height;
        cv::Mat roi = surface(cv::Rect(left,top, width,height));

        surface = roi.clone();

        m_outside = CircleOutline(QPointF(dlg.m_rad, dlg.m_rad), dlg.m_rad);
        mask = makeMask(m_outside, m_center, surface, m_poly);

    }
    else{

        return;
    }
    QApplication::setOverrideCursor(Qt::WaitCursor);
    qDebug() << " done with outline dialog";

    // maybe here is where we enable tiltPSI

    /********************************************************************************
    //debug code to check if mike's and my zernike code produce the same zernike values
    arma::mat r =  rhotheta(surface.rows,surface.cols,0.);

    dumpArma(r, "RhoTheta");
    arma::mat coords = zpmCxx(r(0,0), r(1,0) ,20);
    dumpArma(coords,"coords");

    double r0 = r(0,0);
    double t0 = r(1,0);

    qDebug() << "Rho " << r0 << "Theta" << t0;



    zernikePolar &zpolar = *zernikePolar::get_Instance();
    zpolar.init(r0,t0);

// debug print the difference
//    for (int i = 0; i < 100; ++i){
//        qDebug() << "psi Zernike" <<i <<  coords(0,i) << zpolar.zernike(i,r0, t0);
//    }

    //cv::Mat modul = modu.reshape(0,rows);
    //showData("mod", modul);
// end of debug section
*******************************************************************************/


    cv::Mat phase = surface.clone();
    //qDebug() << "phase "<< phase.cols << phase.rows << m_mask.rows << m_mask.cols;
    //showData("phase",phase.clone());
    //cv::Mat mask = m_mask.clone();
    normalize(phase, phase,0,1.,CV_MINMAX, -1,mask);

    cv::Mat mask2 = mask.clone();
    mask2 = (255 - mask2)/255;
    //showData("mask", mask2.clone());
    cv::Mat result = cv::Mat::zeros(phase.size(), numType);
    unwrap((double *)(phase.data), (double *)(result.data), (char *)(mask2.data),
           phase.cols, phase.rows);

    //showData("surface", result.clone());
    QSettings set;
    if (!Settings2::m_dft->flipv){  // Y is normally inverted because 0 is at bottom not top of image.
        flip(result,result,0); // flip around x axis.
        m_outside.m_center.ry() = (result.rows-1) - m_outside.m_center.y();
        m_center.m_center.ry() =  (result.rows-1) - m_center.m_center.y();
    }
    if (Settings2::m_dft->fliph){
        flip(result,result,1); // flip around x axis.
        m_outside.m_center.rx() = (result.cols-1) - m_outside.m_center.x();
        m_center.m_center.rx() =  (result.cols-1) - m_center.m_center.x();
    }
    QString wfname = QString("PSI")+ finfo.baseName() + QString("-") + QFileInfo(m_psiFiles[imagecount-1]).baseName();
    emit newWavefront(result, m_outside, m_center, wfname, m_poly);
    QApplication::restoreOverrideCursor();
}

void DFTArea::computePhases(){
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_psiFiles = m_Psidlg->files();
    if (m_psiFiles.length() == 0)
        return;
    m_psiPhases = m_Psidlg->phases();
    QVector<double> a = m_psiPhases;
    qDebug() << "Phases found" << m_psiPhases << a;


    m_psiPhases = getPhases();
    m_Psidlg->setPhases(m_psiPhases);
    QApplication::restoreOverrideCursor();
}

QVector<double> DFTArea::getPhases(){

    cv::Mat imagescv = PSILoadFullImages();
    emit statusBarUpdate("computing phases",1);
    //qDebug() << "phases rows cols" << imagescv.rows << imagescv.cols;
    int M = imagescv.cols;
    int N = imagescv.rows;
    //Since cvmats are row major and arma mats are column mats this transposes the cv::image matrix which is desired.
    arma::mat images(reinterpret_cast<double*>(imagescv.data), M,N);
    //qDebug() << "arma images row col" << images.n_rows << images.n_cols;
    //qDebug() << N << " images of width " << M;
    int maxiter = m_Psidlg->maxIter();
    double ptol = m_Psidlg->tolerance();
    arma::rowvec phases_init(m_psiPhases.data(), N);
    arma::rowvec phases = phases_init - phases_init(0);
    QVector<arma::rowvec> trials;
    if (arma::norm(phases) == 0.0){
        phases(0) = 1.;
    }
    arma::rowvec phases_last = phases;

    arma::mat S(3, N);
    arma::mat Phi(M, 3);
    arma::vec phi(M);
    arma::vec mod(M);
    double sdp;
    double bestSdp;
    int bestIteration;
    int i;

    for (i=0; i<maxiter; i++) {
        if (m_Psidlg->m_stop){
        QMessageBox::warning(0, "Failed","Aborted by user");
          break;
        }
        QApplication::processEvents();

        // Estimate the phase from the current estimate of phase shifts

        S = arma::join_cols(arma::join_cols(arma::ones<arma::rowvec>(N), arma::cos(phases)), arma::sin(phases));
        Phi = images * arma::pinv(S);
        phi = arma::atan2(-Phi.col(2), Phi.col(1));
        Phi = arma::join_rows(arma::join_rows(arma::ones(M), arma::cos(phi)), -arma::sin(phi));

        // Estimate phase shifts from current estimate of phase.
        // Solves normal equations for speed

        // S = pinv(Phi.t() * Phi) * Phi.t() * images;

        // Not any more. There's almost no time penalty for doing it right.

        S = arma::pinv(Phi) * images;
        phases = arma::atan2(S.row(2), S.row(1));
        phases = phases - phases(0);
        //dumpArma(phases,"phases");
        sdp = arma::norm(arma::sin(phases-phases_last), 2);
            //qDebug() << "iteration: "<< i << " sdp = " << sdp;
        if (sdp < bestSdp){
            bestSdp = sdp;
            bestIteration = i;
        }
        trials << phases;

        // show current
        QVector<double> a;
        for (int i = 0; i < phases.n_cols; ++i){
            a << phases(i);
        }


        m_Psidlg->setPhases(a);
        m_Psidlg->plot(a, i, sdp);
        // repeat until convergence
        m_Psidlg->setStatusText(QString().sprintf("iteration %d  sdp %lf", i, sdp),i);
        if (i > 1 && (sdp < ptol)) {

            break;
        }
        phases_last = phases;
        emit statusBarUpdate(QString().sprintf("%d %lf",i, sdp),2);
    }
    if (i == maxiter && (sdp > ptol)){
        QString msg = QString().sprintf("The calculated phases did not converge within the maximum number"
                             " of iterations (%d). \nThey proabably are not valid.",maxiter);
        QMessageBox::warning(0, "No convegence", msg );
        phases = trials[bestIteration];
        i = bestIteration;
        sdp = bestSdp;
    }
    // one more calculation of the phase

    S = arma::join_cols(arma::join_cols(arma::ones<arma::rowvec>(N), arma::cos(phases)), arma::sin(phases));
    Phi = images * arma::pinv(S);
    phi = arma::atan2(-Phi.col(2), Phi.col(1));
    QVector<double> a;
    for (int i = 0; i < phases.n_cols; ++i){
        a << phases(i);
    }
    m_Psidlg->setStatusText(QString().sprintf("iteration %d  sdp %lf Compute Phases complete. ", i, sdp),maxiter);

    return a;
}



