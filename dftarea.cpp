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
using namespace cv;

cv::Mat  makeMask(CircleOutline outside, CircleOutline center, cv::Mat data){
    int width = data.cols;
    int height = data.rows;

    double radm = ceil(outside.m_radius);
    double rado = center.m_radius;
    double cx = outside.m_center.x();
    double cy = outside.m_center.y();
    cv::Mat mask = cv::Mat::ones(height,width,CV_8UC1);
    for (int y = 0; y < height; ++y){
        for (int x = 0; x < width; ++x){
            double dx = (double)(x - (cx))/(radm);
            double dy = (double)(y - (cy))/(radm);
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
    return mask;
}

DFTArea::DFTArea(QWidget *mparent, IgramArea *ip, DFTTools * tools, vortexDebug *vdbug) :
    QWidget(mparent),m_size(640), tools(tools),
    dftSizeStr("640 X 640"), m_center_filter(10.),ui(new Ui::DFTArea),igramArea(ip),m_smooth(9.),
    m_vortexDebugTool(vdbug)

{
    ui->setupUi(this);
    m_gamma = 2.5;
    connect(tools,SIGNAL(dftChannel(const QString&)), this, SLOT(setChannel(const QString&)));
    connect(tools,SIGNAL(dftSizeChanged(const QString&)), this, SLOT(dftSizeChanged(const QString&)));
    connect(tools,SIGNAL(dftSizeVal(int)), this, SLOT(dftSizeVal(int)));
    connect(tools,SIGNAL(dftCenterFilter(double)), this, SLOT(dftCenterFilter(double)));
    connect(tools,SIGNAL(makeSurface()), this,SLOT(makeSurface()));
    connect(tools,SIGNAL(doDFT()), this,SLOT(doDFT()));
    tools->connectTo(this);
    capture = false;
    QSettings set;
    m_center_filter = set.value("DFT Center Filter", 10).toDouble();
    emit updateFilterSize(m_center_filter);
    /*
        srand(time(NULL));
    for (int i = 0; i < 5; ++i){
    QwtPlot *test = new QwtPlot();

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(test);
    QPolygonF points;
    QPolygonF runningAvg;
    QPolygonF noise;
    double avg = 0;


    for (int i = 0; i < 50; ++i){
        double n = double((rand()%100 - 50)/100.);
        noise << QPointF(i, n);
        double v = 1. + n;
        points << QPointF( i,  v );
        avg += v;
        runningAvg << QPointF(i, avg/(i+1));
    }
    QwtPlotCurve *c3 = new QwtPlotCurve("noise");
    c3->setSamples(noise);
    c3->setPen(QPen(Qt::green));
    //c3->attach(test);
    QwtPlotCurve *c1 = new QwtPlotCurve("signal");
    c1->setSamples(points);
    c1->attach(test);
    QwtPlotCurve *c2 = new QwtPlotCurve("Avg");
    c2->setSamples(runningAvg);
    c2->setPen(QPen(QColor(Qt::blue)));
    c2->attach(test);;
    test->resize(800,200);
    test->show();
    }
    */
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
    qDebug() << "Setting center value " << v;
    set.setValue("DFT Center Filter", v);
    update();
}

void DFTArea::dftSizeVal(int val){
    m_size = val;
}

void DFTArea::dftSizeChanged(const QString& val){
    dftSizeStr = val;
    if (val == "same as Igram"){
        int newVal = igramArea->igramImage.width();
        emit setDftSizeVal(newVal);
    }
    if (val == "640 x 640") {
        int newVal = 640;
        emit setDftSizeVal(newVal);
    }
}


// return a 32F mat gray scale of the image. scale it down to match the size of the
// desired DFT size.  Default is 640 x 640.  Create mask of mirror only area.
Mat DFTArea::grayComplexMatfromImage(QImage &img){

    // create an roi that is a square around the outline.
    double centerX = igramArea->m_outside.m_center.x();
    double centerY = igramArea->m_outside.m_center.y();
    double rad = igramArea->m_outside.m_radius;
    double radpix = ceil(rad);
    int border = 10;
    double left = floor((centerX - radpix - border));
    double top = floor(centerY - radpix - border);
    vector<Mat > bgr_planes;
    top = max(top,0.);
    left = max(left,0.);
    int width = ceil(2. * (radpix) + 2 * border);
    width = min(width, img.width());

    // new center because of crop
    double xCenterShift = centerX - left;
    double yCenterShift = centerY - top;

    cv::Mat iMat(img.height(), img.width(), CV_8UC4, img.bits(), img.bytesPerLine());
    cv::Mat roi = iMat(cv::Rect((int)left,(int)top,(int)width,(int)width)).clone();

    double centerDx = centerX - igramArea->m_center.m_center.x();
    double centerDy = centerY - igramArea->m_center.m_center.y();

    roi.convertTo(roi,CV_32FC3);

    int dftSize = Settings2::dftSize();
    double scaleFactor = (double)dftSize/roi.rows;
    m_outside = CircleOutline(QPointF(xCenterShift,yCenterShift), rad);
    m_center = CircleOutline(QPointF(xCenterShift - centerDx, yCenterShift - centerDy),
                             igramArea->m_center.m_radius);

    if (scaleFactor < 1.){

        cv::resize(roi,roi, cv::Size(0,0), scaleFactor, scaleFactor);
        double roic = (roi.rows-1)/2.;
        m_outside = CircleOutline(QPointF(roic,roic),roic);
        m_center = CircleOutline(QPointF((roic - centerDx * scaleFactor), (roic - centerDy * scaleFactor)),
                                 m_center.m_radius * scaleFactor);
    }
    else {
        scaleFactor = 1.;
    }
    if (channel =="ALL RGB") {
        cvtColor(roi,roi,CV_BGRA2BGR);
        cvtColor(roi,roi,CV_BGR2HSV);
    }


    // split image into three color planes

    split( roi, bgr_planes );


    cv::Scalar mean;
    mean =  cv::mean(roi);
    double maxMean = 0;
    double maxndx = 0;

    // use the color plane with the largest mean value
    for (int i = 0; i < 3; ++i){
        double m = mean[i];
        //qDebug() << QString().sprintf("%lf", m);

        if (m > maxMean){
            maxMean =m;
            maxndx = i;
        }
    }
    if (channel == "Blue") maxndx = 0;
    else if (channel == "Green") maxndx = 1;
    else if (channel == "Red") maxndx = 2;
    qDebug() << "Max channel " << maxndx;
    Mat padded = bgr_planes[maxndx].clone();

    int m = getOptimalDFTSize( roi.rows ) - roi.rows;
    int n = getOptimalDFTSize( roi.cols ) - roi.cols; // on the border add zero values
    // disabled adding optomizing DFT boarder.
    m = 0;
    n = 0;
    qDebug() << "pady " << m << " padx " << n;
    if (m > 0 || n > 0)
        copyMakeBorder(padded, padded, 0, m, 0, n, BORDER_CONSTANT, Scalar::all(mean[maxndx]));

    padded = padded - mean[maxndx];

    Mat planes[] = {Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F)};

    m_mask = makeMask(m_outside,m_center, *planes);
    if (Settings2::showMask()){
        Mat tmp = planes[0].clone();
        normalize(tmp, tmp,0,255,CV_MINMAX);
        tmp.convertTo(tmp,CV_8U);
        Mat mm;
        Mat channels[3];

        channels[1] = m_mask;
        channels[2] = tmp;
        channels[0] = m_mask;
        merge(channels,3,mm);
        for (int i = 0; i < 3; ++i){
            //imshow(QString().number(i).toStdString().c_str(),channels[i]);
        }
        imshow("mm", mm);
        waitKey(1);

    }
    cv::Mat tmpMask;

    planes[0].copyTo(tmpMask,m_mask);    // Convert image to binary
    planes[0] = tmpMask.clone();
    mean =  cv::mean(planes[0],m_mask);
    planes[0] -= mean;

    Mat  complexI;
    merge(planes, 2, complexI);         // Add to the expanded another plane with zeros
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

void showData(const string& txt, cv::Mat mat, bool useLog){
    cv::Mat tmp = mat.clone();
    if (useLog){
        tmp = mat+1;
        log(tmp, tmp);
    }
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
    qDebug() << mmin << "," << mmax;
    if (doLog)
        log((magI+0.1), magI);

    if (gamma != 0.){
        cv::pow(magI,gamma,magI);
    }
    normalize(magI, magI,0,255,CV_MINMAX, CV_8U);
    minMaxIdx(magI, &mmin,&mmax);
    qDebug() << mmin << "," << mmax;
    cv::Mat tmp = magI.clone();
    cv::waitKey(1);
    cvtColor(magI,magI, CV_GRAY2RGB);
    if (show){
        imshow(title, magI);
        waitKey(1);
    }
    return QImage((uchar*)magI.data, magI.cols, magI.rows, magI.step, QImage::Format_RGB888).copy();
}

void DFTArea::doDFT(){
    QImage img = igramArea->igramImage;

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
    m_dft = complexI/complexI.size().area();

    magIImage = showMag(complexI,false,"", true, m_gamma);
    scale = 1.;
    double h = magIImage.height();
    qDebug() <<" dft size "<< size();
    scale = double(parentWidget()->size().height())/h;
    if (scale < 1.)
        scale = 1.;
    magIImage = magIImage.scaled(magIImage.width() * scale, magIImage.height() * scale);
    setMinimumSize(magIImage.size());

    //emit selectDFTTab();
    update();
    if (Settings2::showDFT())
        emit dftReady(magIImage);     //Creates a thumbnail dft area
}
void DFTArea::gamma(int i){
    double v = 1. + 5. * (double)i/99.;
    m_gamma = v;
    doDFT();
}

void DFTArea::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(QPoint(0,0),magIImage);
    painter.setBrush(QColor(0,0,100,50));
    painter.drawEllipse(QPointF(magIImage.width()/2,magIImage.height()/2),
                        scale * m_center_filter,scale * m_center_filter);
    if (m_center_filter > 0.) {
        double val = (double)m_center_filter * scale;
        val *= val;
        int last = 0;
        for (int i = 0; i < magIImage.width()-1; ++i){
            double rho = i -magIImage.width()/2;
            rho *= rho;

            double a =1-exp (-rho/(.25 * val));
            int y = magIImage.height()/2 + 50  -a * 50;
            painter.drawLine(i,last,i+1,y);
            last = y;
        }
    }
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

    cv::Mat image = grayComplexMatfromImage(img);
    cv::Mat imagePlanes[2];
    split(image,imagePlanes);


    int xsize = image.cols;
    int ysize = xsize;
    int nx = xsize, ny = ysize;
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

    // Prepare the image and convert it to doubles.
    float *pIm = imagePlanes[0].ptr<float>(0);
    for (int i=0; i<size; ++i) {
      imRe[i] = (double)(pIm[i]);

    }
/*
    QwtPlot *pl1 = new QwtPlot();
    QwtPlotCurve *curve1 = new QwtPlotCurve("DFT image");
    QPolygonF points1;
    for (int x = 0; x < xsize; ++x){
        double v = imRe[size/2 + x];
        points1 << QPointF(x,v);
    }
    curve1->setSamples(points1);
    curve1->attach(pl1);



    pl1->resize(1000,400);
    pl1->show();
*/
    if (m_vortexDebugTool->m_showInput){
        cv::Mat tmp = cv::Mat(ysize,xsize,CV_64F, imRe);
        cv::Mat xx;
        tmp.convertTo(xx,CV_32F);
        cv::imshow("input", xx);
        cv::waitKey(1);
    }

    // Take the Fourier transform.
    cv::Mat imPlanes[2] = {cv::Mat(Size(ny,nx), CV_64F, imRe),
                           cv::Mat::zeros(Size(ny,nx),CV_64F)};

    cv::Mat imMat;
    cv::Mat fdomMat;
    cv::Mat fdomPlanes[2];
    merge(imPlanes,2, imMat);

    dft(imMat,fdomMat);
    split(fdomMat, fdomPlanes);

    //p = fftw_plan_dft_2d (ysize, xsize, im, fdom, FFTW_FORWARD, FFTW_ESTIMATE);
    //fftw_execute (p);



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
    dft(fdomMat, imMat, DFT_INVERSE);
    split(imMat,imPlanes);
    imPlanes[0]/= size;
    cv::Mat tmp;
    imPlanes[0].copyTo(tmp, m_mask);
    imPlanes[0] = tmp.clone();
    // Normalize the image by removing the exterior and centering values.
    cv::Scalar mean,std;
    cv::meanStdDev(imPlanes[0],mean,std, m_mask);
    double sum = 0;
    double *q = imPlanes[0].ptr<double>(0);

    merge(imPlanes,2, imMat);

    dft(imMat,fdomMat);
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
    merge(imPlanes,2,imMat);

    // Calculate the intermediate values d1 and d2.

    for (int i=0; i<size; ++i) {
      spiralRe[i] = cos(theta[i]);
      spiralIm[i] = sin(theta[i]);
    }
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
    cv::Mat rPlanes[2] = {cv::Mat::zeros(Size(ysize,xsize),CV_64F), cv::Mat::zeros(Size(ysize,xsize),CV_64F)};
    cv::Mat d1Planes[2];
    cv::Mat d2Planes[2];
    split(d1Mat,d1Planes);
    split(d2Mat,d2Planes);
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
      //showData("smooth", tempPlanes[0].clone());
      merge(tempPlanes,2,temp);
      dft(temp,rMat,DFT_INVERSE);
      //p = fftw_plan_dft_2d (ysize, xsize, temp, r, FFTW_BACKWARD, FFTW_ESTIMATE);
      //fftw_execute (p);
      rMat/= size;
      split(rMat,rPlanes);
    }

    for (int i=0; i<size; ++i)
      orient[i] = atan2 (rIm[i], rRe[i]);

    if (m_vortexDebugTool->m_showOrientation){
        cv::Mat orm(ysize,xsize,CV_64F,orient);
        showData("orient", orm.clone());
    }


    for (int i=0; i<size; ++i) {
      qmap[i] = sqrt (rRe[i]*rRe[i] + rIm[i]*rIm[i]);
      orient[i] /= (2.*M_PI);  // put in range -.5..5 for unwrap
    }

    // Unwrap the orientation to get the direction.
    qg_path_follower_vortex (Size(ysize,xsize), orient, qmap, dir, path);
    for (int i=0; i<size; ++i)
     dir[i] = WRAPPI(dir[i]*M_PI);

    // Calculate the quadrature.
    imPlanes[1] = cv::Mat::zeros(Size(ysize,xsize), CV_64F);
    double *imIm = (double *)(imPlanes[1].data);
    for (int i=0; i<size; ++i)
      imIm[i] = d1Re[i]*cos(-dir[i]) - d1Im[i]*sin(-dir[i]);

      // Display the isolated side lobe.
    cv::Mat sideLobe;
    merge(imPlanes,2,sideLobe);
    shiftDFT(sideLobe);
    dft(sideLobe,fdomMat);
    shiftDFT(fdomMat);\
    if (m_vortexDebugTool->m_showFdom3){
        showMag(fdomMat, true, "fdom3");
    }
    cv::Mat phase(Size(ysize,xsize), CV_64F);
    double *p = (double *)(phase.data);
    for (int i=0; i<size; ++i) {
       p[i] = atan2 (imIm[i], imRe[i]);
     }
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
    delete[] rho;
    delete[] theta;
    delete[] spiralRe;
    delete[] spiralIm;
    delete[] dir;
    delete[] path;
    return phase;
}
#include "simigramdlg.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_renderer.h>
#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>

