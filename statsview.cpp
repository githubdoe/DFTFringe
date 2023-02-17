#include "statsview.h"
#include "ui_statsview.h"
#include <QVBoxLayout>
#include "wftstats.h"
#include "surfacemanager.h"
#include "mirrordlg.h"
#include <QFileDialog>
#include <QPrinter>
#include <opencv2/opencv.hpp>
#include <QApplication>
#include <QMessageBox>
#include "myutils.h"
#include "utils.h"
statsView::statsView(SurfaceManager *parent) :
    QDialog(0),
    ui(new Ui::statsView),m_removeOutliers(false), m_removeRMS(false)
{
    ui->setupUi(this);

    m_stats = new wftStats(mirrorDlg::get_Instance());
    m_stats->wftHistogram->setMinimumHeight(2);
    m_stats->zernPlot->setMinimumHeight(2);
    m_stats->wftPlot->setMinimumHeight(2);
    m_sm = parent;
    getWavefronts();
    replot();

    QVBoxLayout *layout = new QVBoxLayout;
    m_vs = new QSplitter(ui->frame);

    m_vs->setOrientation(Qt::Vertical);
    m_vs->setHandleWidth(6);
    m_hs = new QSplitter();
    w1 = new QWidget();
    QVBoxLayout *l1 = new QVBoxLayout();
    l1->addWidget(m_stats->wftPlot,1);
    w1->setLayout(l1);
    m_hs->addWidget(w1);
    w2 = new QWidget();
    QVBoxLayout *l2 = new QVBoxLayout();
    l2->addWidget(m_stats->wftHistogram,1);
    w2->setLayout(l2);
    m_hs->addWidget(w2);
    m_vs->setHandleWidth(6);
    m_vs->setStyleSheet("QSplitter::handle {background-color: lightblue; border: 2px solid #777; border-radius: 4px;}");
    m_hs->setStyleSheet("QSplitter::handle {background-color: lightblue; border: 2px solid #777; border-radius: 4px;}");

    m_hs->setOrientation(Qt::Horizontal);
    QList<int> sizes;
    m_stats->wftHistogram->setMinimumWidth(200);
    sizes << 800;
    sizes << 200;
    m_hs->setSizes(sizes);
    m_vs->addWidget(m_hs);
    w3 = new QWidget();
    QVBoxLayout *l3 = new QVBoxLayout();
    l3->addWidget(m_stats->zernPlot,1);
    w3->setLayout(l3);
    m_vs->addWidget(w3);

    layout->addWidget(m_vs);
    ui->frame->setLayout(layout);
}

statsView::~statsView()
{
    delete ui;
}
void statsView::getWavefronts(){
    wavefrontsToUse.clear();
    QList<int> doThese =  m_sm->m_surfaceTools->SelectedWaveFronts();
    if (doThese.size() <2) {
        for (int i = 0; i < m_sm->m_wavefronts.size(); ++i){
            wavefrontsToUse << m_sm->m_wavefronts[i];
        }
    }
    else {
        foreach(int i, doThese){
            wavefrontsToUse << m_sm->m_wavefronts[i];
        }
    }
    // check that metrics have been computed for each
    foreach(wavefront *wf, wavefrontsToUse){
       m_sm->computeMetrics(wf);

    }

    if (m_removeOutliers){
        m_stats->computeWftStats(wavefrontsToUse,0);
        m_stats->computeZernStats(0);
        m_stats->computeWftRunningAvg(wavefrontsToUse,0);

        while (m_stats->outliers.size()){
            qSort(m_stats->outliers.begin(), m_stats->outliers.end(), qGreater<int>());
            foreach(int i, m_stats->outliers){
                wavefrontsToUse.removeAt(i);
            }
            m_stats->computeWftStats(wavefrontsToUse,0);
            m_stats->computeZernStats(0);
            m_stats->computeWftRunningAvg(wavefrontsToUse,0);
        }
    }
    if (m_removeRMS){
        QVector<wavefront *> filtered;
        double limit = ui->RMSLimit->text().toDouble();
        foreach (wavefront * wf, wavefrontsToUse){
            qDebug() << wf->std;
            if ( wf->std <= limit ){
                filtered << wf;
            }
        }
        wavefrontsToUse = filtered;
    }
}

