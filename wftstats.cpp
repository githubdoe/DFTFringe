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
#include "wftstats.h"
#include <qwt_scale_draw.h>
#include "wavefront.h"
#include "zernikedlg.h"
#include <qwt_plot_histogram.h>
#include <QTextStream>
class wftNameScaleDraw: public QwtScaleDraw
{
public:
    QVector<wavefront*> names;
    int currentNdx;
    wftNameScaleDraw(QVector<wavefront*> &nameList, int nx)
    {
        names = nameList;
        currentNdx = nx;
        setTickLength( QwtScaleDiv::MajorTick, 10 );
        setTickLength( QwtScaleDiv::MinorTick, 1 );
        setTickLength( QwtScaleDiv::MediumTick, 5 );

        setLabelRotation( -45.0 );
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        setSpacing( 5 );
    }
    virtual QwtText label( double value ) const
    {
        if (value >= names.size())
            return QString("");
        QString n = names[int(((int)value + currentNdx) % names.size())]->name;
        QStringList l = n.split("/");
        if (l.size() > 1)
            n = l[l.size()-2] + "/"+ l[l.size()-1];
        return n;
    }
};

class StrehlScaleDraw: public QwtScaleDraw
{
public:
    StrehlScaleDraw()
    {

    }
    virtual QwtText label( double value ) const
    {
        double st =(2. *M_PI * value);
        st *= st;
        const double  e = 2.7182818285;
        double strl = pow(e,-st);
        return QString().sprintf("%6.3lf",strl);
    }
};

class ZernScaleDraw: public QwtScaleDraw
{
public:
    QVector<QString> m_names;
    ZernScaleDraw(QVector<QString> names)

    {
        m_names = names;
        setTickLength( QwtScaleDiv::MajorTick, 10 );
        setTickLength( QwtScaleDiv::MinorTick, 1 );
        setTickLength( QwtScaleDiv::MediumTick, 5 );

        setLabelRotation( -30.0 );
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );

        setSpacing( 5 );
    }

    virtual QwtText label( double value ) const
    {
        if (value == 2.){
            QwtText x( QString("wave front RMS"));
            x.setColor(Qt::red);

            return x;
        }
        if (value < 4)
            return QString("");
        if (value > m_names.size() +3)
            return QString("");

        QwtText t(QString(m_names[int(value-4)]));
        if (value == 6.)
            t.setColor(Qt::red);
        return t;

    }
};

wftStats::wftStats(mirrorDlg *md):
     wftPlot(new QwtPlot(0)), zernPlot(new QwtPlot(0)), wftHistogram(new QwtPlot(0)),md(md),m_showWftNames(false),
     m_doInputs(true), m_doZernGroup(false), zernFrom(0),zernTo(5)
{
}

void wftStats::showStats(){

}

void wftStats::computeZernStats( int ndx){


    zNames.clear();
    zerns.clear();
    zernMinMax.clear();
    zernStd.clear();
    int col = 4;
    for (int zern = 4; zern < Z_TERMS-1; ++zern){
        QString zname(zernsNames[zern]);
        bool isPair = !QString(zernsNames[zern]).contains("Spheri");
        cv::Mat c = m_Zerns.col(zern);
        QVector<double> zpoints;
        for (int i = 0; i < c.rows; ++i){

            int row = (i + ndx) % c.rows;
            double v = c.at<double>(row);
            if (isPair){
                cv::Mat c2 = m_Zerns.col(zern+1);
                double v2 = c2.at<double>(row);
                double s = sqrt(v * v + v2 * v2);
                v = s;
                zname += " ";
                zname += zernsNames[zern+1];
            }

            if (i == 0) {
                zNames << zname;
                //qDebug() << zname;
            }
            zpoints << v;
        }
        zerns << zpoints;

        std::vector<double > zvec = zpoints.toStdVector();
        cv::Mat aSampleOfZerns(zvec);
        std::sort( zvec.begin(), zvec.end() ); // sort the Zvalues
        double median;
        int sampleCnt = zvec.size();
        if (sampleCnt % 2==0) {
            median = (zvec[zvec.size()/2-1]+zvec[zvec.size()/2])/2;
        }
        else {
            median = zvec[(zvec.size()-1)/2];
        }
        double top = zvec[sampleCnt * .25];
        double bottom = zvec[sampleCnt * .75];
        double min;
        double max;
        minMaxIdx(aSampleOfZerns, &min,&max);

        zernMinMax.append( QwtIntervalSample( col, min, max ) );
        //qDebug() << "minmax " << col << " " << min << " " << max;

        zernStd.append(QwtIntervalSample(col,bottom,top));
        zernStd.append(QwtIntervalSample(col++, bottom, median));
        if (isPair) ++zern;
    }
}

