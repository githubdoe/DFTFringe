/********************************************************************************
** Form generated from reading UI file 'simulationsview.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMULATIONSVIEW_H
#define UI_SIMULATIONSVIEW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "psfplot.h"
#include "qwt_plot.h"

QT_BEGIN_NAMESPACE

class Ui_SimulationsView
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QLabel *label_2;
    QDoubleSpinBox *defocusSB;
    QLabel *label_3;
    QDoubleSpinBox *gammaSB;
    QLabel *label_5;
    QSpinBox *FFTSizeSB;
    QLabel *label_4;
    QDoubleSpinBox *centerMagnifySB;
    QPushButton *MakePB;
    QHBoxLayout *horizontalLayout_7;
    QLabel *inside;
    QLabel *Focused;
    QLabel *outside;
    QHBoxLayout *horizontalLayout_8;
    psfPlot *psfView;
    QwtPlot *MTF;

    void setupUi(QWidget *SimulationsView)
    {
        if (SimulationsView->objectName().isEmpty())
            SimulationsView->setObjectName(QStringLiteral("SimulationsView"));
        SimulationsView->resize(825, 516);
        verticalLayout = new QVBoxLayout(SimulationsView);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_2 = new QLabel(SimulationsView);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_2);

        defocusSB = new QDoubleSpinBox(SimulationsView);
        defocusSB->setObjectName(QStringLiteral("defocusSB"));
        defocusSB->setMinimum(-50);
        defocusSB->setMaximum(49);
        defocusSB->setSingleStep(0.1);
        defocusSB->setValue(10);

        horizontalLayout->addWidget(defocusSB);

        label_3 = new QLabel(SimulationsView);
        label_3->setObjectName(QStringLiteral("label_3"));
        label_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_3);

        gammaSB = new QDoubleSpinBox(SimulationsView);
        gammaSB->setObjectName(QStringLiteral("gammaSB"));
        gammaSB->setMinimum(0.1);
        gammaSB->setSingleStep(0.1);
        gammaSB->setValue(2);

        horizontalLayout->addWidget(gammaSB);

        label_5 = new QLabel(SimulationsView);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_5);

        FFTSizeSB = new QSpinBox(SimulationsView);
        FFTSizeSB->setObjectName(QStringLiteral("FFTSizeSB"));
        FFTSizeSB->setMinimum(200);
        FFTSizeSB->setMaximum(3000);
        FFTSizeSB->setSingleStep(100);
        FFTSizeSB->setValue(512);

        horizontalLayout->addWidget(FFTSizeSB);

        label_4 = new QLabel(SimulationsView);
        label_4->setObjectName(QStringLiteral("label_4"));
        label_4->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        horizontalLayout->addWidget(label_4);

        centerMagnifySB = new QDoubleSpinBox(SimulationsView);
        centerMagnifySB->setObjectName(QStringLiteral("centerMagnifySB"));
        centerMagnifySB->setMinimum(1);
        centerMagnifySB->setValue(4);

        horizontalLayout->addWidget(centerMagnifySB);

        MakePB = new QPushButton(SimulationsView);
        MakePB->setObjectName(QStringLiteral("MakePB"));

        horizontalLayout->addWidget(MakePB);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        inside = new QLabel(SimulationsView);
        inside->setObjectName(QStringLiteral("inside"));

        horizontalLayout_7->addWidget(inside);

        Focused = new QLabel(SimulationsView);
        Focused->setObjectName(QStringLiteral("Focused"));

        horizontalLayout_7->addWidget(Focused);

        outside = new QLabel(SimulationsView);
        outside->setObjectName(QStringLiteral("outside"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(2);
        sizePolicy.setHeightForWidth(outside->sizePolicy().hasHeightForWidth());
        outside->setSizePolicy(sizePolicy);

        horizontalLayout_7->addWidget(outside);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        psfView = new psfPlot(SimulationsView);
        psfView->setObjectName(QStringLiteral("psfView"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(1);
        sizePolicy1.setHeightForWidth(psfView->sizePolicy().hasHeightForWidth());
        psfView->setSizePolicy(sizePolicy1);

        horizontalLayout_8->addWidget(psfView);

        MTF = new QwtPlot(SimulationsView);
        MTF->setObjectName(QStringLiteral("MTF"));
        sizePolicy1.setHeightForWidth(MTF->sizePolicy().hasHeightForWidth());
        MTF->setSizePolicy(sizePolicy1);

        horizontalLayout_8->addWidget(MTF);


        verticalLayout->addLayout(horizontalLayout_8);


        retranslateUi(SimulationsView);

        QMetaObject::connectSlotsByName(SimulationsView);
    } // setupUi

    void retranslateUi(QWidget *SimulationsView)
    {
        SimulationsView->setWindowTitle(QApplication::translate("SimulationsView", "Form", 0));
        label_2->setText(QApplication::translate("SimulationsView", "Defocus:", 0));
#ifndef QT_NO_TOOLTIP
        defocusSB->setToolTip(QApplication::translate("SimulationsView", "Eyepiece defocus in milimeters", 0));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("SimulationsView", "Image contrast (gamma)", 0));
        label_5->setText(QApplication::translate("SimulationsView", "FFT Size", 0));
#ifndef QT_NO_TOOLTIP
        FFTSizeSB->setToolTip(QApplication::translate("SimulationsView", "If alysing occurs increase this value", 0));
#endif // QT_NO_TOOLTIP
        label_4->setText(QApplication::translate("SimulationsView", "Magnify", 0));
#ifndef QT_NO_TOOLTIP
        centerMagnifySB->setToolTip(QApplication::translate("SimulationsView", "Imaage magnifiction (as with an eyepiece)", 0));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        MakePB->setToolTip(QApplication::translate("SimulationsView", "Generat star tests.", 0));
#endif // QT_NO_TOOLTIP
        MakePB->setText(QApplication::translate("SimulationsView", "Make", 0));
        inside->setText(QApplication::translate("SimulationsView", "Inside", 0));
        Focused->setText(QApplication::translate("SimulationsView", "Focused", 0));
        outside->setText(QApplication::translate("SimulationsView", "outside", 0));
    } // retranslateUi

};

namespace Ui {
    class SimulationsView: public Ui_SimulationsView {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMULATIONSVIEW_H