// make a surface from the image using DFT and vortex transfroms.
void DFTArea::makeSurface(){

    igramArea->writeOutlines(igramArea->makeOutlineName());  // save outlines including center filter
    cv::Mat phase = vortex(igramArea->igramImage,  m_center_filter);

    int wx = phase.rows;
    int wy = wx;

    cv::Mat result = cv::Mat::zeros(phase.size(), CV_64F);

    phase.copyTo(result, m_mask);
    phase = result.clone();

    normalize(phase, phase,0,1.,CV_MINMAX, CV_64F,m_mask);


    cv::Mat mask = m_mask.clone();
    mask = (255 - m_mask)/255;

    unwrap((double *)(phase.data), (double *)(result.data), (char *)(mask.data),
           phase.size().width, phase.size().height);

    flip(result,result,0); // flip around x axis.
    m_outside.m_center.ry() = result.rows - m_outside.m_center.y();
    m_center.m_center.ry() = result.rows - m_center.m_center.y();
    mirrorDlg *md = mirrorDlg::get_Instance();
    if (md->fringeSpacing != 1.){
        result *= md->fringeSpacing;
    }
    if (m_vortexDebugTool->m_showUnwrapped)
        showData("result surface", result.clone());
                   //wf was result and should be reset after debugging.
    emit newWavefront(result, m_outside, m_center, QFileInfo(igramArea->m_filename).baseName()+".wft");

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
    int xcenter = (magIImage.width() -1)/2;
    int ycenter = (magIImage.height()-1)/2;
    int rad = sqrt(pow(pos.x()-xcenter,2)+pow(pos.y() - ycenter,2));
    emit updateFilterSize(rad/scale);
}

void DFTArea::mousePressEvent(QMouseEvent *event)
{

    if (event->button() == Qt::LeftButton) {
        QPointF Raw = event->pos();
        int xcenter = (magIImage.width() -1)/2;
        int ycenter = (magIImage.height()-1)/2;
        int rad = sqrt(pow(Raw.x()-xcenter,2)+pow(Raw.y() - ycenter,2));

        emit updateFilterSize(rad/scale);
        capture = true;
    }



}