QVector<int> histoz(const std::vector<double> data, int bins, double min, double max){
    QVector<int> h(bins, 0);
    double interval = (max - min)/bins;
    for (unsigned int i = 0; i < data.size(); ++i){
        double to = interval;
        for (int j = 0; j <  bins; ++j){
            if (data[i] < to) {
                ++h[j];
                break;
            }
            to += interval;
        }
    }
    return h;
}

// create the matrix of zerns X wavefronts and spherical running avg;
void wftStats::computeWftStats( QVector<wavefront*> wavefronts, int ndx){
    spherical.clear();
    sphericaRunningAvg.clear();
    m_Zerns = cv::Mat(wavefronts.size(),Z_TERMS,CV_64F,0.);
    int row = 0;
    double sperAvg = 0;
    for (int i = 0; i < wavefronts.size(); ++i)
    {
        wavefront *wf = wavefronts[(ndx + i )% wavefronts.size()];
        QPolygonF zernPoints;

        for(int ndx = 0; ndx < Z_TERMS; ++ndx){
            double v = wf->InputZerns[ndx];

            // apply software Null if needed
            if (ndx == 8 and md->doNull)
                v -= md->z8 * md->cc;
            double Sigma = computeRMS(ndx,v) * 550./md->lambda;

            if (ndx == 8) {
                spherical << QPointF(row,Sigma);
                sperAvg += Sigma;
                sphericaRunningAvg << QPointF(row,sperAvg/(i+1));
            }

            m_Zerns.at<double>(row,ndx) =  Sigma;

        }
        ++row;
    }


}
double histInterval ;
QVector<int> histox(const std::vector<double> data, int bins){
    QVector<int> h(bins, 0);

    for (unsigned int i = 0; i < data.size(); ++i){
        qDebug() << data[i];
        double bound = histInterval;
        for (int j = 0; j < bins; ++j){
            if (data[i] <= bound) {
                ++h[j];
                qDebug() << "hist " << j << " " << data[i] << " " << bound;
                break;
            }
        bound += histInterval;

        }
    }
    return h;
}

