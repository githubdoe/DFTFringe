/******************************************************************************
**
**  Copyright 2016 Dale Eason
**  This file is part of DFTFringe
**  is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation version 3 of the License

** DFTFringe is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with DFTFringe.  If not, see <http://www.gnu.org/licenses/>.

****************************************************************************/
#include "standastigwizard.h"
#include "ui_standastigwizard.h"
#include <QtWidgets>
#include <QDebug>
#include <counterrotationdlg.h>
#include <QString>
#include "surfacemanager.h"
QString AstigReportTitle = "";
QString AstigReportPdfName = "stand.pdf";
standAstigWizard::standAstigWizard(SurfaceManager *sm, QWidget *parent,Qt::WindowFlags flags) :
    QWizard(parent,flags),
    ui(new Ui::standAstigWizard)
{
    ui->setupUi(this);
    setPage(Page_Intro, new IntroPage);
    setPage(Page_makeAverages, new makeAverages);
    setPage(Page_define_input, new define_input);
    define_input * di = dynamic_cast<define_input *>(page(Page_define_input));
    connect(di, SIGNAL(computeStandAstig(define_input *,QList<rotationDef *>)), sm, SLOT(computeStandAstig(define_input *,QList<rotationDef *>)));

    setStartId(Page_Intro);
    //setOption(HaveHelpButton, true);
    setWindowTitle(tr("Stand Astig analysis"));
    setPixmap(QWizard::WatermarkPixmap, QPixmap(":/res/wats2.png"));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/res/mirror_stand.png"));
    //AstigReportTitle = mirrorDlg::get_Instance()->m_name;
    //resize( QSize(600, 489));
}

standAstigWizard::~standAstigWizard()
{
    delete ui;
}
IntroPage::IntroPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Overview"));
    setSubTitle(tr(" "));

    info = new QTextEdit(tr("<p style=\"font-size:15px\">Remove test stand astig by counter rotating several"
                            " mirror rotations.  These need to be pairs rotated 90 deg apart."
                            " <p style=\"font-size:15px\">To use:"

                            "<OL>"
                            "<li> Before using this wizard, Average each group of wavefronts at same rotation angle and save each averaged group. (It is good to "
                            "include the rotation angle in the name of the averaged file.</li>"
                            "<li>Select a group of files to be analyzed. If a file name has a rotation angle in it that will be used as the rotation angle. </li>"
                            "<li>Inspect the rotation anlge for each file and alter if necessary.</li>"
                            "</li>Press compute</li>"
                            "<li>A pdf file will be generated with the results.</li><br>"
                            "</ol>"
                            "It will counter rotate each of the wavefront files you give it.<br>"
                            "Then it will average them all together and save that as an average in <i>\"Resulting average file</i>\".<br>"
                            "<p style=\"font-size:15px\">"
                            "It will create a pdf file with up to 5 sections.<ul>"
                            "<li>Section 1: shows contours pairs. </li>"
                            "<li>Section 2: shows countours pairs counter rotated.</li>"
                            " <li>Section 3: shows a graph of the x and y astig values for each file.  This graph is used to verify if the resutls are valid.<br>"
                            "              Valid results will have the points on a circle or circles with almost the same diameters.<br>"
                            "              The diameter of the circle represents the astig difference caused by the system at that mirror rotation.<br>"
                            "              If the analysis is valid the center of the circle will the by system astig.</li>"
                            " <li>Section 4: the average of all the counter rotated input files. If the results are valid it will contain only the mirror astig values.</li>"
                            " <li>Section 5: the test stand only contour for each input.</li></ul></p>"

                            "<p style=\"font-size:15px\">if <i><b>include Stand Only Analysis</i></b> is checked "
                            "  It will counter rotate that average to to match each of the original "
                            "  files and then subtract that average from each of those files"
                            "  Resulting in a new wavefront for each of the original files."
                            "  Use those files to see what the stand does to each rotation."
                            "  They should all look similar.  If not then the mirror was not"
                            "  supported the same for each rotation.<br></p>"));




    info->setReadOnly(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(info,10);

    setLayout(layout);
}
makeAverages::makeAverages(QWidget *){
    setTitle(tr("<H1>Prerequisites:</H1>"));
    setSubTitle(" ");
    QTextEdit *txt = new QTextEdit("<p style=\"font-size:15px\"><ul>"
                                   "<br><li>Plan to rotate the mirror on the test stand and take several interferograms at each rotation."
                                   "You need at least two rotation angles that are 90 deg apart. For instance 0,90 deg or 45,135 deg"
                                   " It is better to have more than one 90 deg pair for stastical reasons to reduce variations in test"
                                   " stand induce astig.</li>"
                                   "<li>The test stand needs to induce the same amount of astig at each rotation. If it does not it"
                                                                      " will be obvious in the results of the process if you use more than one pair of rotations.</li>"
                                   "<br><li>Analyze each igram and then average the results of each rotation position. Save the average file for each.<br>"
                                   "Encode the original mirror rotation into the average file name (ie avgCW90.wft for 90 deg clockwise)</li>"
                                   "<br>If the program does not find a number in the file name it looks for it in the Directory name.</ul></p>"
                                   "<p style=\"font-size:15px\"> <b>Important Note:</b><ul style=\"list-style-type:none\""
                                   "<li> Remember that the orthoganal version of the bath interferometer reverses the direction of mirror rotation.</li>"
                                   " That is because when the camera is at 90 deg to the interferometer mirror optical axis the side mirror"
                                   " flips the interferogram image left to right.</p>"
                                   );
    txt->setReadOnly(true);
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(txt);
    setLayout(layout);
}
void define_input::pdfNamesPressed(){
    QString fileName = QFileDialog::getSaveFileName((QWidget* )0, "Export PDF", QString(mirrorDlg::get_Instance()->getProjectPath() +
                                                                                        "/stand.pdf"), "*.pdf");
    if (fileName.isEmpty())
        return;
    if (QFileInfo(fileName).suffix().isEmpty()) { fileName.append(".pdf"); }
    AstigReportPdfName = fileName;
    pdfName->setText(fileName);
    QSettings set;
    set.setValue("stand pdf file", fileName.split("/").last());
}
void define_input::setBasePath(){
    QString baseName = QFileDialog::getExistingDirectory(
                this, "Get Base Directory of rotation average files.",
                basePath->text());
    if (baseName.isEmpty())
        return;
    basePath->setText(baseName);
    QSettings set;
    set.setValue("rotation base path", baseName);
}

