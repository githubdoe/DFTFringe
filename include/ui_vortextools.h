/********************************************************************************
** Form generated from reading UI file 'vortextools.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VORTEXTOOLS_H
#define UI_VORTEXTOOLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSlider>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VortexTools
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *SmoothEdit;
    QSlider *smoothing;
    QCheckBox *showOrientation;

    void setupUi(QDockWidget *VortexTools)
    {
        if (VortexTools->objectName().isEmpty())
            VortexTools->setObjectName(QStringLiteral("VortexTools"));
        VortexTools->resize(187, 128);
        VortexTools->setMaximumSize(QSize(524287, 151));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label = new QLabel(dockWidgetContents);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        SmoothEdit = new QLineEdit(dockWidgetContents);
        SmoothEdit->setObjectName(QStringLiteral("SmoothEdit"));

        horizontalLayout->addWidget(SmoothEdit);


        verticalLayout->addLayout(horizontalLayout);

        smoothing = new QSlider(dockWidgetContents);
        smoothing->setObjectName(QStringLiteral("smoothing"));
        smoothing->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(smoothing);

        showOrientation = new QCheckBox(dockWidgetContents);
        showOrientation->setObjectName(QStringLiteral("showOrientation"));

        verticalLayout->addWidget(showOrientation);

        VortexTools->setWidget(dockWidgetContents);

        retranslateUi(VortexTools);

        QMetaObject::connectSlotsByName(VortexTools);
    } // setupUi

    void retranslateUi(QDockWidget *VortexTools)
    {
        VortexTools->setWindowTitle(QApplication::translate("VortexTools", "Vortex", 0));
        label->setText(QApplication::translate("VortexTools", "Smoothing", 0));
        showOrientation->setText(QApplication::translate("VortexTools", "Show Orientation", 0));
    } // retranslateUi

};

namespace Ui {
    class VortexTools: public Ui_VortexTools {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VORTEXTOOLS_H
