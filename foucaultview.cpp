#include "foucaultview.h"
#include "ui_foucaultview.h"
#include "mirrordlg.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "simulationsview.h"
#include <QVector>
#include <QMenu>
#include "zernikeprocess.h"

extern double outputLambda;
foucaultView *foucaultView::m_instance = 0;

foucaultView::foucaultView(QWidget *parent, SurfaceManager *sm) :
    QWidget(parent),m_sm(sm),
    ui(new Ui::foucaultView), heightMultiply(1)
{
    m_wf = 0;
    lateralOffset = 0;
    needsDrawing = false;
    ui->setupUi(this);
    QSettings set;
    ui->lpiSb->setValue(set.value("ronchiLPI", 100).toDouble());
    connect(&m_guiTimer, SIGNAL(timeout()), this, SLOT(on_makePb_clicked()));
    ui->rocOffsetSb->setSuffix(" inch");
    ui->useMM->setChecked(set.value("simUseMM",false).toBool());
    ui->RonchiX->blockSignals(true);
    ui->RonchiX->setValue(set.value("RonchiROCFactor", 3).toDouble());
    ui->RonchiX->blockSignals(false);
    if (ui->useMM->isChecked()){
        on_useMM_clicked(true);
    }
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showContextMenu(QPoint)));
    setContextMenuPolicy(Qt::CustomContextMenu);
}




foucaultView *foucaultView::get_Instance(SurfaceManager *sm){
    if (m_instance == 0){
        m_instance = new foucaultView(0,sm);
    }
    return m_instance;
}

