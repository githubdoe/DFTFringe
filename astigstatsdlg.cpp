#include "astigstatsdlg.h"
#include "ui_astigstatsdlg.h"
#include "circleutils.h"
#include "circle.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_symbol.h>
#include <qwt_legend.h>
#include <qwt_text.h>
#include <qwt_plot_renderer.h>
#include <qwt_interval_symbol.h>
#include <qwt_plot_intervalcurve.h>
#include <qwt_scale_engine.h>
#include <qwt_plot_marker.h>
#include <QDebug>
#include <math.h>
#include <algorithm>
#include <QFileDialog>
#include <QSettings>
#include <QList>
#include <QStringList>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <QScrollArea>
#include <QGroupBox>
#include <QPrinter>
#include <QTextDocument>
#include <QTextEdit>
#include <QDate>
#include <qwt_plot_item.h>
#include <qwt_legend_label.h>
#include "zernikedlg.h"
#include <QSpinBox>
#include <QLabel>
QString intro( "<p style = font-size:16px ;>Astigmatism values vary from sample to sample due to air currents and placement on the test stand."
               " This report displays that variabity and shows when enough samples have been obtained to"
               " average out the variability of air currents at each mirror rotation. The first plot shows an overview"
               " of all the samples.  The rest of the plots show the sample distribution of each group and an "
               " accumulating average of the x and y astigs of the samples in the group.  Usually each group "
               "is from one mirror rotation."
               " <br><br>If best fit has been selected then a thick black circle is drawn through the average of each"
               " group.  The center of the circle is the average system induced astig( mostly due to test stand.)  The radius of the circle "
               " is the best estimate of the magnitude of the astig on the mirror.  If standard diviation is selected thin "
               " circles are drawn around each group average represent one standard diviation of the samples in that group.</p>");

class Zoomer: public QwtPlotZoomer
{
public:
    QwtPlot * mPlot;
    Zoomer( QWidget *canvas, QwtPlot * plot ):
        QwtPlotZoomer( canvas ),mPlot(plot)
    {
        setRubberBandPen( QColor( Qt::lightGray ) );
        setTrackerMode( QwtPlotPicker::AlwaysOn );
    }

protected:
    virtual QwtText trackerTextF( const QPointF &p ) const
    {

        QString s("");
        const QwtScaleMap xMap = mPlot->canvasMap(2);
        const QwtScaleMap yMap = mPlot->canvasMap(QwtPlot::yLeft);

        const double cx = xMap.transform( p.x());
        const double cy = yMap.transform(p.y());

        const QwtPlotItemList& itmList = mPlot->itemList();
        for ( QwtPlotItemIterator it = itmList.begin();
            it != itmList.end(); ++it )
        {
            if (( *it )->rtti() == QwtPlotItem::Rtti_PlotMarker){
                QwtPlotMarker *mark = static_cast<QwtPlotMarker *>( *it );
                QPointF pm = mark->value();

                double delx = cx - xMap.transform(pm.x());
                double dely = cy - yMap.transform(pm.y());
                double r = sqrt(delx * delx + dely * dely);
                if (r < 4) {
                    s = mark->title().text();
                    break;
                }
            }
        }

        QwtText text( s );
        text.setColor( Qt::black );
        text.setFont(QFont("Arial",12));

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 170 );
        text.setBackgroundBrush( c );

        return text;
    }
};
astigStatsDlg::astigStatsDlg(QVector<wavefront *> wavefronts, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::astigStatsDlg),m_wavefronts(wavefronts), editor(0), PDFMode(false),
    distributionWindow(0), showSamples(false),runningAvgN(20),
    layout(0), m_usePolar(false)
{

    mndx = 0;
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    for(int i = 0; i < m_wavefronts.size(); ++i){
        QStringList paths = m_wavefronts[i]->name.split("/");
        QString name = paths[0];
        if (paths.size() > 1){
            name = paths[paths.size()-2]+"/"+paths[paths.size() -1];
        }
        QStringList items;
        items << name;
        for (int z = 0; z < Z_TERMS; ++z){
            items << QString().sprintf("%6.5lf",m_wavefronts[i]->InputZerns[z]);
        }
        m_zerns << items;
    }
    zoomer = new Zoomer( ui->mPlot->canvas(), ui->mPlot );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    ui->mPlot->setPalette( Qt::white );

    ui->bestFitCB->hide();
    dplot = new QwtPlot;
    plot();
}

