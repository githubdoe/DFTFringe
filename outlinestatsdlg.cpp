#include "outlinestatsdlg.h"
#include "ui_outlinestatsdlg.h"
#include "graphicsutilities.h"
#include <qwt_plot_histogram.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <QMessageBox>
#include <QPen>
#include <qwt_scale_draw.h>
#include <qwt_legend.h>
#include <qwt_plot_zoomer.h>
#include "mainwindow.h"
#include "surfacemanager.h"
class outlineZoomer: public QwtPlotZoomer
{
public:
    QwtPlot * mPlot;
    outlineStatsDlg &m_outlines;
    outlineZoomer(outlineStatsDlg *dlg, QWidget *canvas, QwtPlot * plot ):
        QwtPlotZoomer( canvas ), mPlot(plot), m_outlines(*dlg)
    {
        setRubberBandPen( QPen( Qt::lightGray ) );
        setTrackerMode( QwtPlotPicker::AlwaysOn );
    }

protected:
    virtual QwtText trackerTextF( const QPointF &p ) const
    {

    int ndx = p.x();
    //qDebug() << "tracker"<< cx << cy << p;
    if (ndx > 0 && ndx < m_outlines.m_names.size()){
        QString msg = QString("%1 \n%2,%3").arg(
                                        m_outlines.m_names[ndx].split("/").back()).arg(
                                        m_outlines.xvals[ndx], 6, 'f', 1).arg(m_outlines.yvals[ndx], 6, 'f', 1);
        QwtText text(msg);
        text.setColor( Qt::black );
        text.setFont(QFont("Arial",12));

        QColor c = rubberBandPen().color();
        text.setBorderPen( QPen( c ) );
        text.setBorderRadius( 6 );
        c.setAlpha( 170 );
        text.setBackgroundBrush( c );

        return text;
    }
    else return QwtText("");

    }
};
class outlineScaleDraw: public QwtScaleDraw
{
public:
    QStringList  &names;

    outlineScaleDraw(QStringList &nameList):names(nameList)
    {

        setLabelRotation( -45.0 );
        setLabelAlignment( Qt::AlignLeft | Qt::AlignVCenter );
        setSpacing( 1);
    }
    virtual QwtText label( double value ) const
    {
        if (value >= names.size())
            return QString("");
        QString n = names[(int)(value)];
        QStringList l = n.split("/");

        n = l.back().replace(".oln","");
        return n;
    }
};

// Create a histogram of mirror radius values of igrams
QVector<int> radiusHisto(const QVector<double>& dataQVect, double *minRad, int &bins){

    const std::vector<double> data = std::vector<double>(dataQVect.begin(), dataQVect.end());
    auto result = std::minmax_element (data.begin(),data.end());
    double min = *minRad = *result.first;
    double max = *result.second;
    bins = max - min + 1;
    QVector<int> h(bins, 0);
    for (unsigned int i = 0; i < data.size(); ++i){

        double bound = min;
        for (int j = 0; j < bins; ++j){
            if (data[i] <= bound) {
                ++h[j];

                break;
            }
            bound += 1;
        }
    }
    return h;
}

