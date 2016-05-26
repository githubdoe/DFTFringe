/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionLoad_Interferogram;
    QAction *actionLoad_outline;
    QAction *actionWrite_wavefront;
    QAction *actionWrite_WaveFront;
    QAction *actionSave_outline;
    QAction *actionOutline_tools;
    QAction *actionSurface_Tool_bar;
    QAction *actionMirror;
    QAction *actionPreferences;
    QAction *actionRead_WaveFront;
    QAction *actionNext_Wave_Front;
    QAction *actionPrevious_Wave_front;
    QAction *actionDelete_wave_front;
    QAction *actionSubtract_wave_front;
    QAction *actionIgram;
    QAction *actionSave_All_WaveFront_stats;
    QAction *actionSave_Wavefront;
    QAction *actionLighting_properties;
    QAction *actionSave_screen;
    QAction *actionSave_interferogram;
    QAction *actionWavefront;
    QAction *actionWave_Front_Inspector;
    QAction *actionUser_Color_Map;
    QAction *actionSave_nulled_smoothed_wavefront;
    QAction *actionTest_Stand_Astig_Removal;
    QAction *actionSave_PDF_report;
    QAction *actionAbout;
    QAction *actionHelp;
    QAction *actionVideos;
    QAction *actionError_Margins;
    QAction *actionXxxxx;
    QAction *actionXxxxx_2;
    QAction *actionBatch_Process_Interferograms;
    QAction *actionVersion_History;
    QAction *actionSpeciall_for_Dale;
    QAction *actionIterate_outline;
    QAction *actionLatest_Version;
    QAction *actionShow_Statistics_of_Loaded_wavefronts;
    QAction *actionFull_Screen;
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QTabWidget *tabWidget;
    QWidget *IgramTab;
    QMenuBar *menuBar;
    QMenu *menuFiles;
    QMenu *menuView;
    QMenu *menuConfiguration;
    QMenu *menuTools;
    QMenu *menuSimulations;
    QMenu *menuHelp;
    QStatusBar *statusBar;
    QDockWidget *outlineTools;
    QWidget *dockWidgetContents_2;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_6;
    QPushButton *pushButton;
    QSpacerItem *horizontalSpacer_7;
    QPushButton *HelpOutlinePb;
    QGroupBox *groupBox;
    QHBoxLayout *horizontalLayout;
    QRadioButton *SelectOutSideOutline;
    QRadioButton *SelectObsOutline;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_7;
    QPushButton *saveOutline;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_3;
    QPushButton *undo;
    QPushButton *ShiftUp;
    QPushButton *shiftLeft;
    QPushButton *minus;
    QPushButton *shiftDown;
    QPushButton *pluss;
    QPushButton *shiftRight;
    QPushButton *Redo;
    QHBoxLayout *horizontalLayout_5;
    QCheckBox *checkBox;
    QPushButton *pushButton_8;
    QPushButton *pushButton_5;
    QHBoxLayout *horizontalLayout_7;
    QCheckBox *reverseGammaCB;
    QLineEdit *gammaValue;
    QCheckBox *showIntensity;
    QCheckBox *showChannels;
    QLabel *dftThumb;
    QSpacerItem *verticalSpacer;
    QToolBar *toolBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1065, 746);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MainWindow->sizePolicy().hasHeightForWidth());
        MainWindow->setSizePolicy(sizePolicy);
        MainWindow->setMinimumSize(QSize(640, 640));
        actionLoad_Interferogram = new QAction(MainWindow);
        actionLoad_Interferogram->setObjectName(QStringLiteral("actionLoad_Interferogram"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/igram6.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionLoad_Interferogram->setIcon(icon);
        actionLoad_outline = new QAction(MainWindow);
        actionLoad_outline->setObjectName(QStringLiteral("actionLoad_outline"));
        actionWrite_wavefront = new QAction(MainWindow);
        actionWrite_wavefront->setObjectName(QStringLiteral("actionWrite_wavefront"));
        actionWrite_WaveFront = new QAction(MainWindow);
        actionWrite_WaveFront->setObjectName(QStringLiteral("actionWrite_WaveFront"));
        actionSave_outline = new QAction(MainWindow);
        actionSave_outline->setObjectName(QStringLiteral("actionSave_outline"));
        actionOutline_tools = new QAction(MainWindow);
        actionOutline_tools->setObjectName(QStringLiteral("actionOutline_tools"));
        actionSurface_Tool_bar = new QAction(MainWindow);
        actionSurface_Tool_bar->setObjectName(QStringLiteral("actionSurface_Tool_bar"));
        actionMirror = new QAction(MainWindow);
        actionMirror->setObjectName(QStringLiteral("actionMirror"));
        actionPreferences = new QAction(MainWindow);
        actionPreferences->setObjectName(QStringLiteral("actionPreferences"));
        actionRead_WaveFront = new QAction(MainWindow);
        actionRead_WaveFront->setObjectName(QStringLiteral("actionRead_WaveFront"));
        actionNext_Wave_Front = new QAction(MainWindow);
        actionNext_Wave_Front->setObjectName(QStringLiteral("actionNext_Wave_Front"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/icons/Button-Next-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionNext_Wave_Front->setIcon(icon1);
        actionPrevious_Wave_front = new QAction(MainWindow);
        actionPrevious_Wave_front->setObjectName(QStringLiteral("actionPrevious_Wave_front"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/icons/Button-Previous-icon.png"), QSize(), QIcon::Normal, QIcon::On);
        actionPrevious_Wave_front->setIcon(icon2);
        actionDelete_wave_front = new QAction(MainWindow);
        actionDelete_wave_front->setObjectName(QStringLiteral("actionDelete_wave_front"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/icons/Button-Close-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDelete_wave_front->setIcon(icon3);
        actionSubtract_wave_front = new QAction(MainWindow);
        actionSubtract_wave_front->setObjectName(QStringLiteral("actionSubtract_wave_front"));
        actionIgram = new QAction(MainWindow);
        actionIgram->setObjectName(QStringLiteral("actionIgram"));
        actionSave_All_WaveFront_stats = new QAction(MainWindow);
        actionSave_All_WaveFront_stats->setObjectName(QStringLiteral("actionSave_All_WaveFront_stats"));
        actionSave_Wavefront = new QAction(MainWindow);
        actionSave_Wavefront->setObjectName(QStringLiteral("actionSave_Wavefront"));
        actionLighting_properties = new QAction(MainWindow);
        actionLighting_properties->setObjectName(QStringLiteral("actionLighting_properties"));
        actionLighting_properties->setCheckable(false);
        actionSave_screen = new QAction(MainWindow);
        actionSave_screen->setObjectName(QStringLiteral("actionSave_screen"));
        actionSave_interferogram = new QAction(MainWindow);
        actionSave_interferogram->setObjectName(QStringLiteral("actionSave_interferogram"));
        actionWavefront = new QAction(MainWindow);
        actionWavefront->setObjectName(QStringLiteral("actionWavefront"));
        actionWave_Front_Inspector = new QAction(MainWindow);
        actionWave_Front_Inspector->setObjectName(QStringLiteral("actionWave_Front_Inspector"));
        actionUser_Color_Map = new QAction(MainWindow);
        actionUser_Color_Map->setObjectName(QStringLiteral("actionUser_Color_Map"));
        actionSave_nulled_smoothed_wavefront = new QAction(MainWindow);
        actionSave_nulled_smoothed_wavefront->setObjectName(QStringLiteral("actionSave_nulled_smoothed_wavefront"));
        actionTest_Stand_Astig_Removal = new QAction(MainWindow);
        actionTest_Stand_Astig_Removal->setObjectName(QStringLiteral("actionTest_Stand_Astig_Removal"));
        actionSave_PDF_report = new QAction(MainWindow);
        actionSave_PDF_report->setObjectName(QStringLiteral("actionSave_PDF_report"));
        actionAbout = new QAction(MainWindow);
        actionAbout->setObjectName(QStringLiteral("actionAbout"));
        actionHelp = new QAction(MainWindow);
        actionHelp->setObjectName(QStringLiteral("actionHelp"));
        actionVideos = new QAction(MainWindow);
        actionVideos->setObjectName(QStringLiteral("actionVideos"));
        actionError_Margins = new QAction(MainWindow);
        actionError_Margins->setObjectName(QStringLiteral("actionError_Margins"));
        actionXxxxx = new QAction(MainWindow);
        actionXxxxx->setObjectName(QStringLiteral("actionXxxxx"));
        actionXxxxx_2 = new QAction(MainWindow);
        actionXxxxx_2->setObjectName(QStringLiteral("actionXxxxx_2"));
        actionBatch_Process_Interferograms = new QAction(MainWindow);
        actionBatch_Process_Interferograms->setObjectName(QStringLiteral("actionBatch_Process_Interferograms"));
        actionVersion_History = new QAction(MainWindow);
        actionVersion_History->setObjectName(QStringLiteral("actionVersion_History"));
        actionSpeciall_for_Dale = new QAction(MainWindow);
        actionSpeciall_for_Dale->setObjectName(QStringLiteral("actionSpeciall_for_Dale"));
        actionIterate_outline = new QAction(MainWindow);
        actionIterate_outline->setObjectName(QStringLiteral("actionIterate_outline"));
        actionLatest_Version = new QAction(MainWindow);
        actionLatest_Version->setObjectName(QStringLiteral("actionLatest_Version"));
        actionShow_Statistics_of_Loaded_wavefronts = new QAction(MainWindow);
        actionShow_Statistics_of_Loaded_wavefronts->setObjectName(QStringLiteral("actionShow_Statistics_of_Loaded_wavefronts"));
        actionFull_Screen = new QAction(MainWindow);
        actionFull_Screen->setObjectName(QStringLiteral("actionFull_Screen"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        tabWidget = new QTabWidget(centralWidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setMinimumSize(QSize(0, 0));
        tabWidget->setStyleSheet(QStringLiteral(""));
        IgramTab = new QWidget();
        IgramTab->setObjectName(QStringLiteral("IgramTab"));
        tabWidget->addTab(IgramTab, QString());

        gridLayout->addWidget(tabWidget, 0, 1, 1, 1);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1065, 21));
        menuFiles = new QMenu(menuBar);
        menuFiles->setObjectName(QStringLiteral("menuFiles"));
        menuView = new QMenu(menuBar);
        menuView->setObjectName(QStringLiteral("menuView"));
        menuConfiguration = new QMenu(menuBar);
        menuConfiguration->setObjectName(QStringLiteral("menuConfiguration"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QStringLiteral("menuTools"));
        menuSimulations = new QMenu(menuBar);
        menuSimulations->setObjectName(QStringLiteral("menuSimulations"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        MainWindow->setMenuBar(menuBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);
        outlineTools = new QDockWidget(MainWindow);
        outlineTools->setObjectName(QStringLiteral("outlineTools"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(outlineTools->sizePolicy().hasHeightForWidth());
        outlineTools->setSizePolicy(sizePolicy1);
        outlineTools->setMinimumSize(QSize(344, 867));
        outlineTools->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
        dockWidgetContents_2 = new QWidget();
        dockWidgetContents_2->setObjectName(QStringLiteral("dockWidgetContents_2"));
        gridLayout_2 = new QGridLayout(dockWidgetContents_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        pushButton = new QPushButton(dockWidgetContents_2);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setStyleSheet(QLatin1String("QPushButton {border-style: outset;\n"
"border-width: 3px; border-radius:15px; border-color: darkgray;\n"
"font: bold 12px;\n"
"min-width: 5em;\n"
"padding: 6px;}\n"
"QPushButton:hover{rgb(85, 255, 255)}\n"
"\n"
""));

        horizontalLayout_6->addWidget(pushButton);

        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_7);

        HelpOutlinePb = new QPushButton(dockWidgetContents_2);
        HelpOutlinePb->setObjectName(QStringLiteral("HelpOutlinePb"));
        HelpOutlinePb->setStyleSheet(QLatin1String("QPushButton {border-style: outset;\n"
"border-width: 3px; border-radius:15px; border-color: darkgray;\n"
"font: bold 12px;\n"
"min-width: 5em;\n"
"padding: 6px;}\n"
"QPushButton:hover{rgb(85, 255, 255)}"));

        horizontalLayout_6->addWidget(HelpOutlinePb);


        gridLayout_2->addLayout(horizontalLayout_6, 0, 0, 1, 2);

        groupBox = new QGroupBox(dockWidgetContents_2);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        groupBox->setMinimumSize(QSize(0, 0));
        groupBox->setMaximumSize(QSize(16777215, 100));
        horizontalLayout = new QHBoxLayout(groupBox);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        SelectOutSideOutline = new QRadioButton(groupBox);
        SelectOutSideOutline->setObjectName(QStringLiteral("SelectOutSideOutline"));
        SelectOutSideOutline->setFocusPolicy(Qt::NoFocus);
        SelectOutSideOutline->setChecked(true);

        horizontalLayout->addWidget(SelectOutSideOutline);

        SelectObsOutline = new QRadioButton(groupBox);
        SelectObsOutline->setObjectName(QStringLiteral("SelectObsOutline"));

        horizontalLayout->addWidget(SelectObsOutline);


        gridLayout_2->addWidget(groupBox, 1, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        pushButton_7 = new QPushButton(dockWidgetContents_2);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));

        horizontalLayout_4->addWidget(pushButton_7);

        saveOutline = new QPushButton(dockWidgetContents_2);
        saveOutline->setObjectName(QStringLiteral("saveOutline"));

        horizontalLayout_4->addWidget(saveOutline);


        gridLayout_2->addLayout(horizontalLayout_4, 2, 0, 1, 1);

        groupBox_2 = new QGroupBox(dockWidgetContents_2);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMaximumSize(QSize(16777215, 200));
        groupBox_2->setSizeIncrement(QSize(0, 0));
        gridLayout_3 = new QGridLayout(groupBox_2);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        undo = new QPushButton(groupBox_2);
        undo->setObjectName(QStringLiteral("undo"));
        undo->setMinimumSize(QSize(20, 0));
        undo->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 2\n"
"em;\n"
" padding: 6px;"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/icons/res/1461975017_undo.png"), QSize(), QIcon::Normal, QIcon::Off);
        undo->setIcon(icon4);
        undo->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(undo, 0, 2, 1, 1);

        ShiftUp = new QPushButton(groupBox_2);
        ShiftUp->setObjectName(QStringLiteral("ShiftUp"));
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(ShiftUp->sizePolicy().hasHeightForWidth());
        ShiftUp->setSizePolicy(sizePolicy3);
        ShiftUp->setMinimumSize(QSize(70, 0));
        ShiftUp->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 4em;\n"
" padding: 6px;"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/icons/res/1461974262_f-top_256.png"), QSize(), QIcon::Normal, QIcon::Off);
        ShiftUp->setIcon(icon5);
        ShiftUp->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(ShiftUp, 0, 3, 1, 1);

        shiftLeft = new QPushButton(groupBox_2);
        shiftLeft->setObjectName(QStringLiteral("shiftLeft"));
        shiftLeft->setEnabled(false);
        shiftLeft->setMinimumSize(QSize(70, 0));
        shiftLeft->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 4em;\n"
" padding: 6px;"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/icons/res/1461974250_f-left_256.png"), QSize(), QIcon::Normal, QIcon::Off);
        shiftLeft->setIcon(icon6);
        shiftLeft->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(shiftLeft, 1, 2, 1, 1);

        minus = new QPushButton(groupBox_2);
        minus->setObjectName(QStringLiteral("minus"));
        minus->setMinimumSize(QSize(70, 0));
        minus->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 4em;\n"
" padding: 6px;"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/icons/res/1461975092_minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
        minus->setIcon(icon7);
        minus->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(minus, 2, 2, 1, 1);

        shiftDown = new QPushButton(groupBox_2);
        shiftDown->setObjectName(QStringLiteral("shiftDown"));
        shiftDown->setEnabled(false);
        shiftDown->setMinimumSize(QSize(70, 0));
        shiftDown->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 4em;\n"
" padding: 6px;"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/icons/res/1461974268_f-bottom_256.png"), QSize(), QIcon::Normal, QIcon::Off);
        shiftDown->setIcon(icon8);
        shiftDown->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(shiftDown, 2, 3, 1, 1);

        pluss = new QPushButton(groupBox_2);
        pluss->setObjectName(QStringLiteral("pluss"));
        pluss->setMinimumSize(QSize(70, 0));
        pluss->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 4em;\n"
" padding: 6px;"));
        QIcon icon9;
        icon9.addFile(QStringLiteral(":/icons/res/1461975084_maximize.png"), QSize(), QIcon::Normal, QIcon::Off);
        pluss->setIcon(icon9);
        pluss->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(pluss, 2, 4, 1, 1);

        shiftRight = new QPushButton(groupBox_2);
        shiftRight->setObjectName(QStringLiteral("shiftRight"));
        shiftRight->setEnabled(false);
        shiftRight->setMinimumSize(QSize(70, 0));
        shiftRight->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 4em;\n"
" padding: 6px;"));
        QIcon icon10;
        icon10.addFile(QStringLiteral(":/icons/res/1461974256_f-right_256.png"), QSize(), QIcon::Normal, QIcon::Off);
        shiftRight->setIcon(icon10);
        shiftRight->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(shiftRight, 1, 4, 1, 1);

        Redo = new QPushButton(groupBox_2);
        Redo->setObjectName(QStringLiteral("Redo"));
        Redo->setMinimumSize(QSize(44, 0));
        Redo->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 2em;\n"
" padding: 6px;"));
        QIcon icon11;
        icon11.addFile(QStringLiteral(":/icons/res/redo.png"), QSize(), QIcon::Normal, QIcon::Off);
        Redo->setIcon(icon11);
        Redo->setIconSize(QSize(30, 30));

        gridLayout_3->addWidget(Redo, 0, 4, 1, 1);


        gridLayout_2->addWidget(groupBox_2, 3, 0, 1, 2);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        checkBox = new QCheckBox(dockWidgetContents_2);
        checkBox->setObjectName(QStringLiteral("checkBox"));
        QSizePolicy sizePolicy4(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(checkBox->sizePolicy().hasHeightForWidth());
        checkBox->setSizePolicy(sizePolicy4);
        checkBox->setStyleSheet(QLatin1String(" border-style: outset;\n"
" border-width: 3px;\n"
" border-radius:7px;\n"
" border-color: darkgray;\n"
" min-width: 4em;\n"
" padding: 6px;"));
        checkBox->setCheckable(true);

        horizontalLayout_5->addWidget(checkBox);

        pushButton_8 = new QPushButton(dockWidgetContents_2);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));

        horizontalLayout_5->addWidget(pushButton_8);

        pushButton_5 = new QPushButton(dockWidgetContents_2);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));

        horizontalLayout_5->addWidget(pushButton_5);


        gridLayout_2->addLayout(horizontalLayout_5, 4, 0, 1, 2);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        reverseGammaCB = new QCheckBox(dockWidgetContents_2);
        reverseGammaCB->setObjectName(QStringLiteral("reverseGammaCB"));
        sizePolicy3.setHeightForWidth(reverseGammaCB->sizePolicy().hasHeightForWidth());
        reverseGammaCB->setSizePolicy(sizePolicy3);

        horizontalLayout_7->addWidget(reverseGammaCB);

        gammaValue = new QLineEdit(dockWidgetContents_2);
        gammaValue->setObjectName(QStringLiteral("gammaValue"));
        QSizePolicy sizePolicy5(QSizePolicy::Maximum, QSizePolicy::Fixed);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(gammaValue->sizePolicy().hasHeightForWidth());
        gammaValue->setSizePolicy(sizePolicy5);

        horizontalLayout_7->addWidget(gammaValue);


        gridLayout_2->addLayout(horizontalLayout_7, 5, 0, 1, 2);

        showIntensity = new QCheckBox(dockWidgetContents_2);
        showIntensity->setObjectName(QStringLiteral("showIntensity"));

        gridLayout_2->addWidget(showIntensity, 6, 0, 1, 1);

        showChannels = new QCheckBox(dockWidgetContents_2);
        showChannels->setObjectName(QStringLiteral("showChannels"));

        gridLayout_2->addWidget(showChannels, 6, 1, 1, 1);

        dftThumb = new QLabel(dockWidgetContents_2);
        dftThumb->setObjectName(QStringLiteral("dftThumb"));
        dftThumb->setMinimumSize(QSize(0, 240));

        gridLayout_2->addWidget(dftThumb, 7, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 233, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer, 8, 1, 1, 1);

        outlineTools->setWidget(dockWidgetContents_2);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), outlineTools);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QStringLiteral("toolBar"));
        toolBar->setIconSize(QSize(40, 40));
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);

        menuBar->addAction(menuFiles->menuAction());
        menuBar->addAction(menuView->menuAction());
        menuBar->addAction(menuConfiguration->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuSimulations->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFiles->addAction(actionLoad_Interferogram);
        menuFiles->addAction(actionLoad_outline);
        menuFiles->addAction(actionRead_WaveFront);
        menuFiles->addSeparator();
        menuFiles->addAction(actionSave_outline);
        menuFiles->addAction(actionSave_Wavefront);
        menuFiles->addAction(actionSave_screen);
        menuFiles->addAction(actionSave_interferogram);
        menuFiles->addAction(actionSave_nulled_smoothed_wavefront);
        menuFiles->addSeparator();
        menuFiles->addAction(actionSave_PDF_report);
        menuView->addSeparator();
        menuView->addAction(actionLighting_properties);
        menuView->addAction(actionError_Margins);
        menuView->addAction(actionShow_Statistics_of_Loaded_wavefronts);
        menuConfiguration->addAction(actionMirror);
        menuConfiguration->addAction(actionPreferences);
        menuTools->addAction(actionNext_Wave_Front);
        menuTools->addAction(actionPrevious_Wave_front);
        menuTools->addAction(actionDelete_wave_front);
        menuTools->addAction(actionSubtract_wave_front);
        menuTools->addAction(actionWave_Front_Inspector);
        menuTools->addAction(actionUser_Color_Map);
        menuTools->addAction(actionTest_Stand_Astig_Removal);
        menuTools->addAction(actionIterate_outline);
        menuSimulations->addAction(actionIgram);
        menuSimulations->addAction(actionWavefront);
        menuHelp->addAction(actionAbout);
        menuHelp->addAction(actionHelp);
        menuHelp->addAction(actionVideos);
        menuHelp->addAction(actionVersion_History);
        menuHelp->addAction(actionLatest_Version);
        toolBar->addAction(actionLoad_Interferogram);
        toolBar->addAction(actionRead_WaveFront);
        toolBar->addAction(actionPrevious_Wave_front);
        toolBar->addAction(actionNext_Wave_Front);
        toolBar->addAction(actionDelete_wave_front);
        toolBar->addAction(actionSubtract_wave_front);
        toolBar->addSeparator();

        retranslateUi(MainWindow);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "DFTFringe", 0));
        actionLoad_Interferogram->setText(QApplication::translate("MainWindow", "Load Interferogram", 0));