astigStatsDlg::~astigStatsDlg()
{
    delete zoomer;

    delete ui;
}
class measure{
public:
    QString name;
    QPointF p;
    measure(QString n, QPointF _p):name(n),p(_p){};
    measure(){};
};
class RunningStat
    {
    public:
        RunningStat() : m_n(0),m_min(10000), m_max(-10000) {}

        void Clear()
        {
            m_n = 0;
        }

        void Push(double x)
        {
            m_n++;
            m_min = std::min(m_min,x);
            m_max = std::max(m_max,x);
            // See Knuth TAOCP vol 2, 3rd edition, page 232
            if (m_n == 1)
            {
                m_oldM = m_newM = x;
                m_oldS = 0.0;
            }
            else
            {
                m_newM = m_oldM + (x - m_oldM)/m_n;
                m_newS = m_oldS + (x - m_oldM)*(x - m_newM);

                // set up for next iteration
                m_oldM = m_newM;
                m_oldS = m_newS;
            }
        }
        double max() const {
            return m_max;
        }
        double min() const {
            return m_min;
        }
        int NumDataValues() const
        {
            return m_n;
        }

        double Mean() const
        {
            return (m_n > 0) ? m_newM : 0.0;
        }

        double Variance() const
        {
            return ( (m_n > 1) ? m_newS/(m_n - 1) : 0.0 );
        }

        double StandardDeviation() const
        {
            return sqrt( Variance() );
        }

    private:
        int m_n;
        double m_oldM, m_newM, m_oldS, m_newS, m_min,m_max;
    };

