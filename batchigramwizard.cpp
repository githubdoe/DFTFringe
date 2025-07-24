#include "batchigramwizard.h"
#include "ui_batchigramwizard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include "mainwindow.h"
#include "astigscatterplot.h"
#include "rmsplot.h"
#include "wavefrontfilterdlg.h"
#include "surfacemanager.h"
QCheckBox *batchIgramWizard::autoCb = 0;
QCheckBox *batchIgramWizard::filterCb = 0;
QPushButton *batchIgramWizard::goPb = 0;
QPushButton *batchIgramWizard::skipFile = 0;
QPushButton *batchIgramWizard::addFiles = 0;
QCheckBox *batchIgramWizard::saveFile = 0;
QCheckBox *batchIgramWizard::showProcessPlots = 0;
QCheckBox *batchIgramWizard::deletePreviousWave;
QPushButton *batchIgramWizard::saveZerns = 0;
QLabel *batchIgramWizard::memStatus = 0;
QCheckBox *batchIgramWizard::makeReviewAvi = 0;
QString batchIgramWizard::reviewFileName;
QCheckBox *batchIgramWizard::autoOutlineCenter = 0;
QCheckBox *batchIgramWizard::autoOutlineOutside = 0;

batchIgramWizard::batchIgramWizard(QStringList files, QWidget *parent, Qt::WindowFlags flags) :
    QWizard(parent, flags),
    ui(new Ui::batchIgramWizard)
{
    ui->setupUi(this);
    introPage = new batchIntro(files, parent);
    setPage(batchIgramWizard::Page_Intro, introPage);
    setStartId(batchIgramWizard::Page_Intro);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/res/igram6.png").scaled(40, 40, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    //setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/res/igram6.png"));
    //setOption(HaveHelpButton, true);
    //setWindowTitle(tr("Batch Process Interferograms"));
    QList<QWizard::WizardButton> layout;
    layout << QWizard::Stretch << QWizard::CancelButton;
    setButtonLayout(layout);
    resize(800,600);
}

batchIgramWizard::~batchIgramWizard()
{
    delete ui;
}

void batchIntro::processBatch(){
    QStringList list;
    for(int row = 0; row < filesList->count(); row++)
    {
             list.append(filesList->item(row)->text());
    }
    emit processBatchList(list);
}
void batchIntro::addFiles(){
    QStringList mimeTypeFilters;
    foreach (const QByteArray &mimeTypeName, QImageReader::supportedMimeTypes())
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    QSettings settings;
    QString lastPath = settings.value("lastPath",".").toString();
    QFileDialog dialog(this, tr("Open File"),lastPath);
    dialog.setFileMode(QFileDialog::ExistingFiles);
    dialog.setAcceptMode(QFileDialog::AcceptOpen);
    dialog.setMimeTypeFilters(mimeTypeFilters);
    dialog.selectMimeTypeFilter("image/jpeg");

    if (dialog.exec()){
        filesList->addItems(dialog.selectedFiles());
        update();
    }

}
void batchIntro::eraseItem()
{
    // If multiple selection is on, we need to erase all selected items
    for (int i = 0; i < filesList->selectedItems().size(); ++i) {
        // Get curent item on selected row
        QListWidgetItem *item = filesList->takeItem(filesList->currentRow());
        // And remove it
        delete item;
    }
}

void batchIntro::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = filesList->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Erase",  this, SLOT(eraseItem()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

void batchIntro::setupPlots(){

}

void batchIntro::showPlots(bool flag){
    if (flag){
        astigPlot->show();
        m_rmsPlot->show();
    }
    else{
        astigPlot->hide();
        m_rmsPlot->hide();
    }
}