#ifndef QT_NO_TOOLTIP
        actionLoad_Interferogram->setToolTip(QApplication::translate("MainWindow", "Load One or more interferograms.  If more than one then they will be processed as a batch.", 0));
#endif // QT_NO_TOOLTIP
        actionLoad_outline->setText(QApplication::translate("MainWindow", "Load outline", 0));
        actionWrite_wavefront->setText(QApplication::translate("MainWindow", "Save WaveFront", 0));
        actionWrite_WaveFront->setText(QApplication::translate("MainWindow", "Write WaveFront", 0));
        actionSave_outline->setText(QApplication::translate("MainWindow", "Save outline", 0));
        actionOutline_tools->setText(QApplication::translate("MainWindow", "outline tools", 0));
        actionSurface_Tool_bar->setText(QApplication::translate("MainWindow", "Surface Tool bar", 0));
        actionMirror->setText(QApplication::translate("MainWindow", "Mirror", 0));
        actionPreferences->setText(QApplication::translate("MainWindow", "Preferences", 0));
        actionRead_WaveFront->setText(QApplication::translate("MainWindow", "Read Wavefront", 0));
        actionNext_Wave_Front->setText(QApplication::translate("MainWindow", "Next Wave Front", 0));
        actionPrevious_Wave_front->setText(QApplication::translate("MainWindow", "Previous Wave front", 0));
        actionDelete_wave_front->setText(QApplication::translate("MainWindow", "Delete wave front", 0));
