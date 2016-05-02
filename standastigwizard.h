#ifndef STANDASTIGWIZARD_H
#define STANDASTIGWIZARD_H
#include <QListWidget>
#include <QWizard>
QT_BEGIN_NAMESPACE
class QCheckBox;
class QLabel;
class QTextEdit;
class QRadioButton;
class QScrollArea;
class QLineEdit;
class QPushButton;
class QListWidget;
class SurfaceManager;
QT_END_NAMESPACE
namespace Ui {
    class standAstigWizard;
}


enum { Page_Intro, Page_makeAverages,Page_define_input};
extern QString AstigReportTitle;
extern QString AstigReportPdfName;
class standAstigWizard : public QWizard
{
    Q_OBJECT

public:
    explicit standAstigWizard(SurfaceManager *sm, QWidget *parent = 0);
    static QString AstigReportTitle;
    static QString AstigReportPdfName;
    ~standAstigWizard();

private:
    Ui::standAstigWizard *ui;
};

class IntroPage : public QWizardPage
{
    Q_OBJECT

public:
    IntroPage(QWidget *parent = 0);

    //int nextId() const;

private:
    QTextEdit *info;



};


class rotationDef {
public:
    QString fname;
    double angle;
    rotationDef(QString name, double angle) :
        fname(name), angle(angle){};
};

class makeAverages : public QWizardPage
{
    Q_OBJECT
public:
    makeAverages(QWidget * p= 0);
};

class define_input : public QWizardPage
{
    Q_OBJECT

public:
    define_input(QWidget *parent = 0);
    QListWidget *listDisplay;
    QList<rotationDef *> rotationList;
    QPushButton *pdfName;
    QLineEdit *title;
    QLineEdit *basePath;
    QRadioButton *CWRb;
    QRadioButton *CCWRb;

    //int nextId() const;

private:
    QLabel *lab1;
    QPushButton *browsePb;
    QLabel *lab2;

private slots:
    void compute();
    void pdfNamesPressed();
    void browse();
    void deleteSelected();
    void setBasePath();
    void showContextMenu(const QPoint &pdos);
    void changeRotation();
signals:
    void computeStandAstig(QList<rotationDef *>);
};


#endif // STANDASTIGWIZARD_H
