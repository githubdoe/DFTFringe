#include "foucaultview.h"
#include "ui_foucaultview.h"
#include "mirrordlg.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
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
void foucaultView::setSurface(wavefront *wf){
    m_wf = wf;
    mirrorDlg *md = mirrorDlg::get_Instance();
    double rad = md->diameter/2.;
    double FL = md->roc/2.;
    double mul = (ui->useMM->isChecked()) ? 1. : 1/25.4;
    m_sag = mul * (rad * rad) /( 4 * FL);

    ui->rocOffsetSb->blockSignals(true);
    m_sag = round(100 * m_sag)/100.;


    ui->rocOffsetSlider->setValue((m_sag/2.)/getStep());


    on_autoStepSize_clicked(ui->autoStepSize->isChecked());
    needsDrawing = true;
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
    double r2 = Radius * Radius;
    double fl = md->roc / 2.;
    double Fnumber =  .5 * md->roc/md->diameter;	//ROC is twice FL
    double unitMultiplyer = 1.;
    if (!ui->useMM->isChecked()){
        unitMultiplyer = 25.4;
    }


    double coc_offset_mm = ui->rocOffsetSb->value() * unitMultiplyer;

    double b = (fl * 2) + coc_offset_mm;
    double pv =   ( sqrt((r2)+(fl * fl * 4.))
         - (sqrt(r2+ b * b) - coc_offset_mm) )/ (outputLambda * 1.E-6);

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


    surf_fft = sv->computeStarTest(heightMultiply * sv->nulledSurface(z3), size, pad ,true);
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
        QString msg = QString().sprintf("warning the slit width of %6.5lf may too small. Using one pixel slit instead", ui->slitWidthSb->value());
        QMessageBox::warning(0,"warning", msg);

    }
    QString parms = QString().sprintf(" Pixel width %6.5lf slit size in pixels %d", pixwidth, (int)(2 * slitWidthHalf));
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

    mulSpectrums(knifeSlit, surf_fft, knifeSurf,0,true);
    idft(knifeSurf, knifeSurf, DFT_SCALE);
    shiftDFT(knifeSurf);

    QImage ronchi = showMag(knifeSurf, false,"", false, gamma);
    int startx = size - m_wf->data.cols;
    ronchi = ronchi.copy(startx,startx,m_wf->data.cols, m_wf->data.cols);

    ronchi = ronchi.mirrored(true,false);
    QSize s = ui->ronchiViewLb->size();
    ui->ronchiViewLb->setPixmap(QPixmap::fromImage(ronchi.scaledToWidth(s.width())));

    merge(vknife, 2, complexIn);
    merge(slit,2,complexIn2);


    dft(complexIn, FFT1, DFT_REAL_OUTPUT);
    dft(complexIn2, FFT2, DFT_REAL_OUTPUT);

    mulSpectrums(FFT1, FFT2, knifeSlit, 0, true);
    idft(knifeSlit, knifeSlit, DFT_SCALE); // gives us the correlation result...


    mulSpectrums(knifeSlit, surf_fft, knifeSurf,0,true);
    idft(knifeSurf, knifeSurf, DFT_SCALE);

    QImage foucault = showMag(knifeSurf, false,"", false, gamma);
    startx = size - m_wf->data.cols;
    foucault = foucault.copy(startx,startx,m_wf->data.cols, m_wf->data.cols);



    foucault = foucault.mirrored(true, false);
    s = ui->foucaultViewLb->size();
    ui->foucaultViewLb->setPixmap(QPixmap::fromImage(foucault.scaledToWidth(s.width())));



    QApplication::restoreOverrideCursor();
}

void foucaultView::on_gammaSb_valueChanged(double arg1)
{
      m_guiTimer.start(500);
}

void foucaultView::on_lpiSb_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("ronchiLPI", (ui->useMM->isChecked()) ? arg1 * 25.4: arg1);
      //m_guiTimer.start(500);
}

void foucaultView::on_movingSourceRb_clicked(bool checked)
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
    double step = m_sag/40;

    int pos = val / step;
    ui->rocOffsetSlider->blockSignals(true);
    ui->rocOffsetSlider->setValue(pos);
    ui->rocOffsetSlider->blockSignals(false);

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
    QString suffix = (checked) ? " mm" : " in";
    ui->rocOffsetSb->setValue( ui->rocOffsetSb->value() * mul);
    ui->rocOffsetSb->setSuffix(suffix);
    ui->slitWidthSb->setValue(ui->slitWidthSb->value() * mul);
    ui->slitWidthSb->setSuffix(suffix);
    ui->lpiSb->setValue(ui->lpiSb->value() / mul);
    ui->gridGroupBox->setTitle((checked) ? "Ronchi LPmm ": "Ronchi LPI ");
    ui->rocStepSize->setValue( (checked) ? 25.4 * ui->rocStepSize->value(): ui->rocStepSize->value()/25.4);
    ui->scanEndOffset->setValue ((checked) ? 25.4 * ui->scanEndOffset->value() : ui->scanEndOffset->value() /25.4);
    ui->scanStart->setValue((checked) ? 25.4 * ui->scanStart->value() : ui->scanStart->value()/25.4);
    on_autoStepSize_clicked(ui->autoStepSize->isChecked());
    m_guiTimer.start(500);
}

void foucaultView::on_scanPb_clicked()
{

    double steps = ui->scanSteps->value();
    double start = ui->scanStart->value();
    double end = ui->scanEndOffset->value();
    double step = (end - start)/steps;
    for (double v = start; v <= end ; v += step){

        ui->rocOffsetSb->setValue(v);
        double st = (ui->useMM->isChecked()) ? 24.5 * m_sag/40 : m_sag/40;

        int pos = v / st;
        ui->rocOffsetSlider->blockSignals(true);
        ui->rocOffsetSlider->setValue(pos);
        ui->rocOffsetSlider->blockSignals(false);
        on_makePb_clicked();

        qApp->processEvents();

    }
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
    return (ui->autoStepSize->isChecked())? round(1000. * ((ui->useMM->isChecked()) ? 25.4 * m_sag/40. : m_sag/40))/1000. :
                                            ui->rocStepSize->value();
}

void foucaultView::on_clearCenterCb_clicked()
{
        m_guiTimer.start(100);
}

void foucaultView::on_autoStepSize_clicked(bool checked)
{

    ui->rocStepSize->setEnabled(!checked);

    double step = getStep();

    ui->rocStepSize->setValue(step);

    // create 17 labels where each label is 5 steps apart.
    for (int i = 0; i< 17; ++i){
        double val =  (i - 8) * step * 5;  // label slider every 5 steps.
        findChild<QLabel *>(QString().sprintf("l%d",i))->setText(QString::number(val));
    }
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