void astigStatsDlg::plot(){
    ui->mPlot->detachItems( QwtPlotItem::Rtti_PlotItem);
    int size = m_zerns.size();
    if (size <= 0)
        return;
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QStringList tmp = lastPath.split("/");
    QString defaultPath = tmp.last();
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(ui->mPlot);
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMinorPen(QPen(Qt::gray, 1, Qt::DotLine));
    QwtLegend *l = new QwtLegend();
    ui->mPlot->insertLegend( l, QwtPlot::TopLegend );
    l->setDefaultItemMode( QwtLegendData::Checkable );
    connect(l, SIGNAL(checked(QVariant,bool,int)), this, SLOT(showItem(QVariant ,bool,int)));
    int i = 0;

    //QMap<QString, QColor> colorAssign;

    // sort into rotation groups
    QMap<QString, QList<measure> > groups;

    foreach(QStringList data, m_zerns){
        QString name = data[0];
        QStringList paths = data[0].split("/");
        if (paths.size() == 1){
            paths.insert(0,defaultPath);
        }
        QString fileName = paths[paths.size()-1];
        double x = data[5].toDouble();
        double y = data[6].toDouble();

        if (groups.contains(paths[0])){
            groups[paths[0]].append(measure(fileName,QPointF(x,y)));
        }
        else {
            groups[paths[0]] = QList<measure>();
            groups[paths[0]].append(measure(fileName,QPointF(x,y)));
        }
    }

    // for each group
    double xmin = 1000;
    double ymin = 1000;
    double xmax = -1000;
    double ymax = -1000;
    int colorndx = 0;
    QMap<QString, QPointF> means;
    foreach(QString name, groups.keys()){
        int size = groups[name].size();
        double *xAstig = new double[size];
        double *yAstig = new double[size];
        RunningStat xstats, ystats;

        QMap<QString, QColor> avgColors;

        QColor color = QColor(Qt::GlobalColor( 7 + colorndx%12 ));
        QVector<QPointF> points;
        foreach (measure data, groups[name]){
            xAstig[i] = data.p.x();
            points << QPointF(data.p.x(), data.p.y());
            xstats.Push(xAstig[i]);
            xmin = std::min(xmin, data.p.x());
            xmax = std::max(xmax, data.p.x());
            ymin = std::min(ymin,data.p.y());
            ymax = std::max(ymax, data.p.y());
            yAstig[i] = data.p.y();
            ystats.Push(yAstig[i]);

            // plot marker for the point

            //QwtPlotMarker *m = new QwtPlotMarker(data.name.replace(".wft",""));
//            m->setValue(data.p.x(), data.p.y());
//            m->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, color,color, QSize(10,10)));
//            m->attach(ui->mPlot);

        }
        double xmean = xstats.Mean();
        double ymean = ystats.Mean();
        double xstd = xstats.StandardDeviation();
        double ystd = ystats.StandardDeviation();

        double rad = sqrt(xstd * xstd + ystd * ystd);
        if (!ui->onlyAverages->isChecked()){
            QwtPlotCurve *curve = new QwtPlotCurve(name.replace(".wft","") +
                             QString().sprintf("\n%6.4lf,%6.4lf \nSD: %6.4lf %6.4lf ",xmean, ymean, xstd,ystd));
            curve->setSamples(points);
            curve->setStyle(QwtPlotCurve::Dots);
            curve->setPen(color,10);
            curve->attach(ui->mPlot);
        }

        xmin = std::min(xmin, xstats.min());
        ymin = std::min(ymin, ystats.min());
        xmax = std::max(xmax, xstats.max());
        ymax = std::max(ymax, ystats.max());

        // plot the mean point
        means[name] = QPointF(xmean,ymean);
        QwtPlotMarker *meanm = new QwtPlotMarker(name.replace(".wft",""));
        meanm->setValue(xmean,ymean);
        meanm->setSymbol(new QwtSymbol(QwtSymbol::Ellipse, color,color, QSize(10,10)));
        QStringList paths = name.split("/");
        meanm->setTitle(paths[paths.size()-1].replace(".wft","")+ " " + QString().sprintf("\n%6.4lf,%6.4lf",xmean, ymean));
        meanm->setLabel(paths[paths.size()-1].replace(".wft",""));
        meanm->setLabelAlignment(Qt::AlignBottom);
        meanm->setItemAttribute(QwtPlotItem::Legend, false);
        meanm->attach(ui->mPlot);

        if (ui->sdCB->isChecked()){
            // plot the std


            QPolygonF Circle;
            QString sd= QString().sprintf("SD %6.4lf",rad);
            QwtPlotCurve *circleCv = new QwtPlotCurve();
            for (double rho = 0; rho <= 2 * M_PI; rho += M_PI/32.){
                Circle << QPointF(xmean + rad* cos(rho), ymean + rad * sin(rho));
            }
            circleCv->setPen(color,2);
            circleCv->setSamples(Circle);
            circleCv->setItemAttribute(QwtPlotItem::Legend, false);
            circleCv->attach(ui->mPlot);

        }
        ++colorndx;
        delete[] xAstig;
        delete[] yAstig;
    }

    // set the legended items to be checked.
    QwtPlotItemList items = ui->mPlot->itemList( QwtPlotItem::Rtti_PlotItem);
    for ( int i = 0; i < items.size(); i++ )
    {

        {
            const QVariant itemInfo = ui->mPlot->itemToInfo( items[i] );

            QwtLegendLabel *legendLabel =
                qobject_cast<QwtLegendLabel *>( l->legendWidget( itemInfo ) );
            if ( legendLabel){
                if (legendLabel->text().text() != ""){

                    legendLabel->setChecked( true );
                }
                else {

                    legendLabel->setChecked(false);
                    //items[i]->setItemAttribute(QwtPlotItem::Legend, false );
                }
            }


        }

    }
    // Draw orign lines
    QwtPlotMarker *mX = new QwtPlotMarker();
    mX->setLineStyle( QwtPlotMarker::VLine );
    mX->setLinePen( Qt::black, 3, Qt::DotLine );
    mX->setXValue(0 );
    mX->attach(ui->mPlot);
    QwtPlotMarker *mY = new QwtPlotMarker();
    mY->setLineStyle( QwtPlotMarker::HLine );
    mY->setLinePen( Qt::black, 3, Qt::DotLine );
    mY->setYValue(0 );
    mY->attach(ui->mPlot);

    // find the limits
    double xmag = std::max(fabs(xmin), fabs(xmax)) * 1.25;
    double ymag = std::max(fabs(ymin), fabs(ymax))*1.25;
    double maxmag = max(xmag,ymag);
    ui->mPlot->setAxisScale(QwtPlot::xBottom, -maxmag, maxmag);
    ui->mPlot->setAxisScale(QwtPlot::yLeft,   -maxmag, maxmag);
    ui->mPlot->setAxisTitle(QwtPlot::xBottom,"wavefront X astig");
    ui->mPlot->setAxisTitle(QwtPlot::yLeft,"wavefront Y Astig");


    // draw best fit circle
    if (ui->bestFitCB->isChecked()){
        double *xvals = new double[means.size()];
        double *yvals = new double[means.size()];
        int i = 0;
        foreach(QString name, means.keys()){
            xvals[i] = means[name].x();
            yvals[i] = means[name].y();
            ++i;
        }

        CircleData d(means.size(), xvals, yvals);

        Circle c;

        double xmean = 0;
        double ymean = 0;
        for (int i = 0; i < means.size(); ++i){
            xmean += xvals[i];
            ymean += yvals[i];
        }
        double avgRadius = 0;
        for (int i = 0; i < means.size(); ++i){
            double xdel = xvals[i] - xmean/means.size();
            double ydel = yvals[i] - ymean/means.size();

            avgRadius += sqrt(xdel * xdel + ydel * ydel);
        }
        Circle fittedcircle2 = Circle(xmean/means.size(), ymean/means.size(),
                                      avgRadius/means.size() );
        CircleFitByLevenbergMarquardtFull(d, fittedcircle2, .01, c);

        QPolygonF Circle;

        QwtPlotCurve *circleCv = new QwtPlotCurve(QString().sprintf("Best Fit astig mag %6.5lf",c.r));
        for (double rho = 0; rho <= 2 * M_PI; rho += M_PI/32.){
            Circle << QPointF(c.a + c.r* cos(rho), c.b +c.r * sin(rho));
        }
        QwtPlotMarker *avg = new QwtPlotMarker();

        avg->setSymbol(new QwtSymbol(QwtSymbol::Star1,QColor(0,0,0), QColor(0,0,0), QSize(30,30)));
        avg->setXValue(c.a);
        avg->setYValue(c.b);
        avg->attach(ui->mPlot);
        circleCv->setPen(QColor(Qt::black),3);
        circleCv->setSamples(Circle);
        circleCv->attach(ui->mPlot);
        delete[] xvals;
        delete[] yvals;

    }
    if (means.size() > 1){
        ui->bestFitCB->show();

    }
    else {
        ui->bestFitCB->hide();
    }
    ui->mPlot->setTitle(QString().sprintf("Summary of %d samples", m_zerns.size()));
    zoomer->setZoomBase();

    ui->mPlot->replot();
}