outlineStatsDlg::outlineStatsDlg(QStringList names, QWidget *parent) :
    QDialog(parent), m_names(names),
    ui(new Ui::outlineStatsDlg)
{
    ui->setupUi(this);
    ui->mirrorCenterPlot->setTitle("Mirror center Position x and y");
    ui->mirrorRadiusPlot->setTitle("Mirror Radius");
    ui->mirrorRadiusHistogramPlot->setTitle("Mirror Radius Histogram");
    readFiles();
    ui->mirrorCenterPlot->setAxisMaxMajor(QwtPlot::xBottom, m_names.size()/10);
    ui->mirrorRadiusPlot->setAxisMaxMajor(QwtPlot::xBottom, m_names.size()/10);
    zoomer = new outlineZoomer(this, ui->mirrorCenterPlot->canvas(), ui->mirrorCenterPlot );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoomer->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );
    radZoomer = new outlineZoomer(this, ui->mirrorRadiusPlot->canvas(), ui->mirrorRadiusPlot);
    plot();
}
void outlineStatsDlg::readFiles(){
    int cnt =0;

    foreach(QString fileName, m_names){
        std::ifstream file(fileName.toStdString().c_str());

        int fsize = file.tellg();
        file.seekg( 0, std::ios::end );
        fsize = int(file.tellg()) - fsize;
        file.close();
        file.open(fileName.toStdString().c_str());
        if (!file.is_open()) {
            QMessageBox::warning(this, tr("Read Outline"),
                                 tr("Cannot read file %1: ")
                                 .arg(fileName));
            return;
        }

        CircleOutline outline = readCircle(file);

        xvals << outline.m_center.x();
        yvals << outline.m_center.y();
        radVals << outline.m_radius;
        sn << (double)(cnt++);
    }
}
void outlineStatsDlg::plot(){
    QwtPlotCurve *xpos = new QwtPlotCurve("x");
    QwtPlotCurve *ypos = new QwtPlotCurve("y");
    QwtPlotCurve *radCurve = new QwtPlotCurve("mirror Radius");
    QwtLegend *l = new QwtLegend();
    ui->mirrorCenterPlot->insertLegend( l, QwtPlot::TopLegend );
    radCurve->setSamples(sn, radVals);
    radCurve->setStyle(QwtPlotCurve::Dots);
    radCurve->setPen(Qt::blue, 3);
    radCurve->attach(ui->mirrorRadiusPlot);
    ui->mirrorRadiusPlot->replot();
    ui->mirrorCenterPlot->setAxisScaleDraw(QwtPlot::xBottom, new outlineScaleDraw(m_names));
    ui->mirrorRadiusPlot->setAxisScaleDraw(QwtPlot::xBottom, new outlineScaleDraw(m_names));
    ui->mirrorRadiusHistogramPlot->setAxisTitle(QwtPlot::xBottom, "Mirror Radius");
    ui->mirrorRadiusHistogramPlot->setAxisTitle(QwtPlot::yLeft, "sample count");
    ui->mirrorCenterPlot->setAxisTitle(QwtPlot::yLeft, "Position");
    ui->mirrorRadiusPlot->setAxisTitle(QwtPlot::yLeft, "radius");
    ui->mirrorCenterPlot->setAxisTitle(QwtPlot::xBottom, QString(" %1 samples").arg(m_names.size()));
    xpos->setSamples(sn,xvals);
    xpos->setStyle(QwtPlotCurve::Dots);
    xpos->setPen(Qt::red,4);
    ypos->setPen(Qt::blue,4);
    ypos->setStyle(QwtPlotCurve::Dots);
    ypos->setSamples(sn,yvals);
    xpos->attach(ui->mirrorCenterPlot);
    ypos->attach(ui->mirrorCenterPlot);
    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->attach(ui->mirrorCenterPlot);
    ui->mirrorCenterPlot->resize(200,100);
    ui->mirrorCenterPlot->replot();

    double minRad;
    int bins;

    QVector<int> radhisto = radiusHisto(radVals, &minRad, bins);

    ui->mirrorRadiusHistogramPlot->setTitle("Radius histogram");
    for (int i = 0; i < radhisto.size(); ++i){
        QwtPlotCurve *column = new QwtPlotCurve();
        QVector<QPointF> line;
        line << QPoint(i + minRad, 0) << QPoint(i + minRad, radhisto[i]);
        column->setSamples(line);
        column->attach(ui->mirrorRadiusHistogramPlot);
        column->setPen(Qt::blue, 10);
    }
    QwtPlotGrid *hgrid = new QwtPlotGrid();
    hgrid->attach(ui->mirrorRadiusHistogramPlot);
    ui->mirrorRadiusHistogramPlot->replot();
    zoomer->setZoomBase();
    radZoomer->setZoomBase();

    int ndx = std::distance(radhisto.begin(), std::max_element(radhisto.begin(), radhisto.end()));
    mostFrequentRadius = ndx + minRad;
}
outlineStatsDlg::~outlineStatsDlg()
{
    delete ui;
}
#include "filteroutlinesdlg.h"
void outlineStatsDlg::on_pushButton_clicked()
{
    FilterOutlinesDlg dlg;
    dlg.setRadius(mostFrequentRadius);
    if (dlg.exec()){
        QStringList selectThese;
        for (int i = 0; i < m_names.size(); ++i){
            switch (dlg.filterOperation){
            case outlineFilterOperation::FOGREATERTHAN:
                if (radVals[i] > dlg.radiusTarget){
                    selectThese << m_names[i];
                }

                break;
            case outlineFilterOperation::FOLESSTHAN:
                if (radVals[i] < dlg.radiusTarget){
                    selectThese << m_names[i];
                }
                break;
            case outlineFilterOperation::FOEQUAL:
                if (radVals[i] == dlg.radiusTarget){
                    selectThese << m_names[i];
                }
                break;
            case outlineFilterOperation::FOANY:
                selectThese << m_names[i];
            }
        }
        switch (dlg.actionType){
            case outlineFilterAction::oFLOAD:
            {
            for(int i =0; i < selectThese.size(); ++i){
                selectThese[i].replace(".oln",".wft");
            }
            hide();
            QApplication::setOverrideCursor(Qt::WaitCursor);
            ((MainWindow*)(parent()))->openWaveFrontonInit(selectThese);
            QApplication::restoreOverrideCursor();
            close();
            }
            break;
        case outlineFilterAction::oFDELETE:
            for(int i =0; i < selectThese.size(); ++i){
                selectThese[i].replace(".oln",".wft");
                QFile file (selectThese[i]);
                file.remove();
            }
            break;
        case outlineFilterAction::oFAVERAGE:
            for(int i =0; i < selectThese.size(); ++i){
                selectThese[i].replace(".oln",".wft");
            }
            hide();
            SurfaceManager::get_instance()->averageWavefrontFiles(selectThese);
            close();
            break;
        case outlineFilterAction::oFRESIZE:
        {
            QApplication::setOverrideCursor(Qt::WaitCursor);
            for(int i =0; i < selectThese.size(); ++i){
                QString fileName = selectThese[i];

                std::ifstream file(fileName.toStdString().c_str());
                CircleOutline outside;
                CircleOutline inside;

                int fsize = file.tellg();
                file.seekg( 0, std::ios::end );
                fsize = int(file.tellg()) - fsize;
                file.close();
                file.open(fileName.toStdString().c_str());
                if (!file.is_open()) {
                    QMessageBox::warning(this, tr("Read Outline"),
                                         tr("Cannot read file %1: ")
                                         .arg(fileName));
                    break;
                }
                outside = readCircle(file);
                CircleOutline sideLobe = readCircle(file);

                if ((file.tellg() > 0) && (fsize > file.tellg())) {
                    if (file.peek() != 'P'){
                        inside = readCircle(file);
                    }
                }
                else {
                    inside.m_radius = 0;
                }
                std::string line;
                QVector<std::vector<cv::Point> > polygons;
                while(std::getline(file, line)){
                    if (line == "Poly"){
                        std::getline(file,line);
                        polygons.push_back(std::vector< cv::Point>());

                        QStringList data = QString::fromStdString(line).split(" ");
                        for (int i = 0; i < data.size()-1; ++i){
                            QStringList vals = data[i].split(",");
                            polygons.back().push_back(cv::Point(vals[0].toDouble(),vals[1].toDouble()));

                        }
                    }
                }
                file.close();
                //write out new outline file;
                outside.m_radius = dlg.resizeValue;
                std::ofstream ofile((fileName.toStdString().c_str()));
                if (!ofile.is_open()) {
                    QMessageBox::warning(this, tr("Write Outline"),
                                         tr("Cannot write file %1: ")
                                         .arg(fileName));
                    break;
                }
                // write oustide outline

                writeCircle(ofile,outside);
                QSettings set;
                double filterRad = set.value("DFT Center Filter",10).toDouble();

                CircleOutline filter(QPointF(0,0), filterRad);
                writeCircle(ofile, filter );

                if (inside.m_radius > 0){
                   writeCircle(ofile,inside);
                }
                for (int i = 0; i < polygons.size(); ++ i){
                    if (polygons[i].size() > 0){
                        ofile << "Poly"<<std::endl;
                        for (std::size_t j = 0; j < polygons[i].size(); ++j){
                            ofile <<(polygons[i][j].x) << "," << (polygons[i][j].y) << " ";
                        }
                        ofile << std::endl;
                    }
                }
                ofile.flush();
                ofile.close();
            }
            QApplication::restoreOverrideCursor();
            close();

        }
            break;
        }
    }
}