batchIntro::batchIntro(QStringList files, QWidget *manager, QWidget *p):
    QWizardPage(p),filterFile(false),filterWavefront(false),filterDlg(0)
{
    setTitle(tr("Overview"));
    setSubTitle(tr(" "));
    //setPixmap(QWizard::WatermarkPixmap, QPixmap(":/res/wats2.png"));
    QLabel *lb = new QLabel(
                "<html><head/><body><p>You can process many Igrams in either Manual Mode: Where the program "
                "will load an igram and wait for you to adjust the outline and then press done.<br/>"
                "Then the program will process that igram and wait for you to adjust the DFT center filter. "
                "When you press compute surface it will create the analysis and wavefront.</p><p>In Auto mode "
                "it will not wait but process the igram with the previously used outline and center filter values."
                "</p>.</body></html>"
                );
    filesList = new QListWidget;
    foreach(QString fn, files){
        QListWidgetItem *itm = new QListWidgetItem(fn);
        filesList->addItem(itm);
    }
    filesList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(filesList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));

    connect(this, SIGNAL(processBatchList(QStringList)), qobject_cast<MainWindow *>(manager), SLOT(batchProcess(QStringList)));
    QSettings set;

    pgrBar = new QProgressBar;
    batchIgramWizard::addFiles = new QPushButton(tr("Add Files"));
    batchIgramWizard::skipFile = new QPushButton(tr("Skip"));
    connect(batchIgramWizard::skipFile, SIGNAL(clicked(bool)), qobject_cast<MainWindow *>(manager), SLOT(skipBatchItem()));
    batchIgramWizard::skipFile->setEnabled(false);
    connect(batchIgramWizard::addFiles, SIGNAL(pressed()), this, SLOT(addFiles()));
    batchIgramWizard::autoCb = new QCheckBox(tr("Auto"),this);
    batchIgramWizard::filterCb = new QCheckBox(tr("Filter"),this);
    batchIgramWizard::filterCb->setChecked( set.value("batchWizardFilterFlag", false).toBool());
    connect(batchIgramWizard::filterCb, SIGNAL(clicked(bool)), this, SLOT(on_filter(bool)));
    batchIgramWizard::saveFile = new QCheckBox(tr("Save wavefront file"),this);
    batchIgramWizard::saveFile->setChecked(set.value("batchSaveFile", false).toBool());
    batchIgramWizard::deletePreviousWave = new QCheckBox(tr("Delete Prev Wave"), this);
    batchIgramWizard::deletePreviousWave->setToolTip("Keeps only the last 3 analyzed wavefronts in memory."
                                                     "\n This attemts to free up memory so more igrams can be analysed in batch mode.");
    batchIgramWizard::deletePreviousWave->setChecked(set.value("deletePrevWave", false).toBool());
    batchIgramWizard::showProcessPlots = new QCheckBox(tr("Show Process Plots"));
    batchIgramWizard::showProcessPlots->setChecked(true);
    connect(batchIgramWizard::showProcessPlots, SIGNAL(clicked(bool)),this, SLOT(showPlots(bool)));
    connect(batchIgramWizard::saveFile, SIGNAL(clicked(bool)),this, SLOT(on_saveFiles(bool)));
    connect(batchIgramWizard::deletePreviousWave, SIGNAL(clicked(bool)),this, SLOT(on_deletePreviousWave(bool)));
    QHBoxLayout  *hlayout = new QHBoxLayout();
    QGroupBox  *outlineGB = new QGroupBox("Auto Outlines");
    batchIgramWizard::goPb = new QPushButton(tr("Process Igrams"),this);
    connect(batchIgramWizard::goPb, SIGNAL(pressed()), this, SLOT(processBatch()));
    batchIgramWizard::goPb->setStyleSheet("QPushButton{"
                       " background-color: red;"
                        "border-style: outset;"
                        "border-width: 4px;"
                        "border-radius: 10px;"
                        "border-color: darkgray;"
                        "font: bold 14px;"
                        "padding: 6px;"
                        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                        "stop:0 white, stop: 0.8 lawngreen, stop:1 black);"
                        "pressed {background-color: rgb(224, 0, 0) border-style: inset;}}"
                    "QPushButton:!enabled   { background-color:lightgray    } Working");
    batchIgramWizard::saveZerns = new QPushButton(tr("SaveZerns"), this);
    batchIgramWizard::autoOutlineCenter = new QCheckBox("auto outline center hole");
    batchIgramWizard::autoOutlineCenter->setToolTip(tr("auto outline center if not present in .oln file or if .oln does not exist."));
    //batchIgramWizard::autoOutlineOutside = new QCheckBox("auto outline mirror outside");
    QHBoxLayout *gbLayout = new QHBoxLayout();
    //gbLayout->addWidget(batchIgramWizard::autoOutlineOutside);
    gbLayout->addWidget(batchIgramWizard::autoOutlineCenter);
    gbLayout->addWidget(pgrBar);
    outlineGB->setLayout(gbLayout);
    batchIgramWizard::makeReviewAvi = new QCheckBox(tr("Make review images"));
    batchIgramWizard::makeReviewAvi->setToolTip("Create a sub directory \"review\" where each file \nshows the igram, dft, 3D and contour plot\n"
                                                " of each interferogram analyzed.");

    batchIgramWizard::saveZerns->setEnabled(false);
    batchIgramWizard::saveZerns->setToolTip("If wavefronts were being deleted to save space.\n"
                                            "Use this to save the zernike values just created by\nthe "
                                            "Batch process in a .csv file.");
    outlineGB->setLayout(gbLayout);
    hlayout->addWidget(batchIgramWizard::autoCb);
    hlayout->addWidget(batchIgramWizard::filterCb);
    hlayout->addWidget(batchIgramWizard::saveFile);
    hlayout->addWidget(batchIgramWizard::deletePreviousWave);
    hlayout->addWidget(batchIgramWizard::addFiles);
    hlayout->addWidget(batchIgramWizard::skipFile);
    batchIgramWizard::memStatus = new QLabel();
    QVBoxLayout *layout = new QVBoxLayout();
    astigPlot = new astigScatterPlot;
    m_rmsPlot = new rmsPlot;
    QHBoxLayout *hlayout2 = new QHBoxLayout;
    QHBoxLayout *hlayout3 = new QHBoxLayout;
    hlayout2->addWidget(astigPlot);
    hlayout2->addWidget(m_rmsPlot);
    astigPlot->hide();
    m_rmsPlot->hide();
    layout->addWidget(lb);
    layout->addLayout(hlayout);
    layout->addWidget(outlineGB);
    layout->addWidget(filesList);
    hlayout3->addWidget(batchIgramWizard::goPb,0, Qt::AlignLeft);
    hlayout3->addWidget(batchIgramWizard::showProcessPlots);
    hlayout3->addWidget(batchIgramWizard::makeReviewAvi);
    hlayout3->addWidget(batchIgramWizard::memStatus);
    hlayout3->addWidget(batchIgramWizard::saveZerns);
    layout->addLayout(hlayout3);
    layout->addLayout(hlayout2);
    setLayout(layout);
    setupPlots();
    filterRms = set.value("filterRMS", .1).toDouble();
    filterFile  = set.value("filterRemoveFlag", false).toBool();

}
void batchIntro::on_saveFiles(bool flag){
    QSettings set;
    set.setValue("batchSaveFile", flag);
}
void batchIntro::on_deletePreviousWave(bool flag){
    QSettings set;
    set.setValue("deletePrevWave", flag);
}

