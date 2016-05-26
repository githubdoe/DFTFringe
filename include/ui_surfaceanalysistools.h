/********************************************************************************
** Form generated from reading UI file 'surfaceanalysistools.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SURFACEANALYSISTOOLS_H
#define UI_SURFACEANALYSISTOOLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDial>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_surfaceAnalysisTools
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_5;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout;
    QRadioButton *wavefrontBased;
    QRadioButton *zernikeBased;
    QGroupBox *groupBox_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpinBox *spinBox;
    QLabel *outsideEdgeMaskLabel;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QSpinBox *spinBox_2;
    QLabel *insideEdgeMaskLabel;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout;
    QPushButton *deleteWave;
    QPushButton *transformsPB;
    QPushButton *averagePB;
    QListWidget *wavefrontList;
    QPushButton *SelectAllPB;
    QPushButton *SelectNonePB;
    QPushButton *InvertPB;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *blurMm;
    QCheckBox *checkBox_2;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *blurCB;
    QSpinBox *surfaceSmoothGausianBlurr;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_2;
    QLabel *label_4;
    QCheckBox *checkBox;
    QLabel *defocusNm;
    QDial *defocusDial;
    QLabel *label_3;
    QLineEdit *defocusWaves;

    void setupUi(QDockWidget *surfaceAnalysisTools)
    {
        if (surfaceAnalysisTools->objectName().isEmpty())
            surfaceAnalysisTools->setObjectName(QStringLiteral("surfaceAnalysisTools"));
        surfaceAnalysisTools->resize(275, 760);
        surfaceAnalysisTools->setMinimumSize(QSize(275, 601));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout_5 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_4 = new QVBoxLayout(groupBox);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        wavefrontBased = new QRadioButton(groupBox);
        wavefrontBased->setObjectName(QStringLiteral("wavefrontBased"));
        wavefrontBased->setChecked(true);

        verticalLayout->addWidget(wavefrontBased);

        zernikeBased = new QRadioButton(groupBox);
        zernikeBased->setObjectName(QStringLiteral("zernikeBased"));
        zernikeBased->setChecked(false);

        verticalLayout->addWidget(zernikeBased);


        verticalLayout_4->addLayout(verticalLayout);


        verticalLayout_5->addWidget(groupBox);

        groupBox_2 = new QGroupBox(dockWidgetContents);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        verticalLayout_2 = new QVBoxLayout(groupBox_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(groupBox_2);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        spinBox = new QSpinBox(groupBox_2);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMinimum(-99);
        spinBox->setMaximum(5);

        horizontalLayout->addWidget(spinBox);


        horizontalLayout_4->addLayout(horizontalLayout);

        outsideEdgeMaskLabel = new QLabel(groupBox_2);
        outsideEdgeMaskLabel->setObjectName(QStringLiteral("outsideEdgeMaskLabel"));

        horizontalLayout_4->addWidget(outsideEdgeMaskLabel);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        spinBox_2 = new QSpinBox(groupBox_2);
        spinBox_2->setObjectName(QStringLiteral("spinBox_2"));
        spinBox_2->setMinimum(-99);

        horizontalLayout_2->addWidget(spinBox_2);


        horizontalLayout_5->addLayout(horizontalLayout_2);

        insideEdgeMaskLabel = new QLabel(groupBox_2);
        insideEdgeMaskLabel->setObjectName(QStringLiteral("insideEdgeMaskLabel"));

        horizontalLayout_5->addWidget(insideEdgeMaskLabel);


        verticalLayout_2->addLayout(horizontalLayout_5);


        verticalLayout_5->addWidget(groupBox_2);

        groupBox_4 = new QGroupBox(dockWidgetContents);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout = new QGridLayout(groupBox_4);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        deleteWave = new QPushButton(groupBox_4);
        deleteWave->setObjectName(QStringLiteral("deleteWave"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/icons/Button-Close-icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        deleteWave->setIcon(icon);
        deleteWave->setIconSize(QSize(20, 20));

        gridLayout->addWidget(deleteWave, 3, 0, 1, 1);

        transformsPB = new QPushButton(groupBox_4);
        transformsPB->setObjectName(QStringLiteral("transformsPB"));

        gridLayout->addWidget(transformsPB, 3, 1, 1, 1);

        averagePB = new QPushButton(groupBox_4);
        averagePB->setObjectName(QStringLiteral("averagePB"));
        averagePB->setStyleSheet(QStringLiteral(""));

        gridLayout->addWidget(averagePB, 3, 2, 1, 1);

        wavefrontList = new QListWidget(groupBox_4);
        wavefrontList->setObjectName(QStringLiteral("wavefrontList"));
        wavefrontList->setEditTriggers(QAbstractItemView::DoubleClicked);
        wavefrontList->setAlternatingRowColors(true);
        wavefrontList->setSelectionMode(QAbstractItemView::ExtendedSelection);

        gridLayout->addWidget(wavefrontList, 1, 0, 1, 3);

        SelectAllPB = new QPushButton(groupBox_4);
        SelectAllPB->setObjectName(QStringLiteral("SelectAllPB"));

        gridLayout->addWidget(SelectAllPB, 4, 1, 1, 1);

        SelectNonePB = new QPushButton(groupBox_4);
        SelectNonePB->setObjectName(QStringLiteral("SelectNonePB"));

        gridLayout->addWidget(SelectNonePB, 4, 2, 1, 1);

        InvertPB = new QPushButton(groupBox_4);
        InvertPB->setObjectName(QStringLiteral("InvertPB"));

        gridLayout->addWidget(InvertPB, 4, 0, 1, 1);


        verticalLayout_5->addWidget(groupBox_4);

        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        blurMm = new QLabel(groupBox_3);
        blurMm->setObjectName(QStringLiteral("blurMm"));
        blurMm->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        verticalLayout_3->addWidget(blurMm);

        checkBox_2 = new QCheckBox(groupBox_3);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        verticalLayout_3->addWidget(checkBox_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        blurCB = new QCheckBox(groupBox_3);
        blurCB->setObjectName(QStringLiteral("blurCB"));

        horizontalLayout_3->addWidget(blurCB);

        surfaceSmoothGausianBlurr = new QSpinBox(groupBox_3);
        surfaceSmoothGausianBlurr->setObjectName(QStringLiteral("surfaceSmoothGausianBlurr"));
        surfaceSmoothGausianBlurr->setEnabled(true);
        surfaceSmoothGausianBlurr->setMinimum(0);
        surfaceSmoothGausianBlurr->setMaximum(400);
        surfaceSmoothGausianBlurr->setSingleStep(1);
        surfaceSmoothGausianBlurr->setValue(21);

        horizontalLayout_3->addWidget(surfaceSmoothGausianBlurr);


        verticalLayout_3->addLayout(horizontalLayout_3);

        groupBox_5 = new QGroupBox(groupBox_3);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_2 = new QGridLayout(groupBox_5);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        label_4 = new QLabel(groupBox_5);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 1, 3, 1, 1);

        checkBox = new QCheckBox(groupBox_5);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        gridLayout_2->addWidget(checkBox, 0, 1, 1, 1);

        defocusNm = new QLabel(groupBox_5);
        defocusNm->setObjectName(QStringLiteral("defocusNm"));
        defocusNm->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_2->addWidget(defocusNm, 1, 2, 1, 1);

        defocusDial = new QDial(groupBox_5);
        defocusDial->setObjectName(QStringLiteral("defocusDial"));
        defocusDial->setEnabled(false);
        defocusDial->setMinimum(-100);
        defocusDial->setMaximum(100);
        defocusDial->setInvertedAppearance(false);
        defocusDial->setInvertedControls(false);
        defocusDial->setWrapping(false);
        defocusDial->setNotchesVisible(true);

        gridLayout_2->addWidget(defocusDial, 1, 1, 1, 1);

        label_3 = new QLabel(groupBox_5);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 0, 3, 1, 1);

        defocusWaves = new QLineEdit(groupBox_5);
        defocusWaves->setObjectName(QStringLiteral("defocusWaves"));
        defocusWaves->setEnabled(false);

        gridLayout_2->addWidget(defocusWaves, 0, 2, 1, 1);


        verticalLayout_3->addWidget(groupBox_5);


        verticalLayout_5->addWidget(groupBox_3);

        surfaceAnalysisTools->setWidget(dockWidgetContents);

        retranslateUi(surfaceAnalysisTools);

        QMetaObject::connectSlotsByName(surfaceAnalysisTools);
    } // setupUi

    void retranslateUi(QDockWidget *surfaceAnalysisTools)
    {
        surfaceAnalysisTools->setWindowTitle(QApplication::translate("surfaceAnalysisTools", "Surface", 0));
        groupBox->setTitle(QApplication::translate("surfaceAnalysisTools", "Surface Type", 0));
        wavefrontBased->setText(QApplication::translate("surfaceAnalysisTools", "WaveFront", 0));
        zernikeBased->setText(QApplication::translate("surfaceAnalysisTools", "Zernike Based", 0));
        groupBox_2->setTitle(QApplication::translate("surfaceAnalysisTools", "Ignore Edge Region", 0));
        label->setText(QApplication::translate("surfaceAnalysisTools", "outside Edge", 0));
        outsideEdgeMaskLabel->setText(QApplication::translate("surfaceAnalysisTools", "0 mm", 0));
        label_2->setText(QApplication::translate("surfaceAnalysisTools", "Center Edge", 0));
        insideEdgeMaskLabel->setText(QApplication::translate("surfaceAnalysisTools", "0 mm", 0));
        groupBox_4->setTitle(QApplication::translate("surfaceAnalysisTools", "WaveFront Selection", 0));
#ifndef QT_NO_TOOLTIP
        deleteWave->setToolTip(QApplication::translate("surfaceAnalysisTools", "Delete Selected wavefronts", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        deleteWave->setStatusTip(QApplication::translate("surfaceAnalysisTools", "Unload selected wavefronts from program", 0));
#endif // QT_NO_STATUSTIP
        deleteWave->setText(QString());
        transformsPB->setText(QApplication::translate("surfaceAnalysisTools", "Rotate ", 0));
#ifndef QT_NO_STATUSTIP
        averagePB->setStatusTip(QApplication::translate("surfaceAnalysisTools", "Average selected wavefronts", 0));
#endif // QT_NO_STATUSTIP
        averagePB->setText(QApplication::translate("surfaceAnalysisTools", "Average", 0));
        SelectAllPB->setText(QApplication::translate("surfaceAnalysisTools", "Select All", 0));
        SelectNonePB->setText(QApplication::translate("surfaceAnalysisTools", "Select None", 0));
        InvertPB->setText(QApplication::translate("surfaceAnalysisTools", "Invert ", 0));
        groupBox_3->setTitle(QApplication::translate("surfaceAnalysisTools", "Surface Filter", 0));
        blurMm->setText(QString());
        checkBox_2->setText(QApplication::translate("surfaceAnalysisTools", "special", 0));
        blurCB->setText(QApplication::translate("surfaceAnalysisTools", "Gaussian Blur radius", 0));
        groupBox_5->setTitle(QApplication::translate("surfaceAnalysisTools", "Defocus", 0));
        label_4->setText(QApplication::translate("surfaceAnalysisTools", "mm", 0));
        checkBox->setText(QApplication::translate("surfaceAnalysisTools", "Enable", 0));
        defocusNm->setText(QApplication::translate("surfaceAnalysisTools", "0", 0));
        label_3->setText(QApplication::translate("surfaceAnalysisTools", "Waves", 0));
    } // retranslateUi

};

namespace Ui {
    class surfaceAnalysisTools: public Ui_surfaceAnalysisTools {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SURFACEANALYSISTOOLS_H