void define_input::deleteSelected(){
   QList<QListWidgetItem *> list =  listDisplay->selectedItems();
   for (int i=0; i<list.size(); i++) {
       int row = listDisplay->row(list[i]);
        QListWidgetItem *item = listDisplay->takeItem(row);
        delete item;
        rotationList.removeAt(row);
   }
}

void define_input::changeRotation(){
}

void define_input::showContextMenu(const QPoint &pos)
{
    // Handle global position
    QPoint globalPos = listDisplay->mapToGlobal(pos);

    // Create menu and insert some actions
    QMenu myMenu;
    myMenu.addAction("Erase",  this, SLOT(deleteSelected()));

    // Show context menu at handling position
    myMenu.exec(globalPos);
}

define_input::define_input(QWidget *parent)
    : QWizardPage(parent)
{
    QSettings set;
    setTitle(tr("Specify average input files"));
    setSubTitle(tr("Add each averaged wavefront for each rotation angle. Then Press Compute."));
    browsePb = new QPushButton("Add average Wavefront file to List");
    QString pdfNameStr = set.value("stand pdf file", "stand.pdf").toString();
    connect(browsePb, SIGNAL(pressed()), this, SLOT(browse()));
    AstigReportTitle = mirrorDlg::get_Instance()->m_name;
    AstigReportPdfName = mirrorDlg::get_Instance()->getProjectPath() + "/" + pdfNameStr;
    title = new QLineEdit(AstigReportTitle);
    pdfName = new QPushButton(AstigReportPdfName);
    connect(pdfName, SIGNAL(pressed()), this, SLOT(pdfNamesPressed()));
    runpb = new QPushButton("Compute");
    runpb->setObjectName("Compute");

    this->setStyleSheet("QPushButton#Compute {"
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
                    "QPushButton:hover {border-style: inset; background-color:lightblue;}"
                    "QPushButton#Compute:hover {"
                        "background-color: lightblue;"
                        " border-style: inset;}"
                     "QPushButton#Compute:!enabled {background-color: lightgray}"
                     "QPushButton { font: 16px;"
                        "border-style: outset;"
                        "border-width: 4px;"
                        "border-radius: 10px;"
                        "border-color: darkgray;"
                        "min-width: 10em;"
                        "padding: 6px;"
                                   " }");


    connect(runpb, SIGNAL(pressed()), this, SLOT(compute()));
    QSettings settings;
    QString lastPath = settings.value("projectPath","").toString();
    basePath = new QLineEdit(settings.value("rotation base path",lastPath).toString());
    basePath->setToolTip("Directory were rotation files are stored.");
    QPushButton *browsePath = new QPushButton("...");
    CWRb = new QRadioButton("Rotate CW");
    CCWRb = new QRadioButton("Rotate CCW");
    CCWRb->setChecked(true);
    connect(browsePath, SIGNAL(pressed()), this, SLOT(setBasePath()));

    browsePath->setStyleSheet("");
    QGridLayout *l = new QGridLayout();

    lab2 = new QLabel(tr("List of Average files to counter rotate and the angle they were made at originally:"
                         "   Hint: left click to select then right click to modify item."));
    l->addWidget(new QLabel("        Base path: "),2,0);
    l->addWidget(basePath,2,1,1,4);
    l->addWidget(CWRb, 3,4);
    l->addWidget(CCWRb, 3,5);
    l->addWidget(browsePath,2,5);
    l->addWidget(browsePb,3,0,1,3,Qt::AlignLeft);
    l->addWidget(new QLabel("   "),4,0);
    l->addWidget(lab2,6,0,2,10);
    listDisplay = new QListWidget();
    listDisplay->setSelectionMode( QAbstractItemView::MultiSelection);
    listDisplay->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(listDisplay, SIGNAL(customContextMenuRequested(QPoint)), this,
            SLOT(showContextMenu(QPoint)));

    l->addWidget(listDisplay,8,0,10,-1);
    l->addWidget(new QLabel("Report Title:"),18,0);
    l->addWidget(title, 18,1);
    showWork = new QCheckBox("Show work Files");
    l->addWidget(showWork, 18,2);
    l->addWidget(new QLabel("Pdf File Name:"), 19,0);
    l->addWidget(pdfName, 19,1,1,-1);
    l->addWidget(runpb, 20,0,1,7);
    setLayout(l);

}