#ifndef QT_NO_TOOLTIP
        actionDelete_wave_front->setToolTip(QApplication::translate("MainWindow", "Delete current wave front", 0));
#endif // QT_NO_TOOLTIP
        actionSubtract_wave_front->setText(QApplication::translate("MainWindow", "Subtract wave front", 0));
        actionIgram->setText(QApplication::translate("MainWindow", "Igram", 0));
        actionSave_All_WaveFront_stats->setText(QApplication::translate("MainWindow", "Save All WaveFront stats", 0));
        actionSave_Wavefront->setText(QApplication::translate("MainWindow", "Save Wavefront", 0));
        actionLighting_properties->setText(QApplication::translate("MainWindow", "lighting properties", 0));
        actionSave_screen->setText(QApplication::translate("MainWindow", "save screen", 0));
        actionSave_interferogram->setText(QApplication::translate("MainWindow", "Save interferogram", 0));
        actionWavefront->setText(QApplication::translate("MainWindow", "wavefront", 0));
        actionWave_Front_Inspector->setText(QApplication::translate("MainWindow", "Wave Front Inspector", 0));
        actionUser_Color_Map->setText(QApplication::translate("MainWindow", "User Color Map", 0));
        actionSave_nulled_smoothed_wavefront->setText(QApplication::translate("MainWindow", "save nulled smoothed wavefront", 0));
        actionTest_Stand_Astig_Removal->setText(QApplication::translate("MainWindow", "Test Stand Astig Removal", 0));
        actionSave_PDF_report->setText(QApplication::translate("MainWindow", "Save PDF report", 0));
        actionAbout->setText(QApplication::translate("MainWindow", "About", 0));
        actionHelp->setText(QApplication::translate("MainWindow", "Help", 0));
        actionVideos->setText(QApplication::translate("MainWindow", "Videos", 0));
        actionError_Margins->setText(QApplication::translate("MainWindow", "Null error margins", 0));
        actionXxxxx->setText(QApplication::translate("MainWindow", "xxxxx", 0));
        actionXxxxx_2->setText(QApplication::translate("MainWindow", "xxxxx", 0));
        actionBatch_Process_Interferograms->setText(QApplication::translate("MainWindow", "Batch Process Interferograms", 0));
        actionVersion_History->setText(QApplication::translate("MainWindow", "Version History", 0));
        actionSpeciall_for_Dale->setText(QApplication::translate("MainWindow", "Speciall for Dale", 0));
        actionIterate_outline->setText(QApplication::translate("MainWindow", "Outline Helper", 0));
