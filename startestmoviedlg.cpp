#include "startestmoviedlg.h"
#include "ui_startestmoviedlg.h"
#include <QSettings>
#include <QFileDialog>
#include <QScreen>
#include "simulationsview.h"
startestMovieDlg::startestMovieDlg(SimulationsView *view) :
    m_view(view),    ui(new Ui::startestMovieDlg)
{
    ui->setupUi(this);
    QSettings set;
    ui->wavestart->setValue(set.value("star test start", -10).toDouble());
    ui->waveEnd->setValue(set.value("star test end", 10).toDouble());
    ui->brightnesssb->blockSignals(true);
    ui->brightnesssb->setValue(set.value("star test bright", 1).toDouble());
    ui->brightnesssb->blockSignals(false);
    ui->path->setText(set.value("star test Path", "").toString());
    ui->stepsize->setValue(set.value("star test step", .1).toDouble());
    QScreen *screen = QGuiApplication::primaryScreen();
    int size = screen->availableSize().height()/2;
    ui->theImage->resize(size,size);
    view->makeFrame(-5., this);
}

startestMovieDlg::~startestMovieDlg()
{
    delete ui;
}

void startestMovieDlg::on_start_clicked()
{
    m_recording = true;
    m_frameNumber = 0;
    qDebug() << "started";
    for (double wave = ui->wavestart->value(); m_recording && (ui->stepsize->value() >0)? wave <= ui->waveEnd->value() : wave >= ui->waveEnd->value(); wave += ui->stepsize->value()){
        qDebug() << "wave number" << wave;
        m_view->makeFrame(wave/2., this);
    }
    m_recording = false;
}


QImage brighten(QImage image ,int brightness_factor){
    QImage imaget = image.copy();

    for (int y = 0; y < imaget.height(); ++y) {
        for (int x = 0; x < imaget.width(); ++x) {
            QRgb pixel = imaget.pixel(x, y);
            int red = qRed(pixel);
            int green = qGreen(pixel);
            int blue = qBlue(pixel);
            red = qMin(255, red + brightness_factor);
            green = qMin(255, green + brightness_factor);
            blue = qMin(255, blue + brightness_factor);
            red = qBound(0, red, 255);
            green = qBound(0, green, 255);
            blue = qBound(0, blue, 255);
            imaget.setPixel(x, y, qRgb(red, green, blue));
        }
    }
    return imaget;
}

void startestMovieDlg::setImage(QImage image){
    m_image = image.copy();


    QImage imaget = brighten(m_image, m_bright);

    QPixmap myPixmap = QPixmap::fromImage(imaget);


    ui->theImage->setPixmap(myPixmap.scaled(ui->theImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->theImage->setScaledContents(true); // Ensures the label itself scales the content
    if (m_recording){
        QString name = QString("/frame%1").arg(m_frameNumber++, 3 ,10, QLatin1Char('0'));

         imaget.save(ui->path->text()+name + ".jpg");
    }

}
void startestMovieDlg::on_browse_clicked()
{
    QSettings settings;
    QString filmDir = settings.value("lastPath","").toString();

    filmDir = QFileDialog::getExistingDirectory(this, tr("Directory where images are to be saved"),
                                                 filmDir,
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);

    if (filmDir.isEmpty())
        return;
    ui->path->setText(filmDir);
    settings.setValue("star test Path", filmDir);
}


void startestMovieDlg::on_abort_clicked()
{
    m_recording = false;
}


void startestMovieDlg::on_brightnesssb_valueChanged(int value)
{

    m_bright = value;
    if (m_image.isNull()) return;
    QImage imaget = brighten(m_image, value);


    QPixmap myPixmap = QPixmap::fromImage(imaget);


    ui->theImage->setPixmap(myPixmap.scaled(ui->theImage->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->theImage->setScaledContents(true); // Ensures the label itself scales the content
}

void startestMovieDlg::on_wavestart_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("star test start", arg1);
}


void startestMovieDlg::on_waveEnd_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("star test end", arg1);
}


void startestMovieDlg::on_stepsize_valueChanged(double arg1)
{
    QSettings set;
    set.setValue("star test step", arg1);
}




