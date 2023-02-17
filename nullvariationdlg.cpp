#include "nullvariationdlg.h"
#include "ui_nullvariationdlg.h"
#include "mirrordlg.h"
#include <QDebug>
#include <QTextEdit>
#include "nullmarginhelpdlg.h"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_plot_marker.h>
#include <qwt_symbol.h>
#include <QSettings>
#include "QLayout"
#include "plotcolor.h"
#include "qwt_legend.h"
#include <opencv2/opencv.hpp>
#include <random>

nullVariationDlg::nullVariationDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::nullVariationDlg), isMm(true)
{
    ui->setupUi(this);
    mirrorDlg *md = mirrorDlg::get_Instance();
    ui->roc->blockSignals(true);
    ui->diameter->blockSignals(true);

    ui->diameter->setText(QString::number(md->diameter));
    ui->roc->setText(QString::number(md->roc));

    ui->roc->blockSignals(false);
    ui->diameter->blockSignals(false);

    QSettings set;
    ui->sampleSizeSb->setValue(   set.value("errorNullSampleSize", 50000).toInt());
    ui->rocDelta->blockSignals(true);
    ui->diameterDelta->blockSignals(true);
    ui->rocDelta->setValue(set.value("nullRocTol", 5).toDouble());
    ui->diameterDelta->setValue(set.value("nullDiamTol", 1).toDouble());
    ui->rocDelta->blockSignals(false);
    ui->diameterDelta->blockSignals(false);
    QVBoxLayout *vLayout = new QVBoxLayout();
    histo_plot = new QwtPlot();
    histo_plot->insertLegend( new QwtLegend() , QwtPlot::TopLegend);

    vLayout->addWidget(histo_plot);
    ui->histoPlot->setLayout(vLayout);
    qDebug() << ui->roc->text();
    connect(&m_guiTimer, SIGNAL(timeout()), this, SLOT(on_ComputeSim_clicked()));
    calculate();
}

nullVariationDlg::~nullVariationDlg()
{
    delete ui;
}


void nullVariationDlg::on_useInch_clicked(bool checked)
{
    if (isMm != checked)
        return;
    isMm = !checked;
    ui->diameter->blockSignals(true);
    ui->roc->blockSignals(true);
    ui->rocDelta->blockSignals(true);
    ui->diameterDelta->blockSignals(true);
    double mul = (checked) ? 1./25.4 : 25.4;
    double diam = ui->diameter->text().toDouble() * mul;
    ui->diameter->setText(QString::number(diam));
    double roc = ui->roc->text().toDouble() * mul;
    ui->roc->setText(QString::number(roc));
    double tol = ui->rocDelta->value() * mul;
    ui->rocDelta->setValue(tol);
    tol = ui->diameterDelta->value() *mul;
    ui->diameterDelta->setValue(tol);
    ui->diameter->blockSignals(false);
    ui->roc->blockSignals(false);
    ui->rocDelta->blockSignals(false);
    ui->diameterDelta->blockSignals(false);
    if (checked){
        ui->rocDelta->setSingleStep(.062);
        ui->diameterDelta->setSingleStep(.062);
    }else {
        ui->rocDelta->setSingleStep(1.);
        ui->diameterDelta->setSingleStep(1.);
    }
    calculate();

}

void nullVariationDlg::on_useMm_clicked(bool checked)
{
    on_useInch_clicked(!checked);
}

void nullVariationDlg::on_roc_textChanged(const QString )
{
        calculate();
}

void nullVariationDlg::on_diameter_textChanged(const QString)
{
    calculate();
}

void nullVariationDlg::on_rocDelta_valueChanged(double val)
{
    QSettings set;
    set.setValue("nullRocTol", ((isMm) ? 1. : 25.4) * val);
        calculate();
}

