#include "foucaultview.h"
#include "ui_foucaultview.h"
#include "spdlog/spdlog.h"
#include "mirrordlg.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "simulationsview.h"
#include <QVector>
#include <QMenu>
#include "zernikeprocess.h"
#include "ronchicomparedialog.h"
extern double outputLambda;

foucaultView::foucaultView(QWidget *parent, SurfaceManager *sm) :
    QWidget(parent),
    ui(new Ui::foucaultView), m_sm(sm), heightMultiply(1)
{
    m_wf = 0;
    lateralOffset = 0;
    needsDrawing = false;
    ui->setupUi(this);
    QSettings set;
    ui->lpiSb->setValue(set.value("ronchiLPI", 100).toDouble());
    connect(&m_guiTimer, &QTimer::timeout, this, &foucaultView::on_makePb_clicked);
    ui->rocOffsetSb->setSuffix(" inch");
    ui->useMM->setChecked(set.value("simUseMM",false).toBool());
    ui->RonchiX->blockSignals(true);
    ui->RonchiX->setValue(set.value("RonchiROCFactor", 3).toDouble());
    ui->RonchiX->blockSignals(false);
    if (ui->useMM->isChecked()){
        on_useMM_clicked(true);
    }
    connect(this, &QWidget::customContextMenuRequested, this,
            &foucaultView::showContextMenu);
    setContextMenuPolicy(Qt::CustomContextMenu);
}




foucaultView *foucaultView::get_Instance(SurfaceManager *sm){
    //static foucaultView m_instance{0, sm};
    //return &m_instance;
    // Take care. This is non standard init for when the singleton is supposed to be deleted by parent
    // keeping original version will call class destructor and on_exit will try to clean up static variable m_instance. But the instance doesn't exist anymore.
    static foucaultView *m_instance = new foucaultView(0, sm);
    return m_instance;
}