void wftStats::computeWftRunningAvg( QVector<wavefront*> wavefronts, int ndx){

    outliersInner.clear();
    outliersOuter.clear();
    // normalize the size to the most common size
    int last = wavefronts.length();
    QHash<QString,int> sizes;
    for (int i = 0; i < last; ++i){
        QString size;
        size.sprintf("%d %d",wavefronts[i]->workData.rows, wavefronts[i]->workData.cols);
        if (*sizes.find(size))
        {
            ++sizes[size];
        }
        else
            sizes[size] = 1;
    }
    int max = 0;
    QString maxkey;
    foreach(QString v, sizes.keys()){
        int a = sizes[v];
        if (a > max) {
            max = a;
            maxkey = v;
        }
    }
    int rrows, rcols;

    QTextStream s(&maxkey);

    s >> rrows >> rcols;
    cv::Mat mask = wavefronts[0]->workMask.clone();
    cv::resize(mask,mask,cv::Size(rcols,rrows));
    QVector<wavefront*> twaves = wavefronts;
    cv::Mat sum = cv::Mat::zeros(rrows,rcols, wavefronts[ndx]->workData.type());

    avgPoints.clear();
    wftPoints.clear();
    trueNdx.clear();
    inrange.clear();
    for (int j = 0; j < last; ++j){
        int i = (ndx + j) % wavefronts.size();
        //i = samndx[j];
        cv::Mat resized = twaves[i]->workData.clone();
        if (twaves[i]->workData.rows != rrows || twaves[i]->workData.cols != rcols){
            cv::resize(twaves[i]->workData,resized, cv::Size(rcols, rrows));
        }
        sum += resized;
        cv::Mat avg = sum/(j+1);
        cv::Scalar mean,std;
        cv::meanStdDev(resized,mean,std,mask);
        double stdi = std.val[0]* md->lambda/550.;
        cv::meanStdDev(avg,mean,std,mask);
        avgPoints << QPointF(j,std.val[0] * md->lambda/550.);
        wftPoints << QPointF(j,stdi);
        trueNdx << i;
    }
    cv::Mat wftStatsx(avgPoints.size(), 1, CV_64F);  // used to compute std and mean
    std::vector<double> vecWftRMS;    // used to compute quartiles

    for (int ndx = 0; ndx < wftPoints.size(); ++ndx){
        wftStatsx.at<double>(ndx) = wftPoints[ndx].y();
        qDebug() << "wft " << wftPoints[ndx].y();
        vecWftRMS.push_back( wftPoints[ndx].y());
    }

    std::sort( vecWftRMS.begin(), vecWftRMS.end() ); // sort vecFromMat

    int sampleCnt = vecWftRMS.size();
    if (sampleCnt % 2==0) {
        wftsMedian = (vecWftRMS[vecWftRMS.size()/2-1]+vecWftRMS[vecWftRMS.size()/2])/2;
    }
    else {
        wftsMedian = vecWftRMS[(vecWftRMS.size()-1)/2];
    }
    double bottom = vecWftRMS[sampleCnt * .25];
    double top = vecWftRMS[sampleCnt * .75];
    // outlier detection http://www.itl.nist.gov/div898/handbook/prc/section1/prc16.htm

    IQ = top - bottom;
    double lowerOuterFence = bottom - 3. * IQ;
    double upperOuterFence = top + 3. * IQ;
    double lowerInnerFence = bottom - 1. * IQ;
    double upperInnerFence = top + 1.5 * IQ;
    qDebug() << "Inner Fence " << upperInnerFence << " " << lowerInnerFence;
    outliers.clear();
    for (int ndx = 0; ndx < wftPoints.size(); ++ndx){
        double v = wftStatsx.at<double>(ndx);
        qDebug() << trueNdx[ndx] << " " << v << " " << wavefronts[trueNdx[ndx]]->name;
        if (v <= lowerOuterFence || v >= upperOuterFence){
            outliersOuter << QPointF(2,v);
            outliers << trueNdx[ndx];
            qDebug() << "outer " << v;
        }
        else if (v <= lowerInnerFence || v >= upperInnerFence) {
            outliersInner << QPointF(2,v);
            outliers << trueNdx[ndx];
            qDebug() << "inner " << v;
        }
        else {
            inrange << QPointF(2,v);
        }
    }

    cv::Scalar mean,std;
    cv::meanStdDev(wftStatsx,mean,std);
    double min;
    double maxx;
    minMaxIdx(wftStatsx, &min,&maxx);
    zernMinMax.append( QwtIntervalSample( 2, min, maxx ) );
    zernStd.append(QwtIntervalSample(2,top,bottom));

    int histSize = 11;

    histInterval = (maxx)/histSize;
    //vecWftRMS.clear();
    //vecWftRMS.push_back(histSize);
    m_hist = histox(vecWftRMS, histSize);
}
QwtPlot *wftStats::makeHistoPlot(){

    wftHistogram->detachItems( QwtPlotItem::Rtti_PlotItem);
    wftHistogram->setWindowTitle("Wavefront Stats histogram.");

    QwtPlotHistogram *histPlot = new QwtPlotHistogram("Wave Fronts RMS");

    QVector<QwtIntervalSample> histData;

    for (int i = 0; i < m_hist.size(); ++i){
        QwtInterval interv((i) * histInterval, (i+1)*histInterval);
        //qDebug() << interv.minValue() << " " << interv.maxValue() << " " << hist[i];
        histData << QwtIntervalSample((double)m_hist[i],interv);

    }
    histPlot->setSamples(histData);
    histPlot->attach(wftHistogram);
    wftHistogram->setAxisTitle(QwtPlot::xBottom,"RMS");
    wftHistogram->setTitle("Wave Front RMS Histogram");
    wftHistogram->resize(640,480);
    //wftHistogram->show();
    return wftHistogram;
}