void nullVariationDlg::on_diameterDelta_valueChanged(double val)
{
    QSettings set;
    set.setValue("nullDiamTol", ((isMm) ? 1. : 25.4) * val);
    calculate();
}
#include <qmap.h>
#include <qwt_plot_histogram.h>
QMap<double,int > histo(const QList<double> data, int bins){
    auto minma = std::minmax_element( data.begin(), data.end());
    double range = *minma.second - *minma.first;
    double histInterval = range/bins;
    QMap<double,int> intervals;
    for (int i = 0; i < bins; ++i)
    {
        intervals[ *minma.first + histInterval * (i+1)] = 0;
    }
    QVector<double> keys = intervals.keys().toVector();
    for (int i = 0; i < data.size(); ++i){
        for (int j = 0; j < bins; ++j){
            if (data[i]<= keys[j]) {
                ++intervals[keys[j]];
                break;
            }
        }
    }
    return intervals;
}

void nullVariationDlg::calculate()
{
    double mul = (isMm) ? 1. : 25.4;

    double roc_tol = ui->rocDelta->text().toDouble() * mul;
    double d_tol = ui->diameterDelta->text().toDouble() * mul;
    double diam =  ui->diameter->text().toDouble() * mul;

    double roc = ui->roc->text().toDouble() * mul;
    mirrorDlg *md = mirrorDlg::get_Instance();
    double lambda = md->lambda;

    double center =  1.5 * pow(diam,4) * 1000000. /(384. * lambda * pow(roc,3));
    //qDebug() << center/1.5;

    double diamPV = 4.* center * d_tol/diam;
    double rocPV  = 3. * center *  roc_tol/roc;

    //qDebug() << "z8 compared to z " << center << ((1.5) * pow(diam+d_tol,4) * 1.E6 /(384. * lambda * pow(roc,3)) - center) << diamPV;

    ui->diamPV->setText(QString().sprintf("%6.4lf    %6.2lf%%", diamPV, 100. * diamPV/center));
    ui->rocPV->setText(QString().sprintf( "%6.4lf    %6.2lf%%",rocPV, 100. * rocPV /center));

    double big1 = ((1.5) * pow(diam+d_tol,4) * 1.E6 /(384. * lambda * pow(roc-roc_tol,3)));
    double big2 = ((1.5) * pow(diam-d_tol,4) * 1.E6 /(384. * lambda * pow(roc+roc_tol,3)));
    ui->result->setText(QString().sprintf("%6.4lf  %6.4lf", sqrt(pow(diamPV,2) + pow(rocPV,2)), (big1 - big2)/2));
    m_guiTimer.start(3000);
}

void nullVariationDlg::on_pushButton_clicked()
{
    nullMarginHelpDlg * dlg = new nullMarginHelpDlg;

    dlg->exec();
}



