#include "outlinedialog.h"
#include "ui_outlinedialog.h"
#include <QSettings>
#include <QDebug>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QShortcut>
#include <QMouseEvent>
#include <QScreen>
#include <opencv2/imgproc.hpp>

outlineDialog::outlineDialog(double x, double y, double rad, QWidget *parent) :
    m_x(x),m_y(y),m_rad(rad),
    QDialog(parent), m_xoffset(0), m_yoffset(0), m_radiusOffset(0),
    ui(new Ui::outlineDialog), m_find(false), m_hideOutline(false), dragMode(false)
{
    ui->setupUi(this);
    QSettings set;
    ui->thresholdSlider->blockSignals(true);
    ui->thresholdSlider->setValue(m_threshold = set.value("outlineDlgThreshold", 40).toInt());
    ui->thresholdSlider->blockSignals(false);
    ui->thresholdSb->blockSignals(true);
    ui->thresholdSb->setValue(m_threshold);
    ui->thresholdSb->blockSignals(false);
    ui->blurSlider->blockSignals(true);
    ui->blurSlider->setValue(m_blurrSize = set.value("outlineDLgblurr",3).toInt());
    ui->blurSlider->blockSignals(false);
    ui->blurSB->blockSignals(true);
    ui->blurSB->setValue(m_blurrSize);
    ui->blurSB->blockSignals(false);
    ui->contrastSlider->blockSignals(true);
    m_contrast = set.value("outlineDlgContrast,", .5).toDouble();
    ui->contrastSlider->setValue(m_contrast * 100);
    ui->contrastSlider->blockSignals(false);
    m_findEdgePixels = set.value("outlineDlgshowEdgePixels", true).toBool();

    ui->display->setBackgroundRole(QPalette::Dark);
    ui->display->setAutoFillBackground(true);
    QRect rec = QApplication::desktop()->screenGeometry();
    int height = rec.height();
    int width = rec.width();
    ui->showEdgePixelsCB->blockSignals(true);
    ui->showEdgePixelsCB->setChecked(false);
    ui->showEdgePixelsCB->blockSignals(false);
    hideSearchcontrole(true);
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Down), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(shiftDown()));
    shortcut = new QShortcut(QKeySequence("d"), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(shiftUp()));
    shortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(shiftLeft()));
    shortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(shiftRight()));
    shortcut = new QShortcut(QKeySequence(Qt::Key_Minus), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(decrease()));
    shortcut = new QShortcut(QKeySequence(Qt::Key_Plus), this);
    QObject::connect(shortcut, SIGNAL(activated()), this, SLOT(increase()));
    //ui->display->resize(QSize(height - 100, height - 100));
}
void outlineDialog::shiftDown(){
    m_y += 1;
    updateOutline();
}
void outlineDialog::shiftUp(){
    m_y -= 1;
    updateOutline();
}
void outlineDialog::shiftLeft(){
    m_x -= 1;
    updateOutline();
}
void outlineDialog::shiftRight(){
    m_x += 1;
    updateOutline();
}
void outlineDialog::decrease(){
    m_rad -= 1;
    updateOutline();
}
void outlineDialog::increase(){
    m_rad += 1;
    updateOutline();
}

void outlineDialog::accept(){
    QSettings set;
    set.setValue("outlineDlgContrast", m_contrast);
    set.setValue("outlineDlgThreshold", m_threshold);
    set.setValue("outlineDLgblurr", m_blurrSize);
    m_x += m_xoffset;
    m_y += m_yoffset;
    m_rad += m_radiusOffset;
    QDialog::accept();

}

outlineDialog::~outlineDialog()
{
    QSettings set;
    set.setValue("outlineDlgThreshold",m_threshold);
    delete ui;
}


void outlineDialog::updateDisplay(cv::Mat img){

    QImage tmp = QImage((uchar*)img.data,
                        img.cols,
                        img.rows,
                        img.step,
                        QImage::Format_RGB888).copy();

    ui->display->setPixmap(QPixmap::fromImage(tmp).scaled(ui->display->width(),ui->display->height(),
                                                                                  Qt::KeepAspectRatio));
}

