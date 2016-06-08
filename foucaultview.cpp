#include "foucaultview.h"
#include "ui_foucaultview.h"
#include "mirrordlg.h"
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include "simulationsview.h"
#include <QVector>
#include <QMenu>
#include "zernikeprocess.h"
foucaultView::foucaultView(QWidget *parent, SurfaceManager *sm) :
    QWidget(parent),m_sm(sm),
    ui(new Ui::foucaultView), heightMultiply(1)
{
    ui->setupUi(this);
    connect(&m_guiTimer, SIGNAL(timeout()), this, SLOT(on_makePb_clicked()));
    ui->rocOffsetSb->setSuffix(" inch");

}

foucaultView::~foucaultView()
{
    delete ui;
}
/*
cv::Mat foucaultView::compute_star_test(int pupil_size, double defocus, double pad, bool use_OPD)
{

    int nx = pupil_size;
    int ny = nx;

    cv::Mat in = cv::Mat::zeros(nx,ny,CV_64F);
    double hx = (double)(nx-1)/2.;
    double hy = (double)(ny-1)/2.;
    int pupil_rad = (pupil_size/2.)/pad;
    mirrorDlg &md = *mirrorDlg::get_Instance();
    double obs_radius = md.obs / md.diameter;
    double F_number = md.FNumber;
    // defocus is in mm on input
    wavefront *wf = m_sm->m_wavefronts[m_sm->m_currentNdx];
    std::vector<double> newZerns = wf->InputZerns;
    if (!use_OPD){
        newZerns[3] = -(defocus + newZerns[3]);
        //UseZernikesPtr->z[3] = true;
        // if use wavefront then apply defocus to wavefront.

        {
        zernikeProcess zp;
           cv::Mat nulled =  nulled_surface = zp.null_unwrapped( wf, newZerns,zernEnables);


    }
    igram_wavelength = mirrorDlg.m_Ilambda * 1e-6;


    int mask_holes = 1;

    for (int y = 0; y < nx; ++y)
    {

        double ry = (double)(y - hy)/(double)pupil_rad;

        for (int x = 0; x < nx; ++x)
        {

            double rx = (double)(x - hx)/(double)pupil_rad;

            double r = sqrt(rx * rx + ry * ry);
            int b = mask_holes * r;
            bool block = true;




            {
                double phase;

                    phase = nulled.at<double>(y,x);





                in[ndx][1] = cos(phase);
                in[ndx][0] = -sin(phase);

                if ((x+y) & 0x1)
                {
                    in[ndx][1] = -in[ndx][1];
                    in[ndx][0] = -in[ndx][0];
                }

            }
            else
            {
                in[ndx][0] = 0;
                in[ndx][1] = 0;
            }

        }
    }


    // debug image of real portion
//	double* mtmp = new double [nx * nx];
//	mtmp = Mod(in,nx,nx, true);
//	generate_image_from_doubles(mtmp,nx,nx,CString(L"in"),true);
    //return COMPLEX_MATRIX(out, nx);


    out = (double (*)[2])fftw_malloc(sizeof(fftw_complex) * nx * ny);
    p = fftw_plan_dft_2d(nx, ny, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

    fftw_execute(p);
    fftw_free(in);
    fftw_destroy_plan(p);
    double* mtmp = Mod(out,nx,ny,false);

    // check for aliasing
    // compute edge
    double edge_avg = 0.;
    double center_avg = 0.;
    int half = nx * (ny/2);
    int last = nx * .03;
    for (int i = 0; i < last; ++i)
    {
        edge_avg += mtmp[half + i];
        center_avg += mtmp[half + i + nx /2];
    }

    double ddd = center_avg/edge_avg;
    f_fft_alias= false;


    if (ddd < 2)
    {
        f_fft_alias = true;

        AfxMessageBox(L"Warning, computed PSF was too large for the selected size of the simulation.\n"
                        L"Select larger simulation size from the Configuration Menu\n"
                        L"and try again.\n\n"
                        L"Note: PSF is also used to compute Foucault, Ronchi, and MTF\n"
                        L" Computeing MTF may cause this message 3 times\n"
                        L"Sometime this message is caused by the errors on the surface and so the simulatin may still be usable.\n"
                        L"The error usually shows up as a series of light and dark horizontal bands.");

        //throw FFT_ERROR();
    }

    if (saved_wavefront)
    {
        delete[] m_current_wavefront.mat;
        m_current_wavefront.mat = saved_wavefront;
        m_dirty_zerns = true;
        m_dirty_surface = true;
    }
    delete[] mtmp;
    //return COMPLEX_MATRIX(out, nx);
    return (COMPLEX_MATRIX(out, nx));
}
*/