void astigStatsDlg::showItem(QVariant item, bool on, int /*ndx*/){
    qDebug() << "item " << item;
    QwtPlotItem *t = qvariant_cast<QwtPlotItem *>(item);
    t->setVisible(on);
    ui->mPlot->replot();
}

void astigStatsDlg::on_zernikePB_pressed()
{
    QSettings set;
    QString path = set.value("mirrorConfigFile").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString dd = info.dir().absolutePath();
    QString fName = QFileDialog::getOpenFileName(0,
        tr("csv file"), dd + "//Zerns.csv");

    if (fName.isEmpty())
        return;
    QFile f(fName);
    QTextStream in(&f);
    QFileInfo info2(f);


    if (f.open(QIODevice::ReadOnly)) {
        QString line;
        do {
            line = in.readLine();
            if (line.size() == 0)
                continue;
            if (line[0] == '#')
                continue;
            QStringList data = line.split(',');
            if (!data[0].contains("/")){
                data[0] = info2.absoluteDir().dirName() + "/"+ data[0];
            }
            m_zerns << data;

        } while (!line.isNull());
    }
    plot();
}


void astigStatsDlg::on_bestFitCB_clicked(bool /*checked*/)
{
    plot();
}
#include <QList>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <qwt_plot_histogram.h>
#define SDHEIGHT 250
QwtPlot *makeSDPlot(cv::Mat hist, double min, double max, double mean, int size, int width, int height){
    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int bins = size;
    // hue varies from 0 to 179, see cvtColor
    QwtPlotHistogram *histPlot = new QwtPlotHistogram("Wavefronts PV");
    histPlot->setStyle(QwtPlotHistogram::Columns);
    QVector<QwtIntervalSample> histData;
    double histInterval = (max - min)/bins;
    for (int i = 0; i < bins; ++i){
        QwtInterval interv(mean + min + (i) * histInterval, mean + min+ (i+1)*histInterval);
        histData << QwtIntervalSample((double)(hist.at<float>(i)),interv);
    }
    QwtPlotMarker *mmean = new QwtPlotMarker();
    mmean->setLineStyle( QwtPlotMarker::VLine );
    mmean->setLinePen( Qt::red, 1 );
    mmean->setLabel("mean " + QString().number(mean))  ;
    mmean->setLabelAlignment(Qt::AlignTop | Qt::AlignRight);
    mmean->setXValue(mean );

    QwtPlot *pl = new QwtPlot;
    mmean->attach(pl);
    histPlot->setSamples(histData);
    histPlot->attach(pl);
    pl->resize(width, height);
    pl->setPalette( Qt::white );
    pl->replot();
    return pl;
}