QwtPlot *wftStats::makeWftPlot(QVector<wavefront *> &wavefronts, int ndx){

    wftPlot->detachItems( QwtPlotItem::Rtti_PlotItem);
    wftPlot->setWindowTitle("wavefront stats");

    wftPlot->enableAxis( QwtPlot::yRight );
    wftPlot->insertLegend( new QwtLegend() , QwtPlot::TopLegend);
    QFont label_font("Helvetica");
    label_font.setPointSize(10);
    wftPlot->legend()->setFont(label_font);
    if (m_showWftNames)
        wftPlot->setAxisScaleDraw(QwtPlot::xBottom, new wftNameScaleDraw(wavefronts, ndx));
    else
        wftPlot->setAxisScaleDraw(QwtPlot::xBottom, new QwtScaleDraw);

    if (m_doInputs){
        QwtPlotCurve *InputWfts = new QwtPlotCurve();
        InputWfts->setSamples(wftPoints);
        InputWfts->setPen(Qt::black,1);
        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
                                           QBrush( Qt::red ), QPen( Qt::black, 2 ), QSize( 8, 8 ) );
        InputWfts->setSymbol( symbol );
        InputWfts->attach(wftPlot);
        InputWfts->setTitle("Input .wft's");
    }
    if (m_showWftNames)
        wftPlot->setAxisScaleDraw(QwtPlot::xBottom, new wftNameScaleDraw(wavefronts, ndx));
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setMajorPen( Qt::gray, 0, Qt::DotLine );
    grid->setMinorPen( Qt::gray, 0 , Qt::DotLine );
    grid->enableYMin(true);
    grid->attach(wftPlot);
    QwtPlotCurve *runingavg = new QwtPlotCurve(QString().sprintf("RMS of Running Average of wavefronts"));
    runingavg->setSamples(avgPoints);
    runingavg->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    runingavg->setPen(Qt::blue,2);
    runingavg->attach(wftPlot);
    const char *colors[] =
    {
        "Green",
        "LightSalmon",
        "SteelBlue",
        "Yellow",
        "Fuchsia",
        "PaleGreen",
        "PaleTurquoise",
        "Cornsilk",
        "HotPink",
        "Peru",
        "Maroon"
    };
    if (m_doZernGroup){
        const int zernToCombinedNx[] = {0,0,0,0,0,0,1,1,2,3,3,4,4,5,5,6,7,7,8,8,9,9,10,10,11,
                                       12,12,13,13,14,14,15,15,16,16,17};

        for (int zern = zernToCombinedNx[zernFrom]; zern <= zernToCombinedNx[zernTo]; ++zern){
            QwtPlotCurve *zcurve = new QwtPlotCurve(QString().sprintf("%s",zNames[zern].toStdString().c_str()));
            QPolygonF points;
            int cnt = 0;
            foreach(double v, zerns[zern]){
                points << QPointF(cnt++,v);
            }

            zcurve->setSamples(points);
            zcurve->setPen(QColor(colors[zern%10]),2);
            zcurve->attach(wftPlot);
        }
    }
    wftPlot->replot();
    QwtScaleDiv sd = wftPlot->axisScaleDiv(QwtPlot::yLeft);
    double up = sd.upperBound();
    double lower = sd.lowerBound();


    wftPlot->setAxisScaleDraw(QwtPlot::yRight,new StrehlScaleDraw());
    QwtScaleEngine * se1 = wftPlot->axisScaleEngine(QwtPlot::yRight);
    QwtScaleDiv sd1 = se1->divideScale(lower,up, 20,1);
    QwtScaleEngine *se = wftPlot->axisScaleEngine(QwtPlot::xBottom) ;
    wftPlot->setAxisScaleDiv(QwtPlot::yRight, sd1);

    double x1 = 0;
    double x2 = wavefronts.size()-1;
    double step =  ceilf(x2/20);
    QwtScaleDiv sd2 = se->divideScale(x1, x2, x2, 1, step);
    wftPlot->setAxisScaleDiv(QwtPlot::xBottom,sd2);
    //wftPlot->setAxisScale(QwtPlot::yRight,lower, up );
    wftPlot->setAxisTitle(0,"RMS");
    wftPlot->setAxisTitle(QwtPlot::yRight,"Strehl");
    wftPlot->setAxisTitle(QwtPlot::xBottom,"Wave Front");
    wftPlot->setAxisScaleDiv(QwtPlot::yRight, sd1);
    wftPlot->resize(800,600);
    wftPlot->replot();
    QwtScaleDiv stldiv = wftPlot->axisScaleDiv(QwtPlot::xBottom);
    //wftPlot->show();
    return wftPlot;
}