#ifndef QT_NO_TOOLTIP
        actionIterate_outline->setToolTip(QApplication::translate("MainWindow", "step the outline from -5 to + 5 pixels radius", 0));
#endif // QT_NO_TOOLTIP
        actionLatest_Version->setText(QApplication::translate("MainWindow", "Latest Version", 0));
        actionShow_Statistics_of_Loaded_wavefronts->setText(QApplication::translate("MainWindow", "Show Statistics of Loaded wavefronts", 0));
        actionFull_Screen->setText(QApplication::translate("MainWindow", "Full Screen", 0));
#ifndef QT_NO_TOOLTIP
        tabWidget->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        tabWidget->setTabText(tabWidget->indexOf(IgramTab), QApplication::translate("MainWindow", "Igram", 0));
        menuFiles->setTitle(QApplication::translate("MainWindow", "Files", 0));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0));
        menuConfiguration->setTitle(QApplication::translate("MainWindow", "Configuration", 0));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0));
        menuSimulations->setTitle(QApplication::translate("MainWindow", "Simulations", 0));
        menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
#ifndef QT_NO_WHATSTHIS
        outlineTools->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        outlineTools->setWindowTitle(QApplication::translate("MainWindow", "Interferogram Tools", 0));
#ifndef QT_NO_TOOLTIP
        pushButton->setToolTip(QApplication::translate("MainWindow", "Crop to within 20 pixels of mirror outline and go to DFT analysis ", 0));
