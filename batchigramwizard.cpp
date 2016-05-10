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
QRadioButton *batchIgramWizard::autoRb = 0;
QRadioButton *batchIgramWizard::manualRb = 0;
QPushButton *batchIgramWizard::goPb = 0;

batchIgramWizard::batchIgramWizard(QStringList files, QWidget *parent, Qt::WindowFlags flags) :
    QWizard(parent, flags),
    ui(new Ui::batchIgramWizard)
{
    ui->setupUi(this);
    setPage(batchIgramWizard::Page_Intro, new batchIntro(files, parent));
    setStartId(batchIgramWizard::Page_Intro);
    setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/igram6.png").scaled(40, 40, Qt::IgnoreAspectRatio, Qt::FastTransformation));
    //setPixmap(QWizard::LogoPixmap, QPixmap(":/icons/igram6.png"));
    //setOption(HaveHelpButton, true);
    //setWindowTitle(tr("Batch Process Interferograms"));
    button(QWizard::BackButton)->setVisible(false);

    emit swapBathConnections(true);
}

batchIgramWizard::~batchIgramWizard()
{
    emit swapBathConnections(false);
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

batchIntro::batchIntro(QStringList files, QWidget *manager, QWidget *p):
    QWizardPage(p){
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

    QPushButton *addFiles = new QPushButton("Add Files");
    connect( addFiles, SIGNAL(pressed()), this, SLOT(addFiles()));
    batchIgramWizard::autoRb = new QRadioButton("Auto",this);
    batchIgramWizard::manualRb = new QRadioButton("manual",this);
    batchIgramWizard::manualRb->setChecked(true);

    QHBoxLayout  *hlayout = new QHBoxLayout();
    batchIgramWizard::goPb = new QPushButton("Process Igrams",this);
    connect(batchIgramWizard::goPb, SIGNAL(pressed()), this, SLOT(processBatch()));
    batchIgramWizard::goPb->setStyleSheet("QPushButton{"
                       " background-color: red;"
                        "border-style: outset;"
                        "border-width: 4px;"
                        "border-radius: 10px;"
                        "border-color: darkgray;"
                        "font: bold 14px;"
                        "min-width: 10em;"
                        "padding: 6px;"
                        "background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
                        "stop:0 white, stop: 0.8 lawngreen, stop:1 black);"
                        "pressed {background-color: rgb(224, 0, 0) border-style: inset;}}"
                    "QPushButton:!enabled   { background-color:lightgray    } Working");
    hlayout->addWidget(batchIgramWizard::manualRb);
    hlayout->addWidget(batchIgramWizard::autoRb);
    hlayout->addWidget(addFiles);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(lb);
    layout->addLayout(hlayout);
    layout->addWidget(filesList);
    layout->addWidget(batchIgramWizard::goPb, Qt::AlignLeft);
    setLayout(layout);


}