QwtPlot *astigStatsDlg::avgPlot(cv::Mat x, cv::Mat y, int width){
    RunningStat xrs;
    RunningStat yrs;

    QwtPlot *plot = new QwtPlot();
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(plot);
    grid->enableXMin(true);
    grid->enableYMin(true);
    grid->setMinorPen(QPen(Qt::gray,1, Qt::DotLine));
    QVector<QPointF> xavgPoints;
    QVector<QPointF> yavgPoints;
    QVector<QPointF> xPoints;
    QVector<QPointF> yPoints;
    QVector<QPointF> xdel;
    QVector<QPointF> ydel;
    QVector<QPointF> runningAvgx;
    QVector<QPointF> runningAvgy;
    QwtPlotCurve *xavgc = new QwtPlotCurve();
    QwtPlotCurve *yavgc = new QwtPlotCurve();
    QwtPlotCurve *xvalsc = new QwtPlotCurve();
    QwtPlotCurve *yvalsc = new QwtPlotCurve();

    QwtPlotCurve *runx = new QwtPlotCurve(QString((m_usePolar? "mag ":"x "))+ QString("running avg"));
    QwtPlotCurve *runy = new QwtPlotCurve(QString((m_usePolar? "angle ": "y ")) + QString("running avg"));
    double runAvgx, runAvgy;
    double n = runningAvgN;
    double sumx = 0.;
    double sumy = 0;
    for (int i = 0; i < x.rows; ++i){
        double xz = x.at<float>(i);
        double yz = y.at<float>(i);
        double angle, mag;
        if (m_usePolar){
            angle = atan2(yz,xz)/2;
            mag = sqrt(xz * xz + yz * yz);

            xz = mag;
            yz = angle * 57.2958;
        }
        if (i < n){
            sumx += xz;
            sumy += yz;
            runAvgx = sumx/(i+1);
            runAvgy = sumy/(i+1);


        }

        else{
            runAvgx = (runAvgx * (n-1)/n) + xz/n;
            runAvgy = (runAvgy * (n-1)/n) + yz/n;
        }
        runningAvgx << QPointF(i, runAvgx);
        runningAvgy << QPointF(i, runAvgy);
        xrs.Push(xz);
        yrs.Push(yz);
        xavgPoints << QPointF(i, xrs.Mean());
        xPoints << QPointF(i, xz);
        yavgPoints << QPointF(i, yrs.Mean());
        yPoints << QPointF(i, yz);
    }
    runx->setPen(Qt::black,3);
    runx->setSamples(runningAvgx);
    runx->attach(plot);
    runy->setSamples(runningAvgy);
    runy->setPen(Qt::black,3);
    runy->attach(plot);
    plot->setAxisScale(QwtPlot::yLeft,
                      min(xrs.min(), yrs.min()) - .01, max(xrs.max(), yrs.max()) + .1);
    if (m_usePolar){
        plot->enableAxis(QwtPlot::yRight);
        plot->setAxisScale(QwtPlot::yRight, yrs.min() - 7, yrs.max() + 3);
        plot->setAxisScale(QwtPlot::yLeft, xrs.min() - .01, xrs.max() + .1);
        plot->setAxisTitle(QwtPlot::yRight, "angle");
        yavgc->setYAxis(QwtPlot::yRight);
        runy->setYAxis(QwtPlot::yRight);
    }


    xavgc->setSamples(xavgPoints);
    xavgc->setPen(Qt::blue,1);
    QwtSymbol *xsymbol = new QwtSymbol( QwtSymbol::Ellipse,
                                       QBrush( Qt::blue ), QPen( Qt::blue, 0 ), QSize( 3, 3 ) );
    xvalsc->setSymbol( xsymbol );
    xvalsc->setSamples(xPoints);
    xavgc->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    xavgc->setLegendIconSize(QSize(20,20));
    xavgc->attach(plot);
    xvalsc->setPen(Qt::blue, 1, Qt::DotLine);
    if (showSamples)xvalsc->attach(plot);
    xvalsc->setItemAttribute(QwtPlotItem::Legend, false);
    yvalsc->setItemAttribute(QwtPlotItem::Legend, false);

    xavgc->setTitle(QString(m_usePolar? "Mag ":"X ") +QString("mean"));
    yavgc->setLegendAttribute( QwtPlotCurve::LegendShowLine );
    yavgc->setLegendIconSize(QSize(20,20));

    yvalsc->setPen(Qt::magenta, 1);
    yavgc->setSamples(yavgPoints);
    yavgc->setPen(Qt::magenta,1 );

    QwtSymbol *ysymbol = new QwtSymbol( QwtSymbol::Ellipse,
                                       QBrush( Qt::magenta ), QPen( Qt::magenta, 1 ), QSize( 3, 3 ) );
    yvalsc->setSymbol( ysymbol );
    yvalsc->setSamples(yPoints);
    if (showSamples)yvalsc->attach(plot);
    yavgc->attach(plot);
    yavgc->setTitle(QString(m_usePolar? "angle ":"y ") + "mean");
    plot->setTitle("accumulating astig");


    plot->resize(width * .8,width * .4);

    plot->setAxisTitle(QwtPlot::xBottom, "sample number");
    plot->setAxisTitle(QwtPlot::yLeft, "Zernike astig");
    plot->setCanvasBackground(QColor(252,252,252));
    plot->replot();
    return plot;
}
#define WIDTH 650
void astigStatsDlg::on_savePdf(){

    QSettings set;
    PDFMode = true;

    on_distribution_clicked();
    QString last_path = set.value("lastPath", ".").toString();
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Save PDF",
            last_path + "/astigVariability.pdf","*.pdf");

    if (fileName.isEmpty())
        return;
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    // build the pdf
    QPrinter printer(QPrinter::HighResolution);
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( true );
    printer.setOutputFileName( fileName );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setResolution(96);
    printer.setPaperSize(QPrinter::Imperial8x10);

    QTextDocument *doc = editor->document();
    doc->print(&printer);
    PDFMode = false;
    on_distribution_clicked();
}
void astigStatsDlg::showSamplesChanged(bool arg){
    showSamples = arg;
    on_distribution_clicked();
}