void nullVariationDlg::on_ComputeSim_clicked()
{
    m_guiTimer.stop();
    setCursor(Qt::WaitCursor);
    double mul = (isMm) ? 1. : 25.4;
    double roc_tol = ui->rocDelta->text().toDouble() * mul;
    double d_tol = ui->diameterDelta->text().toDouble() * mul;
    double diam =  ui->diameter->text().toDouble() * mul;

    double roc = ui->roc->text().toDouble() * mul;
    mirrorDlg *md = mirrorDlg::get_Instance();
    double lambda = md->lambda;
    double center =  1.5 * pow(diam,4) * 1000000. /(384. * lambda * pow(roc,3));
    std::default_random_engine generator(time(0));
    std::default_random_engine g2(time(0) + 1000);
    histo_plot->detachItems( QwtPlotItem::Rtti_PlotItem);
    QwtPlotGrid *grid = new QwtPlotGrid();
    histo_plot->setTitle("SA wavefront uncertainty");
    histo_plot->setAxisTitle( QwtPlot::xBottom, "Wavefront SA deviation PV in waves" );
    histo_plot->setAxisTitle(QwtPlot::yLeft ,"Probability");
    //histo_plot->enableAxis(QwtPlot::yLeft, false);
    //grid->enableXMin(true);
    //grid->enableYMin(true);
    grid->setPen( Qt::gray, 0.0, Qt::DotLine );
    grid->attach( histo_plot);
    //QwtPlotLegendItem *customLegend = new QwtPlotLegendItem();
    //customLegend->setAlignment(Qt::AlignTop);
    //customLegend->attach(histo_plot);
    QString ccc[] = {"Red","Green","Blue","Yellow", "Black", "Grey", "Cyan"};
    int colorndx = 0;
    //double rtols[] = {0,1,5,10 };
    double kk = 1.5 * 1.E6/(384 * lambda);
    QPolygonF points;
    //for (int  dndx = 0; dndx < sizeof(rtols)/sizeof(double); ++dndx, ++colorndx){
        std::normal_distribution<double> diamDist(0, d_tol/sqrt(3));
        std::normal_distribution<double> rocDist(0, roc_tol/sqrt(3));
        points.clear();
        QList<double> histData;
        const int samples = ui->sampleSizeSb->value();
        for (int i = 0; i < samples; ++i){

            double d = diamDist(generator);
            double r = rocDist(g2);
            //qDebug() << d << r;
            double del =  kk * pow(diam+d,4) /(pow(roc +r,3)) - center;
            histData << del;
            //points << QPointF(r,d);
        }
        cv::Mat_<double> b(histData.size(),1);
        for (int i = 0; i < histData.size(); ++i){
            b(i,1) = histData[i];
        }
        cv::Scalar mean, std;
        cv::meanStdDev(b, mean,std);
        //qDebug() << points;
        QMap<double, int> his = histo(histData,100);
        //qDebug() << his;
        QVector<QwtIntervalSample> histoData;
        QList<double> keys = his.keys();
        double bound = 0;
        points.clear();
        for (int i = 0; i < keys.size(); ++i){
            QwtInterval interv(bound, keys[i]);
            //qDebug() << bound << his[keys[i]] << interv;

            bound  = keys[i];
            points << QPointF(keys[i], (double)his[keys[i]]/samples);

        }

        QwtPlotCurve *histPlot = new QwtPlotCurve(QString().sprintf("diameter tolerance:%6.1f      ROC tolerance:%6.1lf",d_tol,roc_tol));
        QwtPlotMarker *mY = new QwtPlotMarker();
        mY->setLabel( QString::fromLatin1( "68%" ) );
        mY->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
        mY->setLineStyle( QwtPlotMarker::VLine );
        mY->setLinePen("Magenta");
        mY->setXValue( std[0]);
        mY->attach( histo_plot);
        mY = new QwtPlotMarker();
        mY->setLabel( QString::fromLatin1( "68%" ) );
        mY->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom);
        mY->setLineStyle( QwtPlotMarker::VLine );
        mY->setXValue( -std[0]);
        mY->setLinePen("Magenta");
        mY->attach( histo_plot);
        mY = new QwtPlotMarker();
        mY->setLabel( QString::fromLatin1( "95%" ) );
        mY->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
        mY->setLineStyle( QwtPlotMarker::VLine );
        mY->setXValue( 2. * std[0]);
        mY->attach( histo_plot);
        mY = new QwtPlotMarker();
        mY->setLabel( QString::fromLatin1( "95%" ) );
        mY->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
        mY->setLineStyle( QwtPlotMarker::VLine );
        mY->setXValue( -2. * std[0]);
        mY->attach( histo_plot);
        mY = new QwtPlotMarker();
        mY->setLabel( QString::fromLatin1( "99.5%" ) );
        mY->setLabelAlignment( Qt::AlignRight | Qt::AlignBottom );
        mY->setLineStyle( QwtPlotMarker::VLine );
        mY->setXValue( 3. * std[0]);
        mY->setLinePen("Green",2);
        mY->attach( histo_plot);
        mY = new QwtPlotMarker();
        mY->setLabel( QString::fromLatin1( "99.5%" ) );
        mY->setLabelAlignment( Qt::AlignLeft | Qt::AlignBottom );
        mY->setLineStyle( QwtPlotMarker::VLine );
        mY->setXValue( -3. * std[0]);
        mY->setLinePen("Green",2);
        mY->attach( histo_plot);
        histPlot->setPen(ccc[colorndx % 7],2);
        histPlot->setSamples(points);
        histPlot->attach(histo_plot);

    //}

    histo_plot->replot();
    setCursor(Qt::ArrowCursor);
}

void nullVariationDlg::on_sampleSizeSb_valueChanged(int arg1)
{
    QSettings set;
    set.setValue("errorNullSampleSize", arg1);
}