void statsView::replot(){
    if (wavefrontsToUse.size() == 0){
        QMessageBox::warning(0,"warning", "There are no wavefronts that meet the criteria");
        return;
    }
    m_stats->computeWftStats(wavefrontsToUse,0);
    m_stats->computeZernStats(0);
    m_stats->computeWftRunningAvg(wavefrontsToUse,0);
    m_stats->makeWftPlot(wavefrontsToUse,0);
    m_stats->makeZernPlot();
    m_stats->makeHistoPlot();

}
void statsView::sresize(){
   QList<int> currentSizes = m_vs->sizes();
   currentSizes[0]+= 2;
   currentSizes[1]-= 2;
   m_vs->setSizes(currentSizes);
   qApp->processEvents();
   currentSizes[0]-= 2;
   currentSizes[1]+= 2;
   m_vs->setSizes(currentSizes);

}

void statsView::on_showWftNames_toggled(bool checked)
{
    m_stats->m_showWftNames = checked;
    replot();
    QList<int> currentSizes = m_vs->sizes();
    if (checked){
    currentSizes[0]+= 20;
    currentSizes[1]-= 20;
    }
    else {
        currentSizes[0]-= 20;
        currentSizes[1]+= 20;
    }
    m_vs->setSizes(currentSizes);
}

void statsView::on_removeOutliers_toggled(bool checked)
{
    m_removeOutliers = checked;
    getWavefronts();
    replot();

    sresize();
}
void statsView::on_checkBox_2_toggled(bool checked)
{
    m_removeRMS = checked;
    getWavefronts();
    replot();
    sresize();

}

void statsView::on_saveImg_clicked()
{
    QSettings settings;
    QString path = settings.value("projectPath").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString csvName = path + "/stats.pdf";
    QString name = QFileInfo(csvName).absoluteFilePath() + "/stats.png";
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
                                         tr("Save stats Image"), name,filter.join( ";;" ));
    if (fName.isEmpty())
        return;

    const QSizeF size(800,800);
    const QRect imageRect = QRect(0,0,size.width(),size.height());
    const QRectF topRect( 0.0, 1.0, size.width(), size.height()/2 - 20 );
    QImage image( imageRect.size(), QImage::Format_ARGB32 );

    image.fill( QColor( Qt::white ).rgb() );

    QPainter painter( &image );
    QwtPlotRenderer renderer;
    renderer.render( m_stats->wftPlot, &painter, topRect );
    QRectF d1(0,size.height()/2+10, size.width(),size.height()/2-10);
    renderer.render(m_stats->zernPlot, &painter, d1 );
    painter.end();

    image.save( fName, QFileInfo( fName ).suffix().toLatin1() );
}
QString statsView::title(QString dir){

    QString title = m_sm->m_wavefronts[0]->name;
    title = title.left(title.lastIndexOf("/"));
    title = title.right(title.size() - title.lastIndexOf(dir));
    return title;
}

