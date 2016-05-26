/********************************************************************************
** Form generated from reading UI file 'dfttools.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DFTTOOLS_H
#define UI_DFTTOOLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DFTTools
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_4;
    QPushButton *pushButton;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QDoubleSpinBox *filterAdjuster;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_3;
    QComboBox *colorChannelCB;
    QPushButton *computeDFT;
    QSlider *gammaSl;
    QSpacerItem *verticalSpacer;

    void setupUi(QDockWidget *DFTTools)
    {
        if (DFTTools->objectName().isEmpty())
            DFTTools->setObjectName(QStringLiteral("DFTTools"));
        DFTTools->resize(176, 582);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(1);
        sizePolicy.setVerticalStretch(1);
        sizePolicy.setHeightForWidth(DFTTools->sizePolicy().hasHeightForWidth());
        DFTTools->setSizePolicy(sizePolicy);
        DFTTools->setMinimumSize(QSize(176, 239));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout_4 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        pushButton = new QPushButton(dockWidgetContents);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy1);
        QFont font;
        font.setBold(true);
        font.setItalic(false);
        font.setWeight(75);
        pushButton->setFont(font);
        pushButton->setLayoutDirection(Qt::LeftToRight);
        pushButton->setStyleSheet(QLatin1String("border-style: outset;\n"
"border-width: 3px; border-radius:15px; border-color: darkgray;\n"
"font: bold 12px;\n"
"min-width: 10em;\n"
"padding: 6px;\n"
"hover {background-color: lightblue;}\n"
""));

        verticalLayout_4->addWidget(pushButton);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(dockWidgetContents);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        filterAdjuster = new QDoubleSpinBox(dockWidgetContents);
        filterAdjuster->setObjectName(QStringLiteral("filterAdjuster"));
        filterAdjuster->setDecimals(4);
        filterAdjuster->setMaximum(10000);

        horizontalLayout_3->addWidget(filterAdjuster);


        verticalLayout_4->addLayout(horizontalLayout_3);

        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        verticalLayout_3 = new QVBoxLayout(groupBox_3);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        colorChannelCB = new QComboBox(groupBox_3);
        colorChannelCB->setObjectName(QStringLiteral("colorChannelCB"));

        verticalLayout_3->addWidget(colorChannelCB);


        verticalLayout_4->addWidget(groupBox_3);

        computeDFT = new QPushButton(dockWidgetContents);
        computeDFT->setObjectName(QStringLiteral("computeDFT"));
        sizePolicy1.setHeightForWidth(computeDFT->sizePolicy().hasHeightForWidth());
        computeDFT->setSizePolicy(sizePolicy1);
        computeDFT->setLayoutDirection(Qt::RightToLeft);

        verticalLayout_4->addWidget(computeDFT);

        gammaSl = new QSlider(dockWidgetContents);
        gammaSl->setObjectName(QStringLiteral("gammaSl"));
        gammaSl->setValue(50);
        gammaSl->setOrientation(Qt::Horizontal);

        verticalLayout_4->addWidget(gammaSl);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        DFTTools->setWidget(dockWidgetContents);

        retranslateUi(DFTTools);

        QMetaObject::connectSlotsByName(DFTTools);
    } // setupUi

    void retranslateUi(QDockWidget *DFTTools)
    {
        DFTTools->setWindowTitle(QApplication::translate("DFTTools", "DFT", 0));
        pushButton->setText(QApplication::translate("DFTTools", "Compute Surface", 0));
        label_3->setText(QApplication::translate("DFTTools", "Center Filter", 0));
        groupBox_3->setTitle(QApplication::translate("DFTTools", "Color Channel Select", 0));
        computeDFT->setText(QApplication::translate("DFTTools", "Recompute DFT", 0));
#ifndef QT_NO_TOOLTIP
        gammaSl->setToolTip(QApplication::translate("DFTTools", "<html><head/><body><p>Adjust contrast of View.</p></body></html>", 0));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class DFTTools: public Ui_DFTTools {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DFTTOOLS_H