void astigStatsDlg::runningNChanged(int arg){
    runningAvgN = arg;
    on_distribution_clicked();
}
void astigStatsDlg::usePolar(bool flag){
    m_usePolar = flag;
    on_distribution_clicked();
}

void astigStatsDlg::on_distribution_clicked()
{
    bool first = true;
    if (distributionWindow == 0){
        distributionWindow = new QWidget();
        toolLayout = new QHBoxLayout;
        layout = new QVBoxLayout;
        layout->addLayout(toolLayout);

        distributionWindow->setLayout(layout);
        // build the toolBar
        QPushButton *pbMakePDF = new QPushButton("Save as PDF");
        connect(pbMakePDF, SIGNAL(pressed()), this, SLOT(on_savePdf()));
        QCheckBox *showSamplesCB = new QCheckBox("Show individual samples");
        showSamplesCB->setChecked(showSamples);
        connect(showSamplesCB, SIGNAL(clicked(bool)), this, SLOT(showSamplesChanged(bool)));
        QSpinBox *runningAvgNSB = new QSpinBox;
        runningAvgNSB->setValue(runningAvgN);
        runningAvgNSB->setMaximum(1000);
        runningAvgNSB->setSingleStep(10);
        connect(runningAvgNSB, SIGNAL(valueChanged(int)), this, SLOT(runningNChanged(int)));
        QCheckBox *polarCB = new QCheckBox("Polar");
        connect(polarCB, SIGNAL(clicked(bool)), this, SLOT(usePolar(bool)));


        toolLayout->addWidget(showSamplesCB);
        toolLayout->addWidget(new QLabel("running average samples:"),0,Qt::AlignRight);
        toolLayout->addWidget(runningAvgNSB);
        toolLayout->addWidget(pbMakePDF,0,Qt::AlignRight);
        toolLayout->addWidget(polarCB);
        editor = new QTextEdit;
        layout->addWidget(editor);
    }
    else {
        first = false;
        editor->clear();

    }

    if (PDFMode){
        distributionWindow->resize(800,800);
    }
    else distributionWindow->resize(900,800);
    distributionWindow->setWindowTitle("Sample Distributions");

    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QStringList tmp = lastPath.split("/");
    QString defaultPath = tmp.last();
    QVector<QwtPlot *> avgPlots;

    QMap<QString, QList<QPointF> > groups;
    foreach(QStringList data, m_zerns){
        QString name = data[0];
        QStringList paths = data[0].split("/");
        if (paths.size() == 1){
            paths.insert(0,defaultPath);
        }
        double x = data[5].toDouble();
        double y = data[6].toDouble();

        if (groups.contains(paths[0])){
            groups[paths[0]].append(QPointF(x,y));
        }
        else {
            groups[paths[0]] = QList<QPointF>();
            groups[paths[0]].append(QPointF(x,y));
        }

    }

    QVector<QGroupBox *> plots;
    // build the pdf


    QPrinter printer(QPrinter::ScreenResolution);
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( false );

    printer.setOutputFormat( QPrinter::PdfFormat );

    printer.setPageSize(QPageSize(QPageSize::A4));
    int width = printer.pageLayout().paintRectPixels(printer.resolution()).width()-200;

    int height = printer.pageLayout().paintRectPixels(printer.resolution()).height();
    qDebug() << "set resolution" << printer.resolution() << width << height;
    //pbMakePDF->setFixedWidth(100);


    if (PDFMode){

        editor->resize(printer.pageRect().size());
    }
    QTextDocument *doc = editor->document();
    QList<QString> doc1Res;

    doc->setPageSize(printer.pageRect().size()); // This is necessary if you want to hide the page number

    QString html = ("<html><head/><body><h1><center>Astig Variability analisys</center></h1>");

    html.append(" <center><h2>   <font color='grey'>" + QDate::currentDate().toString() +
                " " +QTime::currentTime().toString()+"</font></center></h2>");

    if (PDFMode){
        html.append(intro);
        ui->mPlot->resize(WIDTH,800);
        ui->mPlot->replot();

        QPixmap pm = ui->mPlot->grab();

        QString imageName = "full.png";
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(pm.toImage()));
        html.append("<p><img src='" +imageName + "'/></p>");
    }
    // for each group
    foreach(QString Name, groups.keys()){

        QList<QPointF> items = groups[Name];
        int size = items.size();
        float *xAstig = new float[size];
        float *yAstig = new float[size];
        for (int i = 0; i < size; ++i){
            xAstig[i] = items[i].x();
            yAstig[i] = items[i].y();
        }

        // compute mean
        cv::Mat xvalues = cv::Mat(size,1,CV_32F, xAstig);
        cv::Mat yvalues = cv::Mat(size,1,CV_32F, yAstig);
        double xmin,xmax,ymin,ymax;
        cv::Scalar xmean,xstd,ymean,ystd;
        cv::meanStdDev(xvalues,xmean,xstd);
        cv::meanStdDev(yvalues, ymean, ystd);
        cv::Mat xsd = xvalues - xmean;
        cv::Mat ysd = yvalues - ymean;
        minMaxIdx(xsd, &xmin,&xmax);
        minMaxIdx(ysd, &ymin,&ymax);

        cv::Mat xhist;
        cv::Mat yhist;

        int bins = 50;
        int histSize[] = {bins};
        float hranges[] = { (float)xmin - .02f, (float)xmax + .02f };
        const float* ranges[] = { hranges };
        int channels[] = {0};
        cv::calcHist( &xsd, 1, channels, cv::Mat(),
                 xhist, 1, histSize, ranges,
                 true, // the histogram is uniform
                 false );
        float yranges[] = {(float)ymin - .02f, (float)ymax + .02f};
        const float* yyranges[] = {yranges};
        cv::calcHist( &ysd, 1, channels, cv::Mat(),
                 yhist, 1, histSize, yyranges,
                 true, // the histogram is uniform
                 false );
        //xvalues -= xmin;
        //yvalues -= ymin;

        QwtPlot *avgplot  = avgPlot(xvalues, yvalues, width);

        avgplot->insertLegend( new QwtLegend(), QwtPlot::RightLegend );
        QwtPlot *xpl = makeSDPlot(xhist, xmin,xmax, xmean.val[0], bins, width * .5, width * .25);
        xpl->resize(width * .4,width * .25);
        xpl->setAxisTitle(QwtPlot::xBottom, " SD: " + QString().number(xstd.val[0]));

        xpl->setTitle(" X "+ QString().number(size) + " samples");
        QwtPlot *ypl = makeSDPlot(yhist, ymin,ymax, ymean.val[0], bins, width * .5, width * .25);




        ypl->setTitle(" Y " + QString().number(size) + " samples");
        ypl->setAxisTitle(QwtPlot::xBottom, " SD: " + QString().number(ystd.val[0]));

        html.append("<p style=\"page-break-before: always\"><br><center><h2> Sample Group " + Name +
                    "</h2></center></p><br><br>");

        QPixmap pm = xpl->grab();

        QString imageName = Name + "x.png";
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(pm.toImage()));

        html.append("<table border = 3 width = 100% ><tr><td><img src='" +imageName + "'/></td>");

        pm = ypl->grab();
        imageName = Name + "y.png";
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(pm.toImage()));

        html.append("<td><img src='" +imageName + "'/></td></tr>");


        pm = avgplot->grab();

        imageName = Name + "avg.png";
        doc->addResource(QTextDocument::ImageResource,  QUrl(imageName), QVariant(pm.toImage()));
        html.append("<tr><td colspan = \"2\"> <img src='" +imageName + "' ></td></tr></table>");

    }


    html.append("<p>");

    // add 2d plot

    html.append("</p>");
    html.append("</body></html>");

    editor->setHtml(html);
    editor->setDocument(doc);

    distributionWindow->show();
}