foucaultView::~foucaultView()
{
    delete ui;
    spdlog::get("logger")->trace("foucaultView::~foucaultView");
}
QString getSaveFileName(const QString &type){
    QSettings settings;
    QString path = settings.value("lastPath","").toString();

    QString fileName = QFileDialog::getSaveFileName(0,
            QString("File name of %1 image to be saved").arg(type),
                                                 path);

    if (!fileName.endsWith(".jpg"))
        fileName = fileName + ".jpg";
    return fileName;

}
void foucaultView::showContextMenu(QPoint pos)
{

// Handle global position
    QPoint globalPos = mapToGlobal(pos);
    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Save Ronchi image",  this, &foucaultView::saveRonchiImage);
    myMenu.addAction("Save Foucault Image", this,  &foucaultView::saveFoucaultImage);
    QAction  *showAllRonchi = new QAction("Show all Selected Wave Fronts using Ronchi");
    connect (showAllRonchi, &QAction::triggered,this, &foucaultView::showSelectedRonchiImages);
    myMenu.addAction(showAllRonchi);

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void foucaultView::showSelectedRonchiImages(){

    surfaceAnalysisTools *saTools = surfaceAnalysisTools::get_Instance();
    QList<int> list = saTools->SelectedWaveFronts();

    QList<wavefront*> wfs;

    for (int i = 0; i < list.size(); ++i){
        wfs << m_sm->m_wavefronts.at(list[i]);
    }
    generateBatchRonchiImage(wfs);
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
    QSettings set;
    double offset = set.value("foucault roc offset", 0.).toDouble();
    m_wf = wf;
    mirrorDlg *md = mirrorDlg::get_Instance();
    double rad = md->diameter/2.;
    double FL = md->roc/2.;
    double mul = (ui->useMM->isChecked()) ? 1. : 1/25.4;
    m_sag = mul * (rad * rad) /( 4 * FL);
    m_sag = round(100 * m_sag)/100.;
    m_temp_sag = m_sag;     // used to set the zernike slider step size to make it appropriate to a faction of the sagitta.

    ui->rocOffsetSb->blockSignals(true);
    ui->rocOffsetSb->setValue(offset);
    ui->rocOffsetSb->blockSignals(false);
    on_autoStepSize_clicked(ui->autoStepSize->isChecked());
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


QImage foucaultView::generateOpticalTestImage(OpticalTestType type, wavefront* wf, const OpticalTestSettings& s, bool bAutoCollimate)
{
    if (!wf || wf->data.cols == 0) return QImage();

    // 1. Setup Constants
    double pad = 1.1;
    int size = (int(wf->data.cols * pad) / 2) * 2;
    double actualPad = (double)size / wf->data.cols;
    double moving_constant = (s.movingSource) ? 1.0 : 2.0;

    mirrorDlg *md = mirrorDlg::get_Instance();
    double unitMultiplyer = s.useMM ? 1.0 : 25.4;
    double coc_offset_mm = s.rocOffset * unitMultiplyer;

    // Physics geometry
    double r2 = (md->diameter / 2.0) * (md->diameter / 2.0);
    double b = md->roc + coc_offset_mm;
    double pv = (sqrt(r2 + (md->roc * md->roc)) - (sqrt(r2 + b * b) - coc_offset_mm)) / (md->lambda * 1.E-6);
    double z3 = pv / moving_constant;
    double effectiveZ3 = (type == OpticalTestType::Ronchi) ? (s.ronchiX * z3) : z3;

    // 2. Wavefront Prep
    std::vector<double> originalZerns = wf->InputZerns;
    std::vector<double> tempZerns = originalZerns;
    tempZerns[3] -= 3 * tempZerns[8];
    wf->InputZerns = tempZerns;

    SimulationsView *sv = SimulationsView::getInstance(0);
    sv->setSurface(wf);

    bool oldDoNull = md->doNull;
    if (bAutoCollimate == false)
        md->doNull = false; // this is normal foucault/ronchi so we *don't* subtract the null (autcoCollimate ronchi or foucault mode will typically subtract the null)

    cv::Mat surf_fft = sv->computeStarTest(s.heightMultiply * sv->nulledSurface(effectiveZ3), size, actualPad, true);

    wf->InputZerns = originalZerns; // Restore state immediately
    md->doNull = oldDoNull;

    // 3. Mask Generation
    cv::Mat mask = cv::Mat::zeros(size, size, CV_64FC1);
    cv::Mat sourceSlit = cv::Mat::zeros(size, size, CV_64FC1);
    int hx = (size - 1) / 2 + s.lateralOffset;
    double pixwidth = s.outputLambda * 1.E-6 * (0.5 * md->roc / md->diameter) * 2. / (25.4 * actualPad);

    if (type == OpticalTestType::Ronchi) {
        double lpi_val = s.lpi * (s.useMM ? 25.4 : 1.0);
        int ppl = std::max(1, (int)((0.5 / lpi_val) / pixwidth));
        int start = (size / 2) - (ppl / 2);
        bool even = ((start / ppl) % 2 == 0) != s.clearCenter;
        int roffset_start = ppl - (start % ppl);

        for (int y = 0; y < size; ++y) {
            int line_no = 0, roffset = roffset_start;
            for (int x = 0; x < size; ++x) {
                if (((even && (line_no % 2 == 0)) || (!even && (line_no % 2 != 0)))) mask.at<double>(y, x) = 1.0;
                if (++roffset >= ppl) { line_no++; roffset = 0; }
                if (x > hx - ppl / 2. && x < hx + ppl / 2.) sourceSlit.at<double>(y, x) = 1.0;
            }
        }
    } else {
        double slitWidthHalf = (.001 / pixwidth) * s.slitWidth * 1000 * (s.useMM ? 1./25.4 : 1.0);
        for (int y = 0; y < size; ++y) {
            double ry = double(y - hx) / hx;
            for (int x = 0; x < size; ++x) {
                if (sqrt(pow(double(x - hx)/hx, 2) + ry*ry) <= 1.0 && std::abs(x - hx) < slitWidthHalf) sourceSlit.at<double>(y, x) = 255.0;
                int k_side = s.knifeOnLeft ? (size - x) : x;
                if (k_side > hx) mask.at<double>(y, x) = 255.0;
            }
        }
    }

    // 4. DFT Pipeline
    cv::Mat FFT1, FFT2, complexMask, complexSlit, combinedFilter, finalResult;
    cv::Mat planesM[] = {mask, cv::Mat::zeros(mask.size(), CV_64FC1)};
    cv::Mat planesS[] = {sourceSlit, cv::Mat::zeros(sourceSlit.size(), CV_64FC1)};
    cv::merge(planesM, 2, complexMask);
    cv::merge(planesS, 2, complexSlit);

    cv::dft(complexMask, FFT1, cv::DFT_REAL_OUTPUT);
    cv::dft(complexSlit, FFT2, cv::DFT_REAL_OUTPUT);
    if (type == OpticalTestType::Ronchi) { shiftDFT(FFT1); shiftDFT(FFT2); }

    cv::mulSpectrums(FFT1, FFT2, combinedFilter, 0, true);
    cv::idft(combinedFilter, combinedFilter, cv::DFT_SCALE);
    if (type == OpticalTestType::Ronchi) shiftDFT(combinedFilter);

    cv::mulSpectrums(combinedFilter, surf_fft, finalResult, 0, true);
    cv::idft(finalResult, finalResult, cv::DFT_SCALE);
    if (type == OpticalTestType::Ronchi) shiftDFT(finalResult);

    // 5. Output Image
    QImage res = showMag(finalResult, false, "", false, s.gamma);
    int startx = size - wf->data.cols;
    return res.copy(startx, startx, wf->data.cols, wf->data.cols).mirrored(true, false);
}


void foucaultView::on_makePb_clicked()
{
    m_guiTimer.stop();
    if (m_wf == nullptr || m_wf->data.cols == 0)
        return;

    if (mirrorDlg::get_Instance()->isEllipse()){
        QMessageBox::warning(0,"warning","Foucaualt is not suppported for flat surfaces");
        return;
    }

    QApplication::setOverrideCursor(Qt::WaitCursor);


    // 1. Pack the current UI state into the settings struct
    OpticalTestSettings settings;
    settings.rocOffset    = ui->rocOffsetSb->value();
    settings.ronchiX      = ui->RonchiX->value();
    settings.lpi          = ui->lpiSb->value();
    settings.gamma        = ui->gammaSb->value();
    settings.slitWidth    = ui->slitWidthSb->value();
    settings.useMM        = ui->useMM->isChecked();
    settings.movingSource = ui->movingSourceRb->isChecked();
    settings.knifeOnLeft  = ui->knifeOnLeftCb->isChecked();
    settings.clearCenter  = ui->clearCenterCb->isChecked();

    // Pass the class members that aren't driven by UI widgets
    settings.heightMultiply = this->heightMultiply;
    settings.lateralOffset  = this->lateralOffset;
    settings.outputLambda   = outputLambda;

    // 2. Call the refactored static engine for both images
    QImage ronchiImg = generateOpticalTestImage(OpticalTestType::Ronchi, m_wf, settings, ui->autocollimation->isChecked());
    QImage foucaultImg = generateOpticalTestImage(OpticalTestType::Foucault, m_wf, settings, ui->autocollimation->isChecked());

    // Store for potential saving/external access
    m_foucaultQimage = foucaultImg;

    // 3. UI Painting Helper (to avoid duplicating the Painter logic)
    auto paintAndDisplay = [&](QLabel* label, const QImage& img, double offsetValue) {
        if (img.isNull()) return;

        QSize s = label->size();
        QPixmap pix = QPixmap::fromImage(img.scaledToWidth(s.width()));

        QPainter painter(&pix);
        painter.save();
        painter.setPen(QPen(QColor(Qt::white)));
        painter.setFont(QFont("Arial", 15));

        QString unit = ui->useMM->isChecked() ? "mm" : "in";
        QString zStr = QString("%1 %2").arg(offsetValue, 6, 'f', 3).arg(unit);
        painter.drawText(20, 40, zStr);

        if (ui->overlayProfile->isChecked()) {
            QPolygonF profile = m_sm->m_profilePlot->createProfile(1.0, m_wf);
            // Assuming scaleProfile is a helper accessible in this scope
            QVector<QPoint> profilePoints = scaleProfile(profile, pix.width(), M_PI/4.0);
            painter.setPen(QPen(QColor(Qt::yellow), 3));
            painter.drawLines(profilePoints);
        }
        painter.restore();
        label->setPixmap(pix);
    };

    // 4. Update the actual labels
    paintAndDisplay(ui->ronchiViewLb, ronchiImg, settings.ronchiX * settings.rocOffset);
    paintAndDisplay(ui->foucaultViewLb, foucaultImg, settings.rocOffset);

    QApplication::restoreOverrideCursor();
}


void foucaultView::generateBatchRonchiImage(const QList<wavefront*>& wavefrontList)
{
    // 1. Initial Checks
    if (wavefrontList.isEmpty() || !m_wf) return;

    // 2. Ask user for Grid Layout
    bool ok;
    int cols = QInputDialog::getInt(this, tr("Batch Ronchi"),
                                    tr("Number of columns:"), 2, 1, 10, 1, &ok);
    if (!ok) return;

    // 3. Prepare Optical Settings from UI
    OpticalTestSettings s;
    s.rocOffset      = ui->rocOffsetSb->value();
    s.ronchiX        = ui->RonchiX->value();
    s.lpi            = ui->lpiSb->value();
    s.gamma          = ui->gammaSb->value();
    s.slitWidth      = ui->slitWidthSb->value();
    s.useMM          = ui->useMM->isChecked();
    s.movingSource   = ui->movingSourceRb->isChecked();
    s.knifeOnLeft    = ui->knifeOnLeftCb->isChecked();
    s.clearCenter    = ui->clearCenterCb->isChecked();
    s.heightMultiply = this->heightMultiply;
    s.lateralOffset  = this->lateralOffset;
    s.outputLambda   = outputLambda;

    // 4. Calculate Grid and Canvas Geometry
    int count = wavefrontList.size();
    int rows = (count + cols - 1) / cols;
    int imgDim = m_wf->data.cols;

    int headerHeight = 70;
    int textBuffer = 40;

    int cellW = imgDim;
    int cellH = imgDim + textBuffer;

    // Total Canvas size
    QImage canvas(cellW * cols, (cellH * rows) + headerHeight, QImage::Format_RGB32);
    canvas.fill(Qt::black);

    QPainter painter(&canvas);
    painter.setRenderHint(QPainter::Antialiasing);
    QApplication::setOverrideCursor(Qt::WaitCursor);

    // NEW: Container for individual Ronchi images to be used in comparison
    QList<QImage> individualRonchis;
    QList<QString> names;
    // 5. Draw Simulation Header
    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 12, QFont::Bold));
    QString unit = s.useMM ? "mm" : "in";
    QString headerText = QString("Ronchi Analysis | LPI: %1 | Offset: %2 %3")
                         .arg(s.lpi).arg(s.rocOffset).arg(unit);
    painter.drawText(20, 35, headerText);

    painter.setPen(QPen(Qt::gray, 2));
    painter.drawLine(10, headerHeight - 15, canvas.width() - 10, headerHeight - 15);

    // 6. Loop through and Render Ronchi Patterns
    painter.setFont(QFont("Arial", 11, QFont::Bold));
    QFontMetrics fm(painter.font());

    for (int i = 0; i < count; ++i) {
        wavefront* currentWf = wavefrontList[i];
        int row = i / cols;
        int col = i % cols;

        QImage ronchi = generateOpticalTestImage(OpticalTestType::Ronchi, currentWf, s, ui->autocollimation->isChecked());

        if (!ronchi.isNull()) {
            // Store a copy for the comparison feature
            individualRonchis.append(ronchi);

            int xPos = col * cellW;
            int yPos = headerHeight + (row * cellH);

            painter.drawImage(xPos, yPos, ronchi);

            QFileInfo fileInfo(currentWf->name);
            QString displayName = fileInfo.baseName();
            names << displayName;
            int textWidth = fm.horizontalAdvance(displayName);
            int xText = xPos + (cellW - textWidth) / 2;
            int yText = yPos + imgDim + (textBuffer / 2) + (fm.ascent() / 2);

            painter.setPen(Qt::yellow);
            painter.drawText(xText, yText, displayName);
        }
    }
    painter.end();
    QApplication::restoreOverrideCursor();

    // 7. Configure Preview Dialog
    QScreen *screen = QGuiApplication::primaryScreen();
    int dlgW = static_cast<int>(screen->availableGeometry().width() * 0.75);
    int dlgH = static_cast<int>(screen->availableGeometry().height() * 0.85);

    QDialog previewDlg(this);
    previewDlg.setWindowTitle(tr("Batch Ronchi Analysis Preview"));
    previewDlg.resize(dlgW, dlgH);

    QVBoxLayout *layout = new QVBoxLayout(&previewDlg);
    QScrollArea *scroll = new QScrollArea(&previewDlg);
    scroll->setWidgetResizable(true);
    scroll->setAlignment(Qt::AlignCenter);
    //scroll->setStyleSheet("background-color: #1a1a1a;");

    QLabel *imgLabel = new QLabel();
    imgLabel->setAlignment(Qt::AlignCenter);
    scroll->setWidget(imgLabel);
    layout->addWidget(scroll);

    // 8. Zoom Slider Integration
    QPixmap previewPixmap = QPixmap::fromImage(canvas);
    QHBoxLayout *zoomLayout = new QHBoxLayout();
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(10, 400);
    slider->setValue(100);

    QLabel *zoomValueLabel = new QLabel("100%");
    zoomLayout->addWidget(new QLabel(tr("Zoom: ")));
    zoomLayout->addWidget(slider);
    zoomLayout->addWidget(zoomValueLabel);
    layout->addLayout(zoomLayout);

    auto updateZoom = [previewPixmap, imgLabel, zoomValueLabel, dlgW](int val) {
        int targetWidth = (dlgW - 80) * val / 100;
        QPixmap scaled = previewPixmap.scaledToWidth(targetWidth, Qt::SmoothTransformation);
        imgLabel->setPixmap(scaled);
        imgLabel->setFixedSize(scaled.size());
        zoomValueLabel->setText(QString("%1%").arg(val));
    };

    connect(slider, &QSlider::valueChanged, updateZoom);
    updateZoom(100);

    // 9. Navigation and Comparison Buttons
    QHBoxLayout *btns = new QHBoxLayout();

    // NEW: Comparison button
    QPushButton *compareBtn = new QPushButton(tr("Compare Top Two Patterns"));
    compareBtn->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    // Feature only enabled if 2 or more wavefronts were processed
    compareBtn->setEnabled(individualRonchis.size() >= 2);

    QPushButton *saveBtn = new QPushButton(tr("Save Grid Image"));
    QPushButton *cancelBtn = new QPushButton(tr("Close"));

    btns->addWidget(compareBtn);
    btns->addStretch();
    btns->addWidget(saveBtn);
    btns->addWidget(cancelBtn);
    layout->addLayout(btns);

    // Connect the comparison trigger
    connect(compareBtn, &QPushButton::clicked, &previewDlg, [=, &previewDlg]() {
        // [=] copies individualRonchis and names so they stay
        // valid even after generateBatchRonchiImage() returns.
        if (individualRonchis.size() >= 2) {
            RonchiCompareDialog compDlg(individualRonchis[0], names[0],
                                        individualRonchis[1], names[1], &previewDlg);
            compDlg.exec();
        }
    });


    connect(saveBtn, &QPushButton::clicked, &previewDlg, &QDialog::accept);
    connect(cancelBtn, &QPushButton::clicked, &previewDlg, &QDialog::reject);


    // 10. Execute Dialog and Save Grid
    if (previewDlg.exec() == QDialog::Accepted) {
        QString path = QFileDialog::getSaveFileName(this, tr("Save Ronchi Grid"),
                                                    imageDir, tr("Images (*.png *.jpg)"));
        if (!path.isEmpty()) {
            canvas.save(path);
        }
    }
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
    QSettings set;
    set.setValue("foucault roc offset", val);

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
    double start = ui->scanStart->value();
    double end = ui->scanEndOffset->value();
    double step = ui->scanSteps->value();
    if (step == 0)
       step = .001;
    foucaultView *fv = foucaultView::get_Instance(0);
    int cnt = 0;
    QSettings settings;
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
            QString fvpng = QString("%1//%2.png").arg(imageDir).arg(cnt++, 6, 10, QLatin1Char('0'));
            qDebug() << "fn"<< fvpng;
            if (ui->saveOnlyFouccault->isChecked()){
                fv->m_foucaultQimage.save(fvpng);
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
    QSettings set;

    set.setValue("foucault roc offset", offset);

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
    double offset = (m_sag/2);
    QSettings set;
    set.setValue("foucault roc offset", offset);
    ui->rocOffsetSb->blockSignals(true);
    ui->rocOffsetSb->setValue(offset);
    ui->rocOffsetSb->blockSignals(false);
    on_autoStepSize_clicked(ui->autoStepSize->isChecked());
    needsDrawing = true;
    on_makePb_clicked();
}





void foucaultView::on_autocollimation_clicked(bool checked)
{
    if (checked)
        ui->h2x->setChecked(true);
    else
        ui->h1x->setChecked(true);
    on_makePb_clicked();
}