#endif // QT_NO_TOOLTIP
        pushButton->setText(QApplication::translate("MainWindow", "Done", 0));
#ifndef QT_NO_TOOLTIP
        HelpOutlinePb->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p><div style = \"width: 300px\";>1. Load an interferogram image file.</p><p>Left click the mouse on one edge of the mirror.</p><p>With the left button down drag the mouse to the other side of the mirror.</p><p>scroll wheel will zoom the image.</p><p>[Shift] drag moves the circle.</p><p>[control] scroll wheel enlareges/shrink the circle.</p><p>Use the adjust buttons to modify the outline.</p><p>[ctrl z] Undo</p><p>[ctrl y] redo</p><p><br/></p></div></body></html>", 0));
#endif // QT_NO_TOOLTIP
        HelpOutlinePb->setText(QApplication::translate("MainWindow", "Help", 0));
#ifndef QT_NO_WHATSTHIS
        groupBox->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        groupBox->setTitle(QApplication::translate("MainWindow", "Outline Selector", 0));
#ifndef QT_NO_TOOLTIP
        SelectOutSideOutline->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Selet the to edit the mirror's outside outline.</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        SelectOutSideOutline->setStatusTip(QApplication::translate("MainWindow", "Work on outside outline", 0));
#endif // QT_NO_STATUSTIP
        SelectOutSideOutline->setText(QApplication::translate("MainWindow", "Mirror Outside", 0));
