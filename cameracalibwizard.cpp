#include "cameracalibwizard.h"
#include "ui_cameracalibwizard.h"
#include <QLabel>
#include <QLayout>
#include "camwizardpage1.h"
#include "spdlog/spdlog.h"

cameraCalibWizard::cameraCalibWizard(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::cameraCalibWizard)
{

    ui->setupUi(this);
    setPage(Page_Intro, new camWizIntro(parent));
    setPage(Page_1, new CamWizardPage1(parent));
    setStartId(cameraCalibWizard::Page_Intro);
    //resize( QSize(600, 489).expandedTo(minimumSizeHint()) );
    //setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/res/camera-icon-60.png").scaled(120, 120, Qt::IgnoreAspectRatio, Qt::FastTransformation));
}

cameraCalibWizard::~cameraCalibWizard()
{
    spdlog::get("logger")->trace("cameraCalibWizard::~cameraCalibWizard");
    delete ui;
}

void cameraCalibWizard::on_cameraCalibWizard_accepted()
{

}

camWizIntro::camWizIntro(QWidget *p):
    QWizardPage(p){
    setTitle(tr("Experimental"));
    //setSubTitle(tr(""));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/res/wats2.png"));
    QLabel *lb = new QLabel(
                "<html><head/><body><p><img src='res/camera-icon-60.png' align = 'right' /></p><p>You can use this to determine if your camera and lens combination has"
                " signifcant distortion.</p><p>If so it can also be used to remove that distortion from the interferogram."
                "</p><p>You will need to take several picures of a special target.</p></body></html>"
                );

    QVBoxLayout *layout = new QVBoxLayout();

    layout->addWidget(lb);
    setLayout(layout);


}

#include <QDesktopServices>
#include <QUrl>
void cameraCalibWizard::on_cameraCalibWizard_helpRequested()
{
    QString link = qApp->applicationDirPath() + "/res/Help/lensDistort.html";
    QDesktopServices::openUrl(QUrl(link));
}
