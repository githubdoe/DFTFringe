/********************************************************************************
** Form generated from reading UI file 'oglcontrols.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_OGLCONTROLS_H
#define UI_OGLCONTROLS_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDockWidget>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OGLControls
{
public:
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *checkBox;
    QGroupBox *groupBox_3;
    QVBoxLayout *verticalLayout_4;
    QSpinBox *surfaceMagnification;
    QGroupBox *groupBox_4;
    QVBoxLayout *verticalLayout_5;
    QLabel *label_4;
    QSpinBox *spinBox;
    QSpacerItem *verticalSpacer;

    void setupUi(QDockWidget *OGLControls)
    {
        if (OGLControls->objectName().isEmpty())
            OGLControls->setObjectName(QStringLiteral("OGLControls"));
        OGLControls->resize(143, 526);
        OGLControls->setMinimumSize(QSize(100, 200));
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QStringLiteral("dockWidgetContents"));
        verticalLayout_3 = new QVBoxLayout(dockWidgetContents);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        checkBox = new QCheckBox(dockWidgetContents);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        verticalLayout_3->addWidget(checkBox);

        groupBox_3 = new QGroupBox(dockWidgetContents);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_3->sizePolicy().hasHeightForWidth());
        groupBox_3->setSizePolicy(sizePolicy);
        verticalLayout_4 = new QVBoxLayout(groupBox_3);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        surfaceMagnification = new QSpinBox(groupBox_3);
        surfaceMagnification->setObjectName(QStringLiteral("surfaceMagnification"));
        surfaceMagnification->setMinimum(10);
        surfaceMagnification->setMaximum(1000);
        surfaceMagnification->setSingleStep(10);
        surfaceMagnification->setValue(200);

        verticalLayout_4->addWidget(surfaceMagnification);


        verticalLayout_3->addWidget(groupBox_3);

        groupBox_4 = new QGroupBox(dockWidgetContents);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy1);
        verticalLayout_5 = new QVBoxLayout(groupBox_4);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        label_4 = new QLabel(groupBox_4);
        label_4->setObjectName(QStringLiteral("label_4"));

        verticalLayout_5->addWidget(label_4);

        spinBox = new QSpinBox(groupBox_4);
        spinBox->setObjectName(QStringLiteral("spinBox"));
        spinBox->setMinimum(10);
        spinBox->setMaximum(100);
        spinBox->setSingleStep(10);
        spinBox->setValue(100);

        verticalLayout_5->addWidget(spinBox);


        verticalLayout_3->addWidget(groupBox_4);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(verticalSpacer);

        OGLControls->setWidget(dockWidgetContents);

        retranslateUi(OGLControls);

        QMetaObject::connectSlotsByName(OGLControls);
    } // setupUi

    void retranslateUi(QDockWidget *OGLControls)
    {
        OGLControls->setWindowTitle(QApplication::translate("OGLControls", "3D", 0));
        checkBox->setText(QApplication::translate("OGLControls", "Fill Grid", 0));
        groupBox_3->setTitle(QApplication::translate("OGLControls", "Height Magnification", 0));
        groupBox_4->setTitle(QApplication::translate("OGLControls", "Resolution", 0));
        label_4->setText(QApplication::translate("OGLControls", "Percent of original", 0));
    } // retranslateUi

};

namespace Ui {
    class OGLControls: public Ui_OGLControls {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_OGLCONTROLS_H