#ifndef QT_NO_TOOLTIP
        SelectObsOutline->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Selet this to edit the center hole of the mirror if presen in the interferogramt.</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        SelectObsOutline->setStatusTip(QApplication::translate("MainWindow", "Work on center hole outline", 0));
#endif // QT_NO_STATUSTIP
        SelectObsOutline->setText(QApplication::translate("MainWindow", "Central hole", 0));
#ifndef QT_NO_TOOLTIP
        pushButton_7->setToolTip(QApplication::translate("MainWindow", "Read outline from a file.", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        pushButton_7->setStatusTip(QApplication::translate("MainWindow", "Read outline from file", 0));
#endif // QT_NO_STATUSTIP
        pushButton_7->setText(QApplication::translate("MainWindow", "Read Outline", 0));
#ifndef QT_NO_TOOLTIP
        saveOutline->setToolTip(QApplication::translate("MainWindow", "Save outline to a file.", 0));
#endif // QT_NO_TOOLTIP
        saveOutline->setText(QApplication::translate("MainWindow", "Save Outline", 0));
        groupBox_2->setTitle(QApplication::translate("MainWindow", "Outline Adjust", 0));
#ifndef QT_NO_TOOLTIP
        undo->setToolTip(QApplication::translate("MainWindow", "<html><head/><body><p>Undo last outline modification</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        undo->setStatusTip(QApplication::translate("MainWindow", "Undo last modification", 0));
#endif // QT_NO_STATUSTIP
        undo->setText(QString());
#ifndef QT_NO_TOOLTIP
        ShiftUp->setToolTip(QApplication::translate("MainWindow", "Move outline up one pixel", 0));
#endif // QT_NO_TOOLTIP
        ShiftUp->setText(QString());
#ifndef QT_NO_TOOLTIP
        shiftLeft->setToolTip(QApplication::translate("MainWindow", "move outline left one pixel", 0));
#endif // QT_NO_TOOLTIP
        shiftLeft->setText(QString());
#ifndef QT_NO_TOOLTIP
        minus->setToolTip(QApplication::translate("MainWindow", "Decrease outline radius 1 pixel", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        minus->setWhatsThis(QApplication::translate("MainWindow", "Decrease outline radius 1 pixel", 0));
#endif // QT_NO_WHATSTHIS
        minus->setText(QString());
#ifndef QT_NO_TOOLTIP
        shiftDown->setToolTip(QApplication::translate("MainWindow", "move outline down one pixel", 0));
#endif // QT_NO_TOOLTIP
        shiftDown->setText(QString());
#ifndef QT_NO_TOOLTIP
        pluss->setToolTip(QApplication::translate("MainWindow", "Expand outline radius 1 pixel", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        pluss->setStatusTip(QApplication::translate("MainWindow", "Expand outline radius 1 pixel", 0));
#endif // QT_NO_STATUSTIP
        pluss->setText(QString());
#ifndef QT_NO_TOOLTIP
        shiftRight->setToolTip(QApplication::translate("MainWindow", "move outline right one pixel", 0));
#endif // QT_NO_TOOLTIP
        shiftRight->setText(QString());
#ifndef QT_NO_TOOLTIP
        Redo->setToolTip(QApplication::translate("MainWindow", "Redo last modification", 0));
#endif // QT_NO_TOOLTIP
        Redo->setText(QString());
        checkBox->setText(QApplication::translate("MainWindow", "Hide Outline", 0));
        pushButton_8->setText(QApplication::translate("MainWindow", "Delete Outline", 0));
#ifndef QT_NO_TOOLTIP
        pushButton_5->setToolTip(QApplication::translate("MainWindow", "Crop image to within 20 pixels of outline.", 0));
#endif // QT_NO_TOOLTIP
        pushButton_5->setText(QApplication::translate("MainWindow", "Crop", 0));
#ifndef QT_NO_TOOLTIP
        reverseGammaCB->setToolTip(QApplication::translate("MainWindow", "Enable Reverse gamma", 0));
#endif // QT_NO_TOOLTIP
        reverseGammaCB->setText(QApplication::translate("MainWindow", "Reverse Gamma correction", 0));
#ifndef QT_NO_WHATSTHIS
        gammaValue->setWhatsThis(QApplication::translate("MainWindow", "Reverse Gamma value", 0));
#endif // QT_NO_WHATSTHIS
        gammaValue->setText(QApplication::translate("MainWindow", "2.2", 0));
#ifndef QT_NO_WHATSTHIS
        showIntensity->setWhatsThis(QApplication::translate("MainWindow", "Show intensity graph of cross section of interferogram", 0));
#endif // QT_NO_WHATSTHIS
        showIntensity->setText(QApplication::translate("MainWindow", "Show intensity Graph", 0));
#ifndef QT_NO_WHATSTHIS
        showChannels->setWhatsThis(QApplication::translate("MainWindow", "Show each of the color channels of interferogram", 0));
#endif // QT_NO_WHATSTHIS
        showChannels->setText(QApplication::translate("MainWindow", "Show color Channels", 0));
        dftThumb->setText(QString());
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0));
#ifndef QT_NO_TOOLTIP
        toolBar->setToolTip(QApplication::translate("MainWindow", "Crop to mirror outline and go to DFT analysis", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        toolBar->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