void define_input::compute(){
    if (listDisplay->count() < 2){
        QMessageBox::warning(0, "", "You must first add at least 2 wavefront files to the list.");
        return;
    }

    AstigReportTitle = title->text();
    runpb->setText("Working");
    runpb->setEnabled(false);
    emit computeStandAstig( this, rotationList);
}

QString getNumberFromQString(const QString &xString)
{

  QStringList l = xString.split("/");
  QString fn = l[l.size()-1];
  QRegExp xRegExp("(\\d+([\\.p]\\d+)?)");
  xRegExp.indexIn(fn);
  QString c = xRegExp.cap();
  c.replace("p",".");
  if (c == ""){
      xRegExp.indexIn(l[l.size()-2]);
      c = xRegExp.cap();
      c.replace("p",".");
  }
  return c;
}
void define_input::browse(){

    QStringList fileNames = QFileDialog::getOpenFileNames(this,
                        tr("Select average ffile"), basePath->text(),
                        tr("wft (*.wft)"));
    if (fileNames.isEmpty())
        return;
    foreach (QString fileName, fileNames){
        QString srot = getNumberFromQString(fileName);
        double rot = 0;
        if (srot != "")
            rot = srot.toDouble();
        CounterRotationDlg dlg( fileName,rot, true);
        dlg.setCCW(CCWRb->isChecked());
        if (dlg.exec()) {
            QString b = QString().sprintf("%s,%s,%s", fileName.toStdString().c_str(),(dlg.isClockwise()) ? "CW" : "CCW",
                                          dlg.getRotation().toStdString().c_str());
            new QListWidgetItem(b,listDisplay);
            rotationDef *rd = new rotationDef(fileName, (dlg.isClockwise() ? 1 : -1) * dlg.getRotation().toDouble());
            rotationList.append(rd);
        }
    }

}


void standAstigWizard::on_standAstigWizard_helpRequested()
{
    QString link = qApp->applicationDirPath() + "/res/Help/StandAstigHelp.html";
    QDesktopServices::openUrl(QUrl(link));
}
