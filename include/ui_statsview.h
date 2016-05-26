/********************************************************************************
** Form generated from reading UI file 'statsview.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_STATSVIEW_H
#define UI_STATSVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_statsView
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *removeOutliers;
    QHBoxLayout *horizontalLayout_2;
    QCheckBox *checkBox_2;
    QDoubleSpinBox *RMSLimit;
    QCheckBox *showWftNames;
    QGroupBox *groupBox_2;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *checkBox_4;
    QSpinBox *zernFromSP;
    QSpinBox *zernToSP;
    QPushButton *replot;
    QLabel *label;
    QPushButton *SaveCSV;
    QPushButton *savePdf;
    QPushButton *saveImg;
    QSpacerItem *verticalSpacer;
    QFrame *frame;

    void setupUi(QDialog *statsView)
    {
        if (statsView->objectName().isEmpty())
            statsView->setObjectName(QStringLiteral("statsView"));
        statsView->resize(939, 526);
        horizontalLayout = new QHBoxLayout(statsView);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(statsView);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        removeOutliers = new QCheckBox(groupBox);
        removeOutliers->setObjectName(QStringLiteral("removeOutliers"));

        verticalLayout->addWidget(removeOutliers);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        checkBox_2 = new QCheckBox(groupBox);
        checkBox_2->setObjectName(QStringLiteral("checkBox_2"));

        horizontalLayout_2->addWidget(checkBox_2);

        RMSLimit = new QDoubleSpinBox(groupBox);
        RMSLimit->setObjectName(QStringLiteral("RMSLimit"));
        RMSLimit->setDecimals(3);
        RMSLimit->setValue(0.5);

        horizontalLayout_2->addWidget(RMSLimit);


        verticalLayout->addLayout(horizontalLayout_2);

        showWftNames = new QCheckBox(groupBox);
        showWftNames->setObjectName(QStringLiteral("showWftNames"));

        verticalLayout->addWidget(showWftNames);

        groupBox_2 = new QGroupBox(groupBox);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        horizontalLayout_3 = new QHBoxLayout(groupBox_2);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        checkBox_4 = new QCheckBox(groupBox_2);
        checkBox_4->setObjectName(QStringLiteral("checkBox_4"));

        horizontalLayout_3->addWidget(checkBox_4);

        zernFromSP = new QSpinBox(groupBox_2);
        zernFromSP->setObjectName(QStringLiteral("zernFromSP"));

        horizontalLayout_3->addWidget(zernFromSP);

        zernToSP = new QSpinBox(groupBox_2);
        zernToSP->setObjectName(QStringLiteral("zernToSP"));
        zernToSP->setMinimum(3);
        zernToSP->setMaximum(34);
        zernToSP->setValue(5);

        horizontalLayout_3->addWidget(zernToSP);


        verticalLayout->addWidget(groupBox_2);

        replot = new QPushButton(groupBox);
        replot->setObjectName(QStringLiteral("replot"));

        verticalLayout->addWidget(replot);

        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setOpenExternalLinks(true);
        label->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);

        verticalLayout->addWidget(label);

        SaveCSV = new QPushButton(groupBox);
        SaveCSV->setObjectName(QStringLiteral("SaveCSV"));

        verticalLayout->addWidget(SaveCSV);

        savePdf = new QPushButton(groupBox);
        savePdf->setObjectName(QStringLiteral("savePdf"));

        verticalLayout->addWidget(savePdf);

        saveImg = new QPushButton(groupBox);
        saveImg->setObjectName(QStringLiteral("saveImg"));

        verticalLayout->addWidget(saveImg);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addWidget(groupBox);

        frame = new QFrame(statsView);
        frame->setObjectName(QStringLiteral("frame"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(8);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy1);
        frame->setStyleSheet(QStringLiteral("background-color: rgb(253, 253, 253);"));
        frame->setFrameShape(QFrame::Box);
        frame->setFrameShadow(QFrame::Plain);
        frame->setLineWidth(1);
        frame->setMidLineWidth(0);

        horizontalLayout->addWidget(frame);

        frame->raise();
        groupBox->raise();

        retranslateUi(statsView);

        QMetaObject::connectSlotsByName(statsView);
    } // setupUi

    void retranslateUi(QDialog *statsView)
    {
        statsView->setWindowTitle(QApplication::translate("statsView", "wavefront statistics", 0));
#ifndef QT_NO_WHATSTHIS
        statsView->setWhatsThis(QApplication::translate("statsView", "<html><head/><body><p>Three plots of statistics about the loaded wavefronts</p><ol style=\"margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; -qt-list-indent: 1;\"><li style=\" margin-top:12px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">RMS of Running average of wavefront. Showing how RMS changes by adding each wavefront to the average.</li><li style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Histogram of wavefront RMS for all loaded wavefronts</li><li style=\" margin-top:0px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">Box plot of wavefront RMS and zernike terms for each loaded wavefront.</li></ol><p>Use the running average plot to determine when you have averaged enough wavefronts in order for the RMS to stabilize.</p><p>The RMS histogram gives you a good idea of the range and variance of the RMS values</p><p>The box pl"
                        "ot also shows the range and variance of the RMS and the Zernike terms. It can indicate which terms are adding to the RMS variability.</p></body></html>", 0));
#endif // QT_NO_WHATSTHIS
        groupBox->setTitle(QString());
#ifndef QT_NO_WHATSTHIS
        removeOutliers->setWhatsThis(QApplication::translate("statsView", "<html><head/><body><p><span style=\" font-family:'Arial';  color:#000000; background-color:#ffffff;\">An outlier is any value that lies more than one and a half times the length of the box from either end of the box.</span></p></body></html>", 0));
#endif // QT_NO_WHATSTHIS
        removeOutliers->setText(QApplication::translate("statsView", "Remove outliers", 0));
        checkBox_2->setText(QApplication::translate("statsView", "Remove if RMS >: ", 0));
        showWftNames->setText(QApplication::translate("statsView", "Show Wavefront File Names", 0));
        groupBox_2->setTitle(QApplication::translate("statsView", "Zerns to include in wavefront plot", 0));
#ifndef QT_NO_WHATSTHIS
        checkBox_4->setWhatsThis(QApplication::translate("statsView", "<html><head/><body><p>Add zernike terms to the running average. </p><p> Select a range of which terms to add.</p></body></html>", 0));
#endif // QT_NO_WHATSTHIS
        checkBox_4->setText(QApplication::translate("statsView", "zerns from - to", 0));
        replot->setText(QApplication::translate("statsView", "Replot", 0));
#ifndef QT_NO_TOOLTIP
        label->setToolTip(QApplication::translate("statsView", "Linik to wiki about box plots", 0));
#endif // QT_NO_TOOLTIP
        label->setText(QApplication::translate("statsView", "<html><head/><body><p><a href=\"https://en.wikipedia.org/wiki/Box_plot\"><span style=\" text-decoration: underline; color:#0000ff;\">What is a Box Plot?</span></a></p></body></html>", 0));
        SaveCSV->setText(QApplication::translate("statsView", "Save zerns as .csv", 0));
        savePdf->setText(QApplication::translate("statsView", "Save .pdf", 0));
        saveImg->setText(QApplication::translate("statsView", "Save as image (.png)", 0));
    } // retranslateUi

};

namespace Ui {
    class statsView: public Ui_statsView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_STATSVIEW_H