foucaultView::~foucaultView()
{
    delete ui;
}
QString getSaveFileName(QString type){
    QSettings settings;
    QString path = settings.value("lastPath","").toString();

    QString fileName = QFileDialog::getSaveFileName(0,
            QString("File name of %1 image to be saved").arg(type),
                                                 path);

    if (!fileName.endsWith(".jpg"))
        fileName = fileName + ".jpg";
    return fileName;

}
void foucaultView::showContextMenu(const QPoint &pos)
{

// Handle global position
    QPoint globalPos = mapToGlobal(pos);
    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Save Ronchi image",  this, SLOT(saveRonchiImage()));
    myMenu.addAction("Save Foucault Image", this,  SLOT(saveFoucaultImage()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}
QImage *foucaultView::render(){
    on_makePb_clicked();
    QSize imsize = ui->foucaultViewLb->size();
    imsize.setWidth(imsize.width()*2.1);
    QImage *result = new QImage(imsize, QImage::Format_ARGB32 );

    ui->ronchiViewLb->render(result,QPoint(0,0));
    ui->foucaultViewLb->render(result, QPoint(imsize.width()/2,0));

    return result;
}

void foucaultView::saveRonchiImage(){
    const QPixmap pm = ui->ronchiViewLb->pixmap(Qt::ReturnByValue);
    pm.save(getSaveFileName("foucault"));

}
void foucaultView::saveFoucaultImage(){
    const QPixmap pm = ui->foucaultViewLb->pixmap(Qt::ReturnByValue);
    pm.save(getSaveFileName("foucault"));
}

void foucaultView::setSurface(wavefront *wf){
    m_wf = wf;
    mirrorDlg *md = mirrorDlg::get_Instance();
    double rad = md->diameter/2.;
    double FL = md->roc/2.;
    double mul = (ui->useMM->isChecked()) ? 1. : 1/25.4;
    m_sag = mul * (rad * rad) /( 4 * FL);
    m_sag = round(100 * m_sag)/100.;
    m_temp_sag = m_sag;
    needsDrawing = true;
}
QVector<QPoint> scaleProfile(QPolygonF points, int width,
                             double angle = 0.){
    double left = points[0].x();
    double right = points.back().x();
    double max = 0;
    double min=  1000000;
    foreach(QPointF p, points){
        if (p.y() < min)
            min = p.y();
        if (p.y() > max)
            max = p.y();
    }
    double xdel = right - left;
    double xscale = width/xdel;
    double ydel = max - min;
    double yscale =  (width/2);
    QVector<QPoint> results;
    double cosangle = cos(angle);
    double sinangle = sin(angle);
    foreach(QPointF p, points){
        double x,y;
        x = p.x() * xscale;
        y = p.y() * -yscale;
        double xx = x * cosangle - y * sinangle;
        double yy  = x * sinangle + y * cosangle;
        x  = xx + width/2;
        y  = yy + width/2;

        results << QPoint(x,y);

    }
    qDebug() << "profile points" << results;

    return results;
}
void foucaultView::on_makePb_clicked()
{
    m_guiTimer.stop();
    if (m_wf == 0 ||( m_wf->data.cols == 0))
        return;
    if (mirrorDlg::get_Instance()->isEllipse()){
        QMessageBox::warning(0,"warning","Foucaualt is not suppported for flat surfaces");
        return;
    }
    qDebug() << "slider" << ui->rocOffsetSlider->value();
    QApplication::setOverrideCursor(Qt::WaitCursor);

    double pad = 1.1;
    int size = m_wf->data.cols * pad;
    size = size/2;
    size *= 2;

    pad = (double)size/m_wf->data.cols;
    double moving_constant = (ui->movingSourceRb->isChecked()) ? 1. : 2.;

    double gamma =     ui->gammaSb->value();
    mirrorDlg *md = mirrorDlg::get_Instance();
    double Radius = md->diameter/2.;
    double inputWavelength = md->lambda;
    double r2 = Radius * Radius;
    double fl = md->roc / 2.;
    double Fnumber =  .5 * md->roc/md->diameter;	//ROC is twice FL
    double unitMultiplyer = 1.;
    if (!ui->useMM->isChecked()){
        unitMultiplyer = 25.4;
    }


    double coc_offset_mm = ui->rocOffsetSb->value() * unitMultiplyer;

    double b = (md->roc) + coc_offset_mm;

    double pv =   ( sqrt((r2)+(md->roc * md->roc))
         - (sqrt(r2+ b * b) - coc_offset_mm) )/ (md->lambda* 1.E-6);

    std::vector<double> zerns = m_wf->InputZerns;
    std::vector<double> newZerns = zerns;
    double z3 = pv / ( moving_constant);

    bool oldDoNull = md->doNull;
    md->doNull = false;

    cv::Mat surf_fft;
    SimulationsView *sv = SimulationsView::getInstance(0);
    newZerns[3] = newZerns[3] - 3 * newZerns[8];
    m_wf->InputZerns = newZerns;
    sv->setSurface(m_wf);

    cv::Mat surf_fftRonchi;
    surf_fft = sv->computeStarTest( heightMultiply * sv->nulledSurface(z3), size, pad ,true);
    surf_fftRonchi = sv->computeStarTest( heightMultiply *
            sv->nulledSurface(ui->RonchiX->value() * z3), size, pad ,true);
    //showMag(surf_fft, true, "star ", true, gamma);
    size = surf_fft.cols;

    int hx = (size -1)/2 + lateralOffset;
    m_wf->InputZerns = zerns;

    md->doNull = oldDoNull;

    double hy = hx;

    cv::Mat vknife[] = {cv::Mat::zeros(size,size,CV_64FC1)
                        ,cv::Mat::zeros(Size(size,size),CV_64FC1)};

    cv::Mat ronchiGrid[] = {cv::Mat::zeros(size,size,CV_64FC1)
                            ,cv::Mat::zeros(Size(size,size),CV_64FC1)};

    cv::Mat slit[] = {cv::Mat::zeros(size,size,CV_64FC1)
                   ,cv::Mat::zeros(Size(size,size),CV_64FC1)};


    cv::Mat ronchiSlit[] = {cv::Mat::zeros(size,size,CV_64FC1)
                   ,cv::Mat::zeros(Size(size,size),CV_64FC1)};

    // compute real world pixel width.
    double pixwidth =  outputLambda * 1.E-6* Fnumber * 2./(25.4 * pad);

    double lpi = ui->lpiSb->value()  * ((ui->useMM->isChecked()) ? 25.4 : 1.);

    double linewidth = .5/lpi;
    int ppl = linewidth/pixwidth;       // pixels per line
    if (ppl <= 0)
        ppl = 1;

    int start = ((double)(size)/2.) -(double)ppl/2.;
    bool even = ((start / ppl) % 2) == 0;

    if (ui->clearCenterCb->isChecked())
        even = !even;

    start = ppl - start % (ppl);// + ui->lateralKnifeSb->value() * unitMultiplyer;

    double pixels_per_thou = .001 / pixwidth;

    double slitWidthHalf = pixels_per_thou * ui->slitWidthSb->value() * 1000 * ((ui->useMM->isChecked()) ? 1./25.4 : 1.);
    if (slitWidthHalf < .75){
        QString msg = QString("warning the slit width of %1 may too small. Using one pixel slit instead").arg(ui->slitWidthSb->value(), 6, 'f', 5);
        QMessageBox::warning(0,"warning", msg);

    }
    QString parms = QString(" Pixel width %1 slit size in pixels %2").arg(pixwidth, 6, 'f', 5).arg((int)(2 * slitWidthHalf));
    ui->pixeParms->setText(parms);
    // compute offset so that line is at center
    for (int y = 0; y < size; ++y)
    {
        double ry = double(y - hy)/(double)hy;
        int roffset = start;
        int line_no = 0;
        for (int x = 0; x < size; ++x)
        {
            double rx = double(x - hx)/(double)hx;
            double r = sqrt(rx * rx + ry *ry);

                // foucault setup
                if (r <= 1.)
                {
                    //slit width is in inches convert to 1/1000 s.
                    if ((x > hx -slitWidthHalf)  && (x < hx + slitWidthHalf))
                    {
                        slit[0].at<double>(y,x) = 255.;
                    }
                }

                int knife_side = x;
                if (ui->knifeOnLeftCb->isChecked())
                    knife_side = size - x;

                if (knife_side > hx )
                {
                    vknife[0].at<double>(y,x) = 255.;
                }


                // ronchi setup
                if ((even && (line_no%2 == 0)) || (!even && (line_no%2 != 0)))
                {
                    ronchiGrid[0].at<double>(y,x) = 1;
                }
                if(++roffset >= ppl)
                {
                    ++line_no;
                    roffset = 0;
                }


                if (x> hx - ppl/2. && x < hx + ppl/2.)
                {
                    ronchiSlit[0].at<double>(y,x) = 1;
                }

            }
        }

    Mat FFT1, FFT2;
    //fftw_plan p;
    cv::Mat complexIn;
    cv::Mat complexIn2;

    merge(ronchiGrid, 2, complexIn);
    merge(ronchiSlit,2,complexIn2);

    //showData("grid", ronchiGrid[0]);
    //showData("rslit", ronchiSlit[0]);


    dft(complexIn, FFT1, DFT_REAL_OUTPUT);
    shiftDFT(FFT1);
    dft(complexIn2, FFT2, DFT_REAL_OUTPUT);
    shiftDFT(FFT2);
    cv::Mat knifeSlit;
    mulSpectrums(FFT1, FFT2, knifeSlit, 0, true);
    idft(knifeSlit, knifeSlit, DFT_SCALE); // gives us the correlation result...
    shiftDFT(knifeSlit);
    cv::Mat knifeSurf;

    mulSpectrums(knifeSlit, surf_fftRonchi, knifeSurf,0,true);
    idft(knifeSurf, knifeSurf, DFT_SCALE);
    shiftDFT(knifeSurf);

    QImage ronchi = showMag(knifeSurf, false,"", false, gamma);
    int startx = size - m_wf->data.cols;
    ronchi = ronchi.copy(startx,startx,m_wf->data.cols, m_wf->data.cols);

    ronchi = ronchi.mirrored(true,false);
    QSize s = ui->ronchiViewLb->size();
    QPixmap rp = QPixmap::fromImage(ronchi.scaledToWidth(s.width()));

    QPainter painter(&rp);
    painter.save();
    painter.drawPixmap(0, 0, rp);
    painter.setPen(QPen(QColor(Qt::white)));
    painter.setFont(QFont("Arial", 15));
    QString zoffsetStr = QString("%1 %2").arg(ui->RonchiX->value() * ui->rocOffsetSb->value(), 6, 'f', 3)
                                           .arg(ui->useMM->isChecked()? "mm" : "in");
    painter.drawText(20, 40, zoffsetStr);
    QVector<QPoint> profilePoints;
    if (ui->overlayProfile->isChecked()){
        // overlay profile onto ronchi plot
        QPolygonF  profile = m_sm->m_profilePlot->createProfile(1.,m_wf);
        profilePoints= scaleProfile(profile, rp.width(), M_PI/4.);
        painter.setPen(QPen(QColor(Qt::yellow),3));
        painter.drawLines(profilePoints);

    }

    painter.restore();


    ui->ronchiViewLb->setPixmap(rp);

    merge(vknife, 2, complexIn);
    merge(slit,2,complexIn2);


    dft(complexIn, FFT1, DFT_REAL_OUTPUT);
    dft(complexIn2, FFT2, DFT_REAL_OUTPUT);

    mulSpectrums(FFT1, FFT2, knifeSlit, 0, true);
    idft(knifeSlit, knifeSlit, DFT_SCALE); // gives us the correlation result...


    mulSpectrums(knifeSlit, surf_fft, knifeSurf,0,true);
    idft(knifeSurf, knifeSurf, DFT_SCALE);

    m_foucultQimage = showMag(knifeSurf, false,"", false, gamma);

    startx = size - m_wf->data.cols;
    QImage foucault = m_foucultQimage.copy(startx,startx,m_wf->data.cols, m_wf->data.cols);
    qDebug() << "foucult" << foucault.size();
/*
    cv::Mat iMat(foucault.height(), foucault.width(), CV_8UC3, foucault.bits(), foucault.bytesPerLine());
    cv::Mat flipped;
    cv::flip(iMat,flipped, 1);
    cv::Mat diffed;
    cv::absdiff(iMat, flipped, diffed);
    diffed = diffed * 10;
    cv::imshow("diffed", diffed);
    cv::waitKey(1);
    foucault = QImage((uchar*)diffed.data, diffed.cols, diffed.rows, diffed.step, QImage::Format_RGB888).copy();
*/
    foucault = foucault.mirrored(true, false);
    s = ui->foucaultViewLb->size();

    QPixmap rpf = QPixmap::fromImage(foucault.scaledToWidth(s.width()));

    QPainter painterf(&rpf);
    painterf.save();
    painterf.drawPixmap(0, 0, rpf);
    painterf.setPen(QPen(QColor(Qt::white)));
    painterf.setFont(QFont("Arial", 15));
    zoffsetStr = QString("%1 %2").arg(ui->rocOffsetSb->value(), 6 , 'f', 3)
                                   .arg(ui->useMM->isChecked()? "mm" : "in");
    painterf.drawText(20, 40, zoffsetStr);
    if (ui->overlayProfile->isChecked()){
        // overlay profile onto ronchi plot
        painterf.setPen(QPen(QColor(Qt::yellow),3));
        painterf.drawLines(profilePoints);
    }

    painterf.restore();


    ui->foucaultViewLb->setPixmap(rpf);
    //ui->foucaultViewLb->setPixmap(QPixmap::fromImage(foucault.scaledToWidth(s.width())));



    QApplication::restoreOverrideCursor();
}

void foucaultView::on_gammaSb_valueChanged(double /*arg1*/)
{
      m_guiTimer.start(500);
}

void foucaultView::on_lpiSb_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("ronchiLPI", (ui->useMM->isChecked()) ? arg1 * 25.4: arg1);
      //m_guiTimer.start(500);
}

void foucaultView::on_movingSourceRb_clicked(bool /*unused*/)
{
     m_guiTimer.start(500);
}

void foucaultView::on_radioButton_2_clicked()
{
     m_guiTimer.start(500);
}

void foucaultView::on_knifeOnLeftCb_clicked()
{
     m_guiTimer.start(500);
}

void foucaultView::on_lpiSb_editingFinished()
{
    m_guiTimer.start(500);
}

void foucaultView::on_rocOffsetSb_editingFinished()
{
    double val = ui->rocOffsetSb->value();
    double step = m_temp_sag/40;

    int pos = val / step;
    ui->rocOffsetSlider->blockSignals(true);
    ui->rocOffsetSlider->setValue(pos);
    ui->rocOffsetSlider->blockSignals(false);
qDebug() << "slider" << val << step << pos;
    m_guiTimer.start(500);
}

void foucaultView::on_slitWidthSb_editingFinished()
{
    m_guiTimer.start(500);
}



void foucaultView::on_useMM_clicked(bool checked)
{
    double mul = 25.4;
    if ( !checked)
        mul = 1./25.4;

    QSettings set;
    set.setValue("simUseMM", checked);
    QString suffix = (checked) ? " mm" : " in";
    ui->rocOffsetSb->setValue( ui->rocOffsetSb->value() * mul);
    ui->rocOffsetSb->setSuffix(suffix);
    ui->slitWidthSb->setValue(ui->slitWidthSb->value() * mul);
    ui->slitWidthSb->setSuffix(suffix);
    ui->lpiSb->setValue(ui->lpiSb->value() / mul);
    ui->gridGroupBox->setTitle((checked) ? "Ronchi LPmm ": "Ronchi LPI ");
    double xx = ui->rocStepSize->value();
 //qDebug() << ui->rocStepSize->value() << mul << xx;
    ui->rocStepSize->setValue( mul * ui->rocStepSize->value());
    ui->scanEndOffset->setValue (mul * ui->scanEndOffset->value());
    ui->scanStart->setValue(mul * ui->scanStart->value());
    //on_autoStepSize_clicked(ui->autoStepSize->isChecked());
         //qDebug() << "xx" << ui->rocStepSize->value();
         draw_ROC_Scale();

    m_guiTimer.start(500);

}
bool inscan = false;
void foucaultView::on_scanPb_clicked()
{
    if (inscan == true){
        inscan = false;
        qDebug() << "abort scan";
        return;
    }
    qDebug() << "scan running";
    inscan = true;
    ui->scanPb->setText("Abort");
    qApp->processEvents();
    double steps = ui->scanSteps->value();
    double start = ui->scanStart->value();
    double end = ui->scanEndOffset->value();
    double step = ui->scanSteps->value();
    if (step == 0)
       step = .001;
    foucaultView *fv = foucaultView::get_Instance(0);
    int cnt = 0;
    QSettings settings;
    QString lastPath = settings.value("lastPath","").toString();
    for (double v = start; v <= end; v += step){

        ui->rocOffsetSb->setValue(v);
        double st = (ui->useMM->isChecked()) ? 24.5 * m_temp_sag/40 : m_temp_sag/40;

        int pos = v / st;
        ui->rocOffsetSlider->blockSignals(true);
        ui->rocOffsetSlider->setValue(pos);
        ui->rocOffsetSlider->blockSignals(false);
        on_makePb_clicked();

        QImage fvImage = QImage(fv->size(),QImage::Format_ARGB32 );

        QPainter p3(&fvImage);
        fv->QWidget::render(&p3);
        if (ui->SaveImageCB->isChecked()){
            QString num = QString("%1").arg(v, 6, 'f', 4).replace(".","_");
            num.replace("-","n");
            QString fvpng = QString("%1//%2_%3.png").arg(imageDir).arg(cnt++, 6, 10, QLatin1Char('0')).arg(num);
            qDebug() << "fn"<< fvpng;
            if (ui->saveOnlyFouccault->isChecked()){
                fv->m_foucultQimage.save(fvpng);
            }
            else {
                fvImage.save(fvpng);
            }
        }
        qApp->processEvents();
        if (inscan == false) {
            qDebug() << "asked to abort";
            break;
        }

    }
    ui->scanPb->setText("Scan");
    inscan = false;
}

void foucaultView::on_h1x_clicked()
{
    heightMultiply = 1;
    m_guiTimer.start(500);
}

void foucaultView::on_h2x_clicked()
{
    heightMultiply = 2;
    m_guiTimer.start(500);
}

void foucaultView::on_h4x_clicked()
{
    heightMultiply = 4;
    m_guiTimer.start(500);
}



void foucaultView::on_rocOffsetSlider_valueChanged(int value)
{

    double step = getStep();
    double offset = (value) * step;

    ui->rocOffsetSb->setValue(offset);

    m_guiTimer.start(1000);

}
inline double foucaultView::getStep(){
    // slider has 40 positive positions and 40 neg positions.
    // A slider step then is sag / 40
    return (ui->autoStepSize->isChecked())? round(1000. * ((ui->useMM->isChecked()) ? 25.4 * m_temp_sag/40. : m_sag/40))/1000. :
                                            ui->rocStepSize->value();
}

void foucaultView::on_clearCenterCb_clicked()
{
        m_guiTimer.start(100);
}
void foucaultView::draw_ROC_Scale(){
    // create 17 labels where each label is 5 steps apart.
    double step = getStep();
    for (int i = 0; i< 17; ++i){
        double val =  (i - 8) * step * 5;  // label slider every 5 steps.
        findChild<QLabel *>(QString("l%1").arg(i))->setText(QString::number(val));
    }
}

void foucaultView::on_autoStepSize_clicked(bool checked)
{

    ui->rocStepSize->setEnabled(!checked);

    double step = getStep();

    if (checked){
        m_temp_sag = m_sag;
        ui->rocStepSize->setValue(step);
    }
    else
        m_temp_sag = 40 * step;

    draw_ROC_Scale();
}

void foucaultView::on_rocStepSize_editingFinished()
{
    on_autoStepSize_clicked(ui->autoStepSize->isChecked());
    m_guiTimer.start(100);
}

void foucaultView::on_lateralOffset_valueChanged(int arg1)
{
    lateralOffset = arg1;
    m_guiTimer.start(100);
}

void foucaultView::on_SaveImageCB_clicked(bool checked)
{
    if (!checked)
        return;
    QSettings settings;
    imageDir = settings.value("lastPath","").toString();

    QString dir = QFileDialog::getExistingDirectory(this, tr("Directory where images are to be saved"),
                                                 imageDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty())
        imageDir = dir;
}
bool foucaultView::saveOnlyFoucault(){
    return ui->saveOnlyFouccault->isChecked();
}

void foucaultView::on_saveOnlyFouccault_clicked(bool /*checked*/)
{

}

void foucaultView::on_overlayProfile_stateChanged(int /*arg1*/)
{
    on_makePb_clicked();
}


void foucaultView::on_RonchiX_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("RonchiROCFactor", arg1);
     m_guiTimer.start(450);
}


void foucaultView::on_pushButton_clicked()
{
    mirrorDlg *md = mirrorDlg::get_Instance();
    double rad = md->diameter/2.;
    double FL = md->roc/2.;
    double mul = (ui->useMM->isChecked()) ? 1. : 1/25.4;
    m_sag = mul * (rad * rad) /( 4 * FL);
    m_sag = round(100 * m_sag)/100.;
    m_temp_sag = m_sag;
    ui->rocOffsetSlider->blockSignals(true);
    ui->rocOffsetSlider->setValue((m_sag/2.)/getStep());
    ui->rocOffsetSlider->blockSignals(false);
    double step = getStep();
    double offset = (ui->rocOffsetSlider->value()) * step;
    ui->rocOffsetSb->blockSignals(true);
    ui->rocOffsetSb->setValue(offset);
    ui->rocOffsetSb->blockSignals(false);
    on_autoStepSize_clicked(ui->autoStepSize->isChecked());
    needsDrawing = true;
    on_makePb_clicked();
}