void outlineDialog::setImage(cv::Mat img){
    cv::Mat tmp;
    img.convertTo(tmp, CV_8UC1);
    cv::cvtColor(tmp,m_igram, cv::COLOR_GRAY2RGB);
    QRect rec = QGuiApplication::primaryScreen()->geometry();
    int height = rec.height();
    int width = rec.width();
    if (img.rows > height || img.cols > width){
        //qDebug() << "resising" << ui->display->size() << img.cols << img.rows;
        ui->display->resize(img.rows, img.rows);
    }
    if (m_x == 0){
        m_find = true;
        ui->showEdgePixelsCB->setChecked(true);
        hideSearchcontrole(false);
    }
    updateOutline();
    update();



}
void outlineDialog::updateOutline(){
    std::vector<std::vector<cv::Point> > contours;
    cv::Mat src;
    cv::Scalar color(255,255,0);
    double x = m_x;
    double y = m_y;
    double radius = m_rad;
    cv::normalize(m_igram, src, 255,0,cv::NORM_MINMAX);
    cv::Mat display = src;
    if (1) {
        if (m_blurrSize > 0)
            cv::blur( src, src, cv::Size(m_blurrSize,m_blurrSize) );
        cv::Mat src2;

        cv::cvtColor(src, src2, cv::COLOR_BGR2GRAY);
        cv::Mat bimage = src2 >= (m_threshold);

        cv::Mat bicolor;
        bimage.convertTo(bicolor, CV_8UC3);
        cv::cvtColor(bimage, bicolor, cv::COLOR_GRAY2BGR);

        double alpha = 1. - m_contrast;
        double beta = 1-alpha;
        cv::addWeighted(src, alpha, bicolor, beta, 0.0, display);

        //src = src * m_contrast;

        cv::findContours(bimage, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

        cv::Mat cimage; //= m_igram.clone();
        m_igram.convertTo(cimage,CV_8UC1);

        float maxSize =0;
        cv::RotatedRect maxBox;
        int maxndx = -1;
        for(size_t i = 0; i < contours.size(); i++)
        {
            size_t count = contours[i].size();
            if( count < 6 )
                continue;

            cv::Mat pointsf;
            cv::Mat(contours[i]).convertTo(pointsf, CV_32F);
            cv::RotatedRect box = cv::fitEllipse(pointsf);
            if( MAX(box.size.width, box.size.height) > MIN(box.size.width, box.size.height)*30 )
                continue;
            float area = box.boundingRect().width * box.boundingRect().height;

            if (area > maxSize){

                maxSize =  area;
                maxBox = box;
                maxndx = i;
            }
        }
        if (maxndx == -1)
            return;
        if (m_findEdgePixels){
            drawContours(display, contours, maxndx, cv::Scalar(0,255,100), 3, 8);
        }
        cv::Scalar color(255,255,0);
        if (m_find){
            x = maxBox.center.x;
            y = maxBox.center.y;
            radius = maxBox.size.width/2.;

            m_x = x;
            m_y = y;
            m_rad = radius;
        }
    }
    m_find = false;
    if (!m_hideOutline){
        radius += m_radiusOffset;
        x+= m_xoffset;
        y += m_yoffset;

        int right = x + radius;
        int left = x - radius;
        int top = y - radius;
        int bottom = y + radius;
        if (left < 0 || top < 0 || right > src.cols || bottom > src.rows){
            color = cv::Scalar(255,0,0);
        }
        cv::circle(display, cv::Point2d(x,y), radius, color,ui->outlineThickness->value());
    }
//    cv::Point2f vtx[4];
//    maxBox.points(vtx);
//    for( int j = 0; j < 4; j++ )
//        line(display, vtx[j], vtx[(j+1)%4], cv::Scalar(0,255,0), 1, CV_AA);
    ui->status->setText( QString("x,y %1,%2 radius:%3 ").arg(x, 6, 'f', 1).arg(y, 6, 'f', 1).arg(radius, 6, 'f', 1));

    updateDisplay(display);
}

void outlineDialog::on_blurSB_valueChanged(int arg1)
{
    if (m_findEdgePixels)
        m_find = true;
    ui->blurSlider->blockSignals(true);
    ui->blurSlider->setValue(arg1);
    m_blurrSize = arg1;
    ui->blurSlider->blockSignals(false);
    updateOutline();
}
void outlineDialog::on_blurSlider_valueChanged(int value)
{
   if (m_findEdgePixels)
    m_find = true;
    m_blurrSize = value;
    ui->blurSB->blockSignals(true);
    ui->blurSB->setValue(value);
    ui->blurSB->blockSignals(false);
    updateOutline();
}

void outlineDialog::on_thresholdSlider_valueChanged(int value)
{
    if (m_findEdgePixels)
        m_find = true;
    m_threshold = value;

    ui->thresholdSb->blockSignals(true);
    ui->thresholdSb->setValue(value);
    ui->thresholdSb->blockSignals(false);
    updateOutline();
}

void outlineDialog::on_contrastSlider_valueChanged(int value)
{
    m_contrast = value /100.;
    m_find = true;
    updateOutline();
}


void outlineDialog::on_radiusoffset_valueChanged(double arg1)
{
    m_radiusOffset = arg1;
    updateOutline();
}



void outlineDialog::on_showEdgePixelsCB_clicked(bool checked)
{
    m_findEdgePixels = checked;
    hideSearchcontrole(!checked);
    m_find = checked;
    updateOutline();
}

void outlineDialog::on_YOffsetSBar_valueChanged(int value)
{
    m_yoffset = value;
    ui->yoffsetSb->blockSignals(true);
    ui->yoffsetSb->setValue(value);
    ui->yoffsetSb->blockSignals(false);
    updateOutline();
}

void outlineDialog::on_XoffsetSBar_valueChanged(int value)
{
    m_xoffset = value;
    ui->xoffsetSB->blockSignals(true);
    ui->xoffsetSB->setValue(value);
    ui->xoffsetSB->blockSignals(false);
    updateOutline();
}

void outlineDialog::on_xoffsetSB_valueChanged(int arg1)
{
    m_xoffset = arg1;
    ui->XoffsetSBar->blockSignals(true);
    ui->XoffsetSBar->setValue(m_xoffset);
    ui->XoffsetSBar->blockSignals(false);
    updateOutline();
}

void outlineDialog::on_yoffsetSb_valueChanged(int arg1)
{
    m_yoffset = arg1;
    ui->YOffsetSBar->blockSignals(true);
    ui->YOffsetSBar->setValue(m_yoffset);
    ui->YOffsetSBar->blockSignals(false);
    updateOutline();
}

void outlineDialog::on_radiusSBar_valueChanged(int value)
{
    m_radiusOffset = 2 * value + 1;
    updateOutline();
}

void outlineDialog::on_pushButton_clicked()
{
    QString msg("This tries to find the circular mirror in the analysis by various values of blur"
                " and thresholding to highlight the mirror edge pixels.\n\n"
                "Yellow circle marks the proposed mirror outline.\n\n"
                "To enable finding and marking edge pixes  shown in green adjust any of the controls.\n"
                "Green pixels are thought to be on the edge of the mirror.\n"
                "The software tries to fit a yellow circle to the green pixels.\n"
                "Red line indicates that the circle goes off the edge of the image and is not usable.\n\n"
                "Adjust the threshold to change the green pixels and outline position.\n"
                "Adjust the blurr to smooth the green outline if it is too noisy\n\n"
                "The far right slider mixes the view between the raw image and the thresholded image.\n\n\n"
                "Press OK to use the displayed outline.\n"
                "Press cancel to abort the PSI process.");

    QMessageBox msgHelp;
    msgHelp.setText(msg);
    msgHelp.setStyleSheet("QLabel{min-width: 900px;}");
    msgHelp.setWindowTitle(" outline help");
    msgHelp.setStandardButtons(QMessageBox::Ok);
    msgHelp.setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    msgHelp.exec();

}

void outlineDialog::on_checkBox_clicked(bool checked)
{
    m_hideOutline = checked;
    updateOutline();
}
void outlineDialog::hideSearchcontrole(bool hide){
    if (hide){
        ui->blurSB->hide();
        ui->blurSlider->hide();
        ui->thresholdSb->hide();
        ui->thresholdSlider->hide();
        ui->contrastSlider->hide();
        ui->threshLab->hide();
        ui->blurLab->hide();
        ui->mixlab1->hide();
        ui->mixlab2->hide();
    }
    else {
        ui->blurSB->show();
        ui->blurSlider->show();
        ui->thresholdSb->show();
        ui->thresholdSlider->show();
        ui->contrastSlider->show();
        ui->threshLab->show();
        ui->blurLab->show();
        ui->mixlab1->show();
        ui->mixlab2->show();
    }
}
void outlineDialog::mousePressEvent(QMouseEvent *event)
{


    if (event->button() == Qt::LeftButton) {
        QPointF Raw = event->pos();

        QPointF pos = Raw;

        setCursor(Qt::OpenHandCursor);
        dragMode = true;
            //cntrlPressed = event->modifiers() & Qt::ControlModifier;
        lastPoint = Raw;
        return;

    }


}

void outlineDialog::mouseMoveEvent(QMouseEvent *event)
{
    //if (igramGray.isNull())
        //return;

    QPointF pos = event->pos();
    //QPointF scaledPos = pos/scale;

    // truncat point to int.
    //scaledPos.setX((int)scaledPos.x());
    //scaledPos.setY((int)scaledPos.y());

    if ((event->buttons() & Qt::LeftButton) & dragMode){

            QPointF del = pos - lastPoint;
            m_x += del.x();
            m_y += del.y();
            updateOutline();
    }
    lastPoint = pos;
}

void outlineDialog::mouseReleaseEvent(QMouseEvent * /*event*/)
{

    setCursor(Qt::ArrowCursor);

    dragMode = false;
}
void outlineDialog::wheelEvent(QWheelEvent *e){
    if (e->angleDelta().y() == 0)
        return;

    int del = e->angleDelta().y()/120;

    m_rad += del;
    updateOutline();
}

void outlineDialog::on_outlineThickness_valueChanged(int /*arg1*/)
{
    updateOutline();
}