QwtPlot *wftStats::makeZernPlot(){
    zernPlot->detachItems( QwtPlotItem::Rtti_PlotItem);

    zernPlot->setWindowTitle("zernike term stats");
    QwtIntervalSymbol *bar = new QwtIntervalSymbol( QwtIntervalSymbol::Bar );
    QwtIntervalSymbol *box = new QwtIntervalSymbol( QwtIntervalSymbol::Box );
    box->setWidth(15);

    QwtPlotIntervalCurve *zernMinMaxCurve = new QwtPlotIntervalCurve("minmax");
    QwtPlotIntervalCurve *zernStdCurve = new QwtPlotIntervalCurve("std");
    zernMinMaxCurve->setSymbol( bar );
    zernMinMaxCurve->setSamples( zernMinMax );
    zernMinMaxCurve->attach( zernPlot );
    zernMinMaxCurve->setStyle( QwtPlotIntervalCurve::NoCurve );
    zernPlot->setAxisTitle(QwtPlot::xBottom, "Zernike terms");
    zernPlot->setAxisTitle(QwtPlot::yLeft, "RMS");
    zernPlot->setAxisScale(QwtPlot::xBottom,0,22,1);
    zernPlot->setAxisScaleDraw( QwtPlot::xBottom, new ZernScaleDraw(zNames) );
    zernStdCurve->setSymbol( box );
    zernStdCurve->setSamples( zernStd );
    zernStdCurve->attach( zernPlot);
    zernStdCurve->setStyle( QwtPlotIntervalCurve::NoCurve );

    if (outliersInner.size()){
        QwtPlotCurve *outers = new QwtPlotCurve("Outer outliers");
        outers->setStyle( QwtPlotCurve::NoCurve );
        outers->setPen(Qt::magenta,1);


        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::HLine,
            QBrush( Qt::magenta ), QPen( Qt::magenta, 2 ), QSize( 10, 5 ) );
        outers->setSymbol( symbol );
        outers->setSamples(outliersInner);
        outers->attach(zernPlot);
    }
    if (outliersOuter.size()){
        QwtPlotCurve *outers = new QwtPlotCurve("Outer outliers");
        outers->setStyle( QwtPlotCurve::NoCurve );
        outers->setPen(Qt::red,1);

        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::HLine,
            QBrush( Qt::red ), QPen( Qt::red, 2 ), QSize( 10, 5 ) );
        outers->setSymbol( symbol );
        outers->setSamples(outliersOuter);
        outers->attach(zernPlot);
    }
    QwtPlotCurve *medn = new QwtPlotCurve("inrange");
    medn->setStyle( QwtPlotCurve::NoCurve );
    medn->setPen(Qt::black,5);
    QwtSymbol *mesymbol = new QwtSymbol( QwtSymbol::HLine,
        QBrush( Qt::black ), QPen( Qt::black, 2 ), QSize( 20, 20 ) );
    medn->setSymbol( mesymbol );
    QPolygonF mm;
    mm << QPointF(2,wftsMedian);
    medn->setSamples(mm);
    medn->attach(zernPlot);
    QwtPlotCurve *inners = new QwtPlotCurve("inrange");
    inners->setPen(Qt::darkGreen,10);

    QwtSymbol *innrsymbol = new QwtSymbol( QwtSymbol::HLine,
        QBrush( Qt::darkGreen ), QPen( Qt::darkGreen, 2 ), QSize( 5, 5 ) );
    inners->setStyle( QwtPlotCurve::NoCurve );
    inners->setSymbol( innrsymbol );
    inners->setSamples(inrange);
    inners->attach(zernPlot);
    zernPlot->resize(800,400);
    zernPlot->replot();
    //zernPlot->show();
    return zernPlot;
}