void astigStatsDlg::on_sdCB_clicked()
{
    plot();
}
#include "helpdlg.h"
void astigStatsDlg::on_help_clicked()
{
    HelpDlg dlg;
    QString html;
    QString path = qApp->applicationDirPath() + "/res/Help/astigsummary.html";
    QDesktopServices::openUrl(QUrl(path));
    /*QFile file(path);
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    html.append(stream.readAll());
    html.append("</body></html>");
    dlg.setHelpText(html);
    dlg.exec();*/
}

void astigStatsDlg::on_clearPb_clicked()
{
    m_zerns.clear();
    plot();
}

void astigStatsDlg::on_onlyAverages_clicked()
{
    plot();
}

void astigStatsDlg::on_savePB_clicked()
{
    QSettings set;
    QString path = set.value("lastPath",".").toString();
    QFile fn(path);

    QString fName = QFileDialog::getSaveFileName(0,
        tr("Save Zernike values"), path + "//Zerns.csv");


    if (fName.isEmpty())
        return;
    QFile f(fName);
    QTextStream out(&f);
    if (f.open(QIODevice::WriteOnly | QIODevice::Append)) {

        out << endl << endl;
        foreach (QStringList l, m_zerns){
            out << l[0];
            for (int i = 1; i < l.size(); ++i){
                out << "," << l[i];
            }
            out << endl;
        }
    }

}