void foucaultView::on_makePb_clicked()
{
    m_guiTimer.stop();
    if (m_sm->m_wavefronts.size() == 0)
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    wavefront *wf = m_sm->m_wavefronts[m_sm->m_currentNdx];

    double pad = 1.1;
    int size = wf->data.cols * pad;
    size = size/2;
    size *= 2;

    pad = (double)size/wf->data.cols;
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

    // subtract the 70% offset needed to put it at ROC with zero offset.
    double cc_adjust =  .707 * .707 * Radius * Radius/(moving_constant* md->roc);


    double b = (fl * 2) + coc_offset_mm;
    double pv =   ( sqrt((r2)+(fl * fl * 4.))
         - (sqrt(r2+ b * b) - coc_offset_mm) )/ (550 * 1.E-6);


    std::vector<double> zerns = wf->InputZerns;
    std::vector<double> newZerns = zerns;
    double z3 = pv / ( moving_constant);

    newZerns[3] =  zerns[8] = md->z8;//3. * zerns[8];
qDebug() << "Z3" << z3 << "z{3]" << newZerns[3] << "pv" << pv << "coc_Offset_mm" << coc_offset_mm;
    bool oldDoNull = md->doNull;
    md->doNull = false;

    cv::Mat surf_fft;
    SimulationsView *sv = SimulationsView::getInstance(0);
    wf->InputZerns = newZerns;
    sv->setSurface(wf);
qDebug() << "size "<< size << "pad" << pad;
    surf_fft = sv->computeStarTest(heightMultiply * sv->nulledSurface(z3), size, pad ,true);
    //showMag(surf_fft, true, "star ", true, gamma);
    size = surf_fft.cols;

    int hx = (size -1)/2;
    wf->InputZerns = zerns;

    md->doNull = oldDoNull;

    double hy = hx;

    cv::Mat vknife[] = {cv::Mat::zeros(size,size,CV_64F)
                        ,cv::Mat::zeros(Size(size,size),CV_64F)};

    cv::Mat ronchiGrid[] = {cv::Mat::zeros(size,size,CV_64F)
                            ,cv::Mat::zeros(Size(size,size),CV_64F)};

    cv::Mat slit[] = {cv::Mat::zeros(size,size,CV_64F)
                   ,cv::Mat::zeros(Size(size,size),CV_64F)};


    cv::Mat ronchiSlit[] = {cv::Mat::zeros(size,size,CV_64F)
                   ,cv::Mat::zeros(Size(size,size),CV_64F)};
    // compute real world pixel width.

    double airyDisk = 2.43932 * 550.E-6 * Fnumber;
    double pixwidth =  550.E-6* Fnumber * 2./(25.4 * pad);
    /*
     *   fl = roc/2      fnumber = fl/diameter = roc/(2 * diameter)
     * */


    double lpi = ui->lpiSb->value()  * ((ui->useMM->isChecked()) ? 25.4 : 1.);

    double linewidth = .5/lpi;
    int ppl = linewidth/pixwidth;       // pixels per line
    if (ppl <= 0)
        ppl = 1;

    int start = ((double)(size-1)/2.) -(double)ppl/2.;
    bool even = ((start / ppl) % 2) == 0;

    if (ui->clearCenterCb->isChecked())
        even = !even;

    start = ppl - start % (ppl) + ui->lateralKnifeSb->value() * unitMultiplyer;

    double pixels_per_thou = .001 / pixwidth;
    double slitWidthHalf = pixels_per_thou * ui->slitWidthSb->value() * 1000 * ((ui->useMM->isChecked()) ? 1./25.4 : 1.);
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
                        slit[0].at<double>(y,x) = 255;
                    }
                }
                int knife_side = x;
                if (ui->knifeOnLeftCb->isChecked())
                    knife_side = size - x;

                if (knife_side > hx + ui->lateralKnifeSb->value()* 1000 *
                    ((ui->useMM->isChecked()) ? 1./25.4 : 1.) * pixels_per_thou)
                {
                    vknife[0].at<double>(y,x) = 255.;
                }


                // ronchi setup
                if ((even && (line_no%2 == 0)) || (!even && (line_no%2 != 0)))
                {
                    ronchiGrid[0].at<double>(y,x) = 255.;
                }
                if(++roffset >= ppl)
                {
                    ++line_no;
                    roffset = 0;
                }


                if (x> hx - ppl/2. && x < hx + ppl/2.)
                {
                    ronchiSlit[0].at<double>(y,x) = 255;
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
    dft(complexIn2, FFT2, DFT_REAL_OUTPUT);
    cv::Mat knifeSlit;
    mulSpectrums(FFT1, FFT2, knifeSlit, 0, true);
    idft(knifeSlit, knifeSlit, DFT_SCALE); // gives us the correlation result...
    cv::Mat knifeSurf;

    mulSpectrums(knifeSlit, surf_fft, knifeSurf,0,true);
    idft(knifeSurf, knifeSurf, DFT_SCALE);

    QImage ronchi = showMag(knifeSurf, false,"", false, gamma);
    int startx = size - wf->data.cols;
    ronchi = ronchi.copy(startx,startx,wf->data.cols, wf->data.cols);

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
    startx = size - wf->data.cols;
    foucault = foucault.copy(startx,startx,wf->data.cols, wf->data.cols);



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
    m_guiTimer.start(500);
}

void foucaultView::on_slitWidthSb_editingFinished()
{
    m_guiTimer.start(500);
}

void foucaultView::on_lateralKnifeSb_editingFinished()
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
    ui->lateralKnifeSb->setValue(ui->lateralKnifeSb->value() * mul);
    ui->lateralKnifeSb->setSuffix(suffix);
    ui->slitWidthSb->setValue(ui->slitWidthSb->value() * mul);
    ui->slitWidthSb->setSuffix(suffix);
    ui->lpiSb->setValue(ui->lpiSb->value() / mul);
    ui->gridGroupBox->setTitle((checked) ? "Ronchi LPmm ": "Ronchi LPI ");
    m_guiTimer.start(500);
}

void foucaultView::on_scanPb_clicked()
{
    for (double roc = -.5; roc < .5; roc += .01){
        ui->rocOffsetSb->setValue(roc);

        qDebug() << roc;
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