void statsView::on_SaveCSV_clicked()
{
    QSettings settings;
    QString path = settings.value("projectPath").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString csvName = path + "/stats.csv";
    QString fileName = QFileDialog::getSaveFileName(0,
                        tr("Save stats csv file"), csvName,
                        tr("stats file (*.csv)"));
    if (fileName.isEmpty())
        return;
    QFile thefile(fileName);
    thefile.open(QFile::ReadWrite);

    QTextStream file(&thefile);
    QString dir = info.dir().path();
    dir = dir.right(dir.size() - dir.lastIndexOf("/")-1);
    file << title(dir) << ",'wavefront RMS'"<< ",,Piston,XTile,Ytilt,Defocus,XAstig,Yastig,Z6,Z7,Spherical,Z9,Z10 " << endl;
    QString out;
    cv::Mat mZerns(m_sm->m_wavefronts.size(),Z_TERMS,numType,0.);
    int row = 0;
    mirrorDlg *md = mirrorDlg::get_Instance();

    QPolygonF spherical;
    QPolygonF sphericaRunningAvg;
    double sperAvg = 0.;
    QPolygonF coma;
    QPolygonF otherTerm;
    QVector<int> trueNdx;
    for (int i = 0; i < wavefrontsToUse.size(); ++i)
    {
        wavefront *wf = wavefrontsToUse[ i ];
        QPolygonF zernPoints;
        QFile fn(wf->name);
        QFileInfo fileInfo(fn.fileName());
        QString filename(fileInfo.fileName());
        QString out(filename );
        file << filename << ",";
        file << wf->std << ",";

        for(int ndx = 0; ndx < Z_TERMS; ++ndx){
            out+= ",";
            out += QString::number(wf->InputZerns[ndx]);
            file << "," << QString::number(wf->InputZerns[ndx]);
            double v = wf->InputZerns[ndx];

            // apply software Null if needed
            if (ndx == 8 and md->doNull)
                v -= md->z8 * md->cc;
            double Sigma = computeRMS(ndx,v) * md->lambda/outputLambda;

            if (ndx == 8) {
                spherical << QPointF(row,Sigma);
                sperAvg += Sigma;
                sphericaRunningAvg << QPointF(row,sperAvg/(i+1));
            }

            mZerns.at<double>(row,ndx) =  Sigma * md->lambda/outputLambda;

        }

        file << endl;
        ++row;

    }
    thefile.close();
}

void statsView::on_savePdf_clicked()
{
    QSettings settings;
    QString path = settings.value("projectPath").toString();
    QFile fn(path);
    QFileInfo info(fn.fileName());
    QString csvName = path + "/stats.pdf";
    QString fileName = QFileDialog::getSaveFileName(0,
                        tr("Save stats pdf file"), csvName,
                        tr("pdf (*.pdf)"));
    if (fileName.isEmpty())
        return;

    QString dir = info.dir().path();
    dir = dir.right(dir.size() - dir.lastIndexOf("/")-1);

    QString Title = title(dir);
    Title = Title.left(Title.lastIndexOf("/"));
    Title = Title.right(Title.size() - Title.lastIndexOf(dir));

    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    QString fname = fileName;
    const QSizeF sizeMM(200,300);

    const double mmToInch = 1.0 / 25.4;
    const QSizeF size = sizeMM * mmToInch * 85;

    const QRectF topRect( 10.0, 10.0, size.width()-50, size.height()/2 );

    const QString fmt("pdf");
    fname.replace(".csv",".pdf");

    QPrinter printer(QPrinter::HighResolution);
    printer.setColorMode( QPrinter::Color );
    printer.setFullPage( true );
    printer.setPaperSize( sizeMM, QPrinter::Millimeter );
    printer.setDocName( Title );
    printer.setOutputFileName( fname );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setResolution(85);

    QPainter painter( &printer );

    QwtPlotRenderer renderer;
    renderer.render( m_stats->wftPlot, &painter, topRect );

    QRectF d1(10,size.height()/2+10, size.width()-50,size.height()/3-10);
    renderer.render(m_stats->zernPlot, &painter, d1 );

    QRectF d2(10,size.height()/2 + size.height()/3+10, size.width()/2,size.height()/6);
    renderer.render(m_stats->wftHistogram, &painter, d2);
}

void statsView::on_replot_clicked()
{
    getWavefronts();
    replot();
    sresize();
}

void statsView::on_checkBox_4_toggled(bool checked)
{
    m_stats->m_doZernGroup=checked;
    if (checked){
        m_stats->zernFrom = ui->zernFromSP->text().toInt();
        m_stats->zernTo = ui->zernToSP->text().toInt();
    }
    getWavefronts();
    replot();
    sresize();
}

void statsView::on_zernFromSP_valueChanged(int arg1)
{
    m_stats->zernFrom = ui->zernFromSP->text().toInt();
    m_stats->zernTo = ui->zernToSP->text().toInt();
    getWavefronts();
    replot();
    sresize();
}

void statsView::on_zernToSP_valueChanged(int arg1)
{
    m_stats->zernFrom = ui->zernFromSP->text().toInt();
    m_stats->zernTo = ui->zernToSP->text().toInt();
    getWavefronts();
    replot();
    sresize();
}
