/********************************************************************************
** Form generated from reading UI file 'vortexdebug.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VORTEXDEBUG_H
#define UI_VORTEXDEBUG_H

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
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vortexDebug
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QSlider *smooth;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QCheckBox *showInput;
    QCheckBox *showFdom;
    QCheckBox *showFdom2;
    QCheckBox *showFdom3;
    QCheckBox *showOrientation;
    QCheckBox *showWrapped;
    QCheckBox *showUnwrapped;
    QSpacerItem *verticalSpacer;

    void setupUi(QDockWidget *vortexDebug)
    {
        if (vortexDebug->objectName().isEmpty())
            vortexDebug->setObjectName(QStringLiteral("vortexDebug"));
        vortexDebug->resize(268, 256);
        vortexDebug->setAllowedAreas(Qt::AllDockWidgetAreas);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        smooth = new QSlider(dockWidgetContents);
        smooth->setObjectName(QStringLiteral("smooth"));
        smooth->setSliderPosition(9);
        smooth->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(smooth);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);


        verticalLayout->addLayout(horizontalLayout);

        groupBox = new QGroupBox(dockWidgetContents);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        showInput = new QCheckBox(groupBox);
        showInput->setObjectName(QStringLiteral("showInput"));

        gridLayout->addWidget(showInput, 0, 0, 1, 1);

        showFdom = new QCheckBox(groupBox);
        showFdom->setObjectName(QStringLiteral("showFdom"));

        gridLayout->addWidget(showFdom, 0, 1, 1, 2);

        showFdom2 = new QCheckBox(groupBox);
        showFdom2->setObjectName(QStringLiteral("showFdom2"));

        gridLayout->addWidget(showFdom2, 0, 3, 1, 1);

        showFdom3 = new QCheckBox(groupBox);
        showFdom3->setObjectName(QStringLiteral("showFdom3"));

        gridLayout->addWidget(showFdom3, 1, 0, 1, 2);

        showOrientation = new QCheckBox(groupBox);
        showOrientation->setObjectName(QStringLiteral("showOrientation"));

        gridLayout->addWidget(showOrientation, 1, 2, 1, 2);

        showWrapped = new QCheckBox(groupBox);
        showWrapped->setObjectName(QStringLiteral("showWrapped"));

        gridLayout->addWidget(showWrapped, 2, 0, 1, 3);

        showUnwrapped = new QCheckBox(groupBox);
        showUnwrapped->setObjectName(QStringLiteral("showUnwrapped"));

        gridLayout->addWidget(showUnwrapped, 3, 0, 1, 3);


        verticalLayout->addWidget(groupBox);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        vortexDebug->setWidget(dockWidgetContents);

        retranslateUi(vortexDebug);

        QMetaObject::connectSlotsByName(vortexDebug);
    } // setupUi

    void retranslateUi(QDockWidget *vortexDebug)
    {
        vortexDebug->setWindowTitle(QApplication::translate("vortexDebug", "Vortex Debug", 0));
        label->setText(QApplication::translate("vortexDebug", "Orientation Smoothing ", 0));
        groupBox->setTitle(QApplication::translate("vortexDebug", "Display data", 0));
        showInput->setText(QApplication::translate("vortexDebug", "Input", 0));
        showFdom->setText(QApplication::translate("vortexDebug", "Fdom", 0));
        showFdom2->setText(QApplication::translate("vortexDebug", "FDom2", 0));
        showFdom3->setText(QApplication::translate("vortexDebug", "Fdom 3", 0));
        showOrientation->setText(QApplication::translate("vortexDebug", "Orientation", 0));
        showWrapped->setText(QApplication::translate("vortexDebug", "Wrapped Phase", 0));
        showUnwrapped->setText(QApplication::translate("vortexDebug", "Unwrapped Phase", 0));
    } // retranslateUi

};

namespace Ui {
    class vortexDebug: public Ui_vortexDebug {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VORTEXDEBUG_H
