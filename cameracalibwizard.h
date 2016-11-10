#ifndef CAMERACALIBWIZARD_H
#define CAMERACALIBWIZARD_H

#include <QWizard>
#include <QListWidget>

namespace Ui {
class cameraCalibWizard;
}

class cameraCalibWizard : public QWizard
{
    Q_OBJECT
    enum { Page_Intro, Page_1};
public:
    explicit cameraCalibWizard(QWidget *parent = 0);
    ~cameraCalibWizard();

private slots:
    void on_cameraCalibWizard_accepted();

    void on_cameraCalibWizard_helpRequested();

private:
    Ui::cameraCalibWizard *ui;
};
class camWizIntro : public QWizardPage
{

    Q_OBJECT

    public:
        camWizIntro(QWidget *parent = 0);
public slots:

signals:

};

#endif // CAMERACALIBWIZARD_H