void batchIntro::on_filter(bool flag){
    QSettings set;
    set.setValue("batchWizardFilterFlag", flag);
    if (flag){
        wavefrontFilterDlg dlg;
        if (dlg.exec()){
            filterRms = dlg.rms();
            filterFile = dlg.shouldFilterFile();
            filterWavefront = dlg.shouldFilterWavefront();
        }
    }
}
bool batchIntro::shouldFilterFile(double rms){
    return (filterFile && rms > filterRms);
}
bool batchIntro::shouldFilterWavefront(double rms){
    return (filterWavefront && rms > filterRms);
}

void batchIgramWizard::addAstig(QString name, QPointF value){

    introPage->astigPlot->addValue(name,value);
}

void batchIgramWizard::progressValue(int min, int max, int value){
    introPage->pgrBar->setRange(min, max);
    introPage->pgrBar->setValue(value);
}

void batchIgramWizard::addRms(QString name, QPointF p){
    introPage->m_rmsPlot->addValue(name,p);
}

void batchIgramWizard::showPlots(bool flags){
    introPage->showPlots(flags);
}

void batchIgramWizard::on_batchIgramWizard_finished(int /*result*/)
{
}

void batchIgramWizard::select(int n){

    introPage->filesList->clearSelection();
    if (introPage->filesList->count() > 0){
        introPage->filesList->setCurrentRow(n);
    }

}
